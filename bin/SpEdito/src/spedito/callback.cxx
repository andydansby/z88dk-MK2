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
#include "helpstrings.h"
#include "fileutils.h"

extern char *spritemap[MAX_Y_RES];
extern char *module_name[MAX_MODULES];
extern char *module_fname[MAX_MODULES];
extern int   module_hasoutputfile[MAX_MODULES];
extern int   modulesnumber;
extern Fl_Window *startupwindow;
extern Fl_Window *mainwindow;
extern Fl_Value_Slider *choosen_x_resolution;
extern Fl_Value_Slider *choosen_y_resolution;
extern Fl_Window *help_win;
extern Fl_Help_View* help_document;
extern DrawingBox *drawingbox;
extern Fl_Window *modulewindow;
extern Fl_Hold_Browser *modulebrowser;


void cb_shiftleft(Fl_Widget *o, void *) {
    for (int y = 0; y < MAX_Y_RES; y++) {
        for (int x = 1; x < MAX_X_RES; x++)
            spritemap[y][x - 1] = spritemap[y][x];
        spritemap[y][MAX_X_RES - 1] = PIXEL_OFF;
    }
    drawingbox->redraw();
}

void cb_shiftright(Fl_Widget *o, void *) {
    for (int y = 0; y < MAX_Y_RES - 1; y++) {
        for (int x = MAX_X_RES - 1; x > 0 ; x--)
            spritemap[y][x] = spritemap[y][x - 1];
        spritemap[y][0] = PIXEL_OFF;
    }
    drawingbox->redraw();
}

void cb_shiftup(Fl_Widget *o, void *) {
    for (int y = 0; y < MAX_Y_RES - 1; y++)
        strcpy(spritemap[y], spritemap[y + 1]);
    for (int x = MAX_X_RES; x > 0 ; x--)
        spritemap[MAX_Y_RES - 1][x] = PIXEL_OFF;
    drawingbox->redraw();
}

void cb_shiftdown(Fl_Widget *o, void *) {
    for (int y = MAX_Y_RES - 1; y > 0; y--)
        strcpy(spritemap[y], spritemap[y - 1]);
    for (int x = MAX_X_RES - 1; x >= 0 ; x--)
        spritemap[0][x] = PIXEL_OFF;
    drawingbox->redraw();
}

void cb_invert(Fl_Widget *o, void *) {
    for (int y = 0; y < MAX_Y_RES; y++)
        for (int x = 0; x < MAX_X_RES; x++)
            spritemap[y][x] = (spritemap[y][x] == PIXEL_OFF) ? PIXEL_ON : PIXEL_OFF;
    drawingbox->redraw();
}

void cb_hflip(Fl_Widget *o, void *) {
    char tmp;
    for(int y0 = 0; y0 < drawingbox->getYRes(); y0++) {
        for (int x0 = 0, x1 = drawingbox->getXRes() - 1; x0 < x1; x0++, x1--) {
            tmp = spritemap[y0][x1];
            spritemap[y0][x1] = spritemap[y0][x0];
            spritemap[y0][x0] = tmp;
        }
    }
    drawingbox->redraw();
}

void cb_vflip(Fl_Widget *o, void *) {
    char tline[MAX_X_RES + 1];
    for (int y0 = 0, y1 = drawingbox->getYRes() - 1; y0 < y1; y0++, y1--) {
        strncpy(tline, spritemap[y0], MAX_X_RES);
        strncpy(spritemap[y0], spritemap[y1], MAX_X_RES);
        strncpy(spritemap[y1], tline, MAX_X_RES);
    }
    drawingbox->redraw();
}

void cb_rotate(Fl_Widget *o, void *data) {
    int resx = drawingbox->getXRes();
    int resy = drawingbox->getYRes();
    int res = (resx > resy) ? resy : resx;
    char tmp;
    // first flip diagonally
    for (int c = 0; c < res; c++) {
        for (int r = c; r < res; r++) {
            tmp = spritemap[r][c];
            spritemap[r][c] = spritemap[c][r];
            spritemap[c][r] = tmp;
        }
    }
    // then flip hotizontally
    for(int y0 = 0; y0 < res; y0++) {
        for (int x0 = 0, x1 = res - 1; x0 < x1; x0++, x1--) {
            tmp = spritemap[y0][x1];
            spritemap[y0][x1] = spritemap[y0][x0];
            spritemap[y0][x0] = tmp;
        }
    }
    drawingbox->redraw();
}

void cb_start(Fl_Widget *o, void *) {
    drawingbox->setXYRes((int) choosen_x_resolution->value(), (int) choosen_y_resolution->value());
    mainwindow->show();
    startupwindow->hide();
}

