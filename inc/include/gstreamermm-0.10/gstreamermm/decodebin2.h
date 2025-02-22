// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_DECODEBIN2_H
#define _GSTREAMERMM_DECODEBIN2_H


#include <glibmm.h>

// Generated by generate_plugin_gmmproc_file. Don't edit this file.


#include <gstreamermm/bin.h>
#include <gstreamermm/caps.h>
#include <gstreamermm/pad.h>
#include <glibmm/valuearray.h>
#include <gstreamermm/enums.h>
#include <gstreamermm/elementfactory.h>


// Plug-in C enums used in signals:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GstDecodeBin2 GstDecodeBin2;
typedef struct _GstDecodeBin2Class GstDecodeBin2Class;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{ class DecodeBin2_Class; } // namespace Gst
namespace Gst
{

/** A Wrapper for the decodebin2 plugin.
 * Please note that, though using the underlying GObject is fine, using its C
 * <B>type</B> is not guaranteed to be API stable across releases because it is
 * not guaranteed to always remain the same.  Also, not all plug-ins are
 * available on all systems so care must be taken that they exist before they
 * are used, otherwise there will be errors and possibly a crash.
 *
 * @ingroup GstPlugins
 */

class DecodeBin2
: public Gst::Bin
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  typedef DecodeBin2 CppObjectType;
  typedef DecodeBin2_Class CppClassType;
  typedef GstDecodeBin2 BaseObjectType;
  typedef GstDecodeBin2Class BaseClassType;

private:  friend class DecodeBin2_Class;
  static CppClassType decodebin2_class_;

private:
  // noncopyable
  DecodeBin2(const DecodeBin2&);
  DecodeBin2& operator=(const DecodeBin2&);

protected:
  explicit DecodeBin2(const Glib::ConstructParams& construct_params);
  explicit DecodeBin2(GstDecodeBin2* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:
  virtual ~DecodeBin2();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_type()      G_GNUC_CONST;


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GstDecodeBin2*       gobj()       { return reinterpret_cast<GstDecodeBin2*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GstDecodeBin2* gobj() const { return reinterpret_cast<GstDecodeBin2*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GstDecodeBin2* gobj_copy();

private:

  
protected:
  DecodeBin2();
  explicit DecodeBin2(const Glib::ustring& name);

public:
  /** Creates a new decodebin2 plugin with a unique name.
   */
  
  static Glib::RefPtr<DecodeBin2> create();


  /** Creates a new decodebin2 plugin with the given name.
   */
  
  static Glib::RefPtr<DecodeBin2> create(const Glib::ustring& name);


  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The caps on which to stop decoding.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Gst::Caps> > property_caps() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The caps on which to stop decoding.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Gst::Caps> > property_caps() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Encoding to assume if input subtitles are not in UTF-8 encoding. If not set, the GST_SUBTITLE_ENCODING environment variable will be checked for an encoding to use. If that is not set either, ISO-8859-15 will be assumed.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<Glib::ustring> property_subtitle_encoding() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Encoding to assume if input subtitles are not in UTF-8 encoding. If not set, the GST_SUBTITLE_ENCODING environment variable will be checked for an encoding to use. If that is not set either, ISO-8859-15 will be assumed.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<Glib::ustring> property_subtitle_encoding() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** The caps of the input data. (NULL = use typefind element).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy< Glib::RefPtr<Gst::Caps> > property_sink_caps() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** The caps of the input data. (NULL = use typefind element).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::RefPtr<Gst::Caps> > property_sink_caps() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Emit GST_MESSAGE_BUFFERING based on low-/high-percent thresholds.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_use_buffering() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Emit GST_MESSAGE_BUFFERING based on low-/high-percent thresholds.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_use_buffering() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Low threshold for buffering to start.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_low_percent() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Low threshold for buffering to start.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_low_percent() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** High threshold for buffering to finish.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<int> property_high_percent() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** High threshold for buffering to finish.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<int> property_high_percent() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Max. amount of bytes in the queue (0=automatic).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<guint> property_max_size_bytes() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Max. amount of bytes in the queue (0=automatic).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint> property_max_size_bytes() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Max. number of buffers in the queue (0=automatic).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<guint> property_max_size_buffers() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Max. number of buffers in the queue (0=automatic).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint> property_max_size_buffers() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Max. amount of data in the queue (in ns, 0=automatic).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<guint64> property_max_size_time() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Max. amount of data in the queue (in ns, 0=automatic).
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<guint64> property_max_size_time() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Post stream-topology messages.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_post_stream_topology() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Post stream-topology messages.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_post_stream_topology() const;
#endif //#GLIBMM_PROPERTIES_ENABLED

  #ifdef GLIBMM_PROPERTIES_ENABLED
/** Expose all streams, including those of unknown type or that don't match the 'caps' property.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy<bool> property_expose_all_streams() ;
#endif //#GLIBMM_PROPERTIES_ENABLED

#ifdef GLIBMM_PROPERTIES_ENABLED
/** Expose all streams, including those of unknown type or that don't match the 'caps' property.
   *
   * You rarely need to use properties because there are get_ and set_ methods for almost all of them.
   * @return A PropertyProxy that allows you to get or set the property of the value, or receive notification when
   * the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly<bool> property_expose_all_streams() const;
#endif //#GLIBMM_PROPERTIES_ENABLED


  /**
   * @par Prototype:
   * <tt>void on_my_%new_decoded_pad(const Glib::RefPtr<Gst::Pad>& arg0, bool arg1)</tt>
   */

  Glib::SignalProxy2< void,const Glib::RefPtr<Gst::Pad>&,bool > signal_new_decoded_pad();

 
  /**
   * @par Prototype:
   * <tt>void on_my_%removed_decoded_pad(const Glib::RefPtr<Gst::Pad>& arg0)</tt>
   */

  Glib::SignalProxy1< void,const Glib::RefPtr<Gst::Pad>& > signal_removed_decoded_pad();

 
  /**
   * @par Prototype:
   * <tt>void on_my_%unknown_type(const Glib::RefPtr<Gst::Pad>& arg0, const Glib::RefPtr<Gst::Caps>& arg1)</tt>
   */

  Glib::SignalProxy2< void,const Glib::RefPtr<Gst::Pad>&,const Glib::RefPtr<Gst::Caps>& > signal_unknown_type();

 
  /**
   * @par Prototype:
   * <tt>bool on_my_%autoplug_continue(const Glib::RefPtr<Gst::Pad>& arg0, const Glib::RefPtr<Gst::Caps>& arg1)</tt>
   */

  Glib::SignalProxy2< bool,const Glib::RefPtr<Gst::Pad>&,const Glib::RefPtr<Gst::Caps>& > signal_autoplug_continue();

 
  /**
   * @par Prototype:
   * <tt>Glib::ValueArray on_my_%autoplug_factories(const Glib::RefPtr<Gst::Pad>& arg0, const Glib::RefPtr<Gst::Caps>& arg1)</tt>
   */

  Glib::SignalProxy2< Glib::ValueArray,const Glib::RefPtr<Gst::Pad>&,const Glib::RefPtr<Gst::Caps>& > signal_autoplug_factories();

 
  /**
   * @par Prototype:
   * <tt>Glib::ValueArray on_my_%autoplug_sort(const Glib::RefPtr<Gst::Pad>& arg0, const Glib::RefPtr<Gst::Caps>& arg1, const Glib::ValueArray& arg2)</tt>
   */

  Glib::SignalProxy3< Glib::ValueArray,const Glib::RefPtr<Gst::Pad>&,const Glib::RefPtr<Gst::Caps>&,const Glib::ValueArray& > signal_autoplug_sort();

 
  /**
   * @par Prototype:
   * <tt>Gst::AutoplugSelectResult on_my_%autoplug_select(const Glib::RefPtr<Gst::Pad>& arg0, const Glib::RefPtr<Gst::Caps>& arg1, const Glib::RefPtr<Gst::ElementFactory>& arg2)</tt>
   */

  Glib::SignalProxy3< Gst::AutoplugSelectResult,const Glib::RefPtr<Gst::Pad>&,const Glib::RefPtr<Gst::Caps>&,const Glib::RefPtr<Gst::ElementFactory>& > signal_autoplug_select();

  
  /**
   * @par Prototype:
   * <tt>void on_my_%drained()</tt>
   */

  Glib::SignalProxy0< void > signal_drained();


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
   * @relates Gst::DecodeBin2
   */
  Glib::RefPtr<Gst::DecodeBin2> wrap(GstDecodeBin2* object, bool take_copy = false);
}


#endif /* _GSTREAMERMM_DECODEBIN2_H */

