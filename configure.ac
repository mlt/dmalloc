#
# Process this file with autoconf to produce a configure script.
#
# Copyright 2020 by Gray Watson
#
# This file is part of the dmalloc package.
#
# Permission to use, copy, modify, and distribute this software for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Gray Watson not be used in
# advertising or publicity pertaining to distribution of the
# document or software without specific, written prior permission.
#
# Gray Watson makes no representations about the suitability of the
# software described herein for any purpose.  It is provided "as is"
# without express or implied warranty.
#
# The author may be contacted via https://dmalloc.com/
#

AC_INIT([dmalloc], [5.6.5], [https://dmalloc.com/])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR(dmalloc.c)
AC_CONFIG_HEADERS([conf.h])

# Add settings.h include at the end of conf.h
AH_BOTTOM([
#if defined(DLL_EXPORT)
#define DMALLOC_API __declspec(dllexport)
#elif _WIN32 && !defined(DMALLOC_STATIC_LINK)
#define DMALLOC_API __declspec(dllimport)
#else
#define DMALLOC_API
#endif

#ifndef __CONF_H__
#define __CONF_H__
#include "settings.h"
#endif
])

# Enable canonical host detection for better platform support
AC_CANONICAL_HOST

dmalloc_version_major=`echo $PACKAGE_VERSION | sed -e '[s/\.[0-9][0-9]*\.[0-9][0-9]*$//]'`
export dmalloc_version_major
export dmalloc_version_minor=`echo $PACKAGE_VERSION | sed -e '[s/^[0-9][0-9]*\.//]' | sed -e '[s/\.[0-9][0-9]*$//]'`
export dmalloc_version_minor
export dmalloc_version_patch=`echo $PACKAGE_VERSION | sed -e '[s/^[0-9][0-9]*\.[0-9][0-9]*\.//]'`
export dmalloc_version_patch
dmalloc_version="${dmalloc_version_major}.${dmalloc_version_minor}.${dmalloc_version_patch}"
export dmalloc_version

# Set up colors for messages
if tput setaf 1 >/dev/null 2>&1; then
    notice=$(tput setaf 6)
    warn=$(tput setaf 3)
    error=$(tput setaf 1)
    clr=$(tput sgr0)
fi
AC_DEFUN([MSG_NOTICE], [AC_MSG_NOTICE([${notice}$1${clr}])])
AC_DEFUN([MSG_WARN], [AC_MSG_WARN([${warn}$1${clr}])])
AC_DEFUN([MSG_ERROR], [AC_MSG_ERROR([${error}$1${clr}])])

MSG_NOTICE([configurations for the dmalloc library version $dmalloc_version])

##############################################################################
# arguments

AC_ARG_ENABLE(cxx,
    [ AS_HELP_STRING([--disable-cxx],
		[do not build in C++ support (enabled by default)] ) ],
    [ # enable_cxx set by default ],
    [ enable_cxx=yes ]
)
AC_ARG_ENABLE(threads,
    [ AS_HELP_STRING([--enable-threads],
		[build in thread support (disabled by default)] ) ],
    [ # enable_threads set by default ],
    [ enable_threads=no ]
)

vcpkg_detours_default="$VCPKG_ROOT/installed/${VCPKG_DEFAULT_TRIPLET:-x64-windows}"

AC_ARG_WITH([detours],
    [ AS_HELP_STRING([--with-detours@<:@=DIR@:>@],
        [build with Microsoft Detours support (MSVC only) @<:@default=VCPKG_PATH@:>@])],
    [
        AS_CASE(["$withval"],
            [yes], [ with_detours=$vcpkg_detours_default ],
            [no],  [ with_detours=no ],
            [*],   [ with_detours=$withval ]
        )
    ],
    [ with_detours=$vcpkg_detours_default ]
)

##############################################################################
MSG_NOTICE([build utilities])

AC_PROG_CC
AC_PROG_CXX

# see if we actually have a CXX program
AC_PATH_PROG(ac_cv_prog_CXX, $CXX)
if test "$ac_cv_prog_CXX" = "" -o ! -x "$ac_cv_prog_CXX"; then
    AC_MSG_WARN(could not find C++ compiler $ac_cv_prog_CXX)
    enable_cxx=no
fi

# hopefully we have a stdc c-compiler
if test "$ac_cv_prog_cc_stdc" = "no" ; then
	AC_MSG_WARN()
	AC_MSG_WARN(no ansi compiler.  this build may fail.)
	AC_MSG_WARN()
fi
AC_PROG_INSTALL

# Initialize automake
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_PROG_AR
AM_SILENT_RULES([yes])

# Ensure cl CXX also uses compile wrapper (like CC does via AM_PROG_CC_C_O)
# This is necessary for MSVC which doesn't support -c and -o together
if test "$am_cv_prog_cc_c_o" != yes; then
  case $CXX in
    *\\* | */*) ;;
    *cl*) CXX="$am_aux_dir/compile $CXX" ;;
  esac
fi

LT_INIT([win32-dll]) dnl benign for other platforms
LT_LANG([C++])

AS_CASE([$host_os], [msys*|cygwin*|mingw*|win*], [windows_platform=yes])
AS_CASE([$enable_cxx,$enable_shared,$windows_platform],
    [yes,yes,yes], [ MSG_WARN(m4_normalize([
        You cannot use global new/delete operators from a shared library (DLL) on Windows.
        Use static library for that instead.]))
    ])

# Set up automake conditionals for build options
AM_CONDITIONAL([WINDOWS_PLATFORM], [test "$windows_platform" = "yes"])
AM_CONDITIONAL([ENABLE_SHARED], [test "$enable_shared" = "yes"])
AM_CONDITIONAL([ENABLE_STATIC], [test "$enable_static" = "yes"])
AM_CONDITIONAL([ENABLE_CXX], [test "$enable_cxx" = "yes"])
AM_CONDITIONAL([ENABLE_THREADS], [test "$enable_threads" = "yes"])
AM_CONDITIONAL([BUILD_DOCS], [false])

AC_CHECK_HEADERS([stdarg.h stdio.h stdlib.h string.h strings.h unistd.h sys/types.h sys/mman.h],
    [],
    [MSG_WARN([some standard headers not found])])
###############################################################################

AC_MSG_CHECKING([C++ support])
if test "$enable_cxx" = "yes"; then
	AC_SUBST([CXX_ON],[])
	AC_SUBST([CXX_OFF],[#])
	AC_MSG_RESULT([enabled])
else
	AC_SUBST([CXX_ON],[#])
	AC_SUBST([CXX_OFF],[])
	AC_MSG_RESULT([disabled])
fi

AC_MSG_CHECKING([thread support])
if test "$enable_threads" = "yes"; then
	AC_SUBST([TH_ON],[])
	AC_SUBST([TH_OFF],[#])
	AC_MSG_RESULT([enabled])
else
	AC_SUBST([TH_ON],[#])
	AC_SUBST([TH_OFF],[])
	AC_MSG_RESULT([disabled])
fi

###############################################################################
#
# autoheader templates
#
AH_TEMPLATE([ALLOCATION_ALIGNMENT],       [Byte alignment for allocations])
AH_TEMPLATE([CONSTRUCTOR_WORKS],          [Define to 1 if constructor attribute works])
AH_TEMPLATE([DESTRUCTOR_WORKS],           [Define to 1 if destructor attribute works])
AH_TEMPLATE([DMALLOC_SIZE],               [Type used for dmalloc sizes])
AH_TEMPLATE([DMALLOC_SIZE_UNSIGNED],      [Define to 1 if DMALLOC_SIZE is unsigned])
AH_TEMPLATE([GETENV_SAFE],                [Define to 1 if getenv is safe to call])
AH_TEMPLATE([HAVE_GETENVIRONMENTVARIABLEA],[Define to 1 if GetEnvironmentVariableA is available])
AH_TEMPLATE([HAVE_PTHREAD_H],             [Define to 1 if <pthread.h> is available])
AH_TEMPLATE([HAVE_PTHREADS_H],            [Define to 1 if <pthreads.h> is available])
AH_TEMPLATE([HAVE_SIGNAL_H],              [Define to 1 if <signal.h> is available])
AH_TEMPLATE([HAVE_SYS_CYGWIN_H],          [Define to 1 if <sys/cygwin.h> is available])
AH_TEMPLATE([HAVE_W32API_WINBASE_H],      [Define to 1 if <w32api/winbase.h> is available])
AH_TEMPLATE([HAVE_W32API_WINDEF_H],       [Define to 1 if <w32api/windef.h> is available])
AH_TEMPLATE([IDENT_WORKS],                [Define to 1 if #ident works])
AH_TEMPLATE([PNT_ARITH_TYPE],             [Type used for pointer arithmetic])
AH_TEMPLATE([PROTECT_ALLOWED],            [Define to 1 if mprotect works])
AH_TEMPLATE([RETURN_MACROS_WORK],         [Define to 1 if return.h macros work])
AH_TEMPLATE([SIGNAL_OKAY],                [Define to 1 if signal() works])
AH_TEMPLATE([THREAD_LOCK_INIT_VAL],       [Initializer for pthread mutex attributes])
AH_TEMPLATE([THREAD_MUTEX_T],             [Type used for pthread mutex])
AH_TEMPLATE([USE_MMAP],                   [Define to 1 if mmap can be used])

###############################################################################
#
# check for size_t
#
AC_TYPE_SIZE_T
if test "$ac_cv_type_size_t" = "yes"; then
	AC_DEFINE(DMALLOC_SIZE,size_t)
else
	AC_DEFINE(DMALLOC_SIZE,unsigned long)
fi

#
# check for unsigned-ness
#
AC_MSG_CHECKING([dmalloc size unsigned])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
int main() { DMALLOC_SIZE x = -1; if (x >= 0) return 0; else return 1; }
]])],
[AC_DEFINE(DMALLOC_SIZE_UNSIGNED,1)  AC_MSG_RESULT([yes])],
[AC_DEFINE(DMALLOC_SIZE_UNSIGNED,0)  AC_MSG_RESULT([no])],
[AC_DEFINE(DMALLOC_SIZE_UNSIGNED,0)  AC_MSG_RESULT([no])]
)

#
# check for strdup macro (linux)
#
AC_MSG_CHECKING([strdup macro])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif

#ifndef strdup
#  error "no strdup macro"
#endif
]])],
[ac_cv_strdup_macro=yes],
[ac_cv_strdup_macro=no],
[ac_cv_strdup_macro=no]
)
AC_MSG_RESULT([$ac_cv_strdup_macro])

#
# check for strndup macro (linux)
#
AC_MSG_CHECKING([strndup macro])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_STRING_H
#  include <string.h>
#endif

#ifndef strndup
#  error "no strndup macro"
#endif
]])],
[ac_cv_strndup_macro=yes],
[ac_cv_strndup_macro=no],
[ac_cv_strndup_macro=no]
)
AC_MSG_RESULT([$ac_cv_strndup_macro])

#
# check for existance of mprotect and associated defines
#
AC_CHECK_FUNCS(mprotect)
AC_CACHE_CHECK([mprotect works], [ac_cv_mprotect_works], [
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_MMAN_H
#  include <sys/mman.h>
#endif
]],[[
  int prot = PROT_NONE | PROT_READ | PROT_WRITE;
  (void)mprotect(0, 0, prot);
]])],
[ ac_cv_mprotect_works="yes" ], [ ac_cv_mprotect_works="no" ])
])

if test "$ac_cv_mprotect_works" = "yes"; then
  AC_DEFINE([PROTECT_ALLOWED], [1], [Set to 1 if the mprotect function was found and the PROT_NONE, PROT_READ, and PROT_WRITE defines were found in sys/mman.h.  This is so that we can restrict access to certain blocks of memory.])
fi

##############################################################################
MSG_NOTICE([important functionality])

AC_CHECK_FUNCS(mmap munmap sbrk)

if test "x$ac_cv_func_mmap" != "xyes" && test "x$ac_cv_func_sbrk" != "xyes"; then
	AC_MSG_WARN()
	AC_MSG_WARN(no mmap nor sbrk function.  See INTERNAL_MEMORY_SPACE in settings.dist.)
	AC_MSG_WARN()
fi

AC_MSG_CHECKING([mmap usage])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#if HAVE_SYS_MMAN_H
#  include <sys/mman.h>
#endif
int main() {
  /* we could open /dev/zero and map to that but I did not want the library
     to open another file-descriptor */
  char *dest, *src, *src_p, *dest_p;
  src = "0123456789";
  dest = (char *)mmap(0L, 10, PROT_READ | PROT_WRITE | PROT_EXEC,
	              MAP_PRIVATE | MAP_ANON
#if defined(MAP_JIT)
                      | MAP_JIT
#endif
                  , -1 /* no fd */,
		      0 /* no offset */);
  src_p = src; dest_p = dest;
  while (src_p < src + 10) { *dest_p++ = *src_p++; }
  src_p = src; dest_p = dest;
  while (src_p < src + 10) {
    if (*src_p++ != *dest_p++) { return 1; }
  }
  return 0;
}
]])],
[ AC_DEFINE(USE_MMAP, 1) ac_cv_use_mmap=yes],
[ AC_DEFINE(USE_MMAP, 0) ac_cv_use_mmap=no],
[ # For cross-compilation, assume mmap works if the function is available
  if test "x$ac_cv_func_mmap" = "xyes"; then
    AC_DEFINE(USE_MMAP, 1) ac_cv_use_mmap=yes
  else
    AC_DEFINE(USE_MMAP, 0) ac_cv_use_mmap=no
  fi
])
AC_MSG_RESULT([$ac_cv_use_mmap])

