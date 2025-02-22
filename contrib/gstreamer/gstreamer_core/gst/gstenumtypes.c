
/* Generated data (by glib-mkenums) */

#include "gst_private.h"
#include <gst/gst.h>
#define C_ENUM(v) ((gint) v)
#define C_FLAGS(v) ((guint) v)
 

/* enumerations from "gstobject.h" */
static void
register_gst_object_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_OBJECT_DISPOSING), "GST_OBJECT_DISPOSING", "disposing" },
    { C_FLAGS(GST_OBJECT_FLOATING), "GST_OBJECT_FLOATING", "floating" },
    { C_FLAGS(GST_OBJECT_FLAG_LAST), "GST_OBJECT_FLAG_LAST", "flag-last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstObjectFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_object_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_object_flags, &id);
  return id;
}

/* enumerations from "gstbin.h" */
static void
register_gst_bin_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_BIN_FLAG_LAST), "GST_BIN_FLAG_LAST", "last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstBinFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_bin_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_bin_flags, &id);
  return id;
}

/* enumerations from "gstbuffer.h" */
static void
register_gst_buffer_flag (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_BUFFER_FLAG_READONLY), "GST_BUFFER_FLAG_READONLY", "readonly" },
    { C_FLAGS(GST_BUFFER_FLAG_PREROLL), "GST_BUFFER_FLAG_PREROLL", "preroll" },
    { C_FLAGS(GST_BUFFER_FLAG_DISCONT), "GST_BUFFER_FLAG_DISCONT", "discont" },
    { C_FLAGS(GST_BUFFER_FLAG_IN_CAPS), "GST_BUFFER_FLAG_IN_CAPS", "in-caps" },
    { C_FLAGS(GST_BUFFER_FLAG_GAP), "GST_BUFFER_FLAG_GAP", "gap" },
    { C_FLAGS(GST_BUFFER_FLAG_DELTA_UNIT), "GST_BUFFER_FLAG_DELTA_UNIT", "delta-unit" },
    { C_FLAGS(GST_BUFFER_FLAG_LAST), "GST_BUFFER_FLAG_LAST", "last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstBufferFlag", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_buffer_flag_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_buffer_flag, &id);
  return id;
}
static void
register_gst_buffer_copy_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_BUFFER_COPY_FLAGS), "GST_BUFFER_COPY_FLAGS", "flags" },
    { C_FLAGS(GST_BUFFER_COPY_TIMESTAMPS), "GST_BUFFER_COPY_TIMESTAMPS", "timestamps" },
    { C_FLAGS(GST_BUFFER_COPY_CAPS), "GST_BUFFER_COPY_CAPS", "caps" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstBufferCopyFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_buffer_copy_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_buffer_copy_flags, &id);
  return id;
}

/* enumerations from "gstbus.h" */
static void
register_gst_bus_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_BUS_FLUSHING), "GST_BUS_FLUSHING", "flushing" },
    { C_FLAGS(GST_BUS_FLAG_LAST), "GST_BUS_FLAG_LAST", "flag-last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstBusFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_bus_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_bus_flags, &id);
  return id;
}
static void
register_gst_bus_sync_reply (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_BUS_DROP), "GST_BUS_DROP", "drop" },
    { C_ENUM(GST_BUS_PASS), "GST_BUS_PASS", "pass" },
    { C_ENUM(GST_BUS_ASYNC), "GST_BUS_ASYNC", "async" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstBusSyncReply", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_bus_sync_reply_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_bus_sync_reply, &id);
  return id;
}

/* enumerations from "gstcaps.h" */
static void
register_gst_caps_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_CAPS_FLAGS_ANY), "GST_CAPS_FLAGS_ANY", "any" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstCapsFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_caps_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_caps_flags, &id);
  return id;
}

/* enumerations from "gstclock.h" */
static void
register_gst_clock_return (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_CLOCK_OK), "GST_CLOCK_OK", "ok" },
    { C_ENUM(GST_CLOCK_EARLY), "GST_CLOCK_EARLY", "early" },
    { C_ENUM(GST_CLOCK_UNSCHEDULED), "GST_CLOCK_UNSCHEDULED", "unscheduled" },
    { C_ENUM(GST_CLOCK_BUSY), "GST_CLOCK_BUSY", "busy" },
    { C_ENUM(GST_CLOCK_BADTIME), "GST_CLOCK_BADTIME", "badtime" },
    { C_ENUM(GST_CLOCK_ERROR), "GST_CLOCK_ERROR", "error" },
    { C_ENUM(GST_CLOCK_UNSUPPORTED), "GST_CLOCK_UNSUPPORTED", "unsupported" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstClockReturn", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_clock_return_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_clock_return, &id);
  return id;
}
static void
register_gst_clock_entry_type (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_CLOCK_ENTRY_SINGLE), "GST_CLOCK_ENTRY_SINGLE", "single" },
    { C_ENUM(GST_CLOCK_ENTRY_PERIODIC), "GST_CLOCK_ENTRY_PERIODIC", "periodic" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstClockEntryType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_clock_entry_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_clock_entry_type, &id);
  return id;
}
static void
register_gst_clock_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_CLOCK_FLAG_CAN_DO_SINGLE_SYNC), "GST_CLOCK_FLAG_CAN_DO_SINGLE_SYNC", "can-do-single-sync" },
    { C_FLAGS(GST_CLOCK_FLAG_CAN_DO_SINGLE_ASYNC), "GST_CLOCK_FLAG_CAN_DO_SINGLE_ASYNC", "can-do-single-async" },
    { C_FLAGS(GST_CLOCK_FLAG_CAN_DO_PERIODIC_SYNC), "GST_CLOCK_FLAG_CAN_DO_PERIODIC_SYNC", "can-do-periodic-sync" },
    { C_FLAGS(GST_CLOCK_FLAG_CAN_DO_PERIODIC_ASYNC), "GST_CLOCK_FLAG_CAN_DO_PERIODIC_ASYNC", "can-do-periodic-async" },
    { C_FLAGS(GST_CLOCK_FLAG_CAN_SET_RESOLUTION), "GST_CLOCK_FLAG_CAN_SET_RESOLUTION", "can-set-resolution" },
    { C_FLAGS(GST_CLOCK_FLAG_CAN_SET_MASTER), "GST_CLOCK_FLAG_CAN_SET_MASTER", "can-set-master" },
    { C_FLAGS(GST_CLOCK_FLAG_LAST), "GST_CLOCK_FLAG_LAST", "last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstClockFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_clock_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_clock_flags, &id);
  return id;
}

