///////////////////////////////////////////////////////////
//  Selector.h
//  Implementation of the Class Selector
//  Created on:      26-五月-2015 9:39:46
//  Original author: imo
///////////////////////////////////////////////////////////

#if !defined(SELECTOR_H_)
#define SELECTOR_H_
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
#include <cxxabi.h>
#include "log.h"


using namespace std;

class Session;
class SessionGroup;


template<class TYPE>
class Selector
{
public:
	Selector();
	virtual ~Selector();

	virtual TYPE Select();
    virtual int AddElement(uint32_t element_id, const TYPE &ele);
    virtual void RemoveElement(uint32_t element_id);
    virtual void SetElements(const vector<TYPE> &eles);
    virtual void SetElementId(uint32_t element_id);

    virtual void ShowInfo();
    virtual void SetHashId(uint32_t hash_id);

	virtual int Reset();

    // 返回全名，包括命名空间
    string GetTypeName() const;

};


// implements
template<class TYPE>
Selector<TYPE>::Selector()
{

}



template<class TYPE>
Selector<TYPE>::~Selector()
{

}

template<class TYPE>
int
Selector<TYPE>::Reset()
{
    // do nothing
    fprintf(stderr, "base class!\n");
    return 0;
}


template<class TYPE>
TYPE
Selector<TYPE>::Select()
{
    // do nothing
    fprintf(stderr, "base class!\n");
	return NULL;
}


template<class TYPE>
void 
Selector<TYPE>::SetHashId(uint32_t )
{
    // do nothing
    fprintf(stderr, "base class!\n");
}

template<class TYPE>
void 
Selector<TYPE>::SetElementId(uint32_t )
{
    // do nothing
    fprintf(stderr, "base class!\n");
}


template<class TYPE>
int 
Selector<TYPE>::AddElement(uint32_t , const TYPE &)
{
    // do nothing
    fprintf(stderr, "base class!\n");
    return -1;
}

template<class TYPE>
void 
Selector<TYPE>::RemoveElement(uint32_t )
{
    // do nothing
    fprintf(stderr, "base class!\n");
}


template<class TYPE>
void 
Selector<TYPE>::SetElements(const vector<TYPE> &)
{
    // do nothing
    fprintf(stderr, "base class!\n");
}

template<class TYPE>
void 
Selector<TYPE>::ShowInfo()
{
    // do nothing
    fprintf(stderr, "base class!\n");
}


template<class TYPE>
std::string 
Selector<TYPE>::GetTypeName() const
{
	char buffer[128];
	size_t buflen = sizeof(buffer);
	int status;
	char* unmangled = abi::__cxa_demangle(typeid(*this).name(), buffer, &buflen, &status);
	if (status) return typeid(*this).name();
	return unmangled;
}



#endif // !defined(SELECTOR_H_)


