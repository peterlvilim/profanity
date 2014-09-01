/*
 * api.c
 *
 * Copyright (C) 2012 - 2014 James Booth <boothj5@gmail.com>
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

#include <stdlib.h>

#include <glib.h>

#include "log.h"
#include "plugins/callbacks.h"
#include "plugins/autocompleters.h"
#include "profanity.h"
#include "ui/windows.h"
#include "ui/ui.h"
#include "config/theme.h"

void
api_cons_alert(void)
{
    cons_alert();
}

void
api_cons_show(const char * const message)
{
    if (message != NULL) {
        cons_show("%s", message);
    }
}

void
api_register_command(const char *command_name, int min_args, int max_args,
    const char *usage, const char *short_help, const char *long_help, void *callback,
    void(*callback_func)(PluginCommand *command, gchar **args))
{
    PluginCommand *command = malloc(sizeof(PluginCommand));
    command->command_name = command_name;
    command->min_args = min_args;
    command->max_args = max_args;
    command->usage = usage;
    command->short_help = short_help;
    command->long_help = long_help;
    command->callback = callback;
    command->callback_func = callback_func;

    callbacks_add_command(command);
}

void
api_register_timed(void *callback, int interval_seconds,
    void (*callback_func)(PluginTimedFunction *timed_function))
{
    PluginTimedFunction *timed_function = malloc(sizeof(PluginTimedFunction));
    timed_function->callback = callback;
    timed_function->callback_func = callback_func;
    timed_function->interval_seconds = interval_seconds;
    timed_function->timer = g_timer_new();

    callbacks_add_timed(timed_function);
}

void
api_register_ac(const char *key, char **items)
{
    autocompleters_add(key, items);
}

void
api_notify(const char *message, const char *category, int timeout_ms)
{
    notify(message, timeout_ms, category);
}

void
api_send_line(char *line)
{
    prof_process_input(line);
}

char *
api_get_current_recipient(void)
{
    win_type_t win_type = ui_current_win_type();
    if (win_type == WIN_CHAT) {
        char *recipient = ui_current_recipient();
        return recipient;
    } else {
        return NULL;
    }
}

void
api_log_debug(const char *message)
{
    log_debug("%s", message);
}

void
api_log_info(const char *message)
{
    log_info("%s", message);
}

void
api_log_warning(const char *message)
{
    log_warning("%s", message);
}

void
api_log_error(const char *message)
{
    log_error("%s", message);
}

int
api_win_exists(const char *tag)
{
    return (wins_get_by_recipient(tag) != NULL);
}

void
api_win_create(const char *tag, void *callback,
    void(*callback_func)(PluginWindowCallback *window_callback, const char *tag, const char * const line))
{
    PluginWindowCallback *window = malloc(sizeof(PluginWindowCallback));
    window->callback = callback;
    window->callback_func = callback_func;
    callbacks_add_window_handler(tag, window);
    wins_new(tag, WIN_PLUGIN);

    // set status bar active
    ProfWin *win = wins_get_by_recipient(tag);
    int num = wins_get_num(win);
    ui_status_bar_active(num);
}

void
api_win_focus(const char *tag)
{
    ProfWin *win = wins_get_by_recipient(tag);
    int num = wins_get_num(win);
    ui_switch_win(num);
}

void
api_win_show(const char *tag, const char *line)
{
    ProfWin *window = wins_get_by_recipient(tag);
    win_save_print(window, '!', NULL, 0, 0, "", line);

    // refresh if current
    if (wins_is_current(window)) {
        int num = wins_get_num(window);
        ui_switch_win(num);
    }
}

void
api_win_show_green(const char *tag, const char *line)
{
    ProfWin *window = wins_get_by_recipient(tag);
    win_save_print(window, '!', NULL, 0, COLOUR_ONLINE, "", line);

    // refresh if current
    if (wins_is_current(window)) {
        int num = wins_get_num(window);
        ui_switch_win(num);
    }
}

void
api_win_show_red(const char *tag, const char *line)
{
    ProfWin *window = wins_get_by_recipient(tag);
    win_save_print(window, '!', NULL, 0, COLOUR_OFFLINE, "", line);

    // refresh if current
    if (wins_is_current(window)) {
        int num = wins_get_num(window);
        ui_switch_win(num);
    }
}

void
api_win_show_cyan(const char *tag, const char *line)
{
    ProfWin *window = wins_get_by_recipient(tag);
    win_save_print(window, '!', NULL, 0, COLOUR_AWAY, "", line);

    // refresh if current
    if (wins_is_current(window)) {
        int num = wins_get_num(window);
        ui_switch_win(num);
    }
}

void
api_win_show_yellow(const char *tag, const char *line)
{
    ProfWin *window = wins_get_by_recipient(tag);
    win_save_print(window, '!', NULL, 0, COLOUR_INCOMING, "", line);

    // refresh if current
    if (wins_is_current(window)) {
        int num = wins_get_num(window);
        ui_switch_win(num);
    }
}
