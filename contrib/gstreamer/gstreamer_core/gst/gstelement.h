/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *               2000,2004 Wim Taymans <wim@fluendo.com>
 *
 * gstelement.h: Header for GstElement
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


#ifndef __GST_ELEMENT_H__
#define __GST_ELEMENT_H__

/* gstelement.h and gstelementfactory.h include eachother */
typedef struct _GstElement GstElement;
typedef struct _GstElementClass GstElementClass;

/* gstmessage.h needs State */
/**
 * GstState:
 * @GST_STATE_VOID_PENDING     : no pending state.
 * @GST_STATE_NULL             : the NULL state or initial state of an element
 * @GST_STATE_READY            : the element is ready to go to PAUSED
 * @GST_STATE_PAUSED           : the element is PAUSED
 * @GST_STATE_PLAYING          : the element is PLAYING
 *
 * The posible states an element can be in.
 */
typedef enum {
  GST_STATE_VOID_PENDING        = 0,
  GST_STATE_NULL                = 1,
  GST_STATE_READY               = 2,
  GST_STATE_PAUSED              = 3,
  GST_STATE_PLAYING             = 4
} GstState;


#include <gst/gstconfig.h>
#include <gst/gstobject.h>
#include <gst/gstpad.h>
#include <gst/gstbus.h>
#include <gst/gstclock.h>
#include <gst/gstelementfactory.h>
#include <gst/gstplugin.h>
#include <gst/gstpluginfeature.h>
#include <gst/gstindex.h>
#include <gst/gstindexfactory.h>
#include <gst/gstiterator.h>
#include <gst/gstmessage.h>
#include <gst/gsttaglist.h>

G_BEGIN_DECLS

#define GST_TYPE_ELEMENT		(gst_element_get_type ())
#define GST_IS_ELEMENT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_ELEMENT))
#define GST_IS_ELEMENT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_ELEMENT))
#define GST_ELEMENT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_ELEMENT, GstElementClass))
#define GST_ELEMENT(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_ELEMENT, GstElement))
#define GST_ELEMENT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_ELEMENT, GstElementClass))
#define GST_ELEMENT_CAST(obj)		((GstElement*)(obj))

/**
 * GstStateChangeReturn:
 * @GST_STATE_CHANGE_FAILURE   : the state change failed
 * @GST_STATE_CHANGE_SUCCESS   : the state change succeeded
 * @GST_STATE_CHANGE_ASYNC     : the state change will happen asynchronously
 * @GST_STATE_CHANGE_NO_PREROLL: the state change succeeded but the element
 *                               cannot produce data in PAUSED. This typically
 *                               happens with live sources.
 *
 * The possible return values from a state change function. Only
 * @GST_STATE_CHANGE_FAILURE is a real failure.
 */
typedef enum {
  GST_STATE_CHANGE_FAILURE             = 0,
  GST_STATE_CHANGE_SUCCESS             = 1,
  GST_STATE_CHANGE_ASYNC               = 2,
  GST_STATE_CHANGE_NO_PREROLL          = 3
} GstStateChangeReturn;

/* NOTE: this probably should be done with an #ifdef to decide
 * whether to safe-cast or to just do the non-checking cast.
 */

/**
 * GST_STATE:
 * @elem: a #GstElement to return state for.
 *
 * This macro returns the current #GstState of the element.
 */
#define GST_STATE(elem)			(GST_ELEMENT_CAST(elem)->current_state)

/**
 * GST_STATE_NEXT:
 * @elem: a #GstElement to return the next state for.
 *
 * This macro returns the next #GstState of the element.
 */
#define GST_STATE_NEXT(elem)		(GST_ELEMENT_CAST(elem)->next_state)

/**
 * GST_STATE_PENDING:
 * @elem: a #GstElement to return the pending state for.
 *
 * This macro returns the currently pending #GstState of the element.
 */
#define GST_STATE_PENDING(elem)		(GST_ELEMENT_CAST(elem)->pending_state)

/**
 * GST_STATE_TARGET:
 * @elem: a #GstElement to return the target state for.
 *
 * This macro returns the target #GstState of the element.
 *
 * Since: 0.10.13
 */
#define GST_STATE_TARGET(elem)		(GST_ELEMENT_CAST(elem)->abidata.ABI.target_state)

/**
 * GST_STATE_RETURN:
 * @elem: a #GstElement to return the last state result for.
 *
 * This macro returns the last #GstStateChangeReturn value.
 */
#define GST_STATE_RETURN(elem)		(GST_ELEMENT_CAST(elem)->last_return)

