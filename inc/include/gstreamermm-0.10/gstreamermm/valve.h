// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_VALVE_H
#define _GSTREAMERMM_VALVE_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/element.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstValve GstValve;
typedef struct _GstValveClass GstValveClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class Valve_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the valve plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class Valve
: public Gst::Element
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef Valve CppObjectType;
  typedef Valve_Class CppClassType;
  typedef GstValve BaseObjectType;
  typedef GstValveClass BaseClassType;

private:  friend class Valve_Class;
  static CppClassType valve_class_;

private:
  // noncopyable
  Valve(const Valve&);
  Valve& operator=(const Valve&);

protected:
  explicit Valve(const Glib::ConstructParams& construct_params);
  explicit Valve(GstValve* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~Valve();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstValve*       gobj()       { return reinterpret_cast<GstValve*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstValve* gobj() const { return reinterpret_cast<GstValve*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstValve* gobj_copy();

private:

  
protected:
  Valve();
  explicit Valve(const Glib::ustring& name);

public:
  /** Creates a new valve plugin with a unique name.
   */
  
  static Glib::RefPtr<Valve> create();


  /** Creates a new valve plugin with the given name.
   */
  
  static Glib::RefPtr<Valve> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Whether to drop buffers and events or let them through.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_drop() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Whether to drop buffers and events or let them through.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_drop() const;
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
   * @relates Gst::Valve
   */
  Glib::RefPtr<Gst::Valve> wrap(GstValve* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_VALVE_H */

