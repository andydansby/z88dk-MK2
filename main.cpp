  /* *********************************  /
 /     Manual setup tool for z88dk     /
/ ********************************** */

// Copyright 2009-2011 Marcello Zaniboni
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2, with the following exceptions:
//  1) this software can be distributed within any official z88dk package,
//     source archive or installer, without affecting z88dk license terms;
//  2) when the software is distributed with z88dk, the whole source code
//     (and optionally the compiled binaries) of this software must be
//     included.
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
#include <cctype>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <unistd.h>
#include <windows.h>

#define USER_INPUT_LENGTH 1024
#define MAX_LINE_LENGTH 256
#define DEFAULT_SETUP_DIR "c:\\z88dk"
#define DEFAULT_CFG_DIR "\\lib\\config"
#define BATCH_FILE_NAME "z88dkenv.bat"
#define FILENAME_LENGTH MAX_PATH * 4


void getnearlyuniquesuffix(char *s) {
    struct tm *ptr;
    time_t t;
    char description[MAX_LINE_LENGTH];

    int a = rand();

    t = time(NULL);
    ptr = localtime(&t);
    strftime(description, 15, "%M%S", ptr);
    sprintf(s, "_id%s%d", description, a);
}

void exitwithpause(char *msg) {
    if (msg) fprintf(stderr, "%s\n", msg);
    if (system("PAUSE")) {
        printf("\nPlease wait some seconds.\n");
        Sleep(8000);
    }
    exit(EXIT_FAILURE);
}

void removenewline(char *line, unsigned int depth) {
    for (unsigned int i = 0; i < strlen(line) && i < depth; i++)
        if (line[i] == '\n' || line[i] == '\r') {
            line[i] = '\0';
            break;
        }
}

void getuserinput(char *s, unsigned int length) {
    if (!fgets(s, length, stdin)) {
        s[0] = '\0';
    } else {
        removenewline(s, length);
    }
}

bool evaluateyes(char *userinput) {
    bool response = false;
    if (userinput)
        if (strlen(userinput) == 1)
            response = (toupper(userinput[0]) == 'Y');
    return response;
}

// returns 1 if str ends with suffix
int str_ends_with(const char * str, const char * suffix) {

    if (str == NULL || suffix == NULL) return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if(suffix_len > str_len) return 0;
    return 0 == (strncmp( str + str_len - suffix_len, suffix, suffix_len ));
}


void documentation() {
    printf("\n");
    printf("+----------------------+\n");
    printf("|  z88dk manual setup  |\n");
    printf("|   version 2.0 beta   |\n");
    printf("+----------------------+\n\n");
    printf("(c) 2009-2011 Marcello Zaniboni (for bugs and comments, mzaniboni@hotmail.com)\n\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY. You may redistribute copies of\n");
    printf("this program under the terms of the GNU General Public License, version 2,\n");
    printf("with some exceptions (please read the file LICENSE.txt for more information).\n\n");
    printf("This program allows the installation of z88dk on Windows without the installer.\n");
    printf("1) Download the zip archive and extract it in the desired folder.\n");
    printf("2) Copy this program to that folder and run it.\n");
    printf("3) It will configure the compiler paths and generate " BATCH_FILE_NAME ".\n");
    printf("4) You can call " BATCH_FILE_NAME " from the command line; in case you are using a bat\n");
    printf("   for compiling your project, you can add a CALL statement to it.\n");
    printf("\n");
}

int backupfile(char *filename) {
    int returnvalue = 1;
    FILE *infile, *outfile;
    if ((infile = fopen(filename, "rb"))) {
        char outfilename[FILENAME_LENGTH];
        strcpy(outfilename, filename);
        strcat(outfilename, ".bak");
        if ((outfile = fopen(outfilename, "wb"))) {
            int c;
            while (EOF != (c = fgetc(infile))) fputc(c, outfile);
            fclose(outfile);
        } else {
            returnvalue = 0;
        }
        fclose(infile);
    } else {
        returnvalue = 0;
    }
    return returnvalue;
}

// replaces in text every occurrence of a with b, ignoring case; result in the fourth parameter
int replaceignoringcase(char *text, char *a, char *b, char *result) {

    char s[MAX_LINE_LENGTH]; // local text copy
    strcpy(s, text);
    char ss[MAX_LINE_LENGTH]; // local lower case text copy
    for (unsigned int i = 0; i < strlen(s); i++) ss[i] = tolower(s[i]);
    char aa[MAX_LINE_LENGTH]; // local lower case copy of a
    for (unsigned int i = 0; i < strlen(a); i++) aa[i] = tolower(a[i]);

    unsigned int resultlength = 0;
    unsigned int position = 0;
    int replacements = 0;
    char *occurrence = NULL;
    while ((occurrence = strstr(ss + position, a)) != NULL) {
        result[0] = '\0';
        position = (unsigned int) (occurrence - ss);
        // printf("\nss=%p occurrence=%p position=%u\n", ss, occurrence, position);

        for (unsigned int i = 0; i < position; i++) result[i] = s[i];
        resultlength = position;
        result[resultlength] = '\0';
        // printf("1) result(%d)=\"%s\"\n", resultlength, result);

        strcat(result, b);
        resultlength += strlen(b);
        // printf("2) result(%d)=\"%s\"\n", resultlength, result);

        strcat(result, s + position + strlen(a));
        resultlength += strlen(s + strlen(a));
        // printf("3) result(%d)=\"%s\"\n", resultlength, result);

        strcpy(s, result);
        strcpy(ss, result);
        for (unsigned int i = 0; i < strlen(s); i++) ss[i] = tolower(s[i]);
        replacements++;
    }
    if (!replacements) strcpy(result, text);
    //printf("replace result=\n  \"%s\"\n", result);
    return replacements;
}

