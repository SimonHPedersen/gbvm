//$Id: GBetaEntities.hpp,v 1.32 2001/05/21 11:42:47 hempe Exp $

#ifndef GBETAENTITIES_HPP
#define GBETAENTITIES_HPP

class PartObject_t;
class Pattern_t;
class EmptyPattern_t;
class Object_t;
class PredefinedStringPartObject_t;
class Mixin_t;
class AttributeInstance_t;
class PatternReference_t;
class ObjectReference_t;
class UserPartObject_t;
template <class T_t> class PredefinedPartObject_t;
class PredefinedStringMixin_t;
template <class T_t> class PredefinedMixin_t;

#include "ByteCodeLoader.hpp"
#include <typeinfo>
#include "Lists.hpp"
#include "vm_globals.hpp"
#include "gc_globals.hpp"
#include "VMObject.hpp"
#include "Thread.hpp"
#include "Instructions.hpp"

#define PREDEF_NOF_ATTR 6

//returns the sourounding part object
PartObject_t* getPredef(Scheduler_t* myScheduler);

class AttributeInstance_t {
};

class Mixin_t {
public:
  void                       *operator new(size_t size);
  void                       *operator new(size_t size, void *location);
  			      Mixin_t(void);
                              Mixin_t(MainPart* mainPart, PartObject_t* origin);
  unsigned int                getOriginOffset(void);
  PartObject_t               *getOrigin(void);
  virtual MainPart           *getMainPart(void);
  virtual PartObject_t       *instantiate(Object_t *object);
  Lst<Instruction>           *getInstructionListPtr();
  virtual Lst<Instruction>   *getMPAttrsInstrPtr(void);
                                //get the list of instrs which initializes
                                //all attrs
  Lst<Instruction>           *getAttrInstrPtr(unsigned int index);
public:
  PartObject_t               *origin;
private:                        
  MainPart                   &mainPart;
                                //MUST be private to ensure that getMainPart()  
                                //is used because this method is overridden 
                                //in PredefinedMixin_t
};

class PartObject_t : public DLstElm<PartObject_t>{
public:
  void                       *operator new(size_t size);
  void                       *operator new(size_t size, void *location);

  unsigned int                getMixinOffset(void);
  unsigned int                getObjectOffset(void);
  unsigned int                get_nextOffset(void);
  unsigned int                get_prevOffset(void);
  Object_t                   *getObject(void);
  PartObject_t               *getOrigin(void);
  char                       *getMainPartId(void);
  virtual void                initAttributes(Thread_t *myThread)=0;
  virtual Instruction        *getFirstInstructionPushPOANF(Thread_t *myThread)=0;
  Lst<Instruction>           *getMPAttrsInstrPtr(void);
                                //get the list of instrs which initializes
                                //all attrs
  virtual AttributeVirtual   *getVirtAttr(char *parentAttrName,
					  int parentAttrIndex,
					  char *parentMainPartID) = 0;
  Mixin_t                    *mixin;
  Object_t                   *object;
};

class PredefinedStringPartObject_t : public PartObject_t{
public:
                              PredefinedStringPartObject_t();
                              PredefinedStringPartObject_t
			        (Mixin_t *mixin, Object_t *object);
                                //no attributes to initialize - empty

  virtual void                initAttributes(Thread_t *myThread){ }
  virtual Instruction        *getFirstInstructionPushPOANF(Thread_t *myThread);
  unsigned int                getStrOffset();
  char                       *getStr();
  void                        setStr(char* _str);
  virtual AttributeVirtual   *getVirtAttr(char *parentAttrName,
					  int parentAttrIndex,
					  char *parentMainPartID)
                                { return 0; }
                                //strings do not have (virtual) attributes...
private:
  char*                         str;
};

template <class T_t>
class PredefinedMixin_t : public Mixin_t{
public:
  void                       *operator new(size_t size, void *location){
                                return location;
                              }

  PredefinedMixin_t( PartObject_t* origin) 
    {
      DB_INDENT;
      if(origin){
	setVMReference(this, (void**)&(this->origin), origin);
      }
      DB_OUTDENT;
    }

  virtual PartObject_t *instantiate(Object_t *object){
    //find object descriptor
    ObjectDescriptor_t* objDes;
    if(typeid(this) == typeid(PredefinedMixin_t<bool>*)){
      objDes = predefCharPODesc;
    } else if(typeid(this) == typeid(PredefinedMixin_t<char>*)){
      objDes = predefCharPODesc;
    } else if(typeid(this) == typeid(PredefinedMixin_t<int>*)){
      objDes = predefIntPODesc;
    } else if(typeid(this) == typeid(PredefinedMixin_t<real>*)){
      objDes = predefRealPODesc;
    } else {
      cerr << "could not find objectdescriptor for unknown type" << endl;
      exit(1);
    }
    PredefinedPartObject_t<T_t> *prePO =
      new (allocateVM(objDes)) PredefinedPartObject_t<T_t>(this, object);
    return prePO;
  }

  virtual MainPart* getMainPart(void){
    if(typeid(this) == typeid(PredefinedMixin_t<bool>*)){
      return sharedBooleanPredefMP;
    }
    if(typeid(this) == typeid(PredefinedMixin_t<char>*)){
      return sharedCharPredefMP;
    }
    if(typeid(this) == typeid(PredefinedMixin_t<int>*)){
      return sharedIntPredefMP;
    }
    if(typeid(this) == typeid(PredefinedMixin_t<real>*)){
      return sharedRealPredefMP;
    }
    cerr << "PredefinedMixin_t::getMainPart(): could not identify template type"
	 << endl;
    exit(1);
    return NULL;
  }

