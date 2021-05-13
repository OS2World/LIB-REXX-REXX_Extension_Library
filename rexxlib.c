/*
 rexxlib.c - an OS/2 REXX extension library to provide access to some
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

#define INCL_REXXSAA
#define INCL_DOS
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_WINSHELLDATA
#define INCL_WINSWITCHLIST
#include <os2.h>
#include <rexxsaa.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <string.h>
#include <direct.h>
#include <error.h>
#include <errno.h>

#include "rxstring.h"
#include "rexxlib.h"

#define RxIsZeroOrEmpty(string) (RXZEROLENSTRING(string) || RXNULLSTRING(string))

void setReturnCode(PRXSTRING Retstr, APIRET rc) /*fold00*/
{
    if (rc==NO_ERROR) {
        strcpy2rx(Retstr, STR_TRUE);
    } else {
        strcpy2rx(Retstr, STR_FALSE);
    }
}

char *strip(char *str) { /*fold00*/
    char *tmp;
    for (tmp=str+strlen(str); tmp>=str && *tmp<=32; *tmp=0, tmp--) {}
    return str;
}

int setCause(APIRET rc, /*fold00*/
             PSZ name,
             PSZ object)
{
    ULONG msg_len;
    char msg[MSG_LENGTH];

    if (object!=NULL) {
        rc=DosGetMessage(&object, 1, msg, MSG_LENGTH, rc, "OSO001.MSG", &msg_len);
    } else {
        rc=DosGetMessage(NULL, 0, msg, MSG_LENGTH, rc, "OSO001.MSG", &msg_len);
    }

    if (rc==NO_ERROR) {
        return SetRexxVariable(name, strip(msg));
    }
    return !RXSHV_OK;
}

INT SetRexxVariable( /*fold00*/
  PSZ        name,                     /* REXX variable to set       */
  PSZ        value)                    /* value to assign            */
{
  SHVBLOCK   block;                    /* variable pool control block*/
  block.shvcode = RXSHV_SYSET;         /* do a symbolic set operation*/
  block.shvret=(UCHAR)0;               /* clear return code field    */
  block.shvnext=(PSHVBLOCK)0;          /* no next block              */
  MAKERXSTRING(block.shvname,name,strlen(name));
  MAKERXSTRING(block.shvvalue, value, strlen(value));
  return RexxVariablePool(&block);     /* set the variable           */
}

ULONG APIENTRY RexxLibQuerySwitchList( /*fold00*/
     PSZ       Name,
     LONG      Argc,
     RXSTRING  Argv[],
     PSZ       Queuename,
     PRXSTRING Retstr)
{
    HAB hab;
    ULONG cbItems=0;            /* Number of items in list */
    ULONG ulBufSize=0;            /* Size of buffer for information */
    ULONG i;
    PSWBLOCK pswblk=NULL;         /* Pointer to information returned */
    PSWCNTRL pswctl=NULL;
    char varname[256], value[6];

    // Default return the error string
    strcpy2rx(Retstr, STR_FALSE);

    if (Argc!=1) return 40;

    if (RXZEROLENSTRING(Argv[0])
        || RXNULLSTRING(Argv[0]))
    {
        return 40;
    }
    if (Argv[0].strlength>240) return 40;
    if (Argv[0].strptr[Argv[0].strlength]=='.')
    {
        Argv[0].strptr[Argv[0].strlength]=0;
        Argv[0].strlength-=1;
    }

    if ((hab=WinInitialize(0))!=NULLHANDLE)
    {
        // Query Window list and kill the matching entries
        if ((cbItems=WinQuerySwitchList(hab, NULL, 0))!=0)
        {
            // Calculate required amount of memory
            ulBufSize = (cbItems * sizeof(SWENTRY)) + sizeof(HSWITCH);
            if (DosAllocMem((PPVOID)&pswblk, ulBufSize, PAG_WRITE | PAG_READ | PAG_COMMIT)==NO_ERROR)
            {
                if ((cbItems=WinQuerySwitchList(hab, pswblk, ulBufSize))!=0)
                {
                    // First entry is "switch to" text
                    for (i=1; i<cbItems; i++)
                    {
                        pswctl=&(pswblk->aswentry[i].swctl);
                        sprintf(varname, "%s.%d", Argv[0].strptr, i);
                        SetRexxVariable(varname, pswctl->szSwtitle);
                    }
                    sprintf(varname, "%s.%d", Argv[0].strptr, 0);
                    sprintf(value, "%d", cbItems-1);
                    SetRexxVariable(varname, value);
                    strcpy2rx(Retstr, STR_TRUE);
                }
                DosFreeMem(pswblk);
            }
        }
        // Clean up
        WinTerminate(hab);
    } // If we cannot get an anchor block fail
    else return 40;
    return 0;
}

