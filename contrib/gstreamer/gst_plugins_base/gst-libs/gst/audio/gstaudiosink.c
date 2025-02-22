/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2005 Wim Taymans <wim@fluendo.com>
 *
 * gstaudiosink.c: simple audio sink base class
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:gstaudiosink
 * @short_description: Simple base class for audio sinks
 * @see_also: #GstBaseAudioSink, #GstRingBuffer, #GstAudioSink.
 *
 * This is the most simple base class for audio sinks that only requires
 * subclasses to implement a set of simple functions:
 *
 * <variablelist>
 *   <varlistentry>
 *     <term>open()</term>
 *     <listitem><para>Open the device.</para></listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>prepare()</term>
 *     <listitem><para>Configure the device with the specified format.</para></listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>write()</term>
 *     <listitem><para>Write samples to the device.</para></listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>reset()</term>
 *     <listitem><para>Unblock writes and flush the device.</para></listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>delay()</term>
 *     <listitem><para>Get the number of samples written but not yet played 
 *     by the device.</para></listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>unprepare()</term>
 *     <listitem><para>Undo operations done by prepare.</para></listitem>
 *   </varlistentry>
 *   <varlistentry>
 *     <term>close()</term>
 *     <listitem><para>Close the device.</para></listitem>
 *   </varlistentry>
 * </variablelist>
 *
 * All scheduling of samples and timestamps is done in this base class
 * together with #GstBaseAudioSink using a default implementation of a
 * #GstRingBuffer that uses threads.
 *
 * Last reviewed on 2006-09-27 (0.10.12)
 */

#include <string.h>

#include "gstaudiosink.h"

#ifdef __SYMBIAN32__
#include <glib_global.h>
#endif

GST_DEBUG_CATEGORY_STATIC (gst_audio_sink_debug);
#define GST_CAT_DEFAULT gst_audio_sink_debug

#define GST_TYPE_AUDIORING_BUFFER        \
        (gst_audioringbuffer_get_type())
#define GST_AUDIORING_BUFFER(obj)        \
        (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AUDIORING_BUFFER,GstAudioRingBuffer))
#define GST_AUDIORING_BUFFER_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AUDIORING_BUFFER,GstAudioRingBufferClass))
#define GST_AUDIORING_BUFFER_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_AUDIORING_BUFFER, GstAudioRingBufferClass))
#define GST_AUDIORING_BUFFER_CAST(obj)        \
        ((GstAudioRingBuffer *)obj)
#define GST_IS_AUDIORING_BUFFER(obj)     \
        (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AUDIORING_BUFFER))
#define GST_IS_AUDIORING_BUFFER_CLASS(klass)\
        (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AUDIORING_BUFFER))

typedef struct _GstAudioRingBuffer GstAudioRingBuffer;
typedef struct _GstAudioRingBufferClass GstAudioRingBufferClass;

#define GST_AUDIORING_BUFFER_GET_COND(buf) (((GstAudioRingBuffer *)buf)->cond)
#define GST_AUDIORING_BUFFER_WAIT(buf)     (g_cond_wait (GST_AUDIORING_BUFFER_GET_COND (buf), GST_OBJECT_GET_LOCK (buf)))
#define GST_AUDIORING_BUFFER_SIGNAL(buf)   (g_cond_signal (GST_AUDIORING_BUFFER_GET_COND (buf)))
#define GST_AUDIORING_BUFFER_BROADCAST(buf)(g_cond_broadcast (GST_AUDIORING_BUFFER_GET_COND (buf)))

struct _GstAudioRingBuffer
{
  GstRingBuffer object;

  gboolean running;
  gint queuedseg;

  GCond *cond;
};

struct _GstAudioRingBufferClass
{
  GstRingBufferClass parent_class;
};

static void gst_audioringbuffer_class_init (GstAudioRingBufferClass * klass);
static void gst_audioringbuffer_init (GstAudioRingBuffer * ringbuffer,
    GstAudioRingBufferClass * klass);
static void gst_audioringbuffer_dispose (GObject * object);
static void gst_audioringbuffer_finalize (GObject * object);

