# $Id: conf.mk,v 1.16 2001/05/30 18:56:20 ilsoe Exp $
#
# All sub makefiles should include this configuration makefile
#
# Available arguments
#
# SETVMREF=m       : setVMReference (write barrier) is inlined
# GC=n             : no garbage collection in virtual machine
#                    best used when also using SETVMREF=m
# OPTIMIZE=n       : set optimization to level n
# DEBUG=n          : disable debugging
# PROFILE=y        : generate executables capable of producing profiling
#                    info for gprof
# NEWGC=n          : use old garbage collector
# NEWGC=a          : use pure allocator, no garbage collection
# INTEGRITYCHECK=y : check the integrity of the heap continuously
# LIVE=y|v         : make live stats before and after each gc
#                    y makes a diet version for the tests,
#                    v makes a verbose version
# STL=y            : use stl implementation of remembered sets
# CAR_EXP=n        : set car size to 2^n
# INTROSP_EXP=n    : set introspace size to 2^n
# COMPACT_VMO=n    : don't use compact vmo implementation, use old impl
# OBJ_HAS_ID=y     : objects has an id field - consumes more space so use only
#                    for debugging
# DISRUPT_STAT=y   : print disruption statistics for gc invocations
# NEW_STACK_RESCUE=n :during stack rescue the gc tries to fill the first car
#                  : in last train to the fraction n,
#                  : that is, n=1 is filled completely, n=0 is empty
# WB2=y            : writebarrier executed twice
# INTRO_FIRST=y    : introductory space is garbage collected before the first car
#                    is (old style)
# FTRMT2ND=y       : (first train referenced (in introsp) moved to 2nd train)
#                    objects in the introductory space referenced from the first train
#                    are moved to the second train, default is the last train
#
################################################################################


# we assume/support/use only linux and (sparc?) solaris

ifeq ($(OSTYPE),Linux)
  DEPFILE=.dep_linux
  DEPDIRS=-I/usr/include/g++-2
else
  DEPFILE=.dep_solaris
  DEPDIRS=-I/pack/gcc/sparc-sun-solaris2.8/include -I/pack/gcc/include/g++-3 -I/pack/gcc/include
endif

CPPFLAGS = -Wall -g

ifneq ($(OPTIMIZE),)
  CPPFLAGS  := $(CPPFLAGS) -O$(OPTIMIZE)
endif

ifneq ($(DEBUG), n)
  CPPFLAGS  := $(CPPFLAGS) -DDEBUG
endif

ifeq ($(PROFILE), y)
  CPPFLAGS  := $(CPPFLAGS) -pg
  ifneq ($(OSTYPE),Linux)
    CPPFLAGS := $(CPPFLAGS) -static
  endif
endif

ifeq ($(SETVMREF), m)
  CPPFLAGS  := $(CPPFLAGS) -DSETVMREFERENCE_IS_MACRO
endif

ifeq ($(LIVE), y)
  CPPFLAGS  := $(CPPFLAGS) -DLIVESTAT
endif

ifeq ($(LIVE), v)
  CPPFLAGS  := $(CPPFLAGS) -DLIVESTAT -DLIVESTAT_VERB
endif

ifeq ($(INTEGRITYCHECK), y)
  CPPFLAGS  := $(CPPFLAGS) -DINTEGRITY_CHECK
endif

ifeq ($(GC), n)
  CPPFLAGS  := $(CPPFLAGS) -DNO_GC
endif

ifeq ($(STL), y)
  CPPFLAGS  := $(CPPFLAGS) -DSTL_REMSET
endif

ifneq ($(CAR_EXP),)
  CPPFLAGS  := $(CPPFLAGS) -DCAR_EXP=$(CAR_EXP)
endif

ifneq ($(INTROSP_EXP),)
  CPPFLAGS  := $(CPPFLAGS) -DINTROSP_EXP=$(INTROSP_EXP)
endif

ifneq ($(COMPACT_VMO),n)
  CPPFLAGS  := $(CPPFLAGS) -DCOMPACT_VMO
endif

ifeq ($(OBJ_HAS_ID),y)
  CPPFLAGS  := $(CPPFLAGS) -DOBJ_HAS_ID
endif

ifeq ($(DISRUPT_STAT),y)
  CPPFLAGS  := $(CPPFLAGS) -DDISRUPT_STAT
endif

ifneq ($(NEW_STACK_RESCUE),)
  CPPFLAGS  := $(CPPFLAGS) -DNEW_STACK_RESCUE=$(NEW_STACK_RESCUE)
endif

ifeq ($(WB2),y)
  CPPFLAGS  := $(CPPFLAGS) -DWB2
endif

ifeq ($(INTRO_FIRST),y)
  CPPFLAGS  := $(CPPFLAGS) -DINTRO_FIRST
endif

ifeq ($(FTRM2ND),y)
  CPPFLAGS  := $(CPPFLAGS) -DFIRST_TRAIN_REFED_TO_2ND_TRAIN
endif


INCLUDES := -I./ -I../common -I../vm -I../gc_common

ifneq ($(NEWGC),n)
 ifeq ($(NEWGC),a)
   GC_PATH = ../allocator
   INCLUDES := $(INCLUDES) -I../allocator
 else
   GC_PATH = ../gc_new
   CPPFLAGS := $(CPPFLAGS) -DNEW_STYLE_GC
   INCLUDES := $(INCLUDES) -I../gc_new
 endif
else
  GC_PATH = ../gc
  INCLUDES := $(INCLUDES) -I../gc
endif