#
# check for basic block size
#
AC_CHECK_FUNCS(getpagesize)
dnl Windows separates hardware page size (dwPageSize is usually 4kb) and
dnl allocation granularity (normally 64kb).
dnl To keep things simple and avoid counting committed memory,
dnl we use allocation granularity as the basic block size on Windows.
AC_CACHE_CHECK([basic-block size], [ac_cv_page_size], [
AC_RUN_IFELSE([AC_LANG_PROGRAM([
#if HAVE_UNISTD_H
  #include <unistd.h>
#endif
  #include <stdio.h>
#if _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  static int getpagesize () {
    SYSTEM_INFO si; GetSystemInfo(&si);
    return si.dwAllocationGranularity;
  }
#endif
  ], [
  long ps = getpagesize(); int bs;
  for (bs = -1; ps; ++bs, ps >>= 1) {}
  fprintf(stderr, "%ld\n", bs);
])],
	[ ac_cv_page_size=$(./conftest$EXEEXT 2>&1) ],
	[ ac_cv_page_size=15 ], [ ac_cv_page_size=15 ])
])
AC_DEFINE_UNQUOTED([BASIC_BLOCK],[$ac_cv_page_size], [This is the basic block size in bits.  If possible, the configure script will set this to be the value returned by the getpagesize() function.  If not then some sort of best guess will be necessary. 15 (meaning basic block size of 32k) will probably be good.

NOTE: some sbrk functions round to the correct page-size. No problems aside from a possible small increase in the administration overhead should happen if this value is too high.])

