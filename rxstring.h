/*
 rxstring.h - A header file for the rxstring.lib from OS/2 4.5x
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

#ifndef RXSTRING_H
#define RXSTRING_H
INT   rxtoi(RXSTRING);
LONG  rxtol(RXSTRING);
ULONG rxtoul(RXSTRING);
LONG rxwrite(HFILE, RXSTRING);
LONG rxread(HFILE, PRXSTRING);
VOID rxprint(RXSTRING);
RXSTRING rxstrdup(RXSTRING);
RXSTRING rxmemcpy(PRXSTRING, PRXSTRING, ULONG);
RXSTRING rxstrcpy(PRXSTRING, PRXSTRING);
RXSTRING rxstrncpy(PRXSTRING, PRXSTRING, ULONG);
RXSTRING rxstrcat(PRXSTRING, PRXSTRING);
RXSTRING rxstrncat(PRXSTRING, PRXSTRING, ULONG);
RXSTRING strdup2rx(PUCHAR, ULONG);
RXSTRING memcpy2rx(PRXSTRING, PUCHAR, ULONG);
RXSTRING strcpy2rx(PRXSTRING, PSZ);
RXSTRING strcat2rx(PRXSTRING, PSZ);
RXSTRING strncat2rx(PRXSTRING, PSZ, ULONG);
LONG  rxstrcmp(PRXSTRING, PRXSTRING);
LONG  rxstricmp(PRXSTRING, PRXSTRING);
LONG  rxmemcmp(PRXSTRING, PRXSTRING, ULONG);
LONG  rxmemicmp(PRXSTRING, PRXSTRING, ULONG);
PUCHAR rxstrchr(PRXSTRING, UCHAR);
PUCHAR rxstrrchr(PRXSTRING, UCHAR);
RXSTRING rxalloc(ULONG);
VOID rxfree(RXSTRING);
VOID rxstrnset(RXSTRING, UCHAR, ULONG);
ULONG rxstrlen(RXSTRING);
RXSTRING rxset_length(PRXSTRING, ULONG);
RXSTRING rxset_null(PRXSTRING);
RXSTRING rxset_zerolen(PRXSTRING);
RXSTRING rxreturn_value(PRXSTRING, PUCHAR, ULONG);
PUCHAR _make_hptr(RXSTRING);
#endif
