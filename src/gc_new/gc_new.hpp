//$Id: gc_new.hpp,v 1.11 2001/04/25 17:26:58 ilsoe Exp $

#ifndef _GC_NEW_HPP_
#define _GC_NEW_HPP_

#include <malloc.h>

#include "globals.hpp"
#include "gc_globals.hpp"
#include "ObjectDescriptor.hpp"
#include "CarTrain.hpp"
#include "RemSetHash.hpp"
#include "RememberedSet.hpp"     //stl/our hashmap
#include "VMObject.hpp"          //ObjectDescriptor
#include "VMOHash.hpp"           //VMObject
#include "Car.hpp"               //RememberedSet, VMObject, ObjectDescriptor, VMOHash
//#include "TrainTable.hpp"
#include "Lists.hpp"
#include "MemoryBlock.hpp"       //Car, Lists
#include "StackSpace.hpp"        //VMObject, ObjectDescriptor, VMOHash
#include "IntroductorySpace.hpp" //ObjectDescriptor, VMOHash, Rememberedset
#include "TrainTable.hpp"        //Car, VMOHash
#include "DirtyCars.hpp"         //stl, Car
#include "garbage_collector.hpp" //ObjDesc, VMObject
#include "Generation.hpp"        //ObjDesc, CarTrain, DirtyCars, MemoryBlocks

/*
#include "CarTrain.hpp"
#include "globals.hpp"
#include "RemSetHash.hpp"
#include "VMObject.hpp"
#include "VMOHash.hpp"
#include "gc_globals.hpp"
//#include "TrainTable.hpp"
#include "ObjectDescriptor.hpp"
#include "Lists.hpp"
#include "MemoryBlock.hpp"
#include "StackSpace.hpp"
#include "IntroductorySpace.hpp"
#include "RememberedSet.hpp"
#include "TrainTable.hpp"
#include "DirtyCars.hpp"
#include "Car.hpp"
#include "garbage_collector.hpp"
#include "Generation.hpp"
*/
#endif