#define __GST_SIGN(val)			((val) < 0 ? -1 : ((val) > 0 ? 1 : 0))
/**
 * GST_STATE_GET_NEXT:
 * @cur: A starting #GstState
 * @pending: A target #GstState
 *
 * Given a current state @cur and a target state @pending, calculate the next (intermediate)
 * #GstState.
 */
#define GST_STATE_GET_NEXT(cur,pending)		((cur) + __GST_SIGN ((gint)(pending) - (gint)(cur)))
/**
 * GST_STATE_TRANSITION:
 * @cur: A current state
 * @next: A next state
 *
 * Given a current state @cur and a next state @next, calculate the associated
 * #GstStateChange transition.
 */
#define GST_STATE_TRANSITION(cur,next)		((GstStateChange)(((cur)<<3)|(next)))
/**
 * GST_STATE_TRANSITION_CURRENT:
 * @trans: A #GstStateChange
 *
 * Given a state transition @trans, extract the current #GstState.
 */
#define GST_STATE_TRANSITION_CURRENT(trans)	((GstState)((trans)>>3))
/**
 * GST_STATE_TRANSITION_NEXT:
 * @trans: A #GstStateChange
 *
 * Given a state transition @trans, extract the next #GstState.
 */
#define GST_STATE_TRANSITION_NEXT(trans)	((GstState)((trans)&0x7))

/**
 * GstStateChange:
 * @GST_STATE_CHANGE_NULL_TO_READY    : state change from NULL to READY
 * @GST_STATE_CHANGE_READY_TO_PAUSED  : state change from READY to PAUSED
 * @GST_STATE_CHANGE_PAUSED_TO_PLAYING: state change from PAUSED to PLAYING
 * @GST_STATE_CHANGE_PLAYING_TO_PAUSED: state change from PLAYING to PAUSED
 * @GST_STATE_CHANGE_PAUSED_TO_READY  : state change from PAUSED to READY
 * @GST_STATE_CHANGE_READY_TO_NULL    : state change from READY to NULL
 *
 * The different (interesting) state changes that are passed to the
 * state change functions of elements.
 */
typedef enum /*< flags=0 >*/
{
  GST_STATE_CHANGE_NULL_TO_READY	= (GST_STATE_NULL<<3) | GST_STATE_READY,
  GST_STATE_CHANGE_READY_TO_PAUSED	= (GST_STATE_READY<<3) | GST_STATE_PAUSED,
  GST_STATE_CHANGE_PAUSED_TO_PLAYING	= (GST_STATE_PAUSED<<3) | GST_STATE_PLAYING,
  GST_STATE_CHANGE_PLAYING_TO_PAUSED	= (GST_STATE_PLAYING<<3) | GST_STATE_PAUSED,
  GST_STATE_CHANGE_PAUSED_TO_READY	= (GST_STATE_PAUSED<<3) | GST_STATE_READY,
  GST_STATE_CHANGE_READY_TO_NULL	= (GST_STATE_READY<<3) | GST_STATE_NULL
} GstStateChange;

/**
 * GstElementFlags:
 * @GST_ELEMENT_LOCKED_STATE: ignore state changes from parent
 * @GST_ELEMENT_IS_SINK: the element is a sink
 * @GST_ELEMENT_UNPARENTING: Child is being removed from the parent bin.
 *  gst_bin_remove() on a child already being removed immediately returns FALSE
 * @GST_ELEMENT_FLAG_LAST: offset to define more flags
 *
 * The standard flags that an element may have.
 */
typedef enum
{
  GST_ELEMENT_LOCKED_STATE      = (GST_OBJECT_FLAG_LAST << 0),
  GST_ELEMENT_IS_SINK           = (GST_OBJECT_FLAG_LAST << 1),
  GST_ELEMENT_UNPARENTING       = (GST_OBJECT_FLAG_LAST << 2),
  /* padding */
  GST_ELEMENT_FLAG_LAST         = (GST_OBJECT_FLAG_LAST << 16)
} GstElementFlags;

/**
 * GST_ELEMENT_IS_LOCKED_STATE:
 * @elem: A #GstElement to query
 *
 * Check if the element is in the locked state and therefore will ignore state
 * changes from its parent object.
 */
#define GST_ELEMENT_IS_LOCKED_STATE(elem)        (GST_OBJECT_FLAG_IS_SET(elem,GST_ELEMENT_LOCKED_STATE))