/* enumerations from "gstdebugutils.h" */
static void
register_gst_debug_graph_details (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE), "GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE", "media-type" },
    { C_FLAGS(GST_DEBUG_GRAPH_SHOW_CAPS_DETAILS), "GST_DEBUG_GRAPH_SHOW_CAPS_DETAILS", "caps-details" },
    { C_FLAGS(GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS), "GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS", "non-default-params" },
    { C_FLAGS(GST_DEBUG_GRAPH_SHOW_STATES), "GST_DEBUG_GRAPH_SHOW_STATES", "states" },
    { C_FLAGS(GST_DEBUG_GRAPH_SHOW_ALL), "GST_DEBUG_GRAPH_SHOW_ALL", "all" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstDebugGraphDetails", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_debug_graph_details_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_debug_graph_details, &id);
  return id;
}

/* enumerations from "gstelement.h" */
static void
register_gst_state (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_STATE_VOID_PENDING), "GST_STATE_VOID_PENDING", "void-pending" },
    { C_ENUM(GST_STATE_NULL), "GST_STATE_NULL", "null" },
    { C_ENUM(GST_STATE_READY), "GST_STATE_READY", "ready" },
    { C_ENUM(GST_STATE_PAUSED), "GST_STATE_PAUSED", "paused" },
    { C_ENUM(GST_STATE_PLAYING), "GST_STATE_PLAYING", "playing" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstState", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_state_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_state, &id);
  return id;
}
static void
register_gst_state_change_return (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_STATE_CHANGE_FAILURE), "GST_STATE_CHANGE_FAILURE", "failure" },
    { C_ENUM(GST_STATE_CHANGE_SUCCESS), "GST_STATE_CHANGE_SUCCESS", "success" },
    { C_ENUM(GST_STATE_CHANGE_ASYNC), "GST_STATE_CHANGE_ASYNC", "async" },
    { C_ENUM(GST_STATE_CHANGE_NO_PREROLL), "GST_STATE_CHANGE_NO_PREROLL", "no-preroll" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstStateChangeReturn", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_state_change_return_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_state_change_return, &id);
  return id;
}
static void
register_gst_state_change (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_STATE_CHANGE_NULL_TO_READY), "GST_STATE_CHANGE_NULL_TO_READY", "null-to-ready" },
    { C_ENUM(GST_STATE_CHANGE_READY_TO_PAUSED), "GST_STATE_CHANGE_READY_TO_PAUSED", "ready-to-paused" },
    { C_ENUM(GST_STATE_CHANGE_PAUSED_TO_PLAYING), "GST_STATE_CHANGE_PAUSED_TO_PLAYING", "paused-to-playing" },
    { C_ENUM(GST_STATE_CHANGE_PLAYING_TO_PAUSED), "GST_STATE_CHANGE_PLAYING_TO_PAUSED", "playing-to-paused" },
    { C_ENUM(GST_STATE_CHANGE_PAUSED_TO_READY), "GST_STATE_CHANGE_PAUSED_TO_READY", "paused-to-ready" },
    { C_ENUM(GST_STATE_CHANGE_READY_TO_NULL), "GST_STATE_CHANGE_READY_TO_NULL", "ready-to-null" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstStateChange", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_state_change_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_state_change, &id);
  return id;
}
static void
register_gst_element_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_ELEMENT_LOCKED_STATE), "GST_ELEMENT_LOCKED_STATE", "locked-state" },
    { C_FLAGS(GST_ELEMENT_IS_SINK), "GST_ELEMENT_IS_SINK", "is-sink" },
    { C_FLAGS(GST_ELEMENT_UNPARENTING), "GST_ELEMENT_UNPARENTING", "unparenting" },
    { C_FLAGS(GST_ELEMENT_FLAG_LAST), "GST_ELEMENT_FLAG_LAST", "flag-last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstElementFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_element_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_element_flags, &id);
  return id;
}