#
# data-alignment size...
#
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(char *)
AC_CHECK_SIZEOF(long long)
AC_MSG_CHECKING([data-alignment size])
if test $ac_cv_sizeof_char_p -lt 8; then
	# we have to make a special case for sun sparc idiocy
	ac_cv_data_align=8
else
	ac_cv_data_align=$ac_cv_sizeof_char_p
fi
AC_DEFINE_UNQUOTED([ALLOCATION_ALIGNMENT],[$ac_cv_data_align])
AC_MSG_RESULT([$ac_cv_data_align])

#
# trying to find good type (64-bit?) for pointer arithmatic
#
AC_MSG_CHECKING([pointer arithmatic type])
if test $ac_cv_sizeof_long = $ac_cv_sizeof_char_p; then
	ac_cv_pnt_arith_type="unsigned long"
else
	if test $ac_cv_sizeof_long_long = $ac_cv_sizeof_char_p; then
		# NOTE: we don't use unsigned here because of 63-bit history
		ac_cv_pnt_arith_type="long long"
	else
		# not sure what to do so take long
		ac_cv_pnt_arith_type="unsigned long"
	fi
fi
AC_DEFINE_UNQUOTED([PNT_ARITH_TYPE],[$ac_cv_pnt_arith_type])
AC_MSG_RESULT([$ac_cv_pnt_arith_type])

