// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_ALSASINK_H
#define _GSTREAMERMM_ALSASINK_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/audiosink.h>
#include <gstreamermm/propertyprobe.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstAlsaSink GstAlsaSink;
typedef struct _GstAlsaSinkClass GstAlsaSinkClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class AlsaSink_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the alsasink plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class AlsaSink
: public Gst::AudioSink, public Gst::PropertyProbe
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef AlsaSink CppObjectType;
  typedef AlsaSink_Class CppClassType;
  typedef GstAlsaSink BaseObjectType;
  typedef GstAlsaSinkClass BaseClassType;

private:  friend class AlsaSink_Class;
  static CppClassType alsasink_class_;

private:
  // noncopyable
  AlsaSink(const AlsaSink&);
  AlsaSink& operator=(const AlsaSink&);

protected:
  explicit AlsaSink(const Glib::ConstructParams& construct_params);
  explicit AlsaSink(GstAlsaSink* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~AlsaSink();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstAlsaSink*       gobj()       { return reinterpret_cast<GstAlsaSink*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstAlsaSink* gobj() const { return reinterpret_cast<GstAlsaSink*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstAlsaSink* gobj_copy();

private:

  
protected:
  AlsaSink();
  explicit AlsaSink(const Glib::ustring& name);

public:
  /** Creates a new alsasink plugin with a unique name.
   */
  
  static Glib::RefPtr<AlsaSink> create();


  /** Creates a new alsasink plugin with the given name.
   */
  
  static Glib::RefPtr<AlsaSink> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** ALSA device, as defined in an asound configuration file.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Glib::ustring> property_device() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** ALSA device, as defined in an asound configuration file.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_device() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Human-readable name of the sound device.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_device_name() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Human-readable name of the sound card.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_card_name() const;
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
   * @relates Gst::AlsaSink
   */
  Glib::RefPtr<Gst::AlsaSink> wrap(GstAlsaSink* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_ALSASINK_H */

