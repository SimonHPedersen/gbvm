//$Id: vm_globals.hpp,v 1.18 2001/05/10 12:37:01 hempe Exp $
#ifndef VM_GLOBALS_HPP
#define VM_GLOBALS_HPP

typedef float real;
typedef char boolean;

//must be a muliplum of sizeof(void*) fits linelength in bytecodeloader
#define MAXSTRLENGTH (500*sizeof(void*)) //in chars
#define MAXPTNLENGTH 256                 //in mixins/refs

#include "ObjectDescriptor.hpp"

extern ObjectDescriptor_t *predefBoolPODesc;
extern ObjectDescriptor_t *predefCharPODesc;
extern ObjectDescriptor_t *predefIntPODesc;
extern ObjectDescriptor_t *predefRealPODesc;
extern ObjectDescriptor_t *predefStringPODesc;

extern ObjectDescriptor_t *mixinDesc;
extern ObjectDescriptor_t *objectDesc;
extern ObjectDescriptor_t *threadDesc;
extern ObjectDescriptor_t *objRefDesc;
extern ObjectDescriptor_t *ptnRefDesc;
extern ObjectDescriptor_t *schedDesc;
extern ObjectDescriptor_t *emptyPtnDesc;
extern ObjectDescriptor_t *oneCharStringDesc;

#include "ByteCodeLoader.hpp"

extern MainPart *sharedStringPredefMP;
extern MainPart *sharedBooleanPredefMP;
extern MainPart *sharedCharPredefMP;
extern MainPart *sharedIntPredefMP;
extern MainPart *sharedRealPredefMP;

#include "GBetaEntities.hpp"
#include "Thread.hpp"
#include "VMObject.hpp"
#include "Stacks.hpp"
#include "Scheduler.hpp"

//MUST be root object - to avoid garbage collection - NOT ANY MORE
//extern EmptyPattern_t *emptyPattern;

ObjectDescriptor_t* getStrObjectDescriptor(unsigned int length);
//length in chars

ObjectDescriptor_t* getPtnObjectDescriptor(unsigned int length);
//length in mixins/refs

//call this on startup!
void fixObjectDescriptors(void);

#endif
