// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_PROPERTYPROBE_H
#define _GSTREAMERMM_PROPERTYPROBE_H


#include <glibmm.h>

/* gstreamermm - a C++ wrapper for gstreamer
 *
 * Copyright 2008 The gstreamermm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gst/interfaces/propertyprobe.h>
#include <glibmm/interface.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstPropertyProbe GstPropertyProbe;
typedef struct _GstPropertyProbeClass GstPropertyProbeClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class PropertyProbe_Class; } // namespace Gst
namespace Gst
{

/** An interface for probing possible property values.
 * The property probe is a way to autodetect allowed values for a GObject
 * property. Its primary use is to autodetect device-names in several
 * elements.
 *
 * The interface is implemented by many hardware sources and sinks.
 * @ingroup GstInterfaces
 */

class PropertyProbe : public Glib::Interface
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef PropertyProbe CppObjectType;
  typedef PropertyProbe_Class CppClassType;
  typedef GstPropertyProbe BaseObjectType;
  typedef GstPropertyProbeInterface BaseClassType;

private:
  friend class PropertyProbe_Class;
  static CppClassType propertyprobe_class_;

  // noncopyable
  PropertyProbe(const PropertyProbe&);
  PropertyProbe& operator=(const PropertyProbe&);

protected:
  PropertyProbe(); // you must derive from this class

  /** Called by constructors of derived classes. Provide the result of 
   * the Class init() function to ensure that it is properly 
   * initialized.
   * 
   * @param interface_class The Class object for the derived type.
   */
  explicit PropertyProbe(const Glib::Interface_Class& interface_class);

public:
  // This is public so that C++ wrapper instances can be
  // created for C instances of unwrapped types.
  // For instance, if an unexpected C type implements the C interface. 
  explicit PropertyProbe(GstPropertyProbe* castitem);

protected:
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~PropertyProbe();

  static void add_interface(GType gtype_implementer);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;
  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstPropertyProbe*       gobj()       { return reinterpret_cast<GstPropertyProbe*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstPropertyProbe* gobj() const { return reinterpret_cast<GstPropertyProbe*>(gobject_); }

private:


public:
  //TODO: Use something other than const GList* as return (A Glib::ListHandle<>
  //is not used because it's hard to know how to place GParamSpec* in one):
  
  /** Get a list of properties for which probing is supported.
   * @return The list of properties for which probing is supported
   * by this element.
   */
  const GList* get_properties() const;

  
  /** Get ParamSpec for a property for which probing is supported.
   * @param name Name of the property.
   * @return The ParamSpec of <tt>0</tt>.
   */
  const GParamSpec* get_property(const Glib::ustring& name) const;

 
  /** Gets the possible (probed) values for the given property,
   * requires the property to have been probed before.
   * @param pspec The ParamSpec property identifier.
   * @return A list of valid values for the given property.
   */
  Glib::ValueArray get_values(const GParamSpec* pspec) const;
  
  /** Same as get_values().
   * @param name The name of the property to get values for.
   * @return A list of valid values for the given property.
   */
  Glib::ValueArray get_values(const Glib::ustring& name) const;

  
  /** Checks whether a property needs a probe. This might be because
   * the property wasn't initialized before, or because host setup
   * changed. This might be, for example, because a new device was
   * added, and thus device probing needs to be refreshed to display
   * the new device.
   * @param pspec A ParamSpec that identifies the property to check.
   * @return <tt>true</tt> if the property needs a new probe, <tt>false</tt> if not.
   */
  bool needs_probe(const GParamSpec* pspec) const;
  
  /** Same as needs_probe().
   * @param name The name of the property to check.
   * @return <tt>true</tt> if the property needs a new probe, <tt>false</tt> if not.
   */
  bool needs_probe(const Glib::ustring& name) const;
  
  /** Check whether the given property requires a new probe. If so,
   * fo the probe. After that, retrieve a value list. Meant as a
   * utility function that wraps the above functions.
   * @param pspec The ParamSpec property identifier.
   * @return The list of valid values for this property.
   */
  Glib::ValueArray probe_and_get_values(const GParamSpec* pspec);
  
  /** Same as probe_and_get_values().
   * @param name The name of the property to get values for.
   * @return The list of valid values for this property.
   */
  Glib::ValueArray probe_and_get_values(const Glib::ustring& name);
  
  /** Runs a probe on the property specified by @a pspec
   * @param pspec ParamSpec of the property.
   */
  void probe_property(const GParamSpec * pspec);
  
  /** Runs a probe on the property specified by @a name.
   * @param name Name of the property.
   */
  void probe_property(const Glib::ustring& name);

  
  /**
   * @par Prototype:
   * <tt>void on_my_%probe_needed(const GParamSpec* pspec)</tt>
   */

  Glib::SignalProxy1< void,const GParamSpec* > signal_probe_needed();


    virtual const GList* get_properties_vfunc() const;

    virtual bool needs_probe_vfunc(guint prop_id, const GParamSpec* pspec) const;

    virtual void probe_property_vfunc(guint prop_id, const GParamSpec* pspec);


    virtual Glib::ValueArray get_values_vfunc(guint prop_id, const GParamSpec* pspec) const;


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::
  virtual void on_probe_needed(const GParamSpec* pspec);


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
   * @relates Gst::PropertyProbe
   */
  Glib::RefPtr<Gst::PropertyProbe> wrap(GstPropertyProbe* object, bool take_copy = false);

} // namespace Glib


#endif /* _GSTREAMERMM_PROPERTYPROBE_H */

