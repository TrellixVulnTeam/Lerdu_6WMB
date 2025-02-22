/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2004-2009 Shaun McCance <shaunm@gnome.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Shaun McCance <shaunm@gnome.org>
 */

#ifndef __YELP_SETTINGS_H__
#define __YELP_SETTINGS_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define YELP_TYPE_SETTINGS         (yelp_settings_get_type ())
#define YELP_SETTINGS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), YELP_TYPE_SETTINGS, YelpSettings))
#define YELP_SETTINGS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), YELP_TYPE_SETTINGS, YelpSettingsClass))
#define YELP_IS_SETTINGS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), YELP_TYPE_SETTINGS))
#define YELP_IS_SETTINGS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), YELP_TYPE_SETTINGS))
#define YELP_SETTINGS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), YELP_TYPE_SETTINGS, YelpSettingsClass))

typedef struct _YelpSettings      YelpSettings;
typedef struct _YelpSettingsClass YelpSettingsClass;
typedef struct _YelpSettingsPriv  YelpSettingsPriv;

struct _YelpSettings {
    GObject           parent;
    YelpSettingsPriv *priv;
};

struct _YelpSettingsClass {
    GObjectClass      parent_class;
};

typedef enum {
    YELP_SETTINGS_COLOR_BASE,
    YELP_SETTINGS_COLOR_TEXT,
    YELP_SETTINGS_COLOR_TEXT_LIGHT,
    YELP_SETTINGS_COLOR_LINK,
    YELP_SETTINGS_COLOR_LINK_VISITED,
    YELP_SETTINGS_COLOR_GRAY_BASE,
    YELP_SETTINGS_COLOR_DARK_BASE,
    YELP_SETTINGS_COLOR_GRAY_BORDER,
    YELP_SETTINGS_COLOR_BLUE_BASE,
    YELP_SETTINGS_COLOR_BLUE_BORDER,
    YELP_SETTINGS_COLOR_RED_BASE,
    YELP_SETTINGS_COLOR_RED_BORDER,
    YELP_SETTINGS_COLOR_YELLOW_BASE,
    YELP_SETTINGS_COLOR_YELLOW_BORDER,
    YELP_SETTINGS_NUM_COLORS
} YelpSettingsColor;

typedef enum {
    YELP_SETTINGS_FONT_VARIABLE,
    YELP_SETTINGS_FONT_FIXED,
    YELP_SETTINGS_NUM_FONTS
} YelpSettingsFont;

typedef enum {
    YELP_SETTINGS_ICON_BUG,
    YELP_SETTINGS_ICON_IMPORTANT,
    YELP_SETTINGS_ICON_NOTE,
    YELP_SETTINGS_ICON_TIP,
    YELP_SETTINGS_ICON_WARNING,
    YELP_SETTINGS_NUM_ICONS
} YelpSettingsIcon;

YelpSettings *      yelp_settings_get_default          (void);

gchar *             yelp_settings_get_color            (YelpSettings       *settings,
                                                        YelpSettingsColor   color);
gchar **            yelp_settings_get_colors           (YelpSettings       *settings);
void                yelp_settings_set_colors           (YelpSettings       *settings,
                                                        YelpSettingsColor   first_color,
                                                        ...);
const gchar*        yelp_settings_get_color_param      (YelpSettingsColor   color);


gchar *             yelp_settings_get_font             (YelpSettings       *settings,
                                                        YelpSettingsFont    font);
gchar *             yelp_settings_get_font_family      (YelpSettings       *settings,
                                                        YelpSettingsFont    font);
gint                yelp_settings_get_font_size        (YelpSettings       *settings,
                                                        YelpSettingsFont    font);
void                yelp_settings_set_fonts            (YelpSettings       *settings,
                                                        YelpSettingsFont    first_font,
                                                        ...);
gint                yelp_settings_get_font_adjustment  (YelpSettings       *settings);
void                yelp_settings_set_font_adjustment  (YelpSettings       *settings,
                                                        gint                adjustment);

gint                yelp_settings_get_icon_size        (YelpSettings       *settings);
void                yelp_settings_set_icon_size        (YelpSettings       *settings,
                                                        gint                size);
gchar *             yelp_settings_get_icon             (YelpSettings       *settings,
                                                        YelpSettingsIcon    icon);
void                yelp_settings_set_icons            (YelpSettings       *settings,
                                                        YelpSettingsIcon    first_icon,
                                                        ...);
const gchar *       yelp_settings_get_icon_param       (YelpSettingsIcon    icon);

gchar **            yelp_settings_get_all_params       (YelpSettings       *settings,
                                                        gint                extra,
                                                        gint               *end);

gboolean            yelp_settings_get_show_text_cursor (YelpSettings       *settings);
void                yelp_settings_set_show_text_cursor (YelpSettings       *settings,
                                                        gboolean            show);

gboolean            yelp_settings_get_editor_mode      (YelpSettings       *settings);
void                yelp_settings_set_editor_mode      (YelpSettings       *settings,
                                                        gboolean            editor_mode);

void                yelp_settings_set_env              (YelpSettings       *settings,
                                                        const gchar        *env);
void                yelp_settings_unset_env            (YelpSettings       *settings,
                                                        const gchar        *env);
gboolean            yelp_settings_check_env            (YelpSettings       *settings,
                                                        const gchar        *env);

gint                yelp_settings_cmp_icons            (const gchar        *icon1,
                                                        const gchar        *icon2);

G_END_DECLS

#endif /* __YELP_SETTINGS_H__ */