ULONG APIENTRY RexxLibCloseObject( /*fold00*/
     PSZ       Name,
     LONG      Argc,
     RXSTRING  Argv[],
     PSZ       Queuename,
     PRXSTRING Retstr)
{
    HAB hab;
    ULONG cbItems=0;            /* Number of items in list */
    ULONG ulBufSize=0;            /* Size of buffer for information */
    ULONG i;
    PSWBLOCK pswblk=NULL;         /* Pointer to information returned */
    PSWCNTRL pswctl=NULL;

    /* If no string to match was specified we fail */
    if (Argc!=1) return 40;
    if (RXZEROLENSTRING(Argv[0])
        || RXNULLSTRING(Argv[0]))
    {
        return 40;
    }

    /* Get a handle to an anchor block */
    if ((hab=WinInitialize(0))!=NULLHANDLE)
    {
        strcpy2rx(Retstr, STR_FALSE);

        // Query Window list and kill the matching entries
        if ((cbItems=WinQuerySwitchList(hab, NULL, 0))!=0)
        {
            // Calculate required amount of memory
            ulBufSize = (cbItems * sizeof(SWENTRY)) + sizeof(HSWITCH);
            if (DosAllocMem((PPVOID)&pswblk, ulBufSize, PAG_WRITE | PAG_READ | PAG_COMMIT)==NO_ERROR)
            {
                if ((cbItems=WinQuerySwitchList(hab, pswblk, ulBufSize))!=0)
                {
                    strcpy2rx(Retstr, STR_FALSE);
                    // First entry is "switch to"
                    for (i=1; i<cbItems; i++)
                    {
                        pswctl=&(pswblk->aswentry[i].swctl);

                        if (stricmp(pswctl->szSwtitle, Argv[0].strptr)==0)
                        {
                            if (WinPostMsg(pswctl->hwnd, WM_CLOSE, 0L, 0L)==TRUE) {
                                strcpy2rx(Retstr, STR_TRUE);
                            } else {
                                strcpy2rx(Retstr, STR_FALSE);
                            }
                        }
                    }
                }
                DosFreeMem(pswblk);
            }
        }
        // Clean up
        WinTerminate(hab);
    } // If we cannot get an anchor block fail
    else return 40;
    return 0;
}

ULONG APIENTRY RexxLibLoadFuncs( /*fold00*/
     PSZ       Name,
     LONG      Argc,
     RXSTRING  Argv[],
     PSZ       Queuename,
     PRXSTRING Retstr)
{
  INT    entries, j, rc, success;


  Retstr->strlength = 0;
  if (Argc > 1) {
      return 40;
  } else {
      if (Argc==1) SetRexxVariable(Argv[0].strptr, STR_VERSION);
      entries = sizeof(RxFncTable)/sizeof(PSZ);
      success=NO_ERROR;
      for (j = 0; j < entries; j++) {
        rc=RexxRegisterFunctionDll(RxFncTable[j],
                                   LIBRARY_NAME, RxFncTable[j]);
        if (rc!=RXFUNC_OK && rc!=RXFUNC_DEFINED) success=rc;
      }
      setReturnCode(Retstr, success);
  }
  return 0;
}

ULONG APIENTRY RexxLibDropFuncs( /*fold00*/
     PSZ       Name,
     LONG      Argc,
     RXSTRING  Argv[],
     PSZ       Queuename,
     PRXSTRING Retstr)
{
  INT    entries, j, rc, success;
  Retstr->strlength = 0;
  if (Argc > 0) return 40;

  entries = sizeof(RxFncTable)/sizeof(PSZ);
  success=NO_ERROR;
  for (j = 0; j < entries; j++) {
      rc=RexxDeregisterFunction(RxFncTable[j]);
      if (rc!=RXFUNC_OK && rc!=RXFUNC_NOTREG) success=rc;
  }
  rc=RexxDeregisterFunction(LOADFUNCS);
  if (rc!=RXFUNC_OK && rc!=RXFUNC_NOTREG) success=rc;
  setReturnCode(Retstr, success);
  return 0;
}