/* enumerations from "gsterror.h" */
static void
register_gst_core_error (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_CORE_ERROR_FAILED), "GST_CORE_ERROR_FAILED", "failed" },
    { C_ENUM(GST_CORE_ERROR_TOO_LAZY), "GST_CORE_ERROR_TOO_LAZY", "too-lazy" },
    { C_ENUM(GST_CORE_ERROR_NOT_IMPLEMENTED), "GST_CORE_ERROR_NOT_IMPLEMENTED", "not-implemented" },
    { C_ENUM(GST_CORE_ERROR_STATE_CHANGE), "GST_CORE_ERROR_STATE_CHANGE", "state-change" },
    { C_ENUM(GST_CORE_ERROR_PAD), "GST_CORE_ERROR_PAD", "pad" },
    { C_ENUM(GST_CORE_ERROR_THREAD), "GST_CORE_ERROR_THREAD", "thread" },
    { C_ENUM(GST_CORE_ERROR_NEGOTIATION), "GST_CORE_ERROR_NEGOTIATION", "negotiation" },
    { C_ENUM(GST_CORE_ERROR_EVENT), "GST_CORE_ERROR_EVENT", "event" },
    { C_ENUM(GST_CORE_ERROR_SEEK), "GST_CORE_ERROR_SEEK", "seek" },
    { C_ENUM(GST_CORE_ERROR_CAPS), "GST_CORE_ERROR_CAPS", "caps" },
    { C_ENUM(GST_CORE_ERROR_TAG), "GST_CORE_ERROR_TAG", "tag" },
    { C_ENUM(GST_CORE_ERROR_MISSING_PLUGIN), "GST_CORE_ERROR_MISSING_PLUGIN", "missing-plugin" },
    { C_ENUM(GST_CORE_ERROR_CLOCK), "GST_CORE_ERROR_CLOCK", "clock" },
    { C_ENUM(GST_CORE_ERROR_DISABLED), "GST_CORE_ERROR_DISABLED", "disabled" },
    { C_ENUM(GST_CORE_ERROR_NUM_ERRORS), "GST_CORE_ERROR_NUM_ERRORS", "num-errors" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstCoreError", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_core_error_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_core_error, &id);
  return id;
}
static void
register_gst_library_error (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_LIBRARY_ERROR_FAILED), "GST_LIBRARY_ERROR_FAILED", "failed" },
    { C_ENUM(GST_LIBRARY_ERROR_TOO_LAZY), "GST_LIBRARY_ERROR_TOO_LAZY", "too-lazy" },
    { C_ENUM(GST_LIBRARY_ERROR_INIT), "GST_LIBRARY_ERROR_INIT", "init" },
    { C_ENUM(GST_LIBRARY_ERROR_SHUTDOWN), "GST_LIBRARY_ERROR_SHUTDOWN", "shutdown" },
    { C_ENUM(GST_LIBRARY_ERROR_SETTINGS), "GST_LIBRARY_ERROR_SETTINGS", "settings" },
    { C_ENUM(GST_LIBRARY_ERROR_ENCODE), "GST_LIBRARY_ERROR_ENCODE", "encode" },
    { C_ENUM(GST_LIBRARY_ERROR_NUM_ERRORS), "GST_LIBRARY_ERROR_NUM_ERRORS", "num-errors" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstLibraryError", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_library_error_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_library_error, &id);
  return id;
}
static void
register_gst_resource_error (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_RESOURCE_ERROR_FAILED), "GST_RESOURCE_ERROR_FAILED", "failed" },
    { C_ENUM(GST_RESOURCE_ERROR_TOO_LAZY), "GST_RESOURCE_ERROR_TOO_LAZY", "too-lazy" },
    { C_ENUM(GST_RESOURCE_ERROR_NOT_FOUND), "GST_RESOURCE_ERROR_NOT_FOUND", "not-found" },
    { C_ENUM(GST_RESOURCE_ERROR_BUSY), "GST_RESOURCE_ERROR_BUSY", "busy" },
    { C_ENUM(GST_RESOURCE_ERROR_OPEN_READ), "GST_RESOURCE_ERROR_OPEN_READ", "open-read" },
    { C_ENUM(GST_RESOURCE_ERROR_OPEN_WRITE), "GST_RESOURCE_ERROR_OPEN_WRITE", "open-write" },
    { C_ENUM(GST_RESOURCE_ERROR_OPEN_READ_WRITE), "GST_RESOURCE_ERROR_OPEN_READ_WRITE", "open-read-write" },
    { C_ENUM(GST_RESOURCE_ERROR_CLOSE), "GST_RESOURCE_ERROR_CLOSE", "close" },
    { C_ENUM(GST_RESOURCE_ERROR_READ), "GST_RESOURCE_ERROR_READ", "read" },
    { C_ENUM(GST_RESOURCE_ERROR_WRITE), "GST_RESOURCE_ERROR_WRITE", "write" },
    { C_ENUM(GST_RESOURCE_ERROR_SEEK), "GST_RESOURCE_ERROR_SEEK", "seek" },
    { C_ENUM(GST_RESOURCE_ERROR_SYNC), "GST_RESOURCE_ERROR_SYNC", "sync" },
    { C_ENUM(GST_RESOURCE_ERROR_SETTINGS), "GST_RESOURCE_ERROR_SETTINGS", "settings" },
    { C_ENUM(GST_RESOURCE_ERROR_NO_SPACE_LEFT), "GST_RESOURCE_ERROR_NO_SPACE_LEFT", "no-space-left" },
    { C_ENUM(GST_RESOURCE_ERROR_NUM_ERRORS), "GST_RESOURCE_ERROR_NUM_ERRORS", "num-errors" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstResourceError", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_resource_error_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_resource_error, &id);
  return id;
}
static void
register_gst_stream_error (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_STREAM_ERROR_FAILED), "GST_STREAM_ERROR_FAILED", "failed" },
    { C_ENUM(GST_STREAM_ERROR_TOO_LAZY), "GST_STREAM_ERROR_TOO_LAZY", "too-lazy" },
    { C_ENUM(GST_STREAM_ERROR_NOT_IMPLEMENTED), "GST_STREAM_ERROR_NOT_IMPLEMENTED", "not-implemented" },
    { C_ENUM(GST_STREAM_ERROR_TYPE_NOT_FOUND), "GST_STREAM_ERROR_TYPE_NOT_FOUND", "type-not-found" },
    { C_ENUM(GST_STREAM_ERROR_WRONG_TYPE), "GST_STREAM_ERROR_WRONG_TYPE", "wrong-type" },
    { C_ENUM(GST_STREAM_ERROR_CODEC_NOT_FOUND), "GST_STREAM_ERROR_CODEC_NOT_FOUND", "codec-not-found" },
    { C_ENUM(GST_STREAM_ERROR_DECODE), "GST_STREAM_ERROR_DECODE", "decode" },
    { C_ENUM(GST_STREAM_ERROR_ENCODE), "GST_STREAM_ERROR_ENCODE", "encode" },
    { C_ENUM(GST_STREAM_ERROR_DEMUX), "GST_STREAM_ERROR_DEMUX", "demux" },
    { C_ENUM(GST_STREAM_ERROR_MUX), "GST_STREAM_ERROR_MUX", "mux" },
    { C_ENUM(GST_STREAM_ERROR_FORMAT), "GST_STREAM_ERROR_FORMAT", "format" },
    { C_ENUM(GST_STREAM_ERROR_NUM_ERRORS), "GST_STREAM_ERROR_NUM_ERRORS", "num-errors" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstStreamError", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_stream_error_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_stream_error, &id);
  return id;
}

