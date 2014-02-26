#$Id: Makefile,v 1.41 2001/05/23 08:43:46 ilsoe Exp $
#
# Makefile for the virtual machine component
#
# Check top level makefile for a description of options
#

include ../../conf.mk

#INCLUDES := -I./ -I../common -I../gc_common
LFLAGS   := $(LFLAGS) -L./ -L../common -lgbvm -lgc -lcommon

INCLUDES := $(INCLUDES) -I$(GC_PATH)
LFLAGS   := $(LFLAGS) -L$(GC_PATH)

CXXFLAGS := $(INCLUDES)

EXEC_OBJS = vm_main.o
#DEBUG_EXEC_OBJS = testRuntimepath.o

OBJECTS  = Thread.o ByteCodeLoader.o Scheduler.o Mixin.o PartObject.o PatternReference.o ObjectReference.o Pattern.o Object.o Stacks.o GBetaEntities.o vm_globals.o Instructions.o Instr_Add.o Instr_Mulop.o Instr_Install.o Instr_Peek.o Instr_Pop.o Instr_Push.o Instr_Relop.o Instr_Sem.o Instr_Special.o Instr_Multiline.o Instr_Misc.o RunTimeSteps.o #AttrStack.o


TARGET = gbvm
LIB_TARGET = libgbvm.a
DEBUG_TARGET = test

all: $(LIB_TARGET) $(TARGET) #$(DEBUG_TARGET)

$(LIB_TARGET): $(OBJECTS)
	rm -f $(LIB_TARGET);
	ar cru $@ $(OBJECTS);
	ranlib $@

$(TARGET): $(LIB_TARGET) $(EXEC_OBJS) $(GC_PATH)/libgc.a
	$(CXX)  $(CPPFLAGS) $(EXEC_OBJS) -o $(TARGET) $(LFLAGS) $(INCLUDES)
#	purify $(CXX)  $(CPPFLAGS) $(EXEC_OBJS) -o $(TARGET) $(LFLAGS) $(INCLUDES)


$(DEBUG_TARGET):$(LIB_TARGET) $(GC_LIB) $(DEBUG_EXEC_OBJS)
	$(CXX)  $(CPPFLAGS) $(DEBUG_EXEC_OBJS) -o $(DEBUG_TARGET) $(LFLAGS) $(INCLUDES)

$(OBJECTS) $(EXEC_OBJS) $(DEBUG_EXEC_OBJS): %.o: %.cc GBetaEntities.hpp ../common/Lists.hpp ../common/globals.hpp
	$(CXX) -c $(CPPFLAGS) $(INCLUDES) $< -o $@


.PHONY: clean
clean:
	rm -f *.o; rm -f *~; rm -f $(TARGET) $(LIB_TARGET) $(DEBUG_TARGET) gmon.out

#include auto generated dependencies

include $(DEPFILE)
