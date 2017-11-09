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


#ifndef spedito_h
#define spedito_h

#define MAX_X_RES 78
#define MAX_Y_RES 60
#define PIXEL_OFF '.'
#define PIXEL_ON  '#'
#define FILE_FORMAT_VERSION "1.0"

#define WINDOW_TITLE "SpEdito - Sprite Editor for z88dk - v. 1.0"
#define BOX_BEGIN_X 10
#define BOX_BEGIN_Y 10
#define BOX_WIDTH 650
#define BOX_HEIGHT 500
#define WINDOW_W 780
#define WINDOW_H 520




#define CONFIG_FILENAME ".spedito"
#define MODULE_DATA_FILENAME "spedito_module.dat"

#if defined(__WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define MODULE_COMMAND_PREFIX "start "
#define MODULE_COMMAND_SUFFIX ""
#else
#define MODULE_COMMAND_PREFIX ""
#define MODULE_COMMAND_SUFFIX " &"
#endif

#define MODULE_DESCR_LEN 512
#define MODULE_LINE_MAX_LEN 1024
#define MAX_MODULES 256

#define ERROR_SEARCHING_CONF_FILE 1
#define ERROR_READING_CONF_FILE 2
#define ERROR_IN_CONF_FILE_EXEC_MISSING 3
#define ERROR_IN_CONF_FILE_OUTF_MISSING 4

#endif
