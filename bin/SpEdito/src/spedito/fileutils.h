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


#ifndef fileutils_h
#define fileutils_h

void getconfigurationfilename(char *fname);
int createifabsent(char *fname);
void pathmanipulator(char *fname);
void inline removenewline(char *line);
int loadmodules(char *module_name[], char *module_fname[], int module_hasoutputfile[], int *modulesnumber);
void saveconfigurationfile(char *module_name[], char *module_fname[], int module_hasoutputfile[], int *modulesnumber);
int savemoduledatafile(char *spritemap[], int xres, int yres);

#endif
