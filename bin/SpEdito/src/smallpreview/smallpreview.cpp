// SpEdito Small Preview Module
//
// Copyright 2005-2009 Marcello Zaniboni
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <malloc.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

#define APPLICATION_FULL_NAME "SpEdito Small Preview Module - v. 1.0"

#define MAX_X_RES 200
#define MAX_Y_RES 120
#define PIXEL_OFF '0'
#define PIXEL_ON  '1'

char *spritemap[MAX_Y_RES];
int xres = 0;
int yres = 0;

class PreviewBox : public Fl_Box {

    public:
        PreviewBox(int x, int y, int w, int h, const char* t=0) : Fl_Box(x, y, w, h, t) { }

        ~PreviewBox() { }

        void draw() {
            int x0 = x(), y0 = y();
            fl_draw_box(FL_BORDER_BOX, x0, y0, w(), h(), FL_LIGHT1);
            for (int x = 0; x < xres; x++) {
                for (int y = 0; y < yres; y++) {
                    int xx = 2 * x + 1 + x0;
                    int yy = 2 * y + 1 + y0;
                    fl_color(spritemap[y][x] == PIXEL_ON ? FL_DARK_GREEN : FL_WHITE);
                    fl_point(xx, yy);
                    fl_point(xx + 1, yy);
                    fl_point(xx, yy + 1);
                    fl_point(xx + 1, yy + 1);
                }
            }
        }

};


int init(char *filename) {
    // init memory
    for (int c = 0; c < MAX_Y_RES; c++) {
        spritemap[c] = (char *) malloc((MAX_X_RES + 1) * sizeof(char));
        spritemap[c][0] = '\0';
    }
    // read data
    FILE *datafile;
    char line[MAX_X_RES * 2 + 1];
    if (!(datafile = fopen(filename, "r"))) return 1;
    int c = 0;
    while (fgets(line, MAX_X_RES * 2, datafile) && c < MAX_Y_RES + 2) {
        for (int i = 0; i < strlen(line); i++) if (line[i] == '\n' || line[i] == '\r') line[i] = '\0';
        if (c == 0) {
            // read x resolution
            xres = atoi(line);
            if (xres < 3 || xres > MAX_X_RES) return 1;
        } else if (c == 1) {
            // read x resolution
            yres = atoi(line);
            if (yres < 3 || yres > MAX_Y_RES) return 1;
        } else {
            // read one sprite row
            if (strlen(line) != xres) {
                fl_message("file row %d, invalid length: %d", c, strlen(line));
                return 1;
            }
            for (int i = 0; i < strlen(line); i++)
                if (line[i] != PIXEL_ON && line[i] != PIXEL_OFF) {
                    fl_message("file row %d, column %d, invalid character", c + 1, i + 1);
                    return 1;
                }
            strncpy(spritemap[c - 2], line, MAX_X_RES);
        }
        c++;
    }
    fclose(datafile);
    remove(filename);
    return 0;
}


int main(int argc, char ** argv) {

    // initial validation
    if (argc != 2) {
        char *errormessage =
            APPLICATION_FULL_NAME
            "\n\tthe module is not correctly configured: please"
            "\n\tprovide the data file only (i.e. one parameter).";
        fprintf(stderr, "%s\n", errormessage);
        fl_alert(errormessage);
        exit(EXIT_FAILURE);
    }
    if (init(argv[1])) {
        char *errormessage =
            APPLICATION_FULL_NAME
            "\n\tthis module cannot open the data file or"
            "\n\tthe file has a wrong format.";
        fprintf(stderr, "%s\n", errormessage);
        fl_alert(errormessage);
        exit(EXIT_FAILURE);
    }

    // window strings
    char spriteinfo[128];
    char currenttime[128];
    char timeinfo[128];
    struct tm *ptr;
    time_t tm;
    sprintf(spriteinfo, "Sprite size: %d x %d", xres, yres);
    tm = time(NULL);
    ptr = localtime(&tm);
    strftime(currenttime, 100, "%Y-%m-%d %H:%M:%S", ptr);
    strcpy(timeinfo, "preview time: ");
    strcat(timeinfo, currenttime);

    // user interface
    int spriteboxh = 2 * yres + 2;
    int spriteboxw = 2 * xres + 2;
    int windowh = 55 + spriteboxh + 10;
    int windoww = 10 + spriteboxw + 10 > 300 ? 10 + spriteboxw + 10 : 300;
	Fl_Window *window = new Fl_Window(windoww, windowh, APPLICATION_FULL_NAME);
    Fl_Box lbl_info(10, 10, 190, 20, spriteinfo);
    lbl_info.align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
    lbl_info.labelsize(12);
    Fl_Box lbl_time(10, 30, 190, 20, timeinfo);
    lbl_time.align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
	PreviewBox *previewbox = new PreviewBox(12, 55, spriteboxw, spriteboxh);
    lbl_time.labelsize(12);

	window->end();
	window->show(1, argv);

	return(Fl::run());
}