static GstRingBufferClass *ring_parent_class = NULL;

static gboolean gst_audioringbuffer_open_device (GstRingBuffer * buf);
static gboolean gst_audioringbuffer_close_device (GstRingBuffer * buf);
static gboolean gst_audioringbuffer_acquire (GstRingBuffer * buf,
    GstRingBufferSpec * spec);
static gboolean gst_audioringbuffer_release (GstRingBuffer * buf);
static gboolean gst_audioringbuffer_start (GstRingBuffer * buf);
static gboolean gst_audioringbuffer_pause (GstRingBuffer * buf);
static gboolean gst_audioringbuffer_stop (GstRingBuffer * buf);
static guint gst_audioringbuffer_delay (GstRingBuffer * buf);

/* ringbuffer abstract base class */
static GType
gst_audioringbuffer_get_type (void)
{
  static GType ringbuffer_type = 0;

  if (!ringbuffer_type) {
    static const GTypeInfo ringbuffer_info = {
      sizeof (GstAudioRingBufferClass),
      NULL,
      NULL,
      (GClassInitFunc) gst_audioringbuffer_class_init,
      NULL,
      NULL,
      sizeof (GstAudioRingBuffer),
      0,
      (GInstanceInitFunc) gst_audioringbuffer_init,
      NULL
    };

    ringbuffer_type =
        g_type_register_static (GST_TYPE_RING_BUFFER, "GstAudioSinkRingBuffer",
        &ringbuffer_info, 0);
  }
  return ringbuffer_type;
}

static void
gst_audioringbuffer_class_init (GstAudioRingBufferClass * klass)
{
  GObjectClass *gobject_class;
  GstObjectClass *gstobject_class;
  GstRingBufferClass *gstringbuffer_class;

  gobject_class = (GObjectClass *) klass;
  gstobject_class = (GstObjectClass *) klass;
  gstringbuffer_class = (GstRingBufferClass *) klass;

  ring_parent_class = g_type_class_peek_parent (klass);

  gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_audioringbuffer_dispose);
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_audioringbuffer_finalize);

  gstringbuffer_class->open_device =
      GST_DEBUG_FUNCPTR (gst_audioringbuffer_open_device);
  gstringbuffer_class->close_device =
      GST_DEBUG_FUNCPTR (gst_audioringbuffer_close_device);
  gstringbuffer_class->acquire =
      GST_DEBUG_FUNCPTR (gst_audioringbuffer_acquire);
  gstringbuffer_class->release =
      GST_DEBUG_FUNCPTR (gst_audioringbuffer_release);
  gstringbuffer_class->start = GST_DEBUG_FUNCPTR (gst_audioringbuffer_start);
  gstringbuffer_class->pause = GST_DEBUG_FUNCPTR (gst_audioringbuffer_pause);
  gstringbuffer_class->resume = GST_DEBUG_FUNCPTR (gst_audioringbuffer_start);
  gstringbuffer_class->stop = GST_DEBUG_FUNCPTR (gst_audioringbuffer_stop);

  gstringbuffer_class->delay = GST_DEBUG_FUNCPTR (gst_audioringbuffer_delay);
}

typedef guint (*WriteFunc) (GstAudioSink * sink, gpointer data, guint length);

/* this internal thread does nothing else but write samples to the audio device.
 * It will write each segment in the ringbuffer and will update the play
 * pointer. 
 * The start/stop methods control the thread.
 */
