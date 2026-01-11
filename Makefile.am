###############################################################################
# Makefile.am for the DMalloc library.
###############################################################################

.PHONY: light heavy utils

ACLOCAL_AMFLAGS = -I m4 --install

# Libraries to build
lib_LTLIBRARIES =

# Header files to install
include_HEADERS = dmalloc.h

# Binary utilities
bin_PROGRAMS = dmalloc

# Documentation
doc_DATA = INSTALL.txt LICENSE.txt
if BUILD_DOCS
doc_DATA += dmalloc.html dmalloc.info
endif

SOURCES = append.c append.h arg_check.c arg_check.h chunk_loc.h	\
	chunk.c chunk.h compat.c compat.h debug_tok.h			\
	dmalloc_argv_loc.h dmalloc_argv.c dmalloc_argv.h		\
	dmalloc_loc.h dmalloc_rand.c dmalloc_rand.h dmalloc_tab_loc.h	\
	dmalloc_tab.c dmalloc_tab.h env.c env.h error_val.h error.c	\
	error.h heap.c heap.h protect.c protect.h return.h		\
	user_malloc.c user_malloc.h

if ENABLE_STATIC
lib_LTLIBRARIES += libdmalloc_static.la
libdmalloc_static_la_SOURCES = $(SOURCES)
libdmalloc_static_la_CPPFLAGS = $(AM_CPPFLAGS) -DDMALLOC_STATIC_LINK
libdmalloc_static_la_LDFLAGS = -static
if ENABLE_CXX
libdmalloc_static_la_SOURCES += dmallocc.cc
endif
if MSVC
libdmalloc_static_la_CPPFLAGS += -Zi -Fddmalloc_static.pdb
libdmalloc_static_la_LDFLAGS += -Wl,-DEBUG
endif
endif # ENABLE_STATIC

# Utility program
dmalloc_SOURCES = dmalloc.c

# Generated headers
BUILT_SOURCES = dmalloc.h dmalloc.h.2 settings.h version.h

# Test programs
TEST = dmalloc_t
TEST_FC = dmalloc_fc_t
check_PROGRAMS = $(TEST) $(TEST_FC)
pkglibexec_PROGRAMS = $(check_PROGRAMS)
dmalloc_t_SOURCES = $(TEST).c
dmalloc_fc_t_SOURCES = $(TEST_FC).c

if ENABLE_SHARED
lib_LTLIBRARIES += libdmalloc.la
libdmalloc_la_SOURCES = $(SOURCES)
libdmalloc_la_LDFLAGS = -shared -no-undefined -avoid-version
if MSVC
libdmalloc_la_CPPFLAGS = -MD -Zi -Fddmalloc.dll.pdb
## To check for the PDB info in the DLL:
## dumpbin.exe -HEADERS .libs/dmalloc.dll | grep RSDS -A 1
#export _LINK_=-DEBUG -PDB:dmalloc.dll.pdb
libdmalloc_la_LDFLAGS += -Wl,-Xlinker,-DEBUG,-Xlinker,-PDB:dmalloc.dll.pdb
if HAVE_DETOURS
lib_LTLIBRARIES += libdmalloc_detours.la
## Detours uses global new/delete operators. This avoids recursion.
libdmalloc_detours_la_SOURCES = dmalloc_detours.c dmallocc.cc
libdmalloc_detours_la_CPPFLAGS = -MD -Zi -Fddmalloc_detours.dll.pdb -I@VCPKG_ROOT_DIR@/include
libdmalloc_detours_la_LDFLAGS = -shared -no-undefined -avoid-version \
 -Wl,@VCPKG_ROOT_DIR@/lib/detours.lib \
 -Wl,-Xlinker,-DEBUG,-Xlinker,-PDB:dmalloc_detours.dll.pdb
libdmalloc_detours_la_LIBADD = libdmalloc.la -lDbghelp
pkglibexec_PROGRAMS += dmalloc_win32_t
dmalloc_win32_t_SOURCES = dmalloc_win32_t.c
dmalloc_win32_t_LDADD = libdmalloc.la
## This has to be -MD
## Also consider -DYNAMICBASE:NO
dmalloc_win32_t_CPPFLAGS = -MD -Zi -Fddmalloc_win32_t.pdb
endif # HAVE_DETOURS
else # MSVC
if ENABLE_CXX
## TODO: There is no point including it on Windows with non-MSVC compiler either
libdmalloc_la_SOURCES += dmallocc.cc
endif
endif # MSVC
dmalloc_LDADD = libdmalloc.la
dmalloc_t_LDADD = libdmalloc.la
dmalloc_fc_t_LDADD = libdmalloc.la
else # ENABLE_SHARED
dmalloc_LDADD = libdmalloc_static.la
dmalloc_CPPFLAGS = $(AM_CPPFLAGS) -DDMALLOC_STATIC_LINK
dmalloc_t_LDADD = libdmalloc_static.la
dmalloc_t_CPPFLAGS = $(AM_CPPFLAGS) -DDMALLOC_STATIC_LINK
dmalloc_fc_t_LDADD = libdmalloc_static.la
dmalloc_fc_t_CPPFLAGS = $(AM_CPPFLAGS) -DDMALLOC_STATIC_LINK
endif # ENABLE_SHARED

TESTS = $(check_PROGRAMS)

utils: dmalloc
light: $(TEST)
	./$(TEST) -s -t 100000
	./$(TEST) -s -t 100000
	./$(TEST) -s -t 100000
	./$(TEST) -s -t 100000
	./$(TEST) -s -t 100000

check-local: heavy

dmalloc_check:
	./$(TEST_FC) -s
	./$(TEST) -s -t 0

heavy: light
	./$(TEST) -s -t 1000000
	./$(TEST) -s -t 1000000
	./$(TEST) -s -t 1000000

dmalloc.h : $(srcdir)/dmalloc.h.1 dmalloc.h.2 $(srcdir)/dmalloc.h.3 $(srcdir)/user_malloc.h \
		$(srcdir)/dmalloc.h.4 $(srcdir)/arg_check.h $(srcdir)/dmalloc.h.5
	rm -f $@ $@.t
	cat $(srcdir)/dmalloc.h.1 dmalloc.h.2 $(srcdir)/dmalloc.h.3 > $@.t
	sed -e '1,/<<<<<<<</d' -e '/<<<<<<<</,$$d' $(srcdir)/user_malloc.h >> $@.t
	cat $(srcdir)/dmalloc.h.4 >> $@.t
	sed -e '1,/<<<<<<<</d' -e '/<<<<<<<</,$$d' -e '/^#if/d' -e '/^#endif/d' $(srcdir)/arg_check.h >> $@.t
	cat $(srcdir)/dmalloc.h.5 >> $@.t
	mv $@.t $@

clean-local:
	@case "$(CC)" in \
	  *cl*) \
	    echo "MSVC detected, cleaning PDB/ILK files..."; \
	    for p in $(bin_PROGRAMS) $(check_PROGRAMS); do \
	      base=$${p%$(EXEEXT)}; \
	      rm -f "$$base.pdb" "$$base.ilk" "$$base.exp"; \
	    done; \
	    rm -f vc*.pdb ;; \
	esac
CLEANFILES = dmalloc.h
DISTCLEANFILES = dmalloc.h.2 settings.h version.h