#
# check the safety of the abort function
#
AC_CHECK_FUNCS(abort)
AC_CACHE_CHECK([abort safe], [ac_cv_abort_safe], [
AC_RUN_IFELSE([AC_LANG_SOURCE([[
static int main_b = 0;
static char heap_mem[102400], *heap_p = heap_mem;
free () { if (main_b) _exit(0); }
char *malloc (int size) {
  char *pnt;
  if (main_b) _exit(0);
  pnt = heap_p;
  heap_p += size;
  return pnt;
}
char *calloc (int number, int size) {
  char *start, *pnt, *end;
  if (main_b) _exit(0);
  /* it should be already 0s */
  start = malloc (number * size);
  pnt = start;
  end = start + size;
  while (pnt < end) { *pnt++ = '\0'; }
  return start;
}
char *realloc (char *old_pnt, int new_size) {
  char *start, *pnt, *end;
  if (main_b) _exit(0);
  start = malloc (new_size);
  pnt = start;
  end = start + new_size;
  while (pnt < end) { *pnt++ = *old_pnt++; }
  return start;
}
int main() { main_b = 1; abort(); return 1; }
]])],
# NOTE: this is reversed because abort core dumps and doesn't exit with
# 0 which means that it should fail
[ ac_cv_abort_safe=no ],
# if it succeeds then something called free on the way out
[ ac_cv_abort_safe=yes ],
[ ac_cv_abort_safe=no ]
)
])
if test "$ac_cv_abort_safe" = "yes"; then
	AC_DEFINE([ABORT_OKAY], [1], m4_normalize([
		This checks to see if the abort routine does extensive cleaning up
		before halting a program.  If so then it may call malloc functions
		making the library go recursive.  If abort is set to not okay then
		you should tune the KILL_PROCESS and SIGNAL_INCLUDE options in
		settings.h if you want the library to be able to dump core.
	]))
fi

AC_CHECK_HEADER([signal.h],
		[AC_DEFINE(HAVE_SIGNAL_H,1) AC_SUBST([HAVE_SIGNAL_H],1)],
		[AC_DEFINE(HAVE_SIGNAL_H,0) AC_SUBST([HAVE_SIGNAL_H],0)])
AC_MSG_CHECKING([signal works])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_SIGNAL_H
  #include <signal.h>
#endif
]], [[
	(void)signal(SIGHUP, SIG_IGN);
	(void)signal(SIGINT, SIG_IGN);
	(void)signal(SIGTERM, SIG_IGN);
]])],
[ AC_DEFINE(SIGNAL_OKAY, 1) AC_MSG_RESULT([yes]) ],
[ AC_DEFINE(SIGNAL_OKAY, 0) AC_MSG_RESULT([no]) ]
)

