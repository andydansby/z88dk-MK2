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


#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(__WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#endif

#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>

#include "fileutils.h"
#include "spedito.h"

void pathmanipulator(char *fname) {
    #if defined(__WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        char tmp[MAX_PATH + 1];
        for (int i = 0; i < strlen(fname); i++)
            if (fname[i] == '/') fname[i] = '\\';
        if (GetShortPathName(fname, tmp, MAX_PATH)) strncpy(fname, tmp, MAX_PATH);
    #else
        char tmp[FL_PATH_MAX * 2 + 1];
        strcpy(tmp, "\"");
        strcat(tmp, fname);
        strcat(tmp, "\"");
        strcpy(fname, tmp);
    #endif
}

void inline removenewline(char *line) {
    for (int i = 0; i < strlen(line); i++) if (line[i] == '\n' || line[i] == '\r') line[i] = '\0';
}

void getconfigurationfilename(char *fname) {
    /*
    char *home;
    char dir[1024];
    int currentdir = 0;
    home = getenv("HOME");
    if (!home) { // not Linux/Unix/etc...
        home = getenv("HOMEDRIVE");
        if (!home) {
            currentdir = 1;
        } else {
            strcpy(dir, home);
            home = getenv("HOMEPATH");
            if (!home) {
                currentdir = 1;
            } else {
                strcat(dir, home);
                strcat(dir, "\\");
            }
        }
    } else { // Linux/Unix/etc...
        strcpy(dir, home);
        strcat(dir, "/");
    }
    strcpy(fname, currentdir ? "" : dir);
    strcat(fname, ".spedito");
    */
    char path[FL_PATH_MAX * 2 + 1];
    Fl_Preferences prefs(Fl_Preferences::USER, "spedito", "modules");
    prefs.getUserdataPath(path, FL_PATH_MAX);
    strncpy(fname, path, FL_PATH_MAX * 2);
    strcat(fname, "config.dat");
    #if defined(__WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        pathmanipulator(fname);
    #endif
}

int createifabsent(char *fname) {
    FILE *configfile;
    int errorcode = 0;
    if (configfile = fopen(fname, "r")) {
        fclose(configfile);
    } else {
        if (configfile = fopen(fname, "w")) {
            printf("Configuration file has been created:\n\t\"%s\"\n", fname);
            fclose(configfile);
        } else {
            printf("ERROR: cannot create configuraion file\n\t\"%s\"\n", fname);
            errorcode = 1;
        }
    }
    return errorcode;
}

int loadmodules(char *module_name[], char *module_fname[], int module_hasoutputfile[], int *modulesnumber) {
    FILE *configfile;
    char fname[FL_PATH_MAX * 2];
    char line[MODULE_LINE_MAX_LEN + 1];

    int error = 0;
    getconfigurationfilename(fname);
    error = createifabsent(fname);
    if (error) return ERROR_SEARCHING_CONF_FILE;
    if (!(configfile = fopen(fname, "r"))) return ERROR_READING_CONF_FILE;
    int i = 0;
    while (fgets(line, MODULE_LINE_MAX_LEN, configfile) && i < MAX_MODULES) {
        removenewline(line);
        strncpy(module_name[i], line, MODULE_DESCR_LEN);
        if (!(fgets(line, MODULE_LINE_MAX_LEN, configfile))) return ERROR_IN_CONF_FILE_EXEC_MISSING;
        removenewline(line);
        strncpy(module_fname[i], line, FL_PATH_MAX * 2);
        if (!(fgets(line, MODULE_LINE_MAX_LEN, configfile))) return ERROR_IN_CONF_FILE_OUTF_MISSING;
        removenewline(line);
        module_hasoutputfile[i] = (line[0] == '1') ? 1 : 0;
        i++;
    }
    fclose(configfile);
    *modulesnumber = i;
    return 0;
}

void saveconfigurationfile(char *module_name[], char *module_fname[], int module_hasoutputfile[], int *modulesnumber) {
    FILE *configfile;
    char configfilename[FL_PATH_MAX * 2];
    getconfigurationfilename(configfilename);
    if (!(configfile = fopen(configfilename, "w"))) {
        fl_alert("Error saving confguration!\n(file name: \"%s\")", configfilename);
        return;
    }
    for (int i = 0; i < (*modulesnumber); i++) {
        fprintf(configfile, "%s\n", module_name[i]);
        fprintf(configfile, "%s\n", module_fname[i]);
        fprintf(configfile, "%d\n", module_hasoutputfile[i]);
    }
    fclose(configfile);
}

int savemoduledatafile(char *spritemap[], int xres, int yres) {
    int error = 0;
    FILE *datafile;
    if (!(datafile = fopen(MODULE_DATA_FILENAME, "w"))) {
        fl_alert("Error creating module data file!\n(file name: \"%s\")", MODULE_DATA_FILENAME);
        error = 1;
    } else {
        fprintf(datafile, "%d\n%d\n", xres, yres);
        for (int r = 0; r < yres; r++) {
            for (int c = 0; c < xres; c++)
                fputc(spritemap[r][c] == PIXEL_ON ? '1' : '0', datafile);
            fprintf(datafile, "\n");
        }
        fclose(datafile);
    }
    return error;
}