ULONG APIENTRY RexxLibStartProgramme(PSZ Name, /*fold00*/
                                LONG Argc,
                                RXSTRING Argv[],
                                PSZ Queuename,
                                PRXSTRING Retstr)
{
    APIRET rc=NO_ERROR;
    PID pid;
    ULONG Session=0;
    STARTDATA sd;
    char *tmp=NULL;
    char ObjectBuffer[CCHMAXPATH]= "";

    strcpy2rx(Retstr, STR_FALSE);
    Retstr->strlength=strlen(Retstr->strptr);

    /* Is the number of parameters within range? */
    if ((Argc<RX_STARTPARAMS_MIN) || (Argc>RX_STARTPARAMS_MAX)) return 40;

    /* Setup the startdata */
    memset(&sd, 0, sizeof(STARTDATA));
    sd.Length=sizeof(STARTDATA);
    sd.Related=SSF_RELATED_INDEPENDENT;
    sd.ObjectBuffer=ObjectBuffer;
    sd.ObjectBuffLen=CCHMAXPATH;

    /* Set the focus option */
    sd.FgBg=SSF_FGBG_FORE;
    if (Argc>RX_STARTPARAMS_FOCUS) {
        if (!RXZEROLENSTRING(Argv[RX_STARTPARAMS_FOCUS])
            && !RXNULLSTRING(Argv[RX_STARTPARAMS_FOCUS]))
        {
            if (strnicmp(Argv[RX_STARTPARAMS_FOCUS].strptr,
                         RX_STARTFOCUS_BACKGROUND,
                         strlen(Argv[RX_STARTPARAMS_FOCUS].strptr))==0) {
                sd.FgBg=SSF_FGBG_BACK;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_FOCUS].strptr,
                             RX_STARTFOCUS_FOREGROUND,
                             strlen(Argv[RX_STARTPARAMS_FOCUS].strptr))==0) {
                sd.FgBg=SSF_FGBG_FORE;
            } else {
                return 40; /* If the type doesn't match any type we return 'invalid parameter' */
            }} /* This is correct, the above if statements are nested */
        } /* end of the zero string test */
    } /* end of the if (Argc>RX_STARTPARAMS_FOCUS) clause */

    /* Set the inherit option to the specified option, or to INHERIT_SHELL if not specified */
    if (Argc>RX_STARTPARAMS_INHERIT) {
        if (RXZEROLENSTRING(Argv[RX_STARTPARAMS_TYPE])
            || RXNULLSTRING(Argv[RX_STARTPARAMS_TYPE]))
        {
            sd.InheritOpt=SSF_INHERTOPT_SHELL;
        } else {
            if (strnicmp(Argv[RX_STARTPARAMS_INHERIT].strptr,
                         RX_STARTINHERIT_PARENT,
                         strlen(Argv[RX_STARTPARAMS_INHERIT].strptr))==0) {
                sd.InheritOpt=SSF_INHERTOPT_PARENT;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_INHERIT].strptr,
                             RX_STARTINHERIT_SHELL,
                             strlen(Argv[RX_STARTPARAMS_INHERIT].strptr))==0) {
                sd.InheritOpt=SSF_INHERTOPT_SHELL;
            } else {
                return 40; /* If the type doesn't match any type we return 'invalid parameter' */
            }} /* This is correct, the above if's are nested */
        } /* end of the zero string test */
    } else {
        sd.InheritOpt=SSF_INHERTOPT_SHELL;
    } /* end of the if (Argc>RX_STARTPARAMS_INHERIT) clause */

    /* Set the start type to the specified type */
    if (Argc>RX_STARTPARAMS_TYPE) {
        if (RXZEROLENSTRING(Argv[RX_STARTPARAMS_TYPE])
            || RXNULLSTRING(Argv[RX_STARTPARAMS_TYPE]))
        {
            sd.SessionType=SSF_TYPE_DEFAULT;
        } else {
            if (strnicmp(Argv[RX_STARTPARAMS_TYPE].strptr,
                         RX_STARTTYPE_OS2_FULLSCREEN,
                         strlen(Argv[RX_STARTPARAMS_TYPE].strptr))==0) {
                sd.SessionType=SSF_TYPE_FULLSCREEN;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_TYPE].strptr,
                             RX_STARTTYPE_OS2_WINDOW,
                             strlen(Argv[RX_STARTPARAMS_TYPE].strptr))==0) {
                sd.SessionType=SSF_TYPE_WINDOWABLEVIO;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_TYPE].strptr,
                             RX_STARTTYPE_OS2_PM,
                             strlen(Argv[RX_STARTPARAMS_TYPE].strptr))==0) {
                sd.SessionType=SSF_TYPE_PM;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_TYPE].strptr,
                             RX_STARTTYPE_DOS_FULLSCREEN,
                             strlen(Argv[RX_STARTPARAMS_TYPE].strptr))==0) {
                sd.SessionType=SSF_TYPE_VDM;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_TYPE].strptr,
                             RX_STARTTYPE_DOS_WINDOW,
                             strlen(Argv[RX_STARTPARAMS_TYPE].strptr))==0) {
                sd.SessionType=SSF_TYPE_WINDOWEDVDM;
            } else {
                if (strnicmp(Argv[RX_STARTPARAMS_TYPE].strptr,
                             RX_STARTTYPE_DEFAULT,
                             strlen(Argv[RX_STARTPARAMS_TYPE].strptr))==0) {
                sd.SessionType=SSF_TYPE_DEFAULT;
            } else {
                return 40; /* If the type doesn't match any type we return 'invalid parameter' */
            }}}}}} /* Yes, this is correct, the if statements above are nested */
        } /* End of the zero string test */
    } else {
        sd.SessionType=SSF_TYPE_DEFAULT;
    } /* end of the if (Argc>RX_STARTPARAMS_TYPE) clause

    /* Set the programme name to the specified programme, or terminate if it is not specified */
    if (RXZEROLENSTRING(Argv[RX_STARTPARAMS_PROGRAMME])
        || RXNULLSTRING(Argv[RX_STARTPARAMS_PROGRAMME])) {
        return 40;
    } else {
        sd.PgmName=RXSTRPTR(Argv[RX_STARTPARAMS_PROGRAMME]);
    }

    if (Argc>RX_STARTPARAMS_PARAMETERS) {
        if (RXZEROLENSTRING(Argv[RX_STARTPARAMS_PARAMETERS])
            || RXNULLSTRING(Argv[RX_STARTPARAMS_PARAMETERS]))
        {
            sd.PgmInputs=NULL;
        } else {
            sd.PgmInputs=RXSTRPTR(Argv[RX_STARTPARAMS_PARAMETERS]);
        }
    }

    sd.Environment=NULL;
    if (Argc>RX_STARTPARAMS_ENVIRONMENT) {
        if (RXZEROLENSTRING(Argv[RX_STARTPARAMS_ENVIRONMENT])
            || RXNULLSTRING(Argv[RX_STARTPARAMS_ENVIRONMENT]))
        {
            sd.Environment=NULL;
        } else {
            /* Allocate memory for the environment string plus 2 terminating null characters */
            if (DosAllocMem((PPVOID)&sd.Environment, Argv[RX_STARTPARAMS_ENVIRONMENT].strlength+2,
                            PAG_WRITE | PAG_READ | PAG_COMMIT)==NO_ERROR) {
                strcpy(sd.Environment, RXSTRPTR(Argv[RX_STARTPARAMS_ENVIRONMENT]));
                /* Add a pipe symbol, that we replace with a null later on */
                strcat(sd.Environment, "|");
                /* Replace all pipe symbols with nulls */
                while (strrchr(sd.Environment, '|')!=NULL)
                {
                    tmp=strrchr(sd.Environment, '|');
                    tmp[0]=0;
                }
            } else {
                return 40; /* Return with an error */
            }
        }
    }


    rc=DosStartSession(&sd, &Session, &pid);
    setReturnCode(Retstr, rc);

    /* If the user requested the cause to be returned do so... */
    if (Argc>RX_STARTPARAMS_CAUSE) {
        if (!RXZEROLENSTRING(Argv[RX_STARTPARAMS_CAUSE])
            && !RXNULLSTRING(Argv[RX_STARTPARAMS_CAUSE]))
        {
            setCause(rc, Argv[RX_STARTPARAMS_CAUSE].strptr, sd.ObjectBuffer);
        }
    }
    if (sd.Environment!=NULL)
    {
        DosFreeMem(sd.Environment);
    }

    return 0;
}