void cb_load(Fl_Widget *o, void *) {
    char *fname = fl_file_chooser(
        "Select the file containing a saved sprite", "*", ".", 0);
    if (fname != NULL) {
        FILE *inputfile;
        if (inputfile = fopen(fname, "r")) {
            int xres, yres;
            char line[MAX_X_RES + 1];
            fgets(line, MAX_X_RES, inputfile);
            if (!strncmp(line, FILE_FORMAT_VERSION, strlen(FILE_FORMAT_VERSION))) {
                fscanf(inputfile, "%d%d", &xres, &yres);
                choosen_x_resolution->value(xres);
                choosen_y_resolution->value(yres);
                int y = 0, formaterror = 0;
                while(y < MAX_X_RES && 0 < fscanf(inputfile, "%s", line)) {
                    if (strlen(line) == MAX_X_RES) {
                        if (y > MAX_Y_RES) {
                            formaterror = 1;
                            break;
                        }
                        strcpy(spritemap[y], line);
                        y++;
                    } else {
                        formaterror = 2;
                        break;
                    }
                }
                if (!formaterror)
                    cb_start(o, NULL);
                else
                    fl_alert("Format error in file\n%s\n%s",
                        fname, 
                        (formaterror == 1 ? 
                            "The number of lines is not correct." :
                            "Line length is not correct."));
            } else {
                fl_alert("Unsupported format version in file\n%s", fname);
            }
            fclose(inputfile);
        } else {
            fl_alert("Unable to open file\n%s", fname);
        }
    }
}

void cb_help(Fl_Widget *o, void *) {
    help_document->value(helpstring);
    help_document->textsize(16);
    help_win->show();
}

void cb_license(Fl_Widget *o, void *) {
    help_document->value(gnugplstring);
    help_document->textsize(12);
    help_win->show();
}

void cb_save(Fl_Widget *o, void *) {
    char *fname = fl_file_chooser(
        "Input the file name for saving the sprite", "*", ".", 0);
    if (fname != NULL) {
        FILE *outfile;
        if (outfile = fopen(fname, "w")) {
            fprintf(outfile, "%s\n", FILE_FORMAT_VERSION);
            fprintf(outfile, "%d %d\n", drawingbox->getXRes(), drawingbox->getYRes());
            for (int y = 0; y < MAX_Y_RES; y++)
                fprintf(outfile, "%s\n", spritemap[y]);
            fclose(outfile);
        } else {
            fl_alert("Error saving to file\n%s", fname);
        }
    }
}

void cb_generate(Fl_Widget *o, void *) {
    char *fname = fl_file_chooser(
        "Input the file name for the generated C code", "*", ".", 0);
    if (fname != NULL) {
        FILE *outfile;
        if (outfile = fopen(fname, "w")) {
            int bytesperrow, bitfill;
            char byte[9];
            bitfill = drawingbox->getXRes() % 8;
            bytesperrow = drawingbox->getXRes() / 8;
            if (bitfill) bytesperrow++;
            byte[8] = '\0';
            fprintf(outfile, "char sprite[] = { %d, %d,\n", drawingbox->getXRes(), drawingbox->getYRes());
            for (int y = 0; y < drawingbox->getYRes(); y++) {
                // fill every copy of the strings with empty bits
                char filledrow[MAX_Y_RES + 9];
                char onefillbit[2];
                onefillbit[0] = PIXEL_OFF; onefillbit[1] = '\0';
                filledrow[0] = '\0';
                strcpy(filledrow, spritemap[y]);
                for (int i = 0; i < bitfill; i++) strcat(filledrow, onefillbit);
                // for each byte
                for (int b = 0; b < bytesperrow; b++) {
                    // extract the byte
                    for (int i = 0; i < 8; i++) byte[i] = filledrow[i + b * 8]; // spritemap[y][i + b * 8];
                    // convert it to a numeric
                    int n = 0, p = 1;
                    for (int i = 7; i >= 0; i--, p *= 2) n += (byte[i] == PIXEL_ON) ? 1 * p : 0;
                    fprintf(outfile, "  0x%02x /* %s */", n, byte);
                    if (!(y == (drawingbox->getYRes() - 1) && b == (bytesperrow - 1))) fprintf(outfile, ",");
                }
                fprintf(outfile, "\n");
            }
            fprintf(outfile, "};\n");
            fclose(outfile);
        } else {
            fl_alert("Error saving to file\n%s", fname);
        }
    }
}

void cb_externalmodules(Fl_Widget *o, void *) {
    modulewindow->show();
}

