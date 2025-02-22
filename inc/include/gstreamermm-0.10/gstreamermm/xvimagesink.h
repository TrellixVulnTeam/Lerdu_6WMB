// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_XVIMAGESINK_H
#define _GSTREAMERMM_XVIMAGESINK_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/videosink.h>
#include <gstreamermm/interface.h>
#include <gstreamermm/navigation.h>
#include <gstreamermm/xoverlay.h>
#include <gstreamermm/colorbalance.h>
#include <gstreamermm/propertyprobe.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstXvImageSink GstXvImageSink;
typedef struct _GstXvImageSinkClass GstXvImageSinkClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class XvImageSink_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the xvimagesink plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class XvImageSink
: public Gst::VideoSink, public Gst::Interface, public Gst::Navigation, public Gst::XOverlay, public Gst::ColorBalance, public Gst::PropertyProbe
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef XvImageSink CppObjectType;
  typedef XvImageSink_Class CppClassType;
  typedef GstXvImageSink BaseObjectType;
  typedef GstXvImageSinkClass BaseClassType;

private:  friend class XvImageSink_Class;
  static CppClassType xvimagesink_class_;

private:
  // noncopyable
  XvImageSink(const XvImageSink&);
  XvImageSink& operator=(const XvImageSink&);

protected:
  explicit XvImageSink(const Glib::ConstructParams& construct_params);
  explicit XvImageSink(GstXvImageSink* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~XvImageSink();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstXvImageSink*       gobj()       { return reinterpret_cast<GstXvImageSink*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstXvImageSink* gobj() const { return reinterpret_cast<GstXvImageSink*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstXvImageSink* gobj_copy();

private:

  
protected:
  XvImageSink();
  explicit XvImageSink(const Glib::ustring& name);

public:
  /** Creates a new xvimagesink plugin with a unique name.
   */
  
  static Glib::RefPtr<XvImageSink> create();


  /** Creates a new xvimagesink plugin with the given name.
   */
  
  static Glib::RefPtr<XvImageSink> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The contrast of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_contrast() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The contrast of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_contrast() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The brightness of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_brightness() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The brightness of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_brightness() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The hue of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_hue() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The hue of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_hue() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The saturation of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_saturation() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The saturation of the video.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_saturation() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** X Display name.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Glib::ustring> property_display() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** X Display name.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_display() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, runs the X display in synchronous mode. (used only for debugging).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_synchronous() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, runs the X display in synchronous mode. (used only for debugging).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_synchronous() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The pixel aspect ratio of the device.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Glib::ustring> property_pixel_aspect_ratio() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The pixel aspect ratio of the device.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_pixel_aspect_ratio() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, scaling will respect original aspect ratio.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_force_aspect_ratio() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, scaling will respect original aspect ratio.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_force_aspect_ratio() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, XEvents will be selected and handled.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_handle_events() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, XEvents will be selected and handled.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_handle_events() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The number of the video adaptor.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Glib::ustring> property_device() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The number of the video adaptor.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_device() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The name of the video adaptor.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_device_name() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, the current frame will always be drawn in response to X Expose events.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_handle_expose() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** When enabled, the current frame will always be drawn in response to X Expose events.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_handle_expose() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Whether to double-buffer the output.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_double_buffer() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Whether to double-buffer the output.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_double_buffer() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Whether to autofill overlay with colorkey.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_autopaint_colorkey() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Whether to autofill overlay with colorkey.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_autopaint_colorkey() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Color to use for the overlay mask.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_colorkey() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Color to use for the overlay mask.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_colorkey() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Draw black borders to fill unused area in force-aspect-ratio mode.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_draw_borders() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Draw black borders to fill unused area in force-aspect-ratio mode.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_draw_borders() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Width of the window.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_window_width() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Height of the window.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_window_height() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


};

} // namespace Gst


namespace Glib
{
  /** A Glib::wrap() method for this object.
   * 
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Gst::XvImageSink
   */
  Glib::RefPtr<Gst::XvImageSink> wrap(GstXvImageSink* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_XVIMAGESINK_H */