ULONG APIENTRY RexxLibCopy(PSZ Name, /*fold00*/
                        LONG Argc,
                        RXSTRING Argv[],
                        PSZ Queuename,
                        PRXSTRING Retstr)
{
    ULONG option=0;
    APIRET rc=NO_ERROR;
    strcpy2rx(Retstr, STR_FALSE);
    /* If we do not have sufficient parameters return invalid call to routine */
    if ((Argc<RX_COPYPARAMS_MIN) || (Argc>RX_COPYPARAMS_MAX)) return 40;

    /* If the source or target parameters are empty return invalid call to routine */
    if (RXZEROLENSTRING(Argv[RX_COPYPARAMS_SRC])
        || RXNULLSTRING(Argv[RX_COPYPARAMS_SRC])
        || RXZEROLENSTRING(Argv[RX_COPYPARAMS_TRG])
        || RXNULLSTRING(Argv[RX_COPYPARAMS_TRG]))
    {
        return 40;
    }

    /* Set the copy option */
    if (Argc>RX_COPYPARAMS_OPTION) {
        if (RXZEROLENSTRING(Argv[RX_COPYPARAMS_OPTION])
            || RXNULLSTRING(Argv[RX_COPYPARAMS_OPTION]))
        {
            option=0;   /* DCPY_FAIL */
        } else {
            if (strnicmp(Argv[RX_COPYPARAMS_OPTION].strptr,
                         RX_COPY_APPEND,
                         strlen(Argv[RX_COPYPARAMS_OPTION].strptr))==0)
            {
                option=DCPY_APPEND;
            } else {
                if (strnicmp(Argv[RX_COPYPARAMS_OPTION].strptr,
                             RX_COPY_REPLACE,
                             strlen(Argv[RX_COPYPARAMS_OPTION].strptr))==0)
            {
                option=DCPY_EXISTING;
            } else {
                if (strnicmp(Argv[RX_COPYPARAMS_OPTION].strptr,
                             RX_COPY_FAIL,
                             strlen(Argv[RX_COPYPARAMS_OPTION].strptr))==0)
            {
                option=0; /* DCPY_FAIL */
            } else {
                return 40; /* If the option doesn't match any valid option, the command fails */
            }}} /* This is correct, the above if statements are nested */
        } /* end of the zero string test */
    } else { /* If no option is specified we will let the copy fail if the target file exists */
        option=0;
    }
    rc=DosCopy(Argv[RX_COPYPARAMS_SRC].strptr,
               Argv[RX_COPYPARAMS_TRG].strptr,
               option);
    setReturnCode(Retstr, rc);

    /* If the user requested the cause to be returned do so... */
    if (Argc>RX_COPYPARAMS_CAUSE) {
        if (!RXZEROLENSTRING(Argv[RX_COPYPARAMS_CAUSE])
            && !RXNULLSTRING(Argv[RX_COPYPARAMS_CAUSE]))
        {
            setCause(rc, Argv[RX_COPYPARAMS_CAUSE].strptr, NULL);
        }
    }

    return 0;
}