void cb_quit(Fl_Widget *o, void *) {
    while(Fl::first_window()) Fl::first_window()->hide();
    remove(MODULE_DATA_FILENAME); // ...modules should do this!
    exit(EXIT_SUCCESS);
}

void cb_clicktest(Fl_Widget *o , void *v) {
    fl_message("click");
    fl_message("%d %d", Fl::event_x(), Fl::event_y());
}

void cb_no_esc_key_window(Fl_Widget *o, void *data) {
    if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) return;
    cb_quit(o, data);
}

void cb_esc_key_window(Fl_Widget *o, void *data) {
    cb_quit(o, data);
}

void cb_notimplemented(Fl_Widget *o, void *) {
    fl_message("Not implemented! :-)");
}


// --------


void redrawbrowser() {
    modulebrowser->clear();
    for (int i = 0; i < modulesnumber; i++) {
        modulebrowser->add(module_name[i]);
    }
    modulebrowser->redraw();
}

void cb_runmodule(Fl_Widget *o, void *data) {
    int selected = modulebrowser->value();
    if (selected) {
        char command[MODULE_LINE_MAX_LEN];
        int i = selected - 1;
        int ok = 1;
        command[0]='\0';
        strcpy(command, MODULE_COMMAND_PREFIX);
        strcat(command, module_fname[i]);
        strcat(command, " ");
        strcat(command, MODULE_DATA_FILENAME);
        if (module_hasoutputfile[i]) {
            char *fname = fl_file_chooser("Select the output file", "*", ".", 0);
            if (fname == NULL) {
                ok = 0;
            } else {
                char outputfilename[FL_PATH_MAX * 2 + 1];
                strncpy(outputfilename, fname, FL_PATH_MAX * 2);
                pathmanipulator(outputfilename);
                strcat(command, " ");
                strcat(command, outputfilename);
            }
        }
        if (ok) {
            if (savemoduledatafile(spritemap, drawingbox->getXRes(), drawingbox->getYRes())) {
                return;
            }
            strcat(command, MODULE_COMMAND_SUFFIX);
            // fl_message("%s", command);
            system(command);
            modulewindow->redraw();
            // modulewindow->take_focus();
            Fl::flush();
        }
    }
}

void cb_infomodule(Fl_Widget *o, void *data) {
    int selected = modulebrowser->value();
    if (selected) {
        int i = selected - 1;
        fl_message("module name: \"%s\"\nexecutable: %s\nneeds output file name = %s",
            module_name[i],
            module_fname[i],
            module_hasoutputfile[i] ? "yes" : "no");
    }
}

void cb_addmodule(Fl_Widget *o, void *data) {
    char name[MODULE_DESCR_LEN + 1];
    char fname[FL_PATH_MAX * 2 + 1];
    const char *userinput;
    char *filename;
    int of;

    userinput = fl_input("Enter the module description", "");
    if ((!userinput) || strlen(userinput) == 0) return;
    strncpy(name, userinput, MODULE_DESCR_LEN);
    filename = fl_file_chooser("Select the module executable", "*", ".", 0);
    if ((!filename) || strlen(filename) == 0) return;
    strncpy(fname, filename, FL_PATH_MAX * 2);
    pathmanipulator(fname);
    of = fl_choice("Does this module need output file name?", NULL, "No", "Yes");
    if (!of) return;
    // add the module
    strncpy(module_name[modulesnumber], name, MODULE_DESCR_LEN);
    strncpy(module_fname[modulesnumber], fname, FL_PATH_MAX * 2);
    module_hasoutputfile[modulesnumber] = of - 1;
    modulesnumber++;
    redrawbrowser();
    saveconfigurationfile(module_name, module_fname, module_hasoutputfile, &modulesnumber);
}

void cb_delmodule(Fl_Widget *o, void *data) {
    int selected = modulebrowser->value();
    if (selected) {
        int delendum = selected - 1;
        int confirm = fl_choice("You are going to remove the configuration for the selected module.\nAre you sure?", NULL, "No", "Yes");
        if (confirm != 2) return; // esc key or dialog closed or answer is no
        modulesnumber--;
        for (int i = delendum; i < modulesnumber; i++) {
            strcpy(module_name[i], module_name[i + 1]);
            strcpy(module_fname[i], module_fname[i + 1]);
            module_hasoutputfile[i] = module_hasoutputfile[i + 1];
        }
    }
    redrawbrowser();
    saveconfigurationfile(module_name, module_fname, module_hasoutputfile, &modulesnumber);
}

void cb_closemodulewindow(Fl_Widget *o, void *data) {
    modulewindow->hide();
}