#
# check the safety of the getenv function
#
AC_CHECK_FUNCS(getenv)
AC_MSG_CHECKING([getenv safe])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
static char heap_mem[102400], *heap_p = heap_mem;
free (char *pnt) {
}
char *malloc (int size) {
  char *pnt;
  getenv("PATH");
  pnt = heap_p;
  heap_p += size;
  return pnt;
}
char *calloc (int number, int size) {
  char *start, *pnt, *end;
  getenv("PATH");
  /* it should be already 0s */
  start = malloc (number * size);
  pnt = start;
  end = start + size;
  while (pnt < end) { *pnt++ = '\0'; }
  return start;
}
char *realloc (char *old_pnt, int new_size) {
  char *start, *pnt, *end;
  getenv("PATH");
  start = malloc (new_size);
  pnt = start;
  end = start + new_size;
  while (pnt < end) { *pnt++ = *old_pnt++; }
  return start;
}
int main() { malloc(10); return 0; }
]])],
[ AC_DEFINE(GETENV_SAFE, 1)
  AC_MSG_RESULT([yes])
  getenv_safe=yes ],
# if it succeeds then something called free on the way out
[ AC_DEFINE(GETENV_SAFE, 0)
  AC_MSG_RESULT([no])
  getenv_safe=no ],
[ AC_DEFINE(GETENV_SAFE, 1)
  AC_MSG_RESULT([yes])
  getenv_safe=yes ]
)

# now check to see if we can replace getenv with GetEnvironmentVariableA
AC_CHECK_HEADER([sys/cygwin.h],
		[AC_DEFINE(HAVE_SYS_CYGWIN_H,1) AC_SUBST([HAVE_SYS_CYGWIN_H],1)],
		[AC_DEFINE(HAVE_SYS_CYGWIN_H,0) AC_SUBST([HAVE_SYS_CYGWIN_H],0)])
AC_CHECK_HEADER([w32api/windef.h],
		[AC_DEFINE(HAVE_W32API_WINDEF_H,1) AC_SUBST([HAVE_W32API_WINDEF_H],1)],
		[AC_DEFINE(HAVE_W32API_WINDEF_H,0) AC_SUBST([HAVE_W32API_WINDEF_H],0)])
AC_CHECK_HEADER([w32api/winbase.h],
		[AC_DEFINE(HAVE_W32API_WINBASE_H,1) AC_SUBST([HAVE_W32API_WINBASE_H],1)],
		[AC_DEFINE(HAVE_W32API_WINBASE_H,0) AC_SUBST([HAVE_W32API_WINBASE_H],0)],
		[[
#if HAVE_W32API_WINDEF_H
# include <w32api/windef.h>
#endif]])
AC_MSG_CHECKING([for GetEnvironmentVariableA])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#if HAVE_SYS_CYGWIN_H
# include <sys/cygwin.h>
#endif
#if HAVE_STDARG_H
# include <stdarg.h>
#endif
#if HAVE_W32API_WINDEF_H
# include <w32api/windef.h>
#endif
#if HAVE_W32API_WINBASE_H
# include <w32api/winbase.h>
#endif
int main() {
  char env_buf[256];
  GetEnvironmentVariableA("PATH", env_buf, sizeof(env_buf));
}
]])],
[ AC_DEFINE(HAVE_GETENVIRONMENTVARIABLEA, 1)
  AC_MSG_RESULT([yes])
  have_getenvironmentvariablea=yes ],
[ AC_DEFINE(HAVE_GETENVIRONMENTVARIABLEA, 0)
  AC_MSG_RESULT([no])
  have_getenvironmentvariablea=no ],
[ AC_DEFINE(HAVE_GETENVIRONMENTVARIABLEA, 1)
  AC_MSG_RESULT([yes])
  have_getenvironmentvariablea=yes ]
)

