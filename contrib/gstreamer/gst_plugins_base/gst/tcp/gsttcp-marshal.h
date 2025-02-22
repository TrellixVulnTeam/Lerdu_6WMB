
#ifndef __gst_tcp_marshal_MARSHAL_H__
#define __gst_tcp_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:STRING,UINT (gsttcp-marshal.list:1) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_tcp_marshal_VOID__STRING_UINT (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);

/* VOID:INT (gsttcp-marshal.list:2) */
#define gst_tcp_marshal_VOID__INT	g_cclosure_marshal_VOID__INT

/* VOID:INT,BOXED (gsttcp-marshal.list:3) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_tcp_marshal_VOID__INT_BOXED (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* VOID:INT,BOOLEAN,INT,UINT64,INT,UINT64 (gsttcp-marshal.list:4) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_tcp_marshal_VOID__INT_BOOLEAN_INT_UINT64_INT_UINT64 (GClosure     *closure,
                                                                     GValue       *return_value,
                                                                     guint         n_param_values,
                                                                     const GValue *param_values,
                                                                     gpointer      invocation_hint,
                                                                     gpointer      marshal_data);

/* BOXED:INT (gsttcp-marshal.list:5) */
#ifdef __SYMBIAN32__
IMPORT_C
#endif

extern void gst_tcp_marshal_BOXED__INT (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

G_END_DECLS

#endif /* __gst_tcp_marshal_MARSHAL_H__ */

