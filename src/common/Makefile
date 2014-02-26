#$Id: Makefile,v 1.11 2001/04/25 17:26:55 ilsoe Exp $
#
# Makefile for the common component
#
# Check top level makefile for a description of options
#

include ../../conf.mk

OBJECTS  = globals.o

DEBUG_OBJS =

LIB_TARGET = libcommon.a

all: $(LIB_TARGET)

$(LIB_TARGET): $(OBJECTS)
	ar cru $@ $(OBJECTS);
	ranlib $@

$(OBJECTS) $(DEBUG_OBJS): %.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	rm -f *.o; rm -f *~; rm -f libcommon.a gmon.out

#include auto generated dependencies

include $(DEPFILE)