if test "x$getenv_safe" != "xyes" && test "x$have_getenvironmentvariablea" != "xyes" ; then
  MSG_WARN([Since getenv allocates memory, it is not safe for dmalloc to use.])
  MSG_WARN([You may want to add the following code to the front of main():])
  MSG_WARN([[  #ifdef DMALLOC]])
  MSG_WARN([[    dmalloc_debug_setup(getenv("DMALLOC_OPTIONS"));]])
  MSG_WARN([[  #endif]])
fi

############

#
# check for constructor attribute support
#
AC_MSG_CHECKING([constructor attribute])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
/* if we call the loc_con constructor then exit with error code 0 */
static void loc_con() __attribute__((constructor));
static void loc_con() { exit(0); }
int main() { return 1; }
]])],
[ AC_DEFINE(CONSTRUCTOR_WORKS, 1) AC_MSG_RESULT([yes]) ],
[ AC_DEFINE(CONSTRUCTOR_WORKS, 0) AC_MSG_RESULT([no]) ],
[ AC_DEFINE(CONSTRUCTOR_WORKS, 1) AC_MSG_RESULT([yes]) ]
)

#
# check for destructor attribute support
#
AC_MSG_CHECKING([destructor attribute])
AC_RUN_IFELSE([AC_LANG_SOURCE([[
/* if we call the loc_decon destructor then exit with error code 0 */
static void loc_decon() __attribute__((destructor));
static void loc_decon() { exit(0); }
int main() { return 1; }
]])],
[ AC_DEFINE(DESTRUCTOR_WORKS, 1)
  AC_MSG_RESULT([yes])
  destructor_works=yes ],
[ AC_DEFINE(DESTRUCTOR_WORKS, 0)
  AC_MSG_RESULT([no])
  destructor_works=no ],
[ AC_DEFINE(DESTRUCTOR_WORKS, 1)
  AC_MSG_RESULT([yes])
  destructor_works=yes ],
)

##############################################################################
XX_OLD_CPPFLAGS=$CPPFLAGS
CPPFLAGS+=" -I$with_detours/include"
AC_CHECK_HEADER([detours/detours.h], [
	AC_DEFINE([HAVE_DETOURS], [1], [Define to 1 if Microsoft Detours is available])
	AC_SUBST([VCPKG_ROOT_DIR], [${with_detours//\\/\/}])
], [], [dnl needed to define architecture macros like _AMD64_
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
])
CPPFLAGS=$XX_OLD_CPPFLAGS
dnl Detours uses new/delete internally. Without dmallocc.cc, we will go into recursion.
AM_CONDITIONAL([HAVE_DETOURS], [test "x$enable_cxx" = "xyes" && test "x$ac_cv_header_detours_detours_h" = "xyes"])

dnl ac_cv_have_decl__MSC_VER
AC_CHECK_DECL([_MSC_VER], [
    ac_cv_return_macros_work=yes
])
AM_CONDITIONAL([MSVC], [test "x$ac_cv_have_decl__MSC_VER" = "xyes"])

##############################################################################

#
# check if the return.h macros work
#
# remember -fno-omit-frame-pointer
AC_CACHE_CHECK([return.h macros work], [ac_cv_return_macros_work], [
XX_OLD_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS -I$srcdir"
AC_RUN_IFELSE([AC_LANG_PROGRAM([

#define __CONF_H__
#define USE_RETURN_MACROS 1
#define RETURN_MACROS_WORK 1

#include "return.h"

static void foo (void)
{
  char	*ret_addr;
  GET_RET_ADDR(ret_addr);
}], [ foo() ])], [
AS_CASE([$host_cpu],
    [x86_64], [ AS_IF(
        [ $OBJDUMP --disassemble=main ./conftest$EXEEXT | grep -B1 "mov.*%rsp,.*%rbp" | grep "push   %rbp" > /dev/null ],
        [ ac_cv_return_macros_work=yes ; ac_cv_have_frame_pointer=yes ],
        [ ac_cv_return_macros_work=no ; ac_cv_have_frame_pointer=no ;
            MSG_WARN([The return.h macros using asm do not work correctly without frame pointers.])
        ]) ],
    [*], [ ac_cv_return_macros_work=yes ; ac_cv_have_frame_pointer=no ] )],
[ ac_cv_return_macros_work=no ; ac_cv_have_frame_pointer=no ],
[ ac_cv_return_macros_work=no ; ac_cv_have_frame_pointer=no ])
CPPFLAGS=$XX_OLD_CPPFLAGS
])
if test "x$ac_cv_have_frame_pointer" = "xyes" ; then
	AC_DEFINE([HAVE_FRAME_POINTER], [1], [Define if frame pointer is available, i.e. -fno-omit-frame-pointer is used.]) 
fi
if test "$ac_cv_return_macros_work" = "yes" ; then
	AC_DEFINE([RETURN_MACROS_WORK], [1], [This checks to see if we can include return.h and use the assembly macros there to call the callers address for logging.  If you do not want this behavior, then set the USE_RETURN_MACROS to 0 in the settings.h file.])
fi

##############################################################################

#
# check to see if ident workds
#
AC_MSG_CHECKING([if ident works])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#ident "configure.ac"
static void foo (void) { }
])],
[AC_DEFINE(IDENT_WORKS, 1) AC_MSG_RESULT([yes])],
[AC_DEFINE(IDENT_WORKS, 0) AC_MSG_RESULT([no])]
)

##############################################################################
MSG_NOTICE([pthread particulars])

XX_OLD_LIBS=$LIBS

AC_SEARCH_LIBS(pthread_mutex_init, pthread pthreads c_r)
AC_CHECK_HEADER([pthread.h],
		[AC_DEFINE(HAVE_PTHREAD_H,1) AC_SUBST([HAVE_PTHREAD_H],1)],
		[AC_DEFINE(HAVE_PTHREAD_H,0) AC_SUBST([HAVE_PTHREAD_H],0)])