/* enumerations from "gstevent.h" */
static void
register_gst_event_type_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_EVENT_TYPE_UPSTREAM), "GST_EVENT_TYPE_UPSTREAM", "upstream" },
    { C_FLAGS(GST_EVENT_TYPE_DOWNSTREAM), "GST_EVENT_TYPE_DOWNSTREAM", "downstream" },
    { C_FLAGS(GST_EVENT_TYPE_SERIALIZED), "GST_EVENT_TYPE_SERIALIZED", "serialized" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstEventTypeFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_event_type_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_event_type_flags, &id);
  return id;
}
static void
register_gst_event_type (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_EVENT_UNKNOWN), "GST_EVENT_UNKNOWN", "unknown" },
    { C_ENUM(GST_EVENT_FLUSH_START), "GST_EVENT_FLUSH_START", "flush-start" },
    { C_ENUM(GST_EVENT_FLUSH_STOP), "GST_EVENT_FLUSH_STOP", "flush-stop" },
    { C_ENUM(GST_EVENT_EOS), "GST_EVENT_EOS", "eos" },
    { C_ENUM(GST_EVENT_NEWSEGMENT), "GST_EVENT_NEWSEGMENT", "newsegment" },
    { C_ENUM(GST_EVENT_TAG), "GST_EVENT_TAG", "tag" },
    { C_ENUM(GST_EVENT_BUFFERSIZE), "GST_EVENT_BUFFERSIZE", "buffersize" },
    { C_ENUM(GST_EVENT_QOS), "GST_EVENT_QOS", "qos" },
    { C_ENUM(GST_EVENT_SEEK), "GST_EVENT_SEEK", "seek" },
    { C_ENUM(GST_EVENT_NAVIGATION), "GST_EVENT_NAVIGATION", "navigation" },
    { C_ENUM(GST_EVENT_LATENCY), "GST_EVENT_LATENCY", "latency" },
    { C_ENUM(GST_EVENT_CUSTOM_UPSTREAM), "GST_EVENT_CUSTOM_UPSTREAM", "custom-upstream" },
    { C_ENUM(GST_EVENT_CUSTOM_DOWNSTREAM), "GST_EVENT_CUSTOM_DOWNSTREAM", "custom-downstream" },
    { C_ENUM(GST_EVENT_CUSTOM_DOWNSTREAM_OOB), "GST_EVENT_CUSTOM_DOWNSTREAM_OOB", "custom-downstream-oob" },
    { C_ENUM(GST_EVENT_CUSTOM_BOTH), "GST_EVENT_CUSTOM_BOTH", "custom-both" },
    { C_ENUM(GST_EVENT_CUSTOM_BOTH_OOB), "GST_EVENT_CUSTOM_BOTH_OOB", "custom-both-oob" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstEventType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_event_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_event_type, &id);
  return id;
}
static void
register_gst_seek_type (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_SEEK_TYPE_NONE), "GST_SEEK_TYPE_NONE", "none" },
    { C_ENUM(GST_SEEK_TYPE_CUR), "GST_SEEK_TYPE_CUR", "cur" },
    { C_ENUM(GST_SEEK_TYPE_SET), "GST_SEEK_TYPE_SET", "set" },
    { C_ENUM(GST_SEEK_TYPE_END), "GST_SEEK_TYPE_END", "end" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstSeekType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_seek_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_seek_type, &id);
  return id;
}
static void
register_gst_seek_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_SEEK_FLAG_NONE), "GST_SEEK_FLAG_NONE", "none" },
    { C_FLAGS(GST_SEEK_FLAG_FLUSH), "GST_SEEK_FLAG_FLUSH", "flush" },
    { C_FLAGS(GST_SEEK_FLAG_ACCURATE), "GST_SEEK_FLAG_ACCURATE", "accurate" },
    { C_FLAGS(GST_SEEK_FLAG_KEY_UNIT), "GST_SEEK_FLAG_KEY_UNIT", "key-unit" },
    { C_FLAGS(GST_SEEK_FLAG_SEGMENT), "GST_SEEK_FLAG_SEGMENT", "segment" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstSeekFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_seek_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_seek_flags, &id);
  return id;
}

/* enumerations from "gstformat.h" */
static void
register_gst_format (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_FORMAT_UNDEFINED), "GST_FORMAT_UNDEFINED", "undefined" },
    { C_ENUM(GST_FORMAT_DEFAULT), "GST_FORMAT_DEFAULT", "default" },
    { C_ENUM(GST_FORMAT_BYTES), "GST_FORMAT_BYTES", "bytes" },
    { C_ENUM(GST_FORMAT_TIME), "GST_FORMAT_TIME", "time" },
    { C_ENUM(GST_FORMAT_BUFFERS), "GST_FORMAT_BUFFERS", "buffers" },
    { C_ENUM(GST_FORMAT_PERCENT), "GST_FORMAT_PERCENT", "percent" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstFormat", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_format_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_format, &id);
  return id;
}