ULONG APIENTRY RexxLibDelete(PSZ Name, /*fold00*/
                        LONG Argc,
                        RXSTRING Argv[],
                        PSZ Queuename,
                        PRXSTRING Retstr)
{
    APIRET rc=NO_ERROR;
    // Options:     Force  Recurse Unrecoverable
    OPTIONS options={FALSE, FALSE,  FALSE};
    strcpy2rx(Retstr, STR_FALSE);
    Retstr->strlength=strlen(Retstr->strptr);
    /* If we do not have sufficient parameters return invalid call to routine */
    if (Argc<RX_DELETEPARAMS_MIN || Argc>RX_DELETEPARAMS_MAX) return 40;

    /* If the filename is empty we fail */
    if (RXZEROLENSTRING(Argv[RX_DELETEPARAMS_FILE])
        || RXNULLSTRING(Argv[RX_DELETEPARAMS_FILE]))
    {
        return 40;
    }

    /* Check the force option */
    if (Argc>RX_DELETEPARAMS_FORCE) {
        if (!RXZEROLENSTRING(Argv[RX_DELETEPARAMS_FORCE])
            && !RXNULLSTRING(Argv[RX_DELETEPARAMS_FORCE]))
        {
            if (strnicmp(Argv[RX_DELETEPARAMS_FORCE].strptr,
                         RX_DELETE_FORCE,
                         strlen(Argv[RX_DELETEPARAMS_FORCE].strptr))==0) {
                options.force=TRUE;
            } else {
                if (strnicmp(Argv[RX_DELETEPARAMS_FORCE].strptr,
                             RX_DELETE_NOFORCE,
                             strlen(Argv[RX_DELETEPARAMS_FORCE].strptr))==0) {
                    options.force=FALSE;
            } else {
                  return 40; /* If the option doesn't match any valid option the command fails */
            }} /* This is correct, the above if statements are nested */
        } /* end of the zero string test */
    }
    /* Check the unrecoverable option */
    if (Argc>RX_DELETEPARAMS_RECOVERABLE) {
        if (!RXZEROLENSTRING(Argv[RX_DELETEPARAMS_RECOVERABLE])
            && !RXNULLSTRING(Argv[RX_DELETEPARAMS_RECOVERABLE]))
        {
            if (strnicmp(Argv[RX_DELETEPARAMS_RECOVERABLE].strptr,
                         RX_DELETE_RECOVERABLE,
                         strlen(Argv[RX_DELETEPARAMS_RECOVERABLE].strptr))==0) {
                options.unrecoverable=FALSE;
            } else {
                if (strnicmp(Argv[RX_DELETEPARAMS_RECOVERABLE].strptr,
                             RX_DELETE_NOTRECOVERABLE,
                             strlen(Argv[RX_DELETEPARAMS_RECOVERABLE].strptr))==0) {
                options.unrecoverable=TRUE;
            } else {
                return 40; /* If the option doesn't match any valid option the command fails */
            }} /* This is correct, the above if statements are nested */
        } /* end of the zero string test */
    }
    rc=ProcessFiles(Argv[RX_DELETEPARAMS_FILE].strptr, &options);
    setReturnCode(Retstr, rc);

    /* If the user requested the cause to be returned do so... */
    if (Argc>RX_DELETEPARAMS_CAUSE) {
        if (!RXZEROLENSTRING(Argv[RX_DELETEPARAMS_CAUSE])
            && !RXNULLSTRING(Argv[RX_DELETEPARAMS_CAUSE])) {
            setCause(rc, Argv[RX_DELETEPARAMS_CAUSE].strptr, NULL);
        }
    }

    return 0;
}

