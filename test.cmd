/*
 test.cmd - an OS/2 REXX extension library to provide access to some
 common functions. TEST.CMD example for the functions in the library
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

call RxFuncAdd 'REXXLIBLoadFuncs', 'REXXLIB', 'REXXLIBLoadFuncs'
call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
call SysLoadFuncs
say "Load the functions:" REXXLIBLoadFuncs('version')
say "The version of the rxmlutil library is "||version

/* This will show how the copy function works */
/* Copy over to makefile.oud, replacing the existing file */
say "Copy - replace:" REXXLIBCopy('makefile', 'makefile.oud', 'R')
call pressanykey
say "Delete - recoverable:" REXXLIBdelete('makefile.oud')
call pressanykey
/* Copy over to makefile.oud, appending to the existing file */
/* The file doesn't exist, but gets created */
say "Copy - append:" REXXLIBCopy('makefile', 'makefile.oud', 'A')
call pressanykey
/* Copy over to makefile.oud, appending to the existing file */
say "Copy - append:" REXXLIBCopy('makefile', 'makefile.oud', 'A')
call pressanykey
/* Copy over to makefile.oud, fail if it exists */
say "Copy - fail if exists:" REXXLIBCopy('makefile', 'makefile.oud', 'F')
call pressanykey
/* Start e.exe in the background, inherit environment of the parent proces */
/*
say "Start program:" REXXLIBstartprogramme('e.exe', 'makefile.oud', , 'DEF', 'back', 'p')
call pressanykey
*/
/* Unrecoverable delete */
say "Delete - not recoverable:" REXXLIBDelete('makefile.oud', 'F')
call pressanykey

/* Query the switchlist, we are smart enough to add a dot if the stem doesn't have one */
/*
say "Query SwitchList - rxmlutil:" REXXLIBQuerySwitchList('TEST')
do i=1 to test.0
    say test.i
end
say test.0||' entries'
call pressanykey
*/
/* Show the difference with the REXXUTIL function */
/*
say 'Query SwitchList - REXXUTIL:' sysQuerySwitchList('testsys.', 'N')
do i=1 to testsys.0
    say testsys.i
end
say testsys.0||' entries'
call pressanykey
*/
/*dir=DIRECTORY()
call SysSetObjectData dir, "OPEN=icon"
call syssleep 1
call pressanykey
*/
/*
say "Close an object:" REXXLIBCloseObject('e.exe')
call pressanykey
*/
say "Search Path:" REXXLIBSearchPath("DPATH", "SYSLEVEL.REQ", "TEST")
say TEST
say "Drop the functions:" REXXLIBDropFuncs()
call pressanykey

exit

pressanykey: procedure
    say "Press a key to continue..."
    call SysGetKey 'NOECHO'
return

