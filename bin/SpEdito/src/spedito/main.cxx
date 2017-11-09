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


#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <malloc.h>


#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Valuator.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hold_Browser.H>

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

#include "spedito.h"
#include "drawingbox.h"
#include "callback.h"
#include "fileutils.h"

#if defined(__WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <FL/x.H>
#include "resource.h"
#endif

/////////////////////////
//// Globals - BEGIN ////

char *spritemap[MAX_Y_RES];
char *module_name[MAX_MODULES];
char *module_fname[MAX_MODULES];
int   module_hasoutputfile[MAX_MODULES];
int   modulesnumber;

Fl_Window *startupwindow = NULL;
Fl_Window *mainwindow = NULL;
Fl_Value_Slider *choosen_x_resolution = NULL;
Fl_Value_Slider *choosen_y_resolution = NULL;
Fl_Window *help_win = NULL;
Fl_Help_View* help_document = NULL;
DrawingBox *drawingbox = NULL;
Fl_Window *modulewindow;
Fl_Hold_Browser *modulebrowser = NULL;

//// Globals - END ////
///////////////////////



/////////////////////////////////////
//// Drawing Box methods - BEGIN ////

DrawingBox :: DrawingBox(int x, int y, int w, int h) : Fl_Box(x, y, w, h, NULL) {
    x_res = 2;      // initial dummy value
    y_res = 2;      // initial dummy value
    mapstep = 0;    // initial dummy value
}

DrawingBox :: ~DrawingBox() { }

void DrawingBox :: draw() {

    // grid
    fl_color(FL_BLACK);
    if (mapstep == 0 ) {
        float step_x = (BOX_WIDTH * 1.0f) / x_res;
        float step_y = (BOX_HEIGHT * 1.0f) / y_res;
        float step = (step_x > step_y) ? step_y : step_x;
        mapstep = step;
    }
    for (int i = 0; i < x_res; i++) {
        int x0 = (int) (BOX_BEGIN_X + i * mapstep);
        int y1 = (int) (BOX_BEGIN_Y + mapstep * y_res);
        fl_line(x0, BOX_BEGIN_Y, x0, y1);
    }
    for (int i = 0; i < y_res; i++) {
        int y0 = (int) (BOX_BEGIN_Y + i * mapstep);
        int x1 = (int) (BOX_BEGIN_X + mapstep * x_res);
        fl_line(BOX_BEGIN_X, y0, x1, y0);
    }

    // great pixels
    int r = (int) (mapstep - 2);
    int px, py;
    for (int x = 0; x < x_res; x++) {
        for (int y = 0; y < y_res; y++) {
            fl_color((spritemap[y][x] != PIXEL_ON) ? FL_WHITE: FL_BLUE);
            px = 1 + (int) (x * mapstep + BOX_BEGIN_X);
            py = 1 + (int) (y * mapstep + BOX_BEGIN_Y);
            fl_rectf(px, py, r, r);
        }
    }

    // border
    int x0 = (int) (BOX_BEGIN_X + mapstep * x_res);
    int y0 = (int) (BOX_BEGIN_Y + mapstep * y_res);
    fl_color(FL_DARK_BLUE);
    fl_line(BOX_BEGIN_X,    BOX_BEGIN_Y,    BOX_BEGIN_X,    y0);
    fl_line(BOX_BEGIN_X,    BOX_BEGIN_Y,    x0,             BOX_BEGIN_Y);
    fl_line(x0,             y0,             BOX_BEGIN_X,    y0);
    fl_line(x0,             y0,             x0,             BOX_BEGIN_Y);
}

int DrawingBox :: handle(int e) {
    if (e == FL_PUSH) {
        int x = (int) ((Fl::event_x() - BOX_BEGIN_X ) / mapstep);
        int y = (int) ((Fl::event_y() - BOX_BEGIN_Y ) / mapstep);
        spritemap[y][x] = (spritemap[y][x] != PIXEL_ON) ? PIXEL_ON : PIXEL_OFF;
        redraw();
        return -1;
    } else {
        return 0;
    }
}

void DrawingBox :: setXRes(int x) { x_res = (x < MAX_X_RES) ? x : MAX_X_RES; }
void DrawingBox :: setYRes(int y) { y_res = (y < MAX_Y_RES) ? y : MAX_Y_RES; }
void DrawingBox :: setXYRes(int x, int y) { setXRes(x); setYRes(y); }
int DrawingBox :: getXRes() { return x_res; }
int DrawingBox :: getYRes() { return y_res; }

//// Drawing Box methods - END ////
///////////////////////////////////



////////////////////////////////////////////
//// Application initialisation - BEGIN ////