/**
 * GST_ELEMENT_NAME:
 * @elem: A #GstElement to query
 *
 * Gets the name of this element. Use only in core as this is not
 * ABI-compatible. Others use gst_element_get_name()
 */
#define GST_ELEMENT_NAME(elem)			(GST_OBJECT_NAME(elem))

/**
 * GST_ELEMENT_PARENT:
 * @elem: A #GstElement to query
 *
 * Get the parent object of this element.
 */
#define GST_ELEMENT_PARENT(elem)		(GST_ELEMENT_CAST(GST_OBJECT_PARENT(elem)))

/**
 * GST_ELEMENT_BUS:
 * @elem: A #GstElement to query
 *
 * Get the message bus of this element.
 */
#define GST_ELEMENT_BUS(elem)			(GST_ELEMENT_CAST(elem)->bus)

/**
 * GST_ELEMENT_CLOCK:
 * @elem: A #GstElement to query
 *
 * Get the clock of this element
 */
#define GST_ELEMENT_CLOCK(elem)			(GST_ELEMENT_CAST(elem)->clock)

/**
 * GST_ELEMENT_PADS:
 * @elem: A #GstElement to query
 *
 * Get the pads of this elements.
 */
#define GST_ELEMENT_PADS(elem)			(GST_ELEMENT_CAST(elem)->pads)

/**
 * GST_ELEMENT_ERROR:
 * @el:     the element that generates the error
 * @domain: like CORE, LIBRARY, RESOURCE or STREAM (see #GstGError)
 * @code:   error code defined for that domain (see #GstGError)
 * @text:   the message to display (format string and args enclosed in
            parentheses)
 * @debug:  debugging information for the message (format string and args
            enclosed in parentheses)
 *
 * Utility function that elements can use in case they encountered a fatal
 * data processing error. The pipeline will post an error message and the
 * application will be requested to stop further media processing.
 */
#define GST_ELEMENT_ERROR(el, domain, code, text, debug)		\
G_STMT_START {								\
  gchar *__txt = _gst_element_error_printf text;			\
  gchar *__dbg = _gst_element_error_printf debug;			\
  if (__txt)								\
    GST_WARNING_OBJECT (el, "error: %s", __txt);			\
  if (__dbg)								\
    GST_WARNING_OBJECT (el, "error: %s", __dbg);			\
  gst_element_message_full (GST_ELEMENT(el), GST_MESSAGE_ERROR,		\
    GST_ ## domain ## _ERROR, GST_ ## domain ## _ERROR_ ## code,	\
    __txt, __dbg, __FILE__, GST_FUNCTION, __LINE__);			\
} G_STMT_END

/**
 * GST_ELEMENT_WARNING:
 * @el:     the element that generates the warning
 * @domain: like CORE, LIBRARY, RESOURCE or STREAM (see #gstreamer-GstGError)
 * @code:   error code defined for that domain (see #gstreamer-GstGError)
 * @text:   the message to display (format string and args enclosed in
            parentheses)
 * @debug:  debugging information for the message (format string and args
            enclosed in parentheses)
 *
 * Utility function that elements can use in case they encountered a non-fatal
 * data processing problem. The pipeline will post a warning message and the
 * application will be informed.
 */
#define GST_ELEMENT_WARNING(el, domain, code, text, debug)		\
G_STMT_START {								\
  gchar *__txt = _gst_element_error_printf text;			\
  gchar *__dbg = _gst_element_error_printf debug;			\
  if (__txt)								\
    GST_WARNING_OBJECT (el, "warning: %s", __txt);			\
  if (__dbg)								\
    GST_WARNING_OBJECT (el, "warning: %s", __dbg);			\
  gst_element_message_full (GST_ELEMENT(el), GST_MESSAGE_WARNING,	\
    GST_ ## domain ## _ERROR, GST_ ## domain ## _ERROR_ ## code,	\
  __txt, __dbg, __FILE__, GST_FUNCTION, __LINE__);			\
} G_STMT_END

/**
 * GST_ELEMENT_INFO:
 * @el:     the element that generates the information
 * @domain: like CORE, LIBRARY, RESOURCE or STREAM (see #gstreamer-GstGError)
 * @code:   error code defined for that domain (see #gstreamer-GstGError)
 * @text:   the message to display (format string and args enclosed in
            parentheses)
 * @debug:  debugging information for the message (format string and args
            enclosed in parentheses)
 *
 * Utility function that elements can use in case they want to inform
 * the application of something noteworthy that is not an error.
 * The pipeline will post a warning message and the
 * application will be informed.
 *
 * Since: 0.10.12
 */
