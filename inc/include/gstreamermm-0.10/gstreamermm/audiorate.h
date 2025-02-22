// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_AUDIORATE_H
#define _GSTREAMERMM_AUDIORATE_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/element.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstAudioRate GstAudioRate;
typedef struct _GstAudioRateClass GstAudioRateClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class AudioRate_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the audiorate plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class AudioRate
: public Gst::Element
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef AudioRate CppObjectType;
  typedef AudioRate_Class CppClassType;
  typedef GstAudioRate BaseObjectType;
  typedef GstAudioRateClass BaseClassType;

private:  friend class AudioRate_Class;
  static CppClassType audiorate_class_;

private:
  // noncopyable
  AudioRate(const AudioRate&);
  AudioRate& operator=(const AudioRate&);

protected:
  explicit AudioRate(const Glib::ConstructParams& construct_params);
  explicit AudioRate(GstAudioRate* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~AudioRate();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstAudioRate*       gobj()       { return reinterpret_cast<GstAudioRate*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstAudioRate* gobj() const { return reinterpret_cast<GstAudioRate*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstAudioRate* gobj_copy();

private:

  
protected:
  AudioRate();
  explicit AudioRate(const Glib::ustring& name);

public:
  /** Creates a new audiorate plugin with a unique name.
   */
  
  static Glib::RefPtr<AudioRate> create();


  /** Creates a new audiorate plugin with the given name.
   */
  
  static Glib::RefPtr<AudioRate> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Number of input samples.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_in() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Number of output samples.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_out() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Number of added samples.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_add() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Number of dropped samples.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_drop() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Don't emit notify for dropped and duplicated frames.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_silent() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Don't emit notify for dropped and duplicated frames.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_silent() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Only act if timestamp jitter/imperfection exceeds indicated tolerance (ns).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<guint64> property_tolerance() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Only act if timestamp jitter/imperfection exceeds indicated tolerance (ns).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_tolerance() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Don't produce buffers before the first one we receive.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_skip_to_first() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Don't produce buffers before the first one we receive.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_skip_to_first() const;
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
   * @relates Gst::AudioRate
   */
  Glib::RefPtr<Gst::AudioRate> wrap(GstAudioRate* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_AUDIORATE_H */