static void
audioringbuffer_thread_func (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  GstAudioRingBuffer *abuf = GST_AUDIORING_BUFFER_CAST (buf);
  WriteFunc writefunc;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);

  GST_DEBUG_OBJECT (sink, "enter thread");

  writefunc = csink->write;
  if (writefunc == NULL)
    goto no_function;

  while (TRUE) {
    gint left, len;
    guint8 *readptr;
    gint readseg;

    if (gst_ring_buffer_prepare_read (buf, &readseg, &readptr, &len)) {
      gint written = 0;

      left = len;
      do {
        written = writefunc (sink, readptr + written, left);
        GST_LOG_OBJECT (sink, "transfered %d bytes of %d from segment %d",
            written, left, readseg);
        if (written < 0 || written > left) {
          GST_WARNING_OBJECT (sink,
              "error writing data (reason: %s), skipping segment",
              g_strerror (errno));
          break;
        }
        left -= written;
      } while (left > 0);

      /* clear written samples */
      gst_ring_buffer_clear (buf, readseg);

      /* we wrote one segment */
      gst_ring_buffer_advance (buf, 1);
    } else {
      GST_OBJECT_LOCK (abuf);
      if (!abuf->running)
        goto stop_running;
      GST_DEBUG_OBJECT (sink, "signal wait");
      GST_AUDIORING_BUFFER_SIGNAL (buf);
      GST_DEBUG_OBJECT (sink, "wait for action");
      GST_AUDIORING_BUFFER_WAIT (buf);
      GST_DEBUG_OBJECT (sink, "got signal");
      if (!abuf->running)
        goto stop_running;
      GST_DEBUG_OBJECT (sink, "continue running");
      GST_OBJECT_UNLOCK (abuf);
    }
  }
  GST_DEBUG_OBJECT (sink, "exit thread");

  return;

  /* ERROR */
no_function:
  {
    GST_DEBUG_OBJECT (sink, "no write function, exit thread");
    return;
  }
stop_running:
  {
    GST_OBJECT_UNLOCK (abuf);
    GST_DEBUG_OBJECT (sink, "stop running, exit thread");
    return;
  }
}

static void
gst_audioringbuffer_init (GstAudioRingBuffer * ringbuffer,
    GstAudioRingBufferClass * g_class)
{
  ringbuffer->running = FALSE;
  ringbuffer->queuedseg = 0;

  ringbuffer->cond = g_cond_new ();
}

static void
gst_audioringbuffer_dispose (GObject * object)
{
  G_OBJECT_CLASS (ring_parent_class)->dispose (object);
}

static void
gst_audioringbuffer_finalize (GObject * object)
{
  GstAudioRingBuffer *ringbuffer = GST_AUDIORING_BUFFER_CAST (object);

  g_cond_free (ringbuffer->cond);

  G_OBJECT_CLASS (ring_parent_class)->finalize (object);
}

static gboolean
gst_audioringbuffer_open_device (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  gboolean result = TRUE;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);

  if (csink->open)
    result = csink->open (sink);

  if (!result)
    goto could_not_open;

  return result;

could_not_open:
  {
    GST_DEBUG_OBJECT (sink, "could not open device");
    return FALSE;
  }
}

static gboolean
gst_audioringbuffer_close_device (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  gboolean result = TRUE;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);

  if (csink->close)
    result = csink->close (sink);

  if (!result)
    goto could_not_close;

  return result;

could_not_close:
  {
    GST_DEBUG_OBJECT (sink, "could not close device");
    return FALSE;
  }
}

static gboolean
gst_audioringbuffer_acquire (GstRingBuffer * buf, GstRingBufferSpec * spec)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  GstAudioRingBuffer *abuf;
  gboolean result = FALSE;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);

  if (csink->prepare)
    result = csink->prepare (sink, spec);

  if (!result)
    goto could_not_prepare;

  /* allocate one more segment as we need some headroom */
  spec->segtotal++;

  buf->data = gst_buffer_new_and_alloc (spec->segtotal * spec->segsize);
  memset (GST_BUFFER_DATA (buf->data), 0, GST_BUFFER_SIZE (buf->data));

  abuf = GST_AUDIORING_BUFFER_CAST (buf);
  abuf->running = TRUE;

  sink->thread =
      g_thread_create ((GThreadFunc) audioringbuffer_thread_func, buf, TRUE,
      NULL);
  GST_AUDIORING_BUFFER_WAIT (buf);

  return result;

could_not_prepare:
  {
    GST_DEBUG_OBJECT (sink, "could not prepare device");
    return FALSE;
  }
}

