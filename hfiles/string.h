/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the, 1992 Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 *	ANSI Standard: 4.11 STRING HANDLING	<string.h>
 */

#ifndef	_STRING_H

#define	_STRING_H	1

#ifdef REAL
#include <features.h>
__BEGIN_DECLS
#endif

/* SAPC: assume size_t already declared via stdio.h, assumed
   included in program before string.h */

#ifdef REAL
/* `memcpy' is a built-in function for gcc 2.x. */
#if defined(__STDC__) && __GNUC__ < 2
/* Copy N bytes of SRC to DEST.  */
extern __ptr_t memcpy __P ((__ptr_t __dest, __const __ptr_t __src,
			    size_t __n));
#endif
#endif

/* Copy SRC to DEST.  */
extern char *strcpy __P ((char *__dest, __const char *__src));
/* Copy no more than N characters of SRC to DEST.  */
extern char *strncpy __P ((char *__dest, __const char *__src, size_t __n));

/* Append SRC onto DEST.  */
extern char *strcat __P ((char *__dest, __const char *__src));
/* Append no more than N characters from SRC onto DEST.  */
extern char *strncat __P ((char *__dest, __const char *__src, size_t __n));

/* Compare S1 and S2.  */
extern int strcmp __P ((__const char *__s1, __const char *__s2));
/* Compare N characters of S1 and S2.  */
extern int strncmp __P ((__const char *__s1, __const char *__s2, size_t __n));

/* Return the length of S.  */
extern size_t strlen __P ((__const char *__s));

/* Find the first occurrence of C in S (same as strchr).  */
extern char *index __P ((__const char *__s, int __c));

/* Find the last occurrence of C in S (same as strrchr).  */
extern char *rindex __P ((__const char *__s, int __c));

#ifdef REAL
__END_DECLS
#endif

#endif /* string.h  */
