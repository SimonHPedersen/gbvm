#$Id: Makefile,v 1.20 2001/05/23 09:41:34 ilsoe Exp $
#
# Makefile for the new gc component
#
# Check top level makefile for a description of options
#

include ../../conf.mk

#INCLUDES := -I./ -I../common -I../vm -I../gc_common
LDFLAGS  := -L../common -L./
LDLIBS   := -lgc -lcommon

COMMON_OBJECTS = ObjectDescriptor.o

OBJECTS  = IntroductorySpace.o StackSpace.o MemoryBlock.o Car.o TrainGeneration.o \
	VMObject.o TrainTable.o gc_globals.o garbage_collector.o RememberedSet.o \
	DirtyCars.o VMOHash.o RemSetHash.o

LIB_OBJECTS = $(COMMON_OBJECTS) $(OBJECTS)

DEBUG_EXTRA_OBJS =# ../common/setVMReference.o 
DEBUG_MAIN_OBJECTS = main_debug1.o testTrainTable.o main_debug2.o main_debug3.o #main_debug4.o testRememberedSet.o
DEBUG_MAINS = main_debug1 testTrainTable main_debug2 main_debug3 #main_debug4 testRememberedSet

DEBUG_OBJS = $(DEBUG_EXTRA_OBJS) $(DEBUG_MAIN_OBJECTS)

LIB_TARGET = libgc.a

all: $(LIB_TARGET) #$(DEBUG_MAINS)

$(LIB_TARGET): $(LIB_OBJECTS)
	rm -f $(LIB_TARGET);
	ar cru $@ $(LIB_OBJECTS);
	ranlib $@

$(COMMON_OBJECTS): %.o: ../gc_common/%.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) ../gc_common/$*.cc -o $@

#$(DEBUG_MAINS): %.o: %.cc
#	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(INCLUDES)

main_debug1: main_debug1.o Makefile $(LIB_TARGET)
	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(LDLIBS) $(INCLUDES)

main_debug2: main_debug2.o Makefile $(LIB_TARGET)
	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(LDLIBS) $(INCLUDES)

main_debug3: main_debug3.o Makefile $(LIB_TARGET)
	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(LDLIBS) $(INCLUDES)

main_debug4: main_debug4.o Makefile $(LIB_TARGET)
	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(LDLIBS) $(INCLUDES)

testTrainTable: testTrainTable.o Makefile $(LIB_TARGET)
	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(LDLIBS) $(INCLUDES)

testRememberedSet: testRememberedSet.o Makefile $(LIB_TARGET)
	$(CXX) $(CPPFLAGS) $(DEBUG_EXTRA_OBJS) $< -o $@ $(LDFLAGS) $(LDLIBS) $(INCLUDES)

$(OBJECTS) $(DEBUG_OBJS): %.o: %.cc ../common/globals.hpp Makefile
	$(CXX) -c $(CPPFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	rm -f *.o; rm -f *~; rm -f $(DEBUG_MAINS) $(LIB_TARGET) gmon.out

#include auto generated dependencies

include $(DEPFILE)