int applicationinit() {
    int error = 0;
    char fname[FL_PATH_MAX * 2 + 1];
    getconfigurationfilename(fname);
    error = createifabsent(fname);
    if (!error) {
        for (int y = 0; y < MAX_Y_RES; y++) {
            spritemap[y] = (char *) malloc((MAX_X_RES + 1) * sizeof(char));
            for (int x = 0; x < MAX_X_RES; x++) spritemap[y][x] = PIXEL_OFF;
            spritemap[y][MAX_X_RES] = '\0';
        }
        for (int y = 0; y < MAX_MODULES; y++) {
            module_name[y] = (char *) malloc((MODULE_DESCR_LEN + 1) * sizeof(char));
            module_name[y][0] = '\0';
            module_fname[y] = (char *) malloc((FL_PATH_MAX * 2 + 1) * sizeof(char));
            module_fname[y][0] = '\0';
            module_hasoutputfile[y] = 0;
        }
    }
    if (loadmodules(module_name, module_fname, module_hasoutputfile, &modulesnumber)) error = 1;
    return error;
}

//// Application initialisation - END ////
//////////////////////////////////////////



///////////////////////////////////////////
//// Main for windows creation - BEGIN ////

int main(int argc, char* argv[]) {

    fl_message_font(FL_HELVETICA, 12);
    Fl::scheme("plastic");

    if (applicationinit()) {
        fl_alert("ERROR creating the configuraion file.\nThe application cannot start.");
        exit(EXIT_FAILURE);
    }


    /* if (argc > 1)... ,  manage an input sprite file */

    // ------- start window

    Fl_Window *sw = new Fl_Window(530, 145, WINDOW_TITLE);
    startupwindow = sw;
    sw->begin();
    Fl_Value_Slider *xreschooser = new Fl_Value_Slider(10, 20, 510, 20, "X resoluion (in pixels):");
    xreschooser->textfont(FL_HELVETICA_BOLD);
    xreschooser->type(FL_HOR_NICE_SLIDER);
    xreschooser->align(FL_ALIGN_TOP_LEFT);
    xreschooser->labelsize(12);
    xreschooser->textsize(14);
    xreschooser->bounds(3.0, (double) (MAX_X_RES));
    xreschooser->precision(0);
    xreschooser->increment(1.0, 1);
    xreschooser->value(26);
    choosen_x_resolution = xreschooser;
    Fl_Value_Slider *yreschooser = new Fl_Value_Slider(10, 70, 510, 20, "Y resoluion (in pixels):");
    yreschooser->textfont(FL_HELVETICA_BOLD);
    yreschooser->type(FL_HOR_NICE_SLIDER);
    yreschooser->align(FL_ALIGN_TOP_LEFT);
    yreschooser->labelsize(12);
    yreschooser->textsize(14);
    yreschooser->bounds(3.0, (double) (MAX_Y_RES));
    yreschooser->precision(0);
    yreschooser->increment(1.0, 1);
    yreschooser->value(20);
    choosen_y_resolution = yreschooser;
    Fl_Button *btn_go = new Fl_Button(10, 110, 120, 25, "New sprite");
    btn_go->labelsize(12);
    btn_go->callback(cb_start);
    Fl_Button *btn_load = new Fl_Button(140, 110, 120, 25, "Load from file");
    btn_load->labelsize(12);
    btn_load->callback(cb_load);
    Fl_Button *btn_help = new Fl_Button(270, 110, 120, 25, "Help");
    btn_help->labelsize(12);
    btn_help->callback(cb_help);
    Fl_Button *btn_license = new Fl_Button(400, 110, 120, 25, "License");
    btn_license->labelsize(12);
    btn_license->callback(cb_license);
    sw->end();
    sw->callback((Fl_Callback *) cb_esc_key_window);
    #if defined(__WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    sw->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
    #endif
    sw->show(1, argv);

    // ------- help window

    help_win = new Fl_Window(700, 500, "Help window for " WINDOW_TITLE);
    help_win->labelsize(12);
    help_document = new Fl_Help_View(5, 5, 690, 490);
    help_document->textfont(FL_HELVETICA);
    help_document->textcolor(FL_DARK_BLUE);
    help_document->color(
        fl_color_cube(
            255 * (FL_NUM_RED   - 1) / 255,
            255 * (FL_NUM_GREEN - 1) / 255,
            180 * (FL_NUM_BLUE  - 1) / 255)
    );
    help_win->end();

    // ------- main window

    Fl_Color btnsymbolcolor = fl_color_cube(
            180 * (FL_NUM_RED   - 1) / 255,
            180 * (FL_NUM_GREEN - 1) / 255,
            180 * (FL_NUM_BLUE  - 1) / 255);

    Fl_Window *w = new Fl_Window(WINDOW_W, WINDOW_H, WINDOW_TITLE);
    w->begin();
    w->labelsize(12);
    Fl_Box* border = new Fl_Box(BOX_BEGIN_X - 5, BOX_BEGIN_Y - 5 , BOX_WIDTH + 10, BOX_HEIGHT + 10);
    border->box(FL_EMBOSSED_BOX);
    drawingbox = new DrawingBox(BOX_BEGIN_X, BOX_BEGIN_Y, BOX_WIDTH, BOX_HEIGHT);
    border->callback(cb_clicktest);
    Fl_Button *btn_rotate = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y - 5, 45, 25, "@-1redo");
    btn_rotate->labelcolor(btnsymbolcolor);
    btn_rotate->tooltip("rotate");
    btn_rotate->callback(cb_rotate);
    Fl_Button *btn_invert = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 70, BOX_BEGIN_Y - 5, 45, 25, "@-31square");
    btn_invert->labelcolor(btnsymbolcolor);
    btn_invert->tooltip("invert pixels (negative image)");
    btn_invert->callback(cb_invert);
    Fl_Button *btn_shiftleft = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y + 25, 45, 25, "@-14->");
    btn_shiftleft->labelcolor(btnsymbolcolor);
    btn_shiftleft->tooltip("shift left");
    btn_shiftleft->callback(cb_shiftleft);
    Fl_Button *btn_shiftright = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 70, BOX_BEGIN_Y + 25, 45, 25, "@-1->");
    btn_shiftright->labelcolor(btnsymbolcolor);
    btn_shiftright->tooltip("shift right");
    btn_shiftright->callback(cb_shiftright);
    Fl_Button *btn_shiftup = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y + 55, 45, 25, "@-18->");
    btn_shiftup->labelcolor(btnsymbolcolor);
    btn_shiftup->tooltip("move up");
    btn_shiftup->callback(cb_shiftup);
    Fl_Button *btn_shiftdown = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 70, BOX_BEGIN_Y + 55, 45, 25, "@-12->");
    btn_shiftdown->labelcolor(btnsymbolcolor);
    btn_shiftdown->tooltip("move down");
    btn_shiftdown->callback(cb_shiftdown);
    Fl_Button *btn_hflip = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y + 85, 45, 25, "@-1>[]");
    btn_hflip->labelcolor(btnsymbolcolor);
    btn_hflip->tooltip("flip horizontally");
    btn_hflip->callback(cb_hflip);
    Fl_Button *btn_vflip = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 70, BOX_BEGIN_Y + 85, 45, 25, "@-18>[]");
    btn_vflip->labelcolor(btnsymbolcolor);
    btn_vflip->tooltip("flip vertically");
    btn_vflip->callback(cb_vflip);

    Fl_Button *btn_externalmodules = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y + 130, 105, 50, "External\nmodules");
    btn_externalmodules->labelsize(12);
    btn_externalmodules->callback(cb_externalmodules);
    Fl_Button *btn_save = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y + 195, 105, 50, "Save the\nsprite...");
    btn_save->labelsize(12);
    btn_save->callback(cb_save);
    Fl_Button *btn_generate = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, BOX_BEGIN_Y + 260, 105, 50, "Generate\nC code...");
    btn_generate->labelsize(12);
    btn_generate->callback(cb_generate);

    Fl_Button *btn_mainhelp = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, (BOX_BEGIN_Y - 5) + 330, 105, 50, "Help");
    btn_mainhelp->labelsize(12);
    btn_mainhelp->callback(cb_help);
    Fl_Button *btn_mainlicense = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, (BOX_BEGIN_Y - 5) + 395, 105, 50, "License");
    btn_mainlicense->labelsize(12);
    btn_mainlicense->callback(cb_license);

    Fl_Button *btn_mainquit = new Fl_Button(BOX_BEGIN_X + BOX_WIDTH + 10, (BOX_BEGIN_Y - 5) + 460, 105, 50, "Exit");
    btn_mainquit->labelsize(12);
    btn_mainquit->callback(cb_quit);
    w->callback((Fl_Callback *) cb_no_esc_key_window);
    w->end();
    mainwindow = w;


    // ------- module window

    modulewindow = new Fl_Window(600, 175, "Spedito Module Manager");
    modulebrowser = new Fl_Hold_Browser(5, 5, 490, 165);
    redrawbrowser();
    Fl_Button *btn_run = new Fl_Button(500, 5, 95, 25, "Run");
    btn_run->labelsize(12);
    btn_run->callback(cb_runmodule);
    Fl_Button *btn_info = new Fl_Button(500, 40, 95, 25, "Info");
    btn_info->labelsize(12);
    btn_info->tooltip("view selected module configuration");
    btn_info->callback(cb_infomodule);
    Fl_Button *btn_add = new Fl_Button(500, 75, 95, 25, "Install");
    btn_add->labelsize(12);
    btn_add->tooltip("configure a new module");
    btn_add->callback(cb_addmodule);
    Fl_Button *btn_del = new Fl_Button(500, 110, 95, 25, "Remove");
    btn_del->labelsize(12);
    btn_del->tooltip("remove the selected module");
    btn_del->callback(cb_delmodule);
    Fl_Button *btn_close = new Fl_Button(500, 145, 95, 25, "Close");
    btn_close->labelsize(12);
    btn_close->callback(cb_closemodulewindow);
    modulewindow->end();
    modulewindow->set_modal();

    // -------
    // -------

    Fl::run();
    return EXIT_SUCCESS;
}

//// Main for windows creation - END ////
/////////////////////////////////////////
