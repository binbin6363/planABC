///////////////////////////////////////////////////////////
//  ConsHashSelector.h
//  Implementation of the Class ConsHashSelector
//  Created on:      26-五月-2015 9:41:06
//  Original author: imo
///////////////////////////////////////////////////////////

#if !defined(CONS_HASH_SELECTOR_H_)
#define CONS_HASH_SELECTOR_H_

#include "selector.h"
#include "crc32.h"


const static uint8_t NEED_CALCULATE = 1;
const static uint8_t NO_CALCULATE = 0;


template<class TYPE>
class ConsHashSelector : public Selector<TYPE>
{

public:
	ConsHashSelector();
	virtual ~ConsHashSelector();

	virtual TYPE Select();
    virtual int AddElement(uint32_t element_id, const TYPE &ele);
    virtual void RemoveElement(uint32_t element_id);
    virtual void SetElements(const vector<TYPE> &eles);
    virtual void SetElementId(uint32_t element_id);
    virtual void SetHashId(uint32_t hash_id);
    virtual void ShowInfo();

    int LazyMakeConsistentList();
    virtual int Reset(); // 重置一致性列表，后续重新计算

private:
    map<uint32_t, TYPE>  elements_;
    uint32_t             element_id_;
    uint8_t              lazy_calculate_;

};



// implements
template<class TYPE>
ConsHashSelector<TYPE>::ConsHashSelector()
    : element_id_(0)
    , lazy_calculate_(NEED_CALCULATE)
{
    elements_.clear();
}



template<class TYPE>
ConsHashSelector<TYPE>::~ConsHashSelector()
{
    element_id_ = 0;
    lazy_calculate_ = NO_CALCULATE; 
    elements_.clear();
}


template<class TYPE>
TYPE
ConsHashSelector<TYPE>::Select()
{
    typedef typename map<uint32_t, TYPE>::const_iterator TypeConstIter;
    LOG(DEBUG)("ENTER SELECT.\n");

    if (NEED_CALCULATE == lazy_calculate_)
    {
        if (0 != LazyMakeConsistentList())
        {
            return NULL;
        }
        lazy_calculate_ = NO_CALCULATE;
    }

//    LOG(DEBUG)("elements size:%zu\n", elements_.size());
    TypeConstIter iter = elements_.upper_bound(element_id_);
    if (iter != elements_.end())
    {
//        LOG(DEBUG)("element id:%u found id:%u.\n", element_id_, iter->first);
        return iter->second;
    }
    else
    {
        LOG(WARN)("not found.\n");
        return NULL;
    }

}


template<class TYPE>
int
ConsHashSelector<TYPE>::AddElement(uint32_t element_id, const TYPE &ele)
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
ConsHashSelector<TYPE>::RemoveElement(uint32_t element_id)
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
ConsHashSelector<TYPE>::SetHashId(uint32_t hash_id)
{
    element_id_ = hash_id;
}


template<class TYPE>
void
ConsHashSelector<TYPE>::SetElementId(uint32_t element_id)
{
    element_id_ = element_id;
}


template<class TYPE>
void
ConsHashSelector<TYPE>::SetElements(const vector<TYPE> &eles)
{
    typedef typename vector<TYPE>::const_iterator TypeConstIter;

    LOG(DEBUG)("cons hash selector set elements");
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
ConsHashSelector<TYPE>::ShowInfo()
{
/*
    LOG(DEBUG)("consistent hash selector. element size:%zu, element id:%u"
        , elements_.size(), element_id_);
*/
}

// lazy make consistent element list
template<class TYPE>
int
ConsHashSelector<TYPE>::LazyMakeConsistentList()
{
    typedef typename map<uint32_t, TYPE>::const_iterator TypeConstIter;

    if (elements_.empty())
    {
        LOG(WARN)("element collection is empty. no calculate.");
        return -1;
    }

    // make really element map
    map<uint32_t, TYPE> replica;
    replica.swap(elements_);
    elements_.clear();
    TypeConstIter iter = replica.begin();
    TypeConstIter end_iter = replica.end();
    for ( ; iter != end_iter; ++iter)
    {
        TYPE ele = iter->second;
        uint32_t id = ele->NodeId();
        LOG(DEBUG)("add node id:%u\n", id);
        if (elements_.find(id) != elements_.end())
        {
            LOG(ERROR)("element is repeated. not add.\n");
            continue;
        }
        elements_[id] = ele;
    }
//    LOG(DEBUG)("done lazy calculate. elements size:%zu.\n", elements_.size());
    return 0;
}



template<class TYPE>
int
ConsHashSelector<TYPE>::Reset()
{
    lazy_calculate_ = NEED_CALCULATE;
    return 0;
}


#endif // !defined(CONS_HASH_SELECTOR_H_)