ULONG APIENTRY RexxLibDelTree(PSZ Name, /*fold00*/
                        LONG Argc,
                        RXSTRING Argv[],
                        PSZ Queuename,
                        PRXSTRING Retstr)
{
    APIRET rc=ERROR_PATH_NOT_FOUND;
    char fpath[_MAX_PATH];

    // Options:     Force  Recurse Unrecoverable
    OPTIONS options={FALSE, TRUE,  TRUE};
    strcpy2rx(Retstr, STR_FALSE);
    Retstr->strlength=strlen(Retstr->strptr);
    /* If we do not have sufficient parameters return invalid call to routine */
    if (Argc<RX_DELTREEPARAMS_MIN || Argc>RX_DELTREEPARAMS_MAX) return 40;

    /* If the filename is empty we fail */
    if (RXZEROLENSTRING(Argv[RX_DELTREEPARAMS_FILE])
        || RXNULLSTRING(Argv[RX_DELTREEPARAMS_FILE]))
    {
        return 40;
    }

    /* Check the force option */
    if (Argc>RX_DELTREEPARAMS_FORCE) {
        if (!RXZEROLENSTRING(Argv[RX_DELTREEPARAMS_FORCE])
            && !RXNULLSTRING(Argv[RX_DELTREEPARAMS_FORCE]))
        {
            if (strnicmp(Argv[RX_DELTREEPARAMS_FORCE].strptr,
                         RX_DELTREE_FORCE,
                         strlen(Argv[RX_DELTREEPARAMS_FORCE].strptr))==0) {
                options.force=TRUE;
            } else {
                if (strnicmp(Argv[RX_DELTREEPARAMS_FORCE].strptr,
                             RX_DELTREE_NOFORCE,
                             strlen(Argv[RX_DELTREEPARAMS_FORCE].strptr))==0) {
                options.force=FALSE;
            } else {
                return 40; /* If the option doesn't match any valid option the command fails */
            }} /* This is correct, the above if statements are nested */
        } /* end of the zero string test */
    }
    memset(fpath, 0, _MAX_PATH);
    if (_fullpath(fpath, Argv[RX_DELTREEPARAMS_FILE].strptr, _MAX_PATH)!=NULL) {
        rc=ProcessRecursing(fpath, &options);
    }
    setReturnCode(Retstr, rc);

    /* If the user requested the cause to be returned do so... */
    if (Argc>RX_DELTREEPARAMS_CAUSE) {
        if (!RXZEROLENSTRING(Argv[RX_DELTREEPARAMS_CAUSE])
            && !RXNULLSTRING(Argv[RX_DELTREEPARAMS_CAUSE])) {
            setCause(rc, Argv[RX_DELTREEPARAMS_CAUSE].strptr, NULL);
        }
    }

    return 0;
}

ULONG APIENTRY RexxLibMkPath(PSZ Name, /*fold00*/
                        LONG Argc,
                        RXSTRING Argv[],
                        PSZ Queuename,
                        PRXSTRING Retstr)
{
    APIRET rc=ERROR_PATH_NOT_FOUND;
    char fpath[_MAX_PATH];

    strcpy2rx(Retstr, STR_FALSE);
    Retstr->strlength=strlen(Retstr->strptr);
    /* If we do not have sufficient parameters return invalid call to routine */
    if (Argc<RX_MKPATHPARAMS_MIN || Argc>RX_MKPATHPARAMS_MAX) return 40;

    /* If the filename is empty we fail */
    if (RXZEROLENSTRING(Argv[RX_MKPATHPARAMS_PATH])
        || RXNULLSTRING(Argv[RX_MKPATHPARAMS_PATH]))
    {
        return 40;
    }
    memset(fpath, 0, _MAX_PATH);
    _fullpath(fpath, Argv[RX_MKPATHPARAMS_PATH].strptr, _MAX_PATH);
    if (strlen(fpath)>0) {
           rc=MakeDirectory(fpath);
    }
    setReturnCode(Retstr, rc);

    /* If the user requested the cause to be returned do so... */
    if (Argc>RX_MKPATHPARAMS_CAUSE) {
        if (!RXZEROLENSTRING(Argv[RX_MKPATHPARAMS_CAUSE])
            && !RXNULLSTRING(Argv[RX_MKPATHPARAMS_CAUSE])) {
            setCause(rc, Argv[RX_MKPATHPARAMS_CAUSE].strptr, NULL);
        }
    }

    return 0;
}

