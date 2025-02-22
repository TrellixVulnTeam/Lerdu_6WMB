// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_XOVERLAY_P_H
#define _GSTREAMERMM_XOVERLAY_P_H


#include <glibmm/private/interface_p.h>

namespace Gst
{

class XOverlay_Class : public Glib::Interface_Class
{
public:
  typedef XOverlay CppObjectType;
  typedef GstXOverlay BaseObjectType;
  typedef GstXOverlayClass BaseClassType;
  typedef Glib::Interface_Class CppClassParent;

  friend class XOverlay;

  const Glib::Interface_Class& init();

  static void iface_init_function(void* g_iface, void* iface_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.

  //Callbacks (virtual functions):
  static void set_xwindow_id_vfunc_callback(GstXOverlay* self, gulong xwindow_id);
  static void expose_vfunc_callback(GstXOverlay* self);
  static void handle_events_vfunc_callback(GstXOverlay* self, gboolean handle_events);
};


} // namespace Gst


#endif /* _GSTREAMERMM_XOVERLAY_P_H */