int applynewpaths(char *outfilename, char *oldpath, char *newpath) {
    int returnvalue = 1;
    FILE *infile, *outfile;
    char infilename[FILENAME_LENGTH];
    strcpy(infilename, outfilename);
    strcat(infilename, ".bak");
    if ((infile = fopen(infilename, "r"))) {
        if ((outfile = fopen(outfilename, "w"))) {
            char inputline[MAX_LINE_LENGTH + 1];
            char outputline[MAX_LINE_LENGTH + 1];
            char slash[2];
            char backslash[2];
            char doublebackslash[3];
            strcpy(slash, "/");
            strcpy(backslash, "\\");
            strcpy(doublebackslash, "\\\\");
            int replacement_count = 0;
            while (fgets(inputline, MAX_LINE_LENGTH, infile)) {
                removenewline(inputline, MAX_LINE_LENGTH);
                replacement_count = replaceignoringcase(inputline, oldpath, newpath, outputline);
                if (replacement_count > 0) {
                    /* [ temporary patch!!! ]
                    if replaceignoringcase > 0,
                    1) replace "/" with "\\"
                    2) replace "\\\\" with "\\"
                      ...ma verificare se si puo'anteporre un pezzo
                      di path per una selezione piu' accurata!!!
                    */
                    char s1[MAX_LINE_LENGTH + 1];
                    replaceignoringcase(outputline, slash, backslash, s1);
                    replaceignoringcase(s1, doublebackslash, backslash, outputline);
                }
                fprintf(outfile, "%s\n", outputline);
            }
            fclose(outfile);
        } else {
            returnvalue = 0;
        }
        fclose(infile);
    } else {
        returnvalue = 0;
    }
    return returnvalue;
}