/* enumerations from "gstindex.h" */
static void
register_gst_index_certainty (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_INDEX_UNKNOWN), "GST_INDEX_UNKNOWN", "unknown" },
    { C_ENUM(GST_INDEX_CERTAIN), "GST_INDEX_CERTAIN", "certain" },
    { C_ENUM(GST_INDEX_FUZZY), "GST_INDEX_FUZZY", "fuzzy" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstIndexCertainty", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_index_certainty_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_index_certainty, &id);
  return id;
}
static void
register_gst_index_entry_type (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_INDEX_ENTRY_ID), "GST_INDEX_ENTRY_ID", "id" },
    { C_ENUM(GST_INDEX_ENTRY_ASSOCIATION), "GST_INDEX_ENTRY_ASSOCIATION", "association" },
    { C_ENUM(GST_INDEX_ENTRY_OBJECT), "GST_INDEX_ENTRY_OBJECT", "object" },
    { C_ENUM(GST_INDEX_ENTRY_FORMAT), "GST_INDEX_ENTRY_FORMAT", "format" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstIndexEntryType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_index_entry_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_index_entry_type, &id);
  return id;
}
static void
register_gst_index_lookup_method (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_INDEX_LOOKUP_EXACT), "GST_INDEX_LOOKUP_EXACT", "exact" },
    { C_ENUM(GST_INDEX_LOOKUP_BEFORE), "GST_INDEX_LOOKUP_BEFORE", "before" },
    { C_ENUM(GST_INDEX_LOOKUP_AFTER), "GST_INDEX_LOOKUP_AFTER", "after" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstIndexLookupMethod", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_index_lookup_method_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_index_lookup_method, &id);
  return id;
}
static void
register_gst_assoc_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_ASSOCIATION_FLAG_NONE), "GST_ASSOCIATION_FLAG_NONE", "none" },
    { C_FLAGS(GST_ASSOCIATION_FLAG_KEY_UNIT), "GST_ASSOCIATION_FLAG_KEY_UNIT", "key-unit" },
    { C_FLAGS(GST_ASSOCIATION_FLAG_DELTA_UNIT), "GST_ASSOCIATION_FLAG_DELTA_UNIT", "delta-unit" },
    { C_FLAGS(GST_ASSOCIATION_FLAG_LAST), "GST_ASSOCIATION_FLAG_LAST", "last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstAssocFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_assoc_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_assoc_flags, &id);
  return id;
}
static void
register_gst_index_resolver_method (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_INDEX_RESOLVER_CUSTOM), "GST_INDEX_RESOLVER_CUSTOM", "custom" },
    { C_ENUM(GST_INDEX_RESOLVER_GTYPE), "GST_INDEX_RESOLVER_GTYPE", "gtype" },
    { C_ENUM(GST_INDEX_RESOLVER_PATH), "GST_INDEX_RESOLVER_PATH", "path" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstIndexResolverMethod", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_index_resolver_method_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_index_resolver_method, &id);
  return id;
}
static void
register_gst_index_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_INDEX_WRITABLE), "GST_INDEX_WRITABLE", "writable" },
    { C_FLAGS(GST_INDEX_READABLE), "GST_INDEX_READABLE", "readable" },
    { C_FLAGS(GST_INDEX_FLAG_LAST), "GST_INDEX_FLAG_LAST", "flag-last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstIndexFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_index_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_index_flags, &id);
  return id;
}

/* enumerations from "gstinfo.h" */
static void
register_gst_debug_level (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_LEVEL_NONE), "GST_LEVEL_NONE", "none" },
    { C_ENUM(GST_LEVEL_ERROR), "GST_LEVEL_ERROR", "error" },
    { C_ENUM(GST_LEVEL_WARNING), "GST_LEVEL_WARNING", "warning" },
    { C_ENUM(GST_LEVEL_INFO), "GST_LEVEL_INFO", "info" },
    { C_ENUM(GST_LEVEL_DEBUG), "GST_LEVEL_DEBUG", "debug" },
    { C_ENUM(GST_LEVEL_LOG), "GST_LEVEL_LOG", "log" },
    { C_ENUM(GST_LEVEL_COUNT), "GST_LEVEL_COUNT", "count" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstDebugLevel", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_debug_level_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_debug_level, &id);
  return id;
}
static void
register_gst_debug_color_flags (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_DEBUG_FG_BLACK), "GST_DEBUG_FG_BLACK", "fg-black" },
    { C_ENUM(GST_DEBUG_FG_RED), "GST_DEBUG_FG_RED", "fg-red" },
    { C_ENUM(GST_DEBUG_FG_GREEN), "GST_DEBUG_FG_GREEN", "fg-green" },
    { C_ENUM(GST_DEBUG_FG_YELLOW), "GST_DEBUG_FG_YELLOW", "fg-yellow" },
    { C_ENUM(GST_DEBUG_FG_BLUE), "GST_DEBUG_FG_BLUE", "fg-blue" },
    { C_ENUM(GST_DEBUG_FG_MAGENTA), "GST_DEBUG_FG_MAGENTA", "fg-magenta" },
    { C_ENUM(GST_DEBUG_FG_CYAN), "GST_DEBUG_FG_CYAN", "fg-cyan" },
    { C_ENUM(GST_DEBUG_FG_WHITE), "GST_DEBUG_FG_WHITE", "fg-white" },
    { C_ENUM(GST_DEBUG_BG_BLACK), "GST_DEBUG_BG_BLACK", "bg-black" },
    { C_ENUM(GST_DEBUG_BG_RED), "GST_DEBUG_BG_RED", "bg-red" },
    { C_ENUM(GST_DEBUG_BG_GREEN), "GST_DEBUG_BG_GREEN", "bg-green" },
    { C_ENUM(GST_DEBUG_BG_YELLOW), "GST_DEBUG_BG_YELLOW", "bg-yellow" },
    { C_ENUM(GST_DEBUG_BG_BLUE), "GST_DEBUG_BG_BLUE", "bg-blue" },
    { C_ENUM(GST_DEBUG_BG_MAGENTA), "GST_DEBUG_BG_MAGENTA", "bg-magenta" },
    { C_ENUM(GST_DEBUG_BG_CYAN), "GST_DEBUG_BG_CYAN", "bg-cyan" },
    { C_ENUM(GST_DEBUG_BG_WHITE), "GST_DEBUG_BG_WHITE", "bg-white" },
    { C_ENUM(GST_DEBUG_BOLD), "GST_DEBUG_BOLD", "bold" },
    { C_ENUM(GST_DEBUG_UNDERLINE), "GST_DEBUG_UNDERLINE", "underline" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstDebugColorFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_debug_color_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_debug_color_flags, &id);
  return id;
}

/* enumerations from "gstiterator.h" */
static void
register_gst_iterator_result (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_ITERATOR_DONE), "GST_ITERATOR_DONE", "done" },
    { C_ENUM(GST_ITERATOR_OK), "GST_ITERATOR_OK", "ok" },
    { C_ENUM(GST_ITERATOR_RESYNC), "GST_ITERATOR_RESYNC", "resync" },
    { C_ENUM(GST_ITERATOR_ERROR), "GST_ITERATOR_ERROR", "error" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstIteratorResult", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_iterator_result_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_iterator_result, &id);
  return id;
}
static void
register_gst_iterator_item (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_ITERATOR_ITEM_SKIP), "GST_ITERATOR_ITEM_SKIP", "skip" },
    { C_ENUM(GST_ITERATOR_ITEM_PASS), "GST_ITERATOR_ITEM_PASS", "pass" },
    { C_ENUM(GST_ITERATOR_ITEM_END), "GST_ITERATOR_ITEM_END", "end" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstIteratorItem", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_iterator_item_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_iterator_item, &id);
  return id;
}

