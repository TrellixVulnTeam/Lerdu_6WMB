// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_GIOSTREAMSINK_H
#define _GSTREAMERMM_GIOSTREAMSINK_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/basesink.h>
#include <giomm/outputstream.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstGioStreamSink GstGioStreamSink;
typedef struct _GstGioStreamSinkClass GstGioStreamSinkClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class GioStreamSink_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the giostreamsink plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class GioStreamSink
: public Gst::BaseSink
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef GioStreamSink CppObjectType;
  typedef GioStreamSink_Class CppClassType;
  typedef GstGioStreamSink BaseObjectType;
  typedef GstGioStreamSinkClass BaseClassType;

private:  friend class GioStreamSink_Class;
  static CppClassType giostreamsink_class_;

private:
  // noncopyable
  GioStreamSink(const GioStreamSink&);
  GioStreamSink& operator=(const GioStreamSink&);

protected:
  explicit GioStreamSink(const Glib::ConstructParams& construct_params);
  explicit GioStreamSink(GstGioStreamSink* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~GioStreamSink();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstGioStreamSink*       gobj()       { return reinterpret_cast<GstGioStreamSink*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstGioStreamSink* gobj() const { return reinterpret_cast<GstGioStreamSink*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstGioStreamSink* gobj_copy();

private:

  
protected:
  GioStreamSink();
  explicit GioStreamSink(const Glib::ustring& name);

public:
  /** Creates a new giostreamsink plugin with a unique name.
   */
  
  static Glib::RefPtr<GioStreamSink> create();


  /** Creates a new giostreamsink plugin with the given name.
   */
  
  static Glib::RefPtr<GioStreamSink> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Stream to write to.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Gio::OutputStream> > property_stream() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Stream to write to.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Gio::OutputStream> > property_stream() const;
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
   * @relates Gst::GioStreamSink
   */
  Glib::RefPtr<Gst::GioStreamSink> wrap(GstGioStreamSink* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_GIOSTREAMSINK_H */

