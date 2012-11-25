/*
 * preferences.c
 *
 * Copyright (C) 2012 James Booth <boothj5@gmail.com>
 *
 * This file is part of Profanity.
 *
 * Profanity is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Profanity is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Profanity.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "config.h"
#include "preferences.h"

#include <stdlib.h>
#include <string.h>

#include <glib.h>
#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#endif
#ifdef HAVE_NCURSES_NCURSES_H
#include <ncurses/ncurses.h>
#endif

#include "log.h"
#include "prof_autocomplete.h"

static GString *prefs_loc;
static GKeyFile *prefs;
gint log_maxsize = 0;

static PAutocomplete login_ac;
static PAutocomplete boolean_choice_ac;

static void _save_prefs(void);

void
prefs_load(void)
{
    GError *err;

    log_info("Loading preferences");
    login_ac = p_autocomplete_new();
    prefs_loc = g_string_new(getenv("HOME"));
    g_string_append(prefs_loc, "/.profanity/config");

    prefs = g_key_file_new();
    g_key_file_load_from_file(prefs, prefs_loc->str, G_KEY_FILE_KEEP_COMMENTS,
        NULL);

    // create the logins searchable list for autocompletion
    gsize njids;
    gchar **jids =
        g_key_file_get_string_list(prefs, "connections", "logins", &njids, NULL);

    gsize i;
    for (i = 0; i < njids; i++) {
        p_autocomplete_add(login_ac, strdup(jids[i]));
    }

    for (i = 0; i < njids; i++) {
        free(jids[i]);
    }
    free(jids);

    err = NULL;
    log_maxsize = g_key_file_get_integer(prefs, "log", "maxsize", &err);
    if (err != NULL) {
        log_maxsize = 0;
        g_error_free(err);
    }

    boolean_choice_ac = p_autocomplete_new();
    p_autocomplete_add(boolean_choice_ac, strdup("on"));
    p_autocomplete_add(boolean_choice_ac, strdup("off"));
}

void
prefs_close(void)
{
    p_autocomplete_clear(login_ac);
    p_autocomplete_clear(boolean_choice_ac);
    g_key_file_free(prefs);
}

char *
prefs_find_login(char *prefix)
{
    return p_autocomplete_complete(login_ac, prefix);
}

void
prefs_reset_login_search(void)
{
    p_autocomplete_reset(login_ac);
}

char *
prefs_autocomplete_boolean_choice(char *prefix)
{
    return p_autocomplete_complete(boolean_choice_ac, prefix);
}

void
prefs_reset_boolean_choice(void)
{
    p_autocomplete_reset(boolean_choice_ac);
}

gboolean
prefs_get_beep(void)
{
    return g_key_file_get_boolean(prefs, "ui", "beep", NULL);
}

void
prefs_set_beep(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "beep", value);
    _save_prefs();
}

gchar *
prefs_get_theme(void)
{
    return g_key_file_get_string(prefs, "ui", "theme", NULL);
}

void
prefs_set_theme(gchar *value)
{
    g_key_file_set_string(prefs, "ui", "theme", value);
    _save_prefs();
}

gboolean
prefs_get_states(void)
{
    return g_key_file_get_boolean(prefs, "ui", "states", NULL);
}

void
prefs_set_states(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "states", value);
    _save_prefs();
}

gboolean
prefs_get_outtype(void)
{
    return g_key_file_get_boolean(prefs, "ui", "outtype", NULL);
}

void
prefs_set_outtype(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "outtype", value);
    _save_prefs();
}

gboolean
prefs_get_notify_typing(void)
{
    return g_key_file_get_boolean(prefs, "notifications", "typing", NULL);
}

void
prefs_set_notify_typing(gboolean value)
{
    g_key_file_set_boolean(prefs, "notifications", "typing", value);
    _save_prefs();
}

gboolean
prefs_get_notify_message(void)
{
    return g_key_file_get_boolean(prefs, "notifications", "message", NULL);
}

void
prefs_set_notify_message(gboolean value)
{
    g_key_file_set_boolean(prefs, "notifications", "message", value);
    _save_prefs();
}

gint
prefs_get_notify_remind(void)
{
    return g_key_file_get_integer(prefs, "notifications", "remind", NULL);
}

void
prefs_set_notify_remind(gint value)
{
    g_key_file_set_integer(prefs, "notifications", "remind", value);
    _save_prefs();
}

gint
prefs_get_max_log_size(void)
{
    if (log_maxsize < PREFS_MIN_LOG_SIZE)
        return PREFS_MAX_LOG_SIZE;
    else
        return log_maxsize;
}

void
prefs_set_max_log_size(gint value)
{
    log_maxsize = value;
    g_key_file_set_integer(prefs, "log", "maxsize", value);
    _save_prefs();
}

gint
prefs_get_priority(void)
{
    return g_key_file_get_integer(prefs, "jabber", "priority", NULL);
}

void
prefs_set_priority(gint value)
{
    g_key_file_set_integer(prefs, "jabber", "priority", value);
    _save_prefs();
}

gint
prefs_get_reconnect(void)
{
    return g_key_file_get_integer(prefs, "jabber", "reconnect", NULL);
}

void
prefs_set_reconnect(gint value)
{
    g_key_file_set_integer(prefs, "jabber", "reconnect", value);
    _save_prefs();
}

gboolean
prefs_get_vercheck(void)
{
    return g_key_file_get_boolean(prefs, "ui", "vercheck", NULL);
}

void
prefs_set_vercheck(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "vercheck", value);
    _save_prefs();
}

gboolean
prefs_get_flash(void)
{
    return g_key_file_get_boolean(prefs, "ui", "flash", NULL);
}

void
prefs_set_flash(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "flash", value);
    _save_prefs();
}

gboolean
prefs_get_intype(void)
{
    return g_key_file_get_boolean(prefs, "ui", "intype", NULL);
}

void
prefs_set_intype(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "intype", value);
    _save_prefs();
}

gboolean
prefs_get_chlog(void)
{
    return g_key_file_get_boolean(prefs, "ui", "chlog", NULL);
}

void
prefs_set_chlog(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "chlog", value);
    _save_prefs();
}

gboolean
prefs_get_history(void)
{
    return g_key_file_get_boolean(prefs, "ui", "history", NULL);
}

void
prefs_set_history(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "history", value);
    _save_prefs();
}

void
prefs_add_login(const char *jid)
{
    gsize njids;
    gchar **jids =
        g_key_file_get_string_list(prefs, "connections", "logins", &njids, NULL);

    // no logins remembered yet
    if (jids == NULL) {
        njids = 1;
        jids = (gchar**) g_malloc(sizeof(gchar *) * 2);
        jids[0] = g_strdup(jid);
        jids[1] = NULL;
        g_key_file_set_string_list(prefs, "connections", "logins",
            (const gchar * const *)jids, njids);
        _save_prefs();
        g_strfreev(jids);

        return;
    } else {
        gsize i;
        for (i = 0; i < njids; i++) {
            if (strcmp(jid, jids[i]) == 0) {
                g_strfreev(jids);
                return;
            }
        }

        // jid not found, add to the list
        jids = (gchar **) g_realloc(jids, (sizeof(gchar *) * (njids+2)));
        jids[njids] = g_strdup(jid);
        njids++;
        jids[njids] = NULL;
        g_key_file_set_string_list(prefs, "connections", "logins",
            (const gchar * const *)jids, njids);
        _save_prefs();
        g_strfreev(jids);

        return;
    }
}

gboolean
prefs_get_showsplash(void)
{
    return g_key_file_get_boolean(prefs, "ui", "showsplash", NULL);
}

void
prefs_set_showsplash(gboolean value)
{
    g_key_file_set_boolean(prefs, "ui", "showsplash", value);
    _save_prefs();
}

static void
_save_prefs(void)
{
    gsize g_data_size;
    char *g_prefs_data = g_key_file_to_data(prefs, &g_data_size, NULL);
    g_file_set_contents(prefs_loc->str, g_prefs_data, g_data_size, NULL);
}