  virtual Lst<Instruction> *getMPAttrsInstrPtr(void){
    return getMainPart()->getMPAttrsInstrPtr();
  }
};

class PredefinedStringMixin_t : public Mixin_t{
public:
                              void *operator new(size_t size, void *location);
                              PredefinedStringMixin_t(PartObject_t* origin);
                              PartObject_t *instantiate(Object_t *object);
                              virtual MainPart* getMainPart(void);
                              virtual Lst<Instruction> *getMPAttrsInstrPtr(void);
};

class UserPartObject_t : public PartObject_t{
public:
  void                       *operator new(size_t size, void *location);
                              UserPartObject_t(ObjectDescriptor_t *objDes);
                                //used during bytecode loading only
                                //- initializes objDes, and
                                //does not create object!
                              UserPartObject_t(Mixin_t *mixin, Object_t *object);
                                //used for creating object
  virtual void                initAttributes(Thread_t *myThread);
  void                        setAttribute(unsigned int index,
					   AttributeInstance_t* value);  
  AttributeInstance_t        *getAttribute(unsigned int index,
					   Thread_t *myThread);
  AttributeInstance_t       **getAttributeAddress(unsigned int index);
  virtual Instruction        *getFirstInstructionPushPOANF(Thread_t *myThread);
  virtual AttributeVirtual   *getVirtAttr(char *parentAttrName,
					  int parentAttrIndex,
					  char *parentMainPartID);
private:
  void                        initAttribute(unsigned int index, 
					    Thread_t *myThread);
  AttributeInstance_t        *attributes[1];
};

template <class T_t>
class PredefinedPartObject_t : public PartObject_t{
public:
  PredefinedPartObject_t(Mixin_t *mixin,
			 Object_t *object){
    
    //doing: this->mixin = mixin
    if(mixin){
      setVMReference(this, (void**)&(this->mixin), mixin);
    }
    //doing: this->object = object;
    if(object){
      setVMReference(this, (void**)&(this->object), object);
    }
  }

  //no attributes to initialize - empty
  virtual void initAttributes(Thread_t *myThread){ }

  virtual Instruction *getFirstInstructionPushPOANF(Thread_t *myThread){
    if(next()){
      return next()->getFirstInstructionPushPOANF(myThread);
    } else {
      return NULL;
    }
  }

  virtual AttributeVirtual   *getVirtAttr(char *parentAttrName,
					  int parentAttrIndex,
					  char *parentMainPartID) {
    return 0;
  }
                                //predef-objs do not have (virtual) attributes.
  T_t                         value;
};

class PatternReference_t : public AttributeInstance_t {
public:
  void                       *operator new(size_t size, void *location);
  void                       *operator new(size_t size);
                              PatternReference_t(){} 
                              PatternReference_t(Pattern_t* qualification, 
						 Pattern_t* value);
  unsigned int                getQualificationOffset(void);
  unsigned int                getValueOffset(void);
  Pattern_t                  *getQualification();
  Pattern_t                  *getValue();
  void                        setValue(Pattern_t* ptn);
private:
  Pattern_t                  *qualification;
  Pattern_t                  *value;
};

class ObjectReference_t : public AttributeInstance_t {
public:
  void                       *operator new(size_t size, void *location);
  void                       *operator new(size_t size);
                              ObjectReference_t(){}
                              ObjectReference_t(Pattern_t* qualification, 
						Object_t* value);
  unsigned int                getQualificationOffset(void);
  unsigned int                getValueOffset(void);
  Pattern_t                  *getQualification();
  Object_t                   *getValue();
  void                        setValue(Object_t* obj);
private:
  Pattern_t                  *qualification;
  Object_t                   *value;
};

class Pattern_t : public AttributeInstance_t {
public:
  void                       *operator new(size_t size, void *location);
                              Pattern_t(ObjectDescriptor_t *objDesc);
                              Pattern_t(void) {}
  void                        addMixin(unsigned int index, Mixin_t* mixinPtr);
  Object_t                   *instantiate(Thread_t* myThread);
  unsigned int                getLength(void) { return length; }
  Mixin_t                   **getMixins(void) { return mixins; }
private:
  unsigned int                length;
                                //number of mixins in Pattern
  Mixin_t                    *mixins[1];
};

class EmptyPattern_t : public Pattern_t {
 public:
  void                       *operator new(size_t size, void *location);
  EmptyPattern_t(void){}
  void                        addMixin(unsigned int index, Mixin_t* mixinPtr);
  Object_t                   *instantiate(Thread_t* myThread);
  unsigned int                getLength(void) { return 0; }
  Mixin_t                   **getMixins(void) { return 0; }
};

class Object_t : public AttributeInstance_t, public DLst<PartObject_t> {
public:
                               //only needed once - during initialization
  void                       *operator new(size_t size);
  void                       *operator new(size_t size, void *location);
                              Object_t();
                              Object_t(Pattern_t *ptn);
  unsigned int                getHeadOffset(void);
  unsigned int                getTailOffset(void);
  unsigned int                getPatternOffset(void);
  void                        initPartObjects(Thread_t *myThread);
                                //initializes attributes of all partobjects
  Instruction*                getFirstInstructionPushPOANF(Thread_t *myThread);
  Pattern_t                  *getPattern(void) {return pattern;}
private:
  Pattern_t                  *pattern;
};

#endif