ULONG APIENTRY RexxLibSearchPath(PSZ Name, /*FOLD00*/
                        LONG Argc,
                        RXSTRING Argv[],
                        PSZ Queuename,
                        PRXSTRING Retstr)
{

    APIRET rc=NO_ERROR;
    char buf[CCHMAXPATH];
    ULONG flag;

    /* If we do not have sufficient parameters return invalid call to routine */
    if (Argc<RX_SEARCHPATHPARAMS_MIN || Argc>RX_SEARCHPATHPARAMS_MAX) return 40;

    /* If we don't have enough parameters */
    if (RxIsZeroOrEmpty(Argv[RX_SEARCHPATHPARAMS_PATH]) ||
        RxIsZeroOrEmpty(Argv[RX_SEARCHPATHPARAMS_FILE]) ||
        RxIsZeroOrEmpty(Argv[RX_SEARCHPATHPARAMS_VAR])) return 40;

    flag=SEARCH_IGNORENETERRS | SEARCH_ENVIRONMENT;
    if (Argc>RX_SEARCHPATHPARAMS_OPTION) {
        if (!RxIsZeroOrEmpty(Argv[RX_SEARCHPATHPARAMS_OPTION])) {

            if (strnicmp(Argv[RX_SEARCHPATHPARAMS_OPTION].strptr,
                         RX_SEARCHPATH_PATH,
                         strlen(Argv[RX_SEARCHPATHPARAMS_OPTION].strptr))==0) {
                flag=SEARCH_IGNORENETERRS;
            } else {
                if (strnicmp(Argv[RX_SEARCHPATHPARAMS_OPTION].strptr,
                             RX_SEARCHPATH_ENV,
                             strlen(Argv[RX_SEARCHPATHPARAMS_OPTION].strptr))==0) {
                    flag=SEARCH_IGNORENETERRS | SEARCH_ENVIRONMENT;
                } else {
                    return 40; /* If the option doesn't match any valid option the command fails */
                }
            }
        }
    }
    memset(buf, 0, CCHMAXPATH);
    rc=DosSearchPath(flag, Argv[RX_SEARCHPATHPARAMS_PATH].strptr,
                     Argv[RX_SEARCHPATHPARAMS_FILE].strptr,
                     buf, CCHMAXPATH);

    /* If we don't have an error store the result */
    if (rc==NO_ERROR) {
        SetRexxVariable(Argv[RX_SEARCHPATHPARAMS_VAR].strptr, buf);
    } else {
        setCause(rc, Argv[RX_SEARCHPATHPARAMS_VAR].strptr, NULL);
    }
    setReturnCode(Retstr, rc);

    return 0;
}


APIRET DeleteDir(char *directory, POPTIONS options) /*fold00*/
{
  APIRET rc;
  rc=DosDeleteDir(directory);
  return rc;
}

APIRET DeleteFile(char *file, POPTIONS options) /*fold00*/
{
  if (options->force)
     chmod(file, S_IWRITE);
  if (options->unrecoverable) {
     return DosForceDelete(file);
  }
  return DosDelete(file);
}

