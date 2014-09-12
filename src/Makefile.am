include $(top_srcdir)/common.mk

#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libOMTFSimulation.la

#######################################
# Build information for each library
# Sources for libRootAnalysis
libOMTFSimulation_la_SOURCES = GoldenPattern.cc OMTFConfiguration.cc OMTFinput.cc OMTFProcessor.cc \
	                       XMLConfigReader.cc XMLConfigWriter.cc \
	                       OMTFResult.cc OMTFSorter.cc L1Obj.cc

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libOMTFSimulation_la_CPPFLAGS = -I$(top_srcdir) -I@ROOTINCDIR@ -std=c++11