#define GST_ELEMENT_INFO(el, domain, code, text, debug)			\
G_STMT_START {								\
  gchar *__txt = _gst_element_error_printf text;			\
  gchar *__dbg = _gst_element_error_printf debug;			\
  if (__txt)								\
    GST_INFO_OBJECT (el, "info: %s", __txt);				\
  if (__dbg)								\
    GST_INFO_OBJECT (el, "info: %s", __dbg);				\
  gst_element_message_full (GST_ELEMENT(el), GST_MESSAGE_INFO,		\
    GST_ ## domain ## _ERROR, GST_ ## domain ## _ERROR_ ## code,	\
  __txt, __dbg, __FILE__, GST_FUNCTION, __LINE__);			\
} G_STMT_END

/* the state change mutexes and conds */
/**
 * GST_STATE_GET_LOCK:
 * @elem:   a #GstElement
 *
 * Get a reference to the state lock of @elem.
 * This lock is used by the core.  It is taken while getting or setting
 * the state, during state changes, and while finalizing.
 */
#define GST_STATE_GET_LOCK(elem)               (GST_ELEMENT_CAST(elem)->state_lock)
/**
 * GST_STATE_GET_COND:
 * @elem: a #GstElement
 *
 * Get the conditional used to signal the completion of a state change.
 */
#define GST_STATE_GET_COND(elem)               (GST_ELEMENT_CAST(elem)->state_cond)

#define GST_STATE_LOCK(elem)                   g_static_rec_mutex_lock(GST_STATE_GET_LOCK(elem))
#define GST_STATE_TRYLOCK(elem)                g_static_rec_mutex_trylock(GST_STATE_GET_LOCK(elem))
#define GST_STATE_UNLOCK(elem)                 g_static_rec_mutex_unlock(GST_STATE_GET_LOCK(elem))
#define GST_STATE_UNLOCK_FULL(elem)            g_static_rec_mutex_unlock_full(GST_STATE_GET_LOCK(elem))
#define GST_STATE_LOCK_FULL(elem,t)            g_static_rec_mutex_lock_full(GST_STATE_GET_LOCK(elem), t)
#define GST_STATE_WAIT(elem)                   g_cond_wait (GST_STATE_GET_COND (elem), \
							GST_OBJECT_GET_LOCK (elem))
#define GST_STATE_TIMED_WAIT(elem, timeval)    g_cond_timed_wait (GST_STATE_GET_COND (elem), \
							GST_OBJECT_GET_LOCK (elem), timeval)
#define GST_STATE_SIGNAL(elem)                 g_cond_signal (GST_STATE_GET_COND (elem));
#define GST_STATE_BROADCAST(elem)              g_cond_broadcast (GST_STATE_GET_COND (elem));

/**
 * GstElement:
 * @state_lock: Used to serialize execution of gst_element_set_state()
 * @state_cond: Used to signal completion of a state change
 * @state_cookie: Used to detect concurrent execution of
 * gst_element_set_state() and gst_element_get_state()
 * @current_state: the current state of an element
 * @next_state: the next state of an element, can be #GST_STATE_VOID_PENDING if
 * the element is in the correct state.
 * @pending_state: the final state the element should go to, can be
 * #GST_STATE_VOID_PENDING if the element is in the correct state
 * @last_return: the last return value of an element state change
 * @bus: the bus of the element. This bus is provided to the element by the
 * parent element or the application. A #GstPipeline has a bus of its own.
 * @clock: the clock of the element. This clock is usually provided by to the
 * element by the toplevel #GstPipeline.
 * @base_time: the time of the clock right before the element is set to
 * PLAYING. Subtracting @base_time from the current clock time in the PLAYING
 * state will yield the stream time.
 * @numpads: number of pads of the element, includes both source and sink pads.
 * @pads: list of pads
 * @numsrcpads: number of source pads of the element.
 * @srcpads: list of source pads
 * @numsinkpads: number of sink pads of the element.
 * @sinkpads: list of sink pads
 * @pads_cookie: updated whenever the a pad is added or removed
 *
 * GStreamer element abstract base class.
 */
struct _GstElement
{
  GstObject		object;

  /*< public >*/ /* with LOCK */
  GStaticRecMutex      *state_lock;

  /* element state */
  GCond                *state_cond;
  guint32		state_cookie;
  GstState              current_state;
  GstState              next_state;
  GstState              pending_state;
  GstStateChangeReturn  last_return;

  GstBus	       *bus;

