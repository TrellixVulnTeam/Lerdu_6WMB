// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_MIXEROPTIONS_H
#define _GSTREAMERMM_MIXEROPTIONS_H


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

#include <gstreamermm/mixertrack.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstMixerOptions GstMixerOptions;
typedef struct _GstMixerOptionsClass GstMixerOptionsClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class MixerOptions_Class; } // namespace Gst
namespace Gst
{

/** A class that represents options for elements that implement the Gst::Mixer
 * interface.
 * @ingroup GstInterfaces
 */

class MixerOptions : public Gst::MixerTrack
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef MixerOptions CppObjectType;
  typedef MixerOptions_Class CppClassType;
  typedef GstMixerOptions BaseObjectType;
  typedef GstMixerOptionsClass BaseClassType;

private:  friend class MixerOptions_Class;
  static CppClassType mixeroptions_class_;

private:
  // noncopyable
  MixerOptions(const MixerOptions&);
  MixerOptions& operator=(const MixerOptions&);

protected:
  explicit MixerOptions(const Glib::ConstructParams& construct_params);
  explicit MixerOptions(GstMixerOptions* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~MixerOptions();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstMixerOptions*       gobj()       { return reinterpret_cast<GstMixerOptions*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstMixerOptions* gobj() const { return reinterpret_cast<GstMixerOptions*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstMixerOptions* gobj_copy();

private:

public:
 

  /** Get the values for the mixer option.
   * @return A list of strings with all the possible values for the mixer
   * option. You must not free or modify the list or its contents, it belongs
   * to the @a mixer_options object.
   */
  Glib::ListHandle< Glib::ustring > get_values() const;


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
   * @relates Gst::MixerOptions
   */
  Glib::RefPtr<Gst::MixerOptions> wrap(GstMixerOptions* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_MIXEROPTIONS_H */