AC_CHECK_HEADER([pthreads.h],
		[AC_DEFINE(HAVE_PTHREADS_H,1) AC_SUBST([HAVE_PTHREADS_H],1)],
		[AC_DEFINE(HAVE_PTHREADS_H,0) AC_SUBST([HAVE_PTHREADS_H],0)])

AC_CHECK_FUNCS(pthread_mutex_init pthread_mutex_lock pthread_mutex_unlock)

AC_MSG_CHECKING([pthread mutex type])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_PTHREAD_H
#  include <pthread.h>
#endif
#if HAVE_PTHREADS_H
#  include <pthreads.h>
#endif
]],[[
   pthread_mutex_t dmalloc_mutex;
]])],
[AC_DEFINE(THREAD_MUTEX_T,pthread_mutex_t) AC_MSG_RESULT([pthread_mutex_t])],
[AC_MSG_RESULT([unknown])]
)

AC_MSG_CHECKING([pthread attribute initialization])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#if HAVE_PTHREAD_H
#  include <pthread.h>
#endif
#if HAVE_PTHREADS_H
#  include <pthreads.h>
#endif
]], [[
	THREAD_MUTEX_T dmalloc_mutex;
	pthread_mutex_init(&dmalloc_mutex, pthread_mutexattr_default);
]])],
[AC_DEFINE(THREAD_LOCK_INIT_VAL,pthread_mutexattr_default)
 AC_MSG_RESULT([pthread_mutexattr_default])],
[AC_DEFINE(THREAD_LOCK_INIT_VAL,0)
 AC_MSG_RESULT([0])]
)

LIBS=$XX_OLD_LIBS

##############################################################################
MSG_NOTICE([functions])

# auto-shutdown functions
AC_CHECK_FUNCS(atexit on_exit)
if test "$ac_cv_func_atexit" = "no" \
   && test "$ac_cv_func_on_exit" = "no" \
   && test "$destructor_works" = "no"; then
	AC_MSG_WARN()
	AC_MSG_WARN(The library cannot automatically shut itself down without)
	AC_MSG_WARN(atexit, on_exit, or destructor support.  You will need to)
	AC_MSG_WARN(call dmalloc_shutdown directly before exit to get get the)
	AC_MSG_WARN(final statistics and unfreed memory information.)
	AC_MSG_WARN()
fi

# other bells and whistles
AC_CHECK_FUNCS(fork gethostname getpid getuid time ctime)
AC_CHECK_FUNCS(vprintf snprintf vsnprintf)
AC_CHECK_FUNCS(recalloc memalign valloc)

# required although we have code for them in compat.c
AC_CHECK_FUNCS(memcmp memcpy memmove memset)
AC_CHECK_FUNCS(strchr strrchr)
AC_CHECK_FUNCS(strlen strnlen strcmp strcpy strsep)

##############################################################################
MSG_NOTICE([various functions for argv files])

# NOTE: some duplicated from above
AC_CHECK_FUNCS(atoi atol getenv)
AC_CHECK_FUNCS(strchr strcmp strcpy strlen strncmp strncpy strsep)

##############################################################################
MSG_NOTICE([various functions for argument checking])

# NOTE: some duplicated from above
AC_CHECK_FUNCS(atoi atol)
AC_CHECK_FUNCS(bcmp bcopy bzero)
AC_CHECK_FUNCS(index)
AC_CHECK_FUNCS(memccpy memchr memcmp memcpy memmove memset)
AC_CHECK_FUNCS(rindex)
AC_CHECK_FUNCS(strdup strndup)
AC_CHECK_FUNCS(strcasecmp strcat strchr strcmp strcpy strcspn strlen)
AC_CHECK_FUNCS(strncasecmp strncat strncmp strncpy)
AC_CHECK_FUNCS(strpbrk strrchr strspn strstr strtok)

AC_CONFIG_FILES(Makefile)