  /* allocated clock */
  GstClock	       *clock;
  GstClockTimeDiff	base_time; /* NULL/READY: 0 - PAUSED: current time - PLAYING: difference to clock */

  /* element pads, these lists can only be iterated while holding
   * the LOCK or checking the cookie after each LOCK. */
  guint16               numpads;
  GList                *pads;
  guint16               numsrcpads;
  GList                *srcpads;
  guint16               numsinkpads;
  GList                *sinkpads;
  guint32               pads_cookie;

  /*< private >*/
  union {
    struct {
      /* state set by application */
      GstState              target_state;
    } ABI;
    /* adding + 0 to mark ABI change to be undone later */
    gpointer _gst_reserved[GST_PADDING + 0];
  } abidata;
};

/**
 * GstElementClass:
 * @parent_class: the parent class structure
 * @details: #GstElementDetails for elements of this class
 * @elementfactory: the #GstElementFactory that creates these elements
 * @padtemplates: a #GList of #GstPadTemplate
 * @numpadtemplates: the number of padtemplates
 * @pad_templ_cookie: changed whenever the padtemplates change
 * @request_new_pad: called when a new pad is requested
 * @release_pad: called when a request pad is to be released
 * @get_state: get the state of the element
 * @set_state: set a new state on the element
 * @change_state: called by @set_state to perform an incremental state change
 * @set_bus: set a #GstBus on the element
 * @provide_clock: gets the #GstClock provided by the element
 * @set_clock: set the #GstClock on the element
 * @get_index: set a #GstIndex on the element
 * @set_index: get the #GstIndex of an element
 * @send_event: send a #GstEvent to the element
 * @get_query_types: get the supported #GstQueryType of this element
 * @query: perform a #GstQuery on the element
 *
 * GStreamer element class. Override the vmethods to implement the element
 * functionality.
 */
struct _GstElementClass
{
  GstObjectClass         parent_class;

  /*< public >*/
  /* the element details */
  GstElementDetails	 details;

  /* factory that the element was created from */
  GstElementFactory	*elementfactory;

  /* templates for our pads */
  GList                 *padtemplates;
  gint                   numpadtemplates;
  guint32                pad_templ_cookie;

  /*< private >*/
  /* signal callbacks */
  void (*pad_added)	(GstElement *element, GstPad *pad);
  void (*pad_removed)	(GstElement *element, GstPad *pad);
  void (*no_more_pads)	(GstElement *element);

  /*< public >*/
  /* virtual methods for subclasses */

  /* request/release pads */
  GstPad*		(*request_new_pad)	(GstElement *element, GstPadTemplate *templ, const gchar* name);
  void			(*release_pad)		(GstElement *element, GstPad *pad);

  /* state changes */
  GstStateChangeReturn (*get_state)		(GstElement * element, GstState * state,
						 GstState * pending, GstClockTime timeout);
  GstStateChangeReturn (*set_state)		(GstElement *element, GstState state);
  GstStateChangeReturn (*change_state)		(GstElement *element, GstStateChange transition);

  /* bus */
  void			(*set_bus)		(GstElement * element, GstBus * bus);

  /* set/get clocks */
  GstClock*		(*provide_clock)	(GstElement *element);
  gboolean		(*set_clock)		(GstElement *element, GstClock *clock);

  /* index */
  GstIndex*		(*get_index)		(GstElement *element);
  void			(*set_index)		(GstElement *element, GstIndex *index);

  /* query functions */
  gboolean		(*send_event)		(GstElement *element, GstEvent *event);

