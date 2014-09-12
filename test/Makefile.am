include $(top_srcdir)/common.mk
#######################################
# Because a.out is only a sample program we don't want it to be installed.
# The 'noinst_' prefix indicates that the following targets are not to be
# installed.
noinst_PROGRAMS=test

#######################################
# Build information for each executable. The variable name is derived
# by use the name of the executable with each non alpha-numeric character is
# replaced by '_'. So a.out becomes a_out and the appropriate suffex added.
# '_SOURCES' for example.

# Sources for the a.out 
test_SOURCES=     test.cc

# Linker options for a.out
test_LDFLAGS = $(top_srcdir)/src/libOMTFSimulation.la -R @ROOTLIBDIR@ -L@ROOTLIBDIR@ -L/usr/lib64 -lxerces-c
test_LDADD   = @ROOTLIBS@ -lProof @ROOTAUXLIBS@ -lboost_regex -lgomp

# Compiler options for a.out
#test_CPPFLAGS = -I./ -I$(top_srcdir) -I@ROOTINCDIR@ -std=c++11 -fopenmp