APIRET ProcessFiles(char *file, POPTIONS options) /*fold00*/
{
  APIRET rc;
  HDIR hdirFindHandle = HDIR_CREATE;
  FILEFINDBUF3 FindBuffer;
  ULONG ulResultBufLen = sizeof(FILEFINDBUF3);
  ULONG ulFindCount = 1;
  char fullname[CCHMAXPATH], *filepart;
  char dir[CCHMAXPATHCOMP], filespec[CCHMAXPATHCOMP];

  memset(&FindBuffer, 0, ulResultBufLen);
  // Zoek het einde van het directory gedeelte.
  filepart=strrchr(file, '\\');
  if (filepart!=NULL) {
      // laat filepart verwijzen naar de bestandsnaam
      filepart++;
      // kopieer de bestandsnaam naar filespec
      strcpy(filespec,filepart);
      // kopieer het directorygedeelte naar dir
      strncpy(dir, file, filepart-file);
      dir[filepart-file]=0;
  } else {
      strcpy(filespec, file);
      dir[0]=0;
  }
  // file bevat nu de volledige naam, inclusief pad, met evt. wildcards
  // filespec bevat nu de bestandsnaam, zonder het pad, met evt. wildcards
  // dir bevat nu het pad

  if (options->force) {
      rc = DosFindFirst(file, &hdirFindHandle, FILE_HIDDEN | FILE_SYSTEM | FILE_NORMAL, &FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD);
  } else {
      rc = DosFindFirst(file, &hdirFindHandle, FILE_NORMAL, &FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD);
  }

  while (rc==NO_ERROR)
  {
      strcpy(fullname, dir);
      strcat(fullname, FindBuffer.achName);
      rc=DeleteFile(fullname, options);
      if (rc!=NO_ERROR) break;
      ulFindCount = 1;
      rc = DosFindNext(hdirFindHandle, &FindBuffer, ulResultBufLen, &ulFindCount);
  }
  DosFindClose(hdirFindHandle);
  if (rc==ERROR_NO_MORE_FILES) return NO_ERROR;
  return rc;
}

BOOL DirectoryExists(char *dir) /*fold00*/
{
    struct stat buf;

    if (strlen(dir)>0)
        if (stat(dir, &buf)==0)
            if (buf.st_mode&S_IFDIR)
                return TRUE;


    return FALSE;
}

APIRET ProcessRecursing(char *file, POPTIONS options) /*fold00*/
{
  APIRET rc=NO_ERROR;
  HDIR hdirFindHandle = HDIR_CREATE;
  FILEFINDBUF3 FindBuffer;
  ULONG ulResultBufLen = sizeof(FILEFINDBUF3);
  ULONG ulFindCount = 1;
  char filespec[CCHMAXPATH];

  // Test if directory exists
  memset(&FindBuffer, 0, ulResultBufLen);
  if (DirectoryExists(file)) {

      // Directory exists
      strcpy(filespec, file);
      strcat(filespec, "\\*");

      // Delete files in this directory
      ProcessFiles(filespec, options);

      // Search for subdirectories
      if (options->force) {
          rc = DosFindFirst(filespec, &hdirFindHandle, FILE_HIDDEN | FILE_SYSTEM | MUST_HAVE_DIRECTORY,
                            &FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD);
      } else {
          rc = DosFindFirst(filespec, &hdirFindHandle, MUST_HAVE_DIRECTORY,
                            &FindBuffer, ulResultBufLen, &ulFindCount, FIL_STANDARD);
      }

      while (rc==NO_ERROR) {
          // Ignore . and .. entries
          if ((strcmp(FindBuffer.achName, ".")!=0) && (strcmp(FindBuffer.achName, "..")!=0)) {

              // construct full path
              strcpy(filespec, file);
              strcat(filespec, "\\");
              strcat(filespec, FindBuffer.achName);

              // recurse into subdirectory
              rc=ProcessRecursing(filespec, options);
              if (rc!=NO_ERROR) break;

          }

          // Find next
          ulFindCount = 1;
          rc = DosFindNext(hdirFindHandle, &FindBuffer, ulResultBufLen, &ulFindCount);
      }
      // Close find
      DosFindClose(hdirFindHandle);
      // if we broke out because of an error
      if (rc!=ERROR_NO_MORE_FILES && rc!=NO_ERROR) return rc;

      // Delete the directory
      return DeleteDir(file, options);
  }

  return ERROR_PATH_NOT_FOUND;
}

int MakeDirectory(char *fpath) /*fold00*/
{
    char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], path[_MAX_PATH];
    int rc=NO_ERROR;

    _splitpath(fpath, drive, dir, fname, ext);
    strcpy(path, drive);
    // If we have a subdirectory check for it (i.e dir is more than just "\")
    if (strlen(dir)>1) {
        // Remove trailing slash
        dir[strlen(dir)-1]=0;
        strcat(path, dir);
        // Test is the dir exists
        if (!DirectoryExists(path)) {
            // No, let's make it
            rc=MakeDirectory(path);
        }
    }
    // If all higher dirs are present
    if (rc==NO_ERROR) {
        if (mkdir(fpath)==-1) {
            switch (errno) {
            case EACCESS:
                return ERROR_ACCESS_DENIED;
            case ENOENT:
                return ERROR_PATH_NOT_FOUND;
            default:
                return NO_ERROR;
            }
        }
    }
    return rc;
}