  const GstQueryType*	(*get_query_types)	(GstElement *element);
  gboolean		(*query)		(GstElement *element, GstQuery *query);

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

/* element class pad templates */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_class_add_pad_template	(GstElementClass *klass, GstPadTemplate *templ);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstPadTemplate*		gst_element_class_get_pad_template	(GstElementClass *element_class, const gchar *name);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GList*                  gst_element_class_get_pad_template_list	(GstElementClass *element_class);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_class_set_details		(GstElementClass *klass, const GstElementDetails *details);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_class_set_details_simple	(GstElementClass *klass,
			                                    	 const gchar     *longname,
			                                    	 const gchar     *classification,
			                                    	 const gchar     *description,
			                                         const gchar     *author);

/* element instance */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GType			gst_element_get_type		(void);

/* basic name and parentage stuff from GstObject */

/**
 * gst_element_get_name:
 * @elem: a #GstElement to set the name of.
 *
 * Gets the name of the element.
 */
#define			gst_element_get_name(elem)	gst_object_get_name(GST_OBJECT_CAST(elem))

/**
 * gst_element_set_name:
 * @elem: a #GstElement to set the name of.
 * @name: the new name
 *
 * Sets the name of the element, getting rid of the old name if there was one.
 */
#define			gst_element_set_name(elem,name)	gst_object_set_name(GST_OBJECT_CAST(elem),name)

/**
 * gst_element_get_parent:
 * @elem: a #GstElement to get the parent of.
 *
 * Gets the parent of an element.
 */
#define			gst_element_get_parent(elem)	gst_object_get_parent(GST_OBJECT_CAST(elem))

/**
 * gst_element_set_parent:
 * @elem: a #GstElement to set the parent of.
 * @parent: the new parent #GstObject of the element.
 *
 * Sets the parent of an element.
 */
#define			gst_element_set_parent(elem,parent)	gst_object_set_parent(GST_OBJECT_CAST(elem),parent)

/* clocking */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_requires_clock	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_provides_clock	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstClock*		gst_element_provide_clock	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstClock*		gst_element_get_clock		(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_set_clock		(GstElement *element, GstClock *clock);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_set_base_time	(GstElement *element, GstClockTime time);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstClockTime		gst_element_get_base_time	(GstElement *element);

/* indexes */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_is_indexable	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_set_index		(GstElement *element, GstIndex *index);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstIndex*		gst_element_get_index		(GstElement *element);

/* bus */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_set_bus		(GstElement * element, GstBus * bus);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstBus *		gst_element_get_bus		(GstElement * element);

/* pad management */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_add_pad		(GstElement *element, GstPad *pad);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_remove_pad		(GstElement *element, GstPad *pad);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_no_more_pads	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstPad*			gst_element_get_pad		(GstElement *element, const gchar *name);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstPad*			gst_element_get_static_pad	(GstElement *element, const gchar *name);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstPad*			gst_element_get_request_pad	(GstElement *element, const gchar *name);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_release_request_pad	(GstElement *element, GstPad *pad);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstIterator *		gst_element_iterate_pads	(GstElement * element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstIterator *		gst_element_iterate_src_pads	(GstElement * element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstIterator *		gst_element_iterate_sink_pads	(GstElement * element);

/* event/query/format stuff */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_send_event		(GstElement *element, GstEvent *event);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_seek		(GstElement *element, gdouble rate,
							 GstFormat format, GstSeekFlags flags,
							 GstSeekType cur_type, gint64 cur,
							 GstSeekType stop_type, gint64 stop);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

G_CONST_RETURN GstQueryType*
			gst_element_get_query_types	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_query		(GstElement *element, GstQuery *query);

/* messages */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_post_message	(GstElement * element, GstMessage * message);

/* error handling */
/* gcc versions < 3.3 warn about NULL being passed as format to printf */
#if (defined(GST_USING_PRINTF_EXTENSION) || !defined(__GNUC__) || (__GNUC__ < 3) || (__GNUC__ == 3 && __GNUC_MINOR__ < 3))
gchar *			_gst_element_error_printf	(const gchar *format, ...);
#else
gchar *			_gst_element_error_printf	(const gchar *format, ...) G_GNUC_PRINTF (1, 2);
#endif
void			gst_element_message_full	(GstElement * element, GstMessageType type,
							 GQuark domain, gint code, gchar * text,
							 gchar * debug, const gchar * file,
							 const gchar * function, gint line);

/* state management */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_is_locked_state	(GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_set_locked_state	(GstElement *element, gboolean locked_state);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

gboolean		gst_element_sync_state_with_parent (GstElement *element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


GstStateChangeReturn	gst_element_get_state		(GstElement * element,
							 GstState * state,
							 GstState * pending,
							 GstClockTime timeout);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstStateChangeReturn	gst_element_set_state		(GstElement *element, GstState state);
#ifdef __SYMBIAN32__
IMPORT_C
#endif


void			gst_element_abort_state		(GstElement * element);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstStateChangeReturn    gst_element_change_state        (GstElement * element,
		                                         GstStateChange transition);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstStateChangeReturn	gst_element_continue_state	(GstElement * element,
                                                         GstStateChangeReturn ret);
#ifdef __SYMBIAN32__
IMPORT_C
#endif

void			gst_element_lost_state	        (GstElement * element);

/* factory management */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

GstElementFactory*	gst_element_get_factory		(GstElement *element);

G_END_DECLS

#endif /* __GST_ELEMENT_H__ */