int main(int argc, char *argv[]) {
    bool firstrun = true;
    bool pathfromarg = false;
    char path[2 * FILENAME_LENGTH];
    char shortpath[FILENAME_LENGTH + 1];
    char userinput[USER_INPUT_LENGTH + 1];

    if (argc == 2) pathfromarg = true;

    if (!pathfromarg) {
        /* Little help */
        documentation();
        /* Read current directory */
        if (!getcwd(path, FILENAME_LENGTH)) {
            char msg[160];
            sprintf(msg, "ERROR: cannot read current directory name.");
            exitwithpause(msg);
        }
    } else {
        strncpy(path, argv[1], (2 * FILENAME_LENGTH - 1));
        // the setup does not include "z88dk" in the path
		if (!str_ends_with(path, "z88dk")) {
			if (str_ends_with(path, "\\"))
				strcat(path, "z88dk");
			else
				strcat(path, "\\z88dk");
		}
		printf("Installation folder:\n  \"%s\"\n\n", path);
    }

    /* Get the short path */
    if (!GetShortPathName(path, shortpath, FILENAME_LENGTH)) {
        char msg[FILENAME_LENGTH];
        sprintf(msg, "ERROR: cannot get short path for \"%s\".", path);
        exitwithpause(msg);
    } else {
        // we want only upper case paths
        for (unsigned int i = 0; i < strlen(shortpath); i++) shortpath[i] = toupper(shortpath[i]);
    }
	printf("Installation folder (short path):\n  \"%s\"\n\n", shortpath);

    /* Confirmation */
    if (!pathfromarg) {
        printf("Current path:\n  \"%s\"\n", path);
        printf("do you want to proceed with the installation in this folder (y=yes, n=no)?\n");
        getuserinput(userinput, USER_INPUT_LENGTH);
        if (!evaluateyes(userinput)) exit(EXIT_FAILURE);
        printf("\n");
    }

    /* Test for dir & files */
    FILE *testfile;
    char testfilename[2 * FILENAME_LENGTH];
    sprintf(testfilename, "%s\\%s\\zx.cfg", shortpath, DEFAULT_CFG_DIR);
    if ((testfile = fopen(testfilename, "r"))) {
        fclose(testfile);
    } else {
        char msg[FILENAME_LENGTH];
        sprintf(msg, "ERROR: cannot find cfg directory in \"%s\\%s\".", shortpath, DEFAULT_CFG_DIR);
        exitwithpause(msg);
    }

    /* Test for dir & files */
    sprintf(testfilename, "%s\\%s\\zx.cfg.bak", shortpath, DEFAULT_CFG_DIR);
    if ((testfile = fopen(testfilename, "r"))) {
        fclose(testfile);
        firstrun = false;
        printf("this program already run: using original files (backup copies)...\n\n");
    }

    /* Parse directory files, if the directory is != "C:\\Z88DK" */
	if (strcmp(shortpath, "C:\\Z88DK")) {
		char z88dkcfgfolder[2 * FILENAME_LENGTH];
		sprintf(z88dkcfgfolder, "%s\\%s", shortpath, DEFAULT_CFG_DIR);
		printf("processing cfg files...\n");
		DIR *dirp;
		struct dirent *entry;
		if((dirp = opendir(z88dkcfgfolder))) {
			while((entry = readdir(dirp))) {
				char file[FILENAME_LENGTH];
				strcpy(file, entry->d_name);
				int l = strlen(file);
				if (l >= 5) {
					if (
						file[l - 4] == '.' &&
						tolower(file[l - 3]) == 'c' &&
						tolower(file[l - 2]) == 'f' &&
						tolower(file[l - 1]) == 'g'
						) {
							char filename[FILENAME_LENGTH];
							strcpy(filename, z88dkcfgfolder);
							strcat(filename, "\\");
							strcat(filename, file);
							if (firstrun) {
								if (!backupfile(filename)) {
									char msg[FILENAME_LENGTH];
									sprintf(msg, "ERROR: cannot backup file \"%s\".", filename);
									exitwithpause(msg);
								}
							}
							char defaultsetupdir[FILENAME_LENGTH];
							strcpy(defaultsetupdir, DEFAULT_SETUP_DIR);
							if (!applynewpaths(filename, defaultsetupdir, shortpath)) {
								char msg[FILENAME_LENGTH];
								sprintf(msg, "ERROR: cannot work on file \"%s\".", filename);
								exitwithpause(msg);
							}
							printf("  %s\n", file);
						}
				}

			}
			closedir(dirp);
		} else {
			char msg[FILENAME_LENGTH];
			sprintf(msg, "ERROR: cannot list directory \"%s\".", z88dkcfgfolder);
			exitwithpause(msg);
		}
	} else {
		printf("cfg files are ok\n");
	}

    /* Batch file */
    char batchfile_name[2 * FILENAME_LENGTH];
    sprintf(batchfile_name, "%s\\%s", shortpath, BATCH_FILE_NAME);
    printf("\ngenerating " BATCH_FILE_NAME "...\n");
    FILE *batchfile;
    if ((batchfile = fopen(batchfile_name, "w"))) {
        char identifier[80];
        char uniquesuffix[16];
        strcpy(identifier, "Z88DK_ENV");
        getnearlyuniquesuffix(uniquesuffix);
        strcat(identifier, uniquesuffix);
        fprintf(batchfile, "@echo off\n");
        fprintf(batchfile, "rem *********************************\n");
        fprintf(batchfile, "rem ** z88dk environment variables **\n");
        fprintf(batchfile, "rem *********************************\n");
        fprintf(batchfile, "rem\n");
        fprintf(batchfile, "rem **************************************************\n");
        fprintf(batchfile, "rem ** You can call this file from the command line **\n");
        fprintf(batchfile, "rem ** before running the compiler executables; or  **\n");
        fprintf(batchfile, "rem ** from inside another .bat file using the CALL **\n");
        fprintf(batchfile, "rem ** statement.                                   **\n");
        fprintf(batchfile, "rem **************************************************\n");
        fprintf(batchfile, "\n");
        fprintf(batchfile, "IF NOT \"%%%s%%\"==\"\" GOTO exit_without_settings\n", identifier);
        fprintf(batchfile, "SET Z80_OZFILES=%s\\lib\\\n", shortpath);
        fprintf(batchfile, "SET ZCCCFG=%s\\lib\\config\\\n", shortpath);
        fprintf(batchfile, "SET PATH=%s\\bin;%%PATH%%\n", shortpath);
        fprintf(batchfile, "SET %s=true\n", identifier);
        fprintf(batchfile, "GOTO the_end\n");
        fprintf(batchfile, "\n");
        fprintf(batchfile, ":exit_without_settings\n");
        fprintf(batchfile, "rem echo Nothing to set :-)\n");
        fprintf(batchfile, ":the_end\n");
        fclose(batchfile);
    } else {
        char msg[FILENAME_LENGTH];
        strcpy(msg, "Cannot write file \"" BATCH_FILE_NAME "\".\n");
        exitwithpause(msg);
    }

    /* final pause */
    printf("\ndone!\n\n");
    if (!pathfromarg) {
        if (system("PAUSE")) {
            printf("\nPlease wait some seconds.\n");
            Sleep(4000);
        }
    }

    return EXIT_SUCCESS;
}

