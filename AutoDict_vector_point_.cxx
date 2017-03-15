#include "struct.hh"
#ifdef __CINT__ 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class vector<point>+;
#pragma link C++ class vector<point>::*;
#ifdef G__VECTOR_HAS_CLASS_ITERATOR
#pragma link C++ operators vector<point>::iterator;
#pragma link C++ operators vector<point>::const_iterator;
#pragma link C++ operators vector<point>::reverse_iterator;
#endif
#endif
