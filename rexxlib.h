/*
 rexxlib.h - an OS/2 REXX extension library to provide access to some
 common functions.
 Copyright ¸ 2004, D.J. van Enckevort

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 or http://www.gnu.org/copyleft/lesser.html
 */

#ifndef REXXLIB
#define REXXLIB

#define STR_FALSE       "0"
#define STR_TRUE        "1"
#define STR_VERSION     "1.3"
#define MSG_LENGTH      256
#define MAX_FF_BUF      32

#ifdef ECSRXLIB
#define LIBRARY_NAME    "ECSRXLIB"
#define QUERYSWITCHLIST "eCSQuerySwitchList"
#define CLOSEOBJECT     "eCSCloseObject"
#define LOADFUNCS       "eCSLoadFuncs"
#define DROPFUNCS       "eCSDropFuncs"
#define STARTPROGRAMME  "eCSStartProgramme"
#define COPY            "eCSCopy"
#define DELETE          "eCSDelete"
#define DELTREE         "eCSDelTree"
#define MAKEPATH        "eCSMakePath"
#define SEARCHPATH      "eCSSearchPath"
#else
/*
#define LIBRARY_NAME    "RXMLUTIL"
#define QUERYSWITCHLIST "RexxMailQuerySwitchList"
#define CLOSEOBJECT     "RexxMailCloseObject"
#define LOADFUNCS       "RexxMailLoadFuncs"
#define DROPFUNCS       "RexxMailDropFuncs"
#define STARTPROGRAMME  "RexxMailStartProgramme"
#define COPY            "RexxMailCopy"
#define DELETE          "RexxMailDelete"
#define DELTREE         "RexxMailDelTree"
#define MAKEPATH        "RexxMailMakePath"
#define SEARCHPATH      "RexxMailSearchPath"
*/
#define LIBRARY_NAME    "REXXLIB"
#define QUERYSWITCHLIST "RexxLibQuerySwitchList"
#define CLOSEOBJECT     "RexxLibCloseObject"
#define LOADFUNCS       "RexxLibLoadFuncs"
#define DROPFUNCS       "RexxLibDropFuncs"
#define STARTPROGRAMME  "RexxLibStartProgramme"
#define COPY            "RexxLibCopy"
#define DELETE          "RexxLibDelete"
#define DELTREE         "RexxLibDelTree"
#define MAKEPATH        "RexxLibMakePath"
#define SEARCHPATH      "RexxLibSearchPath"
#endif

static PSZ  RxFncTable[] =             /* exported function names    */
{
    STARTPROGRAMME,
    COPY,
    DELETE,
    DELTREE,
    MAKEPATH,
    SEARCHPATH,
    CLOSEOBJECT,
    QUERYSWITCHLIST,
    DROPFUNCS
};



ULONG APIENTRY RexxLibQuerySwitchList(PSZ       Name,
                                      LONG      Argc,
                                      RXSTRING  Argv[],
                                      PSZ       Queuename,
                                      PRXSTRING Retstr);

ULONG APIENTRY RexxLibCloseObject(PSZ Name,
                                  LONG Argc,
                                  RXSTRING Argv[],
                                  PSZ Queuename,
                                  PRXSTRING Retstr);

ULONG APIENTRY RexxLibLoadFuncs(PSZ Name,
                                LONG Argc,
                                RXSTRING Argv[],
                                PSZ Queuename,
                                PRXSTRING Retstr);

ULONG APIENTRY RexxLibDropFuncs(PSZ Name,
                                LONG Argc,
                                RXSTRING Argv[],
                                PSZ Queuename,
                                PRXSTRING Retstr);

#define RX_STARTTYPE_OS2_FULLSCREEN     "OS2FULL"
#define RX_STARTTYPE_OS2_WINDOW         "OS2WIN"
#define RX_STARTTYPE_OS2_PM             "OS2PM"
#define RX_STARTTYPE_DOS_FULLSCREEN     "DOSFULL"
#define RX_STARTTYPE_DOS_WINDOW         "DOSWIN"
#define RX_STARTTYPE_DEFAULT            "DEFAULT"

#define RX_STARTFOCUS_BACKGROUND        "BACKGROUND"
#define RX_STARTFOCUS_FOREGROUND        "FOREGROUND"

#define RX_STARTINHERIT_PARENT          "PARENT"
#define RX_STARTINHERIT_SHELL           "SHELL"