AC_CONFIG_COMMANDS([dmalloc.h.2],
[
newfile=dmalloc.h.2
rm -f $newfile.t
echo '/* this is dmalloc.h.2 */' > $newfile.t
echo '/* produced by configure, inserted into dmalloc.h */' >> $newfile.t
echo '' >> $newfile.t

if test "$ac_cv_strdup_macro" = "yes"; then
	echo '/* strdup is a macro */' >> $newfile.t
	echo '#define DMALLOC_STRDUP_MACRO' >> $newfile.t
else
	echo '/* strdup is not a macro */' >> $newfile.t
	echo '#undef DMALLOC_STRDUP_MACRO' >> $newfile.t
fi
if test "$ac_cv_strndup_macro" = "yes"; then
	echo '/* strndup is a macro */' >> $newfile.t
	echo '#define DMALLOC_STRNDUP_MACRO' >> $newfile.t
else
	echo '/* strndup is not a macro */' >> $newfile.t
	echo '#undef DMALLOC_STRNDUP_MACRO' >> $newfile.t
fi
echo '' >> $newfile.t

echo '/*' >> $newfile.t
echo ' * the definition of DMALLOC_SIZE' >> $newfile.t
echo ' *' >> $newfile.t
echo ' * NOTE: some architectures have malloc, realloc, etc.' >> $newfile.t
echo ' * using unsigned instead of unsigned long.  You may' >> $newfile.t
echo ' * have to edit this by hand to fix any compilation' >> $newfile.t
echo ' * warnings or errors.' >> $newfile.t
echo ' */' >> $newfile.t
if test "$ac_cv_type_size_t" = "yes"; then
	echo "#include <sys/types.h>" >> $newfile.t
	echo "#define DMALLOC_SIZE size_t" >> $newfile.t
else
	echo "/* no include file needed */" >> $newfile.t
	echo "#define DMALLOC_SIZE unsigned long" >> $newfile.t
fi
echo '' >> $newfile.t
echo '/*' >> $newfile.t
echo ' * We use stdarg.h for the dmalloc_message and' >> $newfile.t
echo ' * dmalloc_vmessage functions.' >> $newfile.t
echo ' */' >> $newfile.t
if test "$ac_cv_header_stdarg_h" = "yes"; then
	echo "#include <stdarg.h>" >> $newfile.t
	echo "#define DMALLOC_STDARG 1" >> $newfile.t
else
	echo "/* no stdarg.h available */" >> $newfile.t
	echo "#define DMALLOC_STDARG 0" >> $newfile.t
fi
echo '' >> $newfile.t
echo '/* dmalloc version defines */' >> $newfile.t
echo "#define DMALLOC_VERSION_MAJOR ${dmalloc_version_major} /* X.0.0 */" >> $newfile.t
echo "#define DMALLOC_VERSION_MINOR ${dmalloc_version_minor} /* 0.X.0 */" >> $newfile.t
echo "#define DMALLOC_VERSION_PATCH ${dmalloc_version_patch} /* 0.0.X */" >> $newfile.t
echo '' >> $newfile.t
if cmp -s $newfile $newfile.t 2>/dev/null; then
	MSG_NOTICE([$newfile is unchanged])
	rm -f $newfile.t
else
	rm -f $newfile
	mv $newfile.t $newfile
fi
],
[
# dmalloc.h.2 initialization commands
ac_cv_strdup_macro=$ac_cv_strdup_macro
ac_cv_strndup_macro=$ac_cv_strndup_macro
ac_cv_type_size_t=$ac_cv_type_size_t
ac_cv_header_stdarg_h=$ac_cv_header_stdarg_h
])
AC_CONFIG_COMMANDS([version.h],
[
newfile="version.h"
rm -f $newfile.t
cat > $newfile.t<<EOF
/* this is produced by configure */
#ifndef __VERSION_H__
#define __VERSION_H__
static char *dmalloc_version = "${dmalloc_version}";
#endif /* ! __VERSION_H__ */
EOF
if cmp -s $newfile $newfile.t 2>/dev/null; then
	MSG_NOTICE([$newfile is unchanged])
	rm -f $newfile.t
else
	rm -f $newfile
	mv $newfile.t $newfile
fi
],
[
# version.h initialization commands
dmalloc_version_major=$dmalloc_version_major
dmalloc_version_minor=$dmalloc_version_minor
dmalloc_version_patch=$dmalloc_version_patch
dmalloc_version=$dmalloc_version
])

AC_CONFIG_COMMANDS([settings.h],
[
newfile=settings.h
rm -f $newfile.t
echo '/*' > $newfile.t
echo ' * WARNING: this file was produced from settings.dist' >> $newfile.t
echo ' * by the configure program.  The configure script, when' >> $newfile.t
echo ' * run again, will overwrite changed made here.' >> $newfile.t
echo ' */' >> $newfile.t
echo '' >> $newfile.t
cat $srcdir/settings.dist >> $newfile.t

if cmp -s $newfile.t $newfile 2>/dev/null; then
	MSG_NOTICE([$newfile is unchanged])
	rm -f $newfile.t
else
	rm -f $newfile
	mv $newfile.t $newfile
fi
])
AC_OUTPUT

MSG_NOTICE([])
MSG_NOTICE([Please check-out Makefile and conf.h to make sure that])
MSG_NOTICE([sane configuration values were a result.])
MSG_NOTICE([])
MSG_NOTICE([You may want to change values in settings.h before])
MSG_NOTICE([running 'make'.])
MSG_NOTICE([])
MSG_NOTICE([To run the basic library tests, you can execute:])
MSG_NOTICE([  make light])
MSG_NOTICE([or])
MSG_NOTICE([  make heavy])
MSG_NOTICE([])
