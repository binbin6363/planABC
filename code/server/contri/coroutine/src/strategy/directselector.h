///////////////////////////////////////////////////////////
//  DirectSelector.h
//  Implementation of the Class DirectSelector
//  Created on:      26-五月-2015 9:40:59
//  Original author: imo
///////////////////////////////////////////////////////////

#if !defined(DIRECT_SELECTOR_H_)
#define DIRECT_SELECTOR_H_

#include "selector.h"


template<class TYPE>
class DirectSelector : public Selector<TYPE>
{

public:
	DirectSelector();
	virtual ~DirectSelector();

	virtual TYPE Select();
    virtual int AddElement(uint32_t element_id, const TYPE &ele);
    virtual void RemoveElement(uint32_t element_id);
    virtual void SetElements(const vector<TYPE> &eles);

    virtual void ShowInfo();

    virtual void SetElementId(uint32_t element_id);
    virtual void SetHashId(uint32_t element_id);
    
private:
    uint32_t            element_id_;
    map<uint32_t, TYPE> elements_;

};


// implements
template<class TYPE>
DirectSelector<TYPE>::DirectSelector()
    : element_id_(0)
{
    elements_.clear();
}



template<class TYPE>
DirectSelector<TYPE>::~DirectSelector()
{
    element_id_ = 0;
    elements_.clear();
}


template<class TYPE>
TYPE
DirectSelector<TYPE>::Select()
{
    typedef typename map<uint32_t, TYPE>::const_iterator TypeConstIter;

    TypeConstIter iter = elements_.find(element_id_);
    if (iter == elements_.end())
    {
        LOG(ERROR)("not found the element matching with element id:%u", element_id_);
        // TODO: CAN USE AS FOLLOWS?
        return NULL;
    }

//    LOG(DEBUG)("find the element matching with element id:%u", element_id_);
    return iter->second;
}



template<class TYPE>
int
DirectSelector<TYPE>::AddElement(uint32_t element_id, const TYPE &ele)
{
    typedef typename map<uint32_t, TYPE>::const_iterator TypeConstIter;

    TypeConstIter iter = elements_.find(element_id);
    if (iter != elements_.end())
    {
        LOG(WARN)("add element repeated, id:%u, failed", element_id);
        return -1;
    }
    elements_[element_id] = ele;
    return 0;
}

template<class TYPE>
void
DirectSelector<TYPE>::RemoveElement(uint32_t element_id)
{
    LOG(DEBUG)("remove element id:%u", element_id);
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;
    
    TypeIter iter = elements_.find(element_id);
    if (iter != elements_.end())
    {
        elements_.erase(iter);
        LOG(DEBUG)("remove element id:%u ok.", element_id);
    }

}

template<class TYPE>
void
DirectSelector<TYPE>::SetElements(const vector<TYPE> &eles)
{
    typedef typename vector<TYPE>::const_iterator TypeConstIter;

    elements_.clear();
    TypeConstIter iter = eles.begin();
    TypeConstIter end_iter = eles.end();
    for (; iter != end_iter; ++iter)
    {
        // TYPE 元素需要实现GetId方法
        uint32_t element_id = (*iter)->GetId();
        elements_[element_id] = *iter;
    }
}


template<class TYPE>
void
DirectSelector<TYPE>::SetElementId(uint32_t element_id)
{
    element_id_ = element_id;
}

template<class TYPE>
void
DirectSelector<TYPE>::SetHashId(uint32_t element_id)
{
    element_id_ = element_id;
}


template<class TYPE>
void
DirectSelector<TYPE>::ShowInfo()
{
/*
    LOG(DEBUG)("direct selector. element size:%zu, element id:%u"
        , elements_.size(), element_id_);
*/
}


#endif // !defined(DIRECT_SELECTOR_H_)