#define RX_STARTPARAMS_MAX              7
#define RX_STARTPARAMS_MIN              1
#define RX_STARTPARAMS_PROGRAMME        0
#define RX_STARTPARAMS_PARAMETERS       1
#define RX_STARTPARAMS_ENVIRONMENT      2
#define RX_STARTPARAMS_TYPE             3
#define RX_STARTPARAMS_FOCUS            4
#define RX_STARTPARAMS_INHERIT          5
#define RX_STARTPARAMS_CAUSE            6

/* From REXX this function is called like this
 rc=RexxLibStartProgramme(Programme, [Parameters], [Environment], [StartType], [StartFocus], [StartOptions], [Cause])
 For example
 rc=RexxLibStartProgramma("e.exe", "myfile.txt", "ENVA=1|ENVB=2|", "DEFAULT", "FOREGROUND", "SHELL")
 */

ULONG APIENTRY RexxLibStartProgramme(PSZ Name,
                                     LONG Argc,
                                     RXSTRING Argv[],
                                     PSZ Queuename,
                                     PRXSTRING Retstr);

#define RX_COPY_APPEND                  "APPEND"
#define RX_COPY_REPLACE                 "REPLACE"
#define RX_COPY_FAIL                    "FAIL"
#define RX_COPYPARAMS_MIN               2
#define RX_COPYPARAMS_MAX               4
#define RX_COPYPARAMS_SRC               0
#define RX_COPYPARAMS_TRG               1
#define RX_COPYPARAMS_OPTION            2
#define RX_COPYPARAMS_CAUSE             3

ULONG APIENTRY RexxLibCopy(PSZ Name,
                           LONG Argc,
                           RXSTRING Argv[],
                           PSZ Queuename,
                           PRXSTRING Retstr);

#define RX_DELETE_FORCE                 "FORCE"
#define RX_DELETE_NOFORCE               "NOFORCE"
#define RX_DELETE_RECOVERABLE           "RECOVERABLE"
#define RX_DELETE_NOTRECOVERABLE        "NOTRECOVERABLE"
#define RX_DELETEPARAMS_MIN             1
#define RX_DELETEPARAMS_MAX             4
#define RX_DELETEPARAMS_FILE            0
#define RX_DELETEPARAMS_FORCE           1
#define RX_DELETEPARAMS_RECOVERABLE     2
#define RX_DELETEPARAMS_CAUSE           3

ULONG APIENTRY RexxLibDelete(PSZ Name,
                             LONG Argc,
                             RXSTRING Argv[],
                             PSZ Queuename,
                             PRXSTRING Retstr);

#define RX_DELTREE_FORCE                 "FORCE"
#define RX_DELTREE_NOFORCE               "NOFORCE"
#define RX_DELTREEPARAMS_MIN             1
#define RX_DELTREEPARAMS_MAX             3
#define RX_DELTREEPARAMS_FILE            0
#define RX_DELTREEPARAMS_FORCE           1
#define RX_DELTREEPARAMS_CAUSE           2

ULONG APIENTRY RexxLibDelTree(PSZ Name,
                              LONG Argc,
                              RXSTRING Argv[],
                              PSZ Queuename,
                              PRXSTRING Retstr);


#define RX_MKPATHPARAMS_MIN             1
#define RX_MKPATHPARAMS_MAX             2
#define RX_MKPATHPARAMS_PATH            0
#define RX_MKPATHPARAMS_CAUSE           1


ULONG APIENTRY RexxLibMkPath(PSZ Name,
                             LONG Argc,
                             RXSTRING Argv[],
                             PSZ Queuename,
                             PRXSTRING Retstr);


#define RX_SEARCHPATH_PATH  "PATH"
#define RX_SEARCHPATH_ENV   "ENVIRONMENT"
#define RX_SEARCHPATHPARAMS_MIN               2
#define RX_SEARCHPATHPARAMS_MAX               4
#define RX_SEARCHPATHPARAMS_PATH              0
#define RX_SEARCHPATHPARAMS_FILE              1
#define RX_SEARCHPATHPARAMS_VAR               2
#define RX_SEARCHPATHPARAMS_OPTION            3

ULONG APIENTRY RexxLibSearchPath(PSZ Name,
                                 LONG Argc,
                                 RXSTRING Argv[],
                                 PSZ Queuename,
                                 PRXSTRING Retstr);




INT SetRexxVariable(PSZ name, PSZ value);

typedef struct _OPTIONS {
    BOOL force, recurse, unrecoverable;
} OPTIONS, *POPTIONS;

APIRET DeleteFile(char *file, POPTIONS options);
APIRET DeleteDir(char *directory, POPTIONS options);
APIRET ProcessRecursing(char *file, POPTIONS options);
APIRET ProcessFiles(char *file, POPTIONS options);
BOOL DirectoryExists(char *dir);
int MakeDirectory(char *fpath);

#endif