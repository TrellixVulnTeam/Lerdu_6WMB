// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_AUDIOSRC_P_H
#define _GSTREAMERMM_AUDIOSRC_P_H


#include <glibmm/class.h>

namespace Gst
{

class AudioSrc_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef AudioSrc CppObjectType;
  typedef GstAudioSrc BaseObjectType;
  typedef GstAudioSrcClass BaseClassType;
  typedef Gst::BaseAudioSrc_Class CppClassParent;
  typedef GstBaseAudioSrcClass BaseClassParent;

  friend class AudioSrc;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();


  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.

  //Callbacks (virtual functions):
  static gboolean open_vfunc_callback(GstAudioSrc* self);
  static gboolean unprepare_vfunc_callback(GstAudioSrc* self);
  static gboolean close_vfunc_callback(GstAudioSrc* self);
  static guint delay_vfunc_callback(GstAudioSrc* self);
  static void reset_vfunc_callback(GstAudioSrc* self);
  static gboolean prepare_vfunc_callback(GstAudioSrc* self, GstRingBufferSpec* spec);
  static guint read_vfunc_callback(GstAudioSrc* self, gpointer data, guint length);
  };


} // namespace Gst

#include <gstreamermm/private/baseaudiosrc_p.h>


#endif /* _GSTREAMERMM_AUDIOSRC_P_H */

