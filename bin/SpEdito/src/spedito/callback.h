  /************************************ /
 /  SpEdito - Sprite Editor for z88dk  /
/ ************************************/

// Copyright 2009 Marcello Zaniboni
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2, with the following exceptions:
//  1) this software can be distributed within any official z88dk package,
//     source archive or installer, without affecting z88dk license terms;
//  2) when the software is distributed with z88dk, the whole source code
//     (and optionally the compiled binaries) for SpEdito, ASM Generator
//     Module and Small Preview Module must be included.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#ifndef callbacks_h
#define callbacks_h


// main window
void cb_shiftleft           (Fl_Widget *, void *);
void cb_shiftright          (Fl_Widget *, void *);
void cb_shiftup             (Fl_Widget *, void *);
void cb_shiftdown           (Fl_Widget *, void *);
void cb_invert              (Fl_Widget *, void *);
void cb_hflip               (Fl_Widget *, void *);
void cb_vflip               (Fl_Widget *, void *);
void cb_rotate              (Fl_Widget *, void *);
void cb_start               (Fl_Widget *, void *);
void cb_load                (Fl_Widget *, void *);
void cb_help                (Fl_Widget *, void *);
void cb_license             (Fl_Widget *, void *);
void cb_save                (Fl_Widget *, void *);
void cb_generate            (Fl_Widget *, void *);
void cb_externalmodules     (Fl_Widget *, void *);
void cb_quit                (Fl_Widget *, void *);
void cb_clicktest           (Fl_Widget *, void *);
void cb_no_esc_key_window   (Fl_Widget *, void *);
void cb_esc_key_window      (Fl_Widget *, void *);
void cb_notimplemented      (Fl_Widget *, void *);

// module window
void cb_runmodule           (Fl_Widget *, void *);
void cb_infomodule          (Fl_Widget *, void *);
void cb_addmodule           (Fl_Widget *, void *);
void cb_delmodule           (Fl_Widget *, void *);
void cb_closemodulewindow   (Fl_Widget *, void *);
void redrawbrowser();

#endif