/* function is called with LOCK */
static gboolean
gst_audioringbuffer_release (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  GstAudioRingBuffer *abuf;
  gboolean result = FALSE;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);
  abuf = GST_AUDIORING_BUFFER_CAST (buf);

  abuf->running = FALSE;
  GST_DEBUG_OBJECT (sink, "signal wait");
  GST_AUDIORING_BUFFER_SIGNAL (buf);
  GST_OBJECT_UNLOCK (buf);

  /* join the thread */
  g_thread_join (sink->thread);

  GST_OBJECT_LOCK (buf);

  /* free the buffer */
  gst_buffer_unref (buf->data);
  buf->data = NULL;

  if (csink->unprepare)
    result = csink->unprepare (sink);

  if (!result)
    goto could_not_unprepare;

  return result;

could_not_unprepare:
  {
    GST_DEBUG_OBJECT (sink, "could not unprepare device");
    return FALSE;
  }
}

static gboolean
gst_audioringbuffer_start (GstRingBuffer * buf)
{
  GstAudioSink *sink;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));

  GST_DEBUG_OBJECT (sink, "start, sending signal");
  GST_AUDIORING_BUFFER_SIGNAL (buf);

  return TRUE;
}

static gboolean
gst_audioringbuffer_pause (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);

  /* unblock any pending writes to the audio device */
  if (csink->reset) {
    GST_DEBUG_OBJECT (sink, "reset...");
    csink->reset (sink);
    GST_DEBUG_OBJECT (sink, "reset done");
  }

  return TRUE;
}

static gboolean
gst_audioringbuffer_stop (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  GstAudioRingBuffer *abuf;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);
  abuf = GST_AUDIORING_BUFFER_CAST (buf);

  /* unblock any pending writes to the audio device */
  if (csink->reset) {
    GST_DEBUG_OBJECT (sink, "reset...");
    csink->reset (sink);
    GST_DEBUG_OBJECT (sink, "reset done");
  }

  if (abuf->running) {
    GST_DEBUG_OBJECT (sink, "stop, waiting...");
    GST_AUDIORING_BUFFER_WAIT (buf);
    GST_DEBUG_OBJECT (sink, "stopped");
  }

  return TRUE;
}

static guint
gst_audioringbuffer_delay (GstRingBuffer * buf)
{
  GstAudioSink *sink;
  GstAudioSinkClass *csink;
  guint res = 0;

  sink = GST_AUDIO_SINK (GST_OBJECT_PARENT (buf));
  csink = GST_AUDIO_SINK_GET_CLASS (sink);

  if (csink->delay)
    res = csink->delay (sink);

  return res;
}

/* AudioSink signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  ARG_0,
};

#define _do_init(bla) \
    GST_DEBUG_CATEGORY_INIT (gst_audio_sink_debug, "audiosink", 0, "audiosink element");

GST_BOILERPLATE_FULL (GstAudioSink, gst_audio_sink, GstBaseAudioSink,
    GST_TYPE_BASE_AUDIO_SINK, _do_init);

static GstRingBuffer *gst_audio_sink_create_ringbuffer (GstBaseAudioSink *
    sink);

static void
gst_audio_sink_base_init (gpointer g_class)
{
}

static void
gst_audio_sink_class_init (GstAudioSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbasesink_class;
  GstBaseAudioSinkClass *gstbaseaudiosink_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesink_class = (GstBaseSinkClass *) klass;
  gstbaseaudiosink_class = (GstBaseAudioSinkClass *) klass;

  gstbaseaudiosink_class->create_ringbuffer =
      GST_DEBUG_FUNCPTR (gst_audio_sink_create_ringbuffer);
}

static void
gst_audio_sink_init (GstAudioSink * audiosink, GstAudioSinkClass * g_class)
{
}

static GstRingBuffer *
gst_audio_sink_create_ringbuffer (GstBaseAudioSink * sink)
{
  GstRingBuffer *buffer;

  GST_DEBUG_OBJECT (sink, "creating ringbuffer");
  buffer = g_object_new (GST_TYPE_AUDIORING_BUFFER, NULL);
  GST_DEBUG_OBJECT (sink, "created ringbuffer @%p", buffer);

  return buffer;
}
