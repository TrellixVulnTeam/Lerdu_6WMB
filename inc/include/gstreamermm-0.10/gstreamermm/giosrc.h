// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_GIOSRC_H
#define _GSTREAMERMM_GIOSRC_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/basesrc.h>
#include <giomm/file.h>
#include <gstreamermm/urihandler.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstGioSrc GstGioSrc;
typedef struct _GstGioSrcClass GstGioSrcClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class GioSrc_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the giosrc plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class GioSrc
: public Gst::BaseSrc, public Gst::URIHandler
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef GioSrc CppObjectType;
  typedef GioSrc_Class CppClassType;
  typedef GstGioSrc BaseObjectType;
  typedef GstGioSrcClass BaseClassType;

private:  friend class GioSrc_Class;
  static CppClassType giosrc_class_;

private:
  // noncopyable
  GioSrc(const GioSrc&);
  GioSrc& operator=(const GioSrc&);

protected:
  explicit GioSrc(const Glib::ConstructParams& construct_params);
  explicit GioSrc(GstGioSrc* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~GioSrc();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstGioSrc*       gobj()       { return reinterpret_cast<GstGioSrc*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstGioSrc* gobj() const { return reinterpret_cast<GstGioSrc*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstGioSrc* gobj_copy();

private:

  
protected:
  GioSrc();
  explicit GioSrc(const Glib::ustring& name);

public:
  /** Creates a new giosrc plugin with a unique name.
   */
  
  static Glib::RefPtr<GioSrc> create();


  /** Creates a new giosrc plugin with the given name.
   */
  
  static Glib::RefPtr<GioSrc> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** URI location to read from.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Glib::ustring> property_location() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** URI location to read from.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_location() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** GFile to read from.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Gio::File> > property_file() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** GFile to read from.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Gio::File> > property_file() const;
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
   * @relates Gst::GioSrc
   */
  Glib::RefPtr<Gst::GioSrc> wrap(GstGioSrc* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_GIOSRC_H */

