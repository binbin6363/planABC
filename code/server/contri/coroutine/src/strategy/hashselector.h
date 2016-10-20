///////////////////////////////////////////////////////////
//  ModHashSelector.h
//  Implementation of the Class ModHashSelector
//  Created on:      26-五月-2015 9:39:55
//  Original author: imo
///////////////////////////////////////////////////////////

#if !defined(HASH_SELECTOR_H_)
#define HASH_SELECTOR_H_

#include "selector.h"

template<class TYPE>
class ModHashSelector : public Selector<TYPE>
{

public:
	ModHashSelector();
	virtual ~ModHashSelector();

	virtual TYPE Select();
    virtual int AddElement(uint32_t element_id, const TYPE &ele);
    virtual void RemoveElement(uint32_t element_id);
    virtual void SetElements(const vector<TYPE> &eles);
    virtual void SetElementId(uint32_t element_id);
    virtual void SetHashId(uint32_t hash_id);

    virtual void ShowInfo();

private:
    uint32_t            hash_id_;
    map<uint32_t, TYPE> elements_;
};


// implements
template<class TYPE>
ModHashSelector<TYPE>::ModHashSelector()
    : hash_id_(0)
{
    elements_.clear();
}



template<class TYPE>
ModHashSelector<TYPE>::~ModHashSelector()
{
    hash_id_ = 0;
    elements_.clear();
}



template<class TYPE>
TYPE
ModHashSelector<TYPE>::Select()
{
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;
    uint32_t element_size = (uint32_t)elements_.size();

    if (0 == element_size)
    {
        LOG(ERROR)("elements is empty.\n");
        return NULL;
    }

    uint32_t index = hash_id_ % element_size;

//    LOG(DEBUG)("element size:%d, index:%u\n", element_size, index);

    // 遍历寻找位于索引位置的元素
    TYPE element = NULL;
    TypeIter ele_iter = elements_.begin();
    TypeIter ele_end_iter = elements_.end();
    for (uint32_t i = 0; (ele_iter != ele_end_iter) && (i != index); ++ele_iter, ++i)
    {
        // do nothig, just for hitting i==index
    }
    
    if (ele_iter != ele_end_iter) 
    {
        element = ele_iter->second;
    } else {
        LOG(ERROR)("find element error.");
    }

    return element;
}


template<class TYPE>
int 
ModHashSelector<TYPE>::AddElement(uint32_t element_id, const TYPE &ele)
{
    LOG(DEBUG)("add element id:%u", element_id);
    if (elements_.find(element_id) == elements_.end())
    {
        elements_[element_id] = ele;
        return 0;
    }
    else
    {
        LOG(ERROR)("add element, but exist already. element id:%u, failed", element_id);
        return -1;
    }
}

template<class TYPE>
void 
ModHashSelector<TYPE>::RemoveElement(uint32_t element_id)
{
    LOG(DEBUG)("remove element id:%u", element_id);
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;

    TypeIter iter = elements_.find(element_id);
    if (elements_.end() != iter)
    {
        elements_.erase(iter);
    }
}



template<class TYPE>
void 
ModHashSelector<TYPE>::SetElements(const vector<TYPE> &eles)
{
    typedef typename vector<TYPE>::const_iterator TypeConstIter;

    elements_.clear();
    TypeConstIter iter = eles.begin();
    TypeConstIter end_iter = eles.end();
    for (; iter != end_iter; ++iter)
    {
        uint32_t id = (*iter)->GetId();
        elements_[id] = (*iter);
    }
}


template<class TYPE>
void 
ModHashSelector<TYPE>::SetHashId(uint32_t hash_id)
{
    hash_id_ = hash_id;
}


template<class TYPE>
void 
ModHashSelector<TYPE>::SetElementId(uint32_t element_id)
{
    hash_id_ = element_id;
}


template<class TYPE>
void 
ModHashSelector<TYPE>::ShowInfo()
{
/*
    LOG(DEBUG)("mod hash selector. elements size:%zu, cur hashid:%u."
        , elements_.size(), hash_id_);
    
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;
    TypeIter iter = elements_.begin();
    TypeIter end_iter = elements_.end();
    for (; iter != end_iter; ++iter)
    {
    // TODO: CRASH
        LOG(DEBUG)("element, id:%u, ele:%p, ele id:%u"
        , iter->first, iter->second, iter->second->GetId());
    }
*/
}





#endif // !defined(HASH_SELECTOR_H_)