/* enumerations from "gstmessage.h" */
static void
register_gst_message_type (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_MESSAGE_UNKNOWN), "GST_MESSAGE_UNKNOWN", "unknown" },
    { C_FLAGS(GST_MESSAGE_EOS), "GST_MESSAGE_EOS", "eos" },
    { C_FLAGS(GST_MESSAGE_ERROR), "GST_MESSAGE_ERROR", "error" },
    { C_FLAGS(GST_MESSAGE_WARNING), "GST_MESSAGE_WARNING", "warning" },
    { C_FLAGS(GST_MESSAGE_INFO), "GST_MESSAGE_INFO", "info" },
    { C_FLAGS(GST_MESSAGE_TAG), "GST_MESSAGE_TAG", "tag" },
    { C_FLAGS(GST_MESSAGE_BUFFERING), "GST_MESSAGE_BUFFERING", "buffering" },
    { C_FLAGS(GST_MESSAGE_STATE_CHANGED), "GST_MESSAGE_STATE_CHANGED", "state-changed" },
    { C_FLAGS(GST_MESSAGE_STATE_DIRTY), "GST_MESSAGE_STATE_DIRTY", "state-dirty" },
    { C_FLAGS(GST_MESSAGE_STEP_DONE), "GST_MESSAGE_STEP_DONE", "step-done" },
    { C_FLAGS(GST_MESSAGE_CLOCK_PROVIDE), "GST_MESSAGE_CLOCK_PROVIDE", "clock-provide" },
    { C_FLAGS(GST_MESSAGE_CLOCK_LOST), "GST_MESSAGE_CLOCK_LOST", "clock-lost" },
    { C_FLAGS(GST_MESSAGE_NEW_CLOCK), "GST_MESSAGE_NEW_CLOCK", "new-clock" },
    { C_FLAGS(GST_MESSAGE_STRUCTURE_CHANGE), "GST_MESSAGE_STRUCTURE_CHANGE", "structure-change" },
    { C_FLAGS(GST_MESSAGE_STREAM_STATUS), "GST_MESSAGE_STREAM_STATUS", "stream-status" },
    { C_FLAGS(GST_MESSAGE_APPLICATION), "GST_MESSAGE_APPLICATION", "application" },
    { C_FLAGS(GST_MESSAGE_ELEMENT), "GST_MESSAGE_ELEMENT", "element" },
    { C_FLAGS(GST_MESSAGE_SEGMENT_START), "GST_MESSAGE_SEGMENT_START", "segment-start" },
    { C_FLAGS(GST_MESSAGE_SEGMENT_DONE), "GST_MESSAGE_SEGMENT_DONE", "segment-done" },
    { C_FLAGS(GST_MESSAGE_DURATION), "GST_MESSAGE_DURATION", "duration" },
    { C_FLAGS(GST_MESSAGE_LATENCY), "GST_MESSAGE_LATENCY", "latency" },
    { C_FLAGS(GST_MESSAGE_ASYNC_START), "GST_MESSAGE_ASYNC_START", "async-start" },
    { C_FLAGS(GST_MESSAGE_ASYNC_DONE), "GST_MESSAGE_ASYNC_DONE", "async-done" },
    { C_FLAGS(GST_MESSAGE_ANY), "GST_MESSAGE_ANY", "any" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstMessageType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_message_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_message_type, &id);
  return id;
}

/* enumerations from "gstminiobject.h" */
static void
register_gst_mini_object_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_MINI_OBJECT_FLAG_READONLY), "GST_MINI_OBJECT_FLAG_READONLY", "readonly" },
    { C_FLAGS(GST_MINI_OBJECT_FLAG_LAST), "GST_MINI_OBJECT_FLAG_LAST", "last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstMiniObjectFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_mini_object_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_mini_object_flags, &id);
  return id;
}

