//$Id: VMObject.hpp,v 1.1 2001/03/16 08:26:17 ilsoe Exp $

#ifndef _VMOBJECT_HPP_
#define _VMOBJECT_HPP_

#include <iostream.h>

class VMObject_t;

ostream& operator << (ostream& stream, VMObject_t* vmo);
ostream& operator << (ostream& stream, VMObject_t& vmo);

#include "ObjectDescriptor.hpp"

class VMObject_t {
public:
  void * operator             new(size_t mySize, void *address,
				  ObjectDescriptor_t *_objDesc);
                              VMObject_t(void);
  unsigned int                getID(void) { return id; }
  void                       *getDataPtr(void) { return data; }
  void                        printOn(ostream &stream);
private:
  static unsigned int         nextID;

private: //variables
  ObjectDescriptor_t         *objDesc;
  unsigned int const          id;
  void                      *data[1];
                                //_MUST_ BE LAST IN OBJECT
                                //this array may grow out of the object!
                                //objectDescriptor knows how much
};

#endif
