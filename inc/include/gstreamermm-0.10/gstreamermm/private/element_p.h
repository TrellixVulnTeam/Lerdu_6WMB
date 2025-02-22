// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_ELEMENT_P_H
#define _GSTREAMERMM_ELEMENT_P_H


#include <glibmm/class.h>

namespace Gst
{

class Element_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Element CppObjectType;
  typedef GstElement BaseObjectType;
  typedef GstElementClass BaseClassType;
  typedef Gst::Object_Class CppClassParent;
  typedef GstObjectClass BaseClassParent;

  friend class Element;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();


  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.
  static void no_more_pads_callback(GstElement* self);
  static void pad_added_callback(GstElement* self, GstPad* p0);
  static void pad_removed_callback(GstElement* self, GstPad* p0);

  //Callbacks (virtual functions):
  static GstPad* request_new_pad_vfunc_callback(GstElement* self, GstPadTemplate* templ, const gchar* name);
  static void release_pad_vfunc_callback(GstElement* self, GstPad* pad);
  static GstStateChangeReturn get_state_vfunc_callback(GstElement* self, GstState* state, GstState* pending, GstClockTime timeout);
  static GstStateChangeReturn set_state_vfunc_callback(GstElement* self, GstState state);
  static GstStateChangeReturn change_state_vfunc_callback(GstElement* self, GstStateChange transition);
  static void set_bus_vfunc_callback(GstElement* self, GstBus* bus);
  static GstClock* provide_clock_vfunc_callback(GstElement* self);
  static GstIndex* get_index_vfunc_callback(GstElement* self);
  static void set_index_vfunc_callback(GstElement* self, GstIndex* index);
  static gboolean send_event_vfunc_callback(GstElement* self, GstEvent* event);
  static gboolean query_vfunc_callback(GstElement* self, GstQuery* query);
  static gboolean set_clock_vfunc_callback(GstElement* self, GstClock* clock);
  static const GstQueryType* get_query_types_vfunc_callback(GstElement* self);
  };


} // namespace Gst

#include <gstreamermm/private/object_p.h>


#endif /* _GSTREAMERMM_ELEMENT_P_H */