/* enumerations from "gstpad.h" */
static void
register_gst_pad_link_return (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_PAD_LINK_OK), "GST_PAD_LINK_OK", "ok" },
    { C_ENUM(GST_PAD_LINK_WRONG_HIERARCHY), "GST_PAD_LINK_WRONG_HIERARCHY", "wrong-hierarchy" },
    { C_ENUM(GST_PAD_LINK_WAS_LINKED), "GST_PAD_LINK_WAS_LINKED", "was-linked" },
    { C_ENUM(GST_PAD_LINK_WRONG_DIRECTION), "GST_PAD_LINK_WRONG_DIRECTION", "wrong-direction" },
    { C_ENUM(GST_PAD_LINK_NOFORMAT), "GST_PAD_LINK_NOFORMAT", "noformat" },
    { C_ENUM(GST_PAD_LINK_NOSCHED), "GST_PAD_LINK_NOSCHED", "nosched" },
    { C_ENUM(GST_PAD_LINK_REFUSED), "GST_PAD_LINK_REFUSED", "refused" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstPadLinkReturn", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_pad_link_return_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_pad_link_return, &id);
  return id;
}
static void
register_gst_flow_return (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_FLOW_CUSTOM_SUCCESS), "GST_FLOW_CUSTOM_SUCCESS", "custom-success" },
    { C_ENUM(GST_FLOW_RESEND), "GST_FLOW_RESEND", "resend" },
    { C_ENUM(GST_FLOW_OK), "GST_FLOW_OK", "ok" },
    { C_ENUM(GST_FLOW_NOT_LINKED), "GST_FLOW_NOT_LINKED", "not-linked" },
    { C_ENUM(GST_FLOW_WRONG_STATE), "GST_FLOW_WRONG_STATE", "wrong-state" },
    { C_ENUM(GST_FLOW_UNEXPECTED), "GST_FLOW_UNEXPECTED", "unexpected" },
    { C_ENUM(GST_FLOW_NOT_NEGOTIATED), "GST_FLOW_NOT_NEGOTIATED", "not-negotiated" },
    { C_ENUM(GST_FLOW_ERROR), "GST_FLOW_ERROR", "error" },
    { C_ENUM(GST_FLOW_NOT_SUPPORTED), "GST_FLOW_NOT_SUPPORTED", "not-supported" },
    { C_ENUM(GST_FLOW_CUSTOM_ERROR), "GST_FLOW_CUSTOM_ERROR", "custom-error" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstFlowReturn", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_flow_return_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_flow_return, &id);
  return id;
}
static void
register_gst_activate_mode (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_ACTIVATE_NONE), "GST_ACTIVATE_NONE", "none" },
    { C_ENUM(GST_ACTIVATE_PUSH), "GST_ACTIVATE_PUSH", "push" },
    { C_ENUM(GST_ACTIVATE_PULL), "GST_ACTIVATE_PULL", "pull" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstActivateMode", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_activate_mode_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_activate_mode, &id);
  return id;
}
static void
register_gst_pad_direction (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_PAD_UNKNOWN), "GST_PAD_UNKNOWN", "unknown" },
    { C_ENUM(GST_PAD_SRC), "GST_PAD_SRC", "src" },
    { C_ENUM(GST_PAD_SINK), "GST_PAD_SINK", "sink" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstPadDirection", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_pad_direction_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_pad_direction, &id);
  return id;
}
static void
register_gst_pad_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_PAD_BLOCKED), "GST_PAD_BLOCKED", "blocked" },
    { C_FLAGS(GST_PAD_FLUSHING), "GST_PAD_FLUSHING", "flushing" },
    { C_FLAGS(GST_PAD_IN_GETCAPS), "GST_PAD_IN_GETCAPS", "in-getcaps" },
    { C_FLAGS(GST_PAD_IN_SETCAPS), "GST_PAD_IN_SETCAPS", "in-setcaps" },
    { C_FLAGS(GST_PAD_BLOCKING), "GST_PAD_BLOCKING", "blocking" },
    { C_FLAGS(GST_PAD_FLAG_LAST), "GST_PAD_FLAG_LAST", "flag-last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstPadFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_pad_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_pad_flags, &id);
  return id;
}

/* enumerations from "gstpadtemplate.h" */
static void
register_gst_pad_presence (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_PAD_ALWAYS), "GST_PAD_ALWAYS", "always" },
    { C_ENUM(GST_PAD_SOMETIMES), "GST_PAD_SOMETIMES", "sometimes" },
    { C_ENUM(GST_PAD_REQUEST), "GST_PAD_REQUEST", "request" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstPadPresence", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_pad_presence_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_pad_presence, &id);
  return id;
}
static void
register_gst_pad_template_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_PAD_TEMPLATE_FIXED), "GST_PAD_TEMPLATE_FIXED", "fixed" },
    { C_FLAGS(GST_PAD_TEMPLATE_FLAG_LAST), "GST_PAD_TEMPLATE_FLAG_LAST", "flag-last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstPadTemplateFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_pad_template_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_pad_template_flags, &id);
  return id;
}

/* enumerations from "gstpipeline.h" */
static void
register_gst_pipeline_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_PIPELINE_FLAG_FIXED_CLOCK), "GST_PIPELINE_FLAG_FIXED_CLOCK", "fixed-clock" },
    { C_FLAGS(GST_PIPELINE_FLAG_LAST), "GST_PIPELINE_FLAG_LAST", "last" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstPipelineFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_pipeline_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_pipeline_flags, &id);
  return id;
}

/* enumerations from "gstplugin.h" */
static void
register_gst_plugin_error (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_PLUGIN_ERROR_MODULE), "GST_PLUGIN_ERROR_MODULE", "module" },
    { C_ENUM(GST_PLUGIN_ERROR_DEPENDENCIES), "GST_PLUGIN_ERROR_DEPENDENCIES", "dependencies" },
    { C_ENUM(GST_PLUGIN_ERROR_NAME_MISMATCH), "GST_PLUGIN_ERROR_NAME_MISMATCH", "name-mismatch" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstPluginError", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_plugin_error_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_plugin_error, &id);
  return id;
}
static void
register_gst_plugin_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_PLUGIN_FLAG_CACHED), "GST_PLUGIN_FLAG_CACHED", "cached" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstPluginFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_plugin_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_plugin_flags, &id);
  return id;
}

/* enumerations from "gstpluginfeature.h" */
static void
register_gst_rank (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_RANK_NONE), "GST_RANK_NONE", "none" },
    { C_ENUM(GST_RANK_MARGINAL), "GST_RANK_MARGINAL", "marginal" },
    { C_ENUM(GST_RANK_SECONDARY), "GST_RANK_SECONDARY", "secondary" },
    { C_ENUM(GST_RANK_PRIMARY), "GST_RANK_PRIMARY", "primary" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstRank", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_rank_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_rank, &id);
  return id;
}

/* enumerations from "gstquery.h" */
static void
register_gst_query_type (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_QUERY_NONE), "GST_QUERY_NONE", "none" },
    { C_ENUM(GST_QUERY_POSITION), "GST_QUERY_POSITION", "position" },
    { C_ENUM(GST_QUERY_DURATION), "GST_QUERY_DURATION", "duration" },
    { C_ENUM(GST_QUERY_LATENCY), "GST_QUERY_LATENCY", "latency" },
    { C_ENUM(GST_QUERY_JITTER), "GST_QUERY_JITTER", "jitter" },
    { C_ENUM(GST_QUERY_RATE), "GST_QUERY_RATE", "rate" },
    { C_ENUM(GST_QUERY_SEEKING), "GST_QUERY_SEEKING", "seeking" },
    { C_ENUM(GST_QUERY_SEGMENT), "GST_QUERY_SEGMENT", "segment" },
    { C_ENUM(GST_QUERY_CONVERT), "GST_QUERY_CONVERT", "convert" },
    { C_ENUM(GST_QUERY_FORMATS), "GST_QUERY_FORMATS", "formats" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstQueryType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_query_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_query_type, &id);
  return id;
}

/* enumerations from "gsttaglist.h" */
static void
register_gst_tag_merge_mode (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_TAG_MERGE_UNDEFINED), "GST_TAG_MERGE_UNDEFINED", "undefined" },
    { C_ENUM(GST_TAG_MERGE_REPLACE_ALL), "GST_TAG_MERGE_REPLACE_ALL", "replace-all" },
    { C_ENUM(GST_TAG_MERGE_REPLACE), "GST_TAG_MERGE_REPLACE", "replace" },
    { C_ENUM(GST_TAG_MERGE_APPEND), "GST_TAG_MERGE_APPEND", "append" },
    { C_ENUM(GST_TAG_MERGE_PREPEND), "GST_TAG_MERGE_PREPEND", "prepend" },
    { C_ENUM(GST_TAG_MERGE_KEEP), "GST_TAG_MERGE_KEEP", "keep" },
    { C_ENUM(GST_TAG_MERGE_KEEP_ALL), "GST_TAG_MERGE_KEEP_ALL", "keep-all" },
    { C_ENUM(GST_TAG_MERGE_COUNT), "GST_TAG_MERGE_COUNT", "count" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstTagMergeMode", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_tag_merge_mode_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_tag_merge_mode, &id);
  return id;
}
static void
register_gst_tag_flag (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_TAG_FLAG_UNDEFINED), "GST_TAG_FLAG_UNDEFINED", "undefined" },
    { C_ENUM(GST_TAG_FLAG_META), "GST_TAG_FLAG_META", "meta" },
    { C_ENUM(GST_TAG_FLAG_ENCODED), "GST_TAG_FLAG_ENCODED", "encoded" },
    { C_ENUM(GST_TAG_FLAG_DECODED), "GST_TAG_FLAG_DECODED", "decoded" },
    { C_ENUM(GST_TAG_FLAG_COUNT), "GST_TAG_FLAG_COUNT", "count" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstTagFlag", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_tag_flag_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_tag_flag, &id);
  return id;
}

/* enumerations from "gsttask.h" */
static void
register_gst_task_state (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_TASK_STARTED), "GST_TASK_STARTED", "started" },
    { C_ENUM(GST_TASK_STOPPED), "GST_TASK_STOPPED", "stopped" },
    { C_ENUM(GST_TASK_PAUSED), "GST_TASK_PAUSED", "paused" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstTaskState", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_task_state_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_task_state, &id);
  return id;
}

/* enumerations from "gsttrace.h" */
static void
register_gst_alloc_trace_flags (GType* id)
{
  static const GFlagsValue values[] = {
    { C_FLAGS(GST_ALLOC_TRACE_LIVE), "GST_ALLOC_TRACE_LIVE", "live" },
    { C_FLAGS(GST_ALLOC_TRACE_MEM_LIVE), "GST_ALLOC_TRACE_MEM_LIVE", "mem-live" },
    { 0, NULL, NULL }
  };
  *id = g_flags_register_static ("GstAllocTraceFlags", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_alloc_trace_flags_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_alloc_trace_flags, &id);
  return id;
}

/* enumerations from "gsttypefind.h" */
static void
register_gst_type_find_probability (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_TYPE_FIND_MINIMUM), "GST_TYPE_FIND_MINIMUM", "minimum" },
    { C_ENUM(GST_TYPE_FIND_POSSIBLE), "GST_TYPE_FIND_POSSIBLE", "possible" },
    { C_ENUM(GST_TYPE_FIND_LIKELY), "GST_TYPE_FIND_LIKELY", "likely" },
    { C_ENUM(GST_TYPE_FIND_NEARLY_CERTAIN), "GST_TYPE_FIND_NEARLY_CERTAIN", "nearly-certain" },
    { C_ENUM(GST_TYPE_FIND_MAXIMUM), "GST_TYPE_FIND_MAXIMUM", "maximum" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstTypeFindProbability", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_type_find_probability_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_type_find_probability, &id);
  return id;
}

/* enumerations from "gsturi.h" */
static void
register_gst_uri_type (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_URI_UNKNOWN), "GST_URI_UNKNOWN", "unknown" },
    { C_ENUM(GST_URI_SINK), "GST_URI_SINK", "sink" },
    { C_ENUM(GST_URI_SRC), "GST_URI_SRC", "src" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstURIType", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_uri_type_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_uri_type, &id);
  return id;
}

/* enumerations from "gstparse.h" */
static void
register_gst_parse_error (GType* id)
{
  static const GEnumValue values[] = {
    { C_ENUM(GST_PARSE_ERROR_SYNTAX), "GST_PARSE_ERROR_SYNTAX", "syntax" },
    { C_ENUM(GST_PARSE_ERROR_NO_SUCH_ELEMENT), "GST_PARSE_ERROR_NO_SUCH_ELEMENT", "no-such-element" },
    { C_ENUM(GST_PARSE_ERROR_NO_SUCH_PROPERTY), "GST_PARSE_ERROR_NO_SUCH_PROPERTY", "no-such-property" },
    { C_ENUM(GST_PARSE_ERROR_LINK), "GST_PARSE_ERROR_LINK", "link" },
    { C_ENUM(GST_PARSE_ERROR_COULD_NOT_SET_PROPERTY), "GST_PARSE_ERROR_COULD_NOT_SET_PROPERTY", "could-not-set-property" },
    { C_ENUM(GST_PARSE_ERROR_EMPTY_BIN), "GST_PARSE_ERROR_EMPTY_BIN", "empty-bin" },
    { C_ENUM(GST_PARSE_ERROR_EMPTY), "GST_PARSE_ERROR_EMPTY", "empty" },
    { 0, NULL, NULL }
  };
  *id = g_enum_register_static ("GstParseError", values);
}
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GType
gst_parse_error_get_type (void)
{
  static GType id;
  static GOnce once = G_ONCE_INIT;

  g_once (&once, (GThreadFunc)register_gst_parse_error, &id);
  return id;
}

/* Generated data ends here */

