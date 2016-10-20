///////////////////////////////////////////////////////////
//  ScoreSelector.h
//  Implementation of the Class ScoreSelector
//  Created on:      7-七月-2015 17:39:55
//  Original author: imo
///////////////////////////////////////////////////////////

#if !defined(SCORE_SELECTOR_H_)
#define SCORE_SELECTOR_H_

#include "selector.h"

template<class TYPE>
class ScoreSelector : public Selector<TYPE>
{

public:
	ScoreSelector();
	virtual ~ScoreSelector();

	virtual TYPE Select();
    virtual int AddElement(uint32_t element_id, const TYPE &ele);
    virtual void RemoveElement(uint32_t element_id);
    virtual void SetElements(const vector<TYPE> &eles);
    virtual void SetElementId(uint32_t element_id);

    virtual void ShowInfo();

private:
    map<uint32_t, TYPE> elements_; // server_id -> session*
};


// implements
template<class TYPE>
ScoreSelector<TYPE>::ScoreSelector()
{
    elements_.clear();
}



template<class TYPE>
ScoreSelector<TYPE>::~ScoreSelector()
{
    elements_.clear();
}



/*****************************************************************************
 函 数 名  : ScoreSelector.Select
 功能描述  : 外部调用，选择元素，依据得分，无需输入,仅仅用于选择session，不用于选择group
 输入参数  : 无
 输出参数  : 无
 返 回 值  : TYPE
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : binbinwang
    修改内容   : 新生成函数

*****************************************************************************/
template<class TYPE>
TYPE
ScoreSelector<TYPE>::Select()
{
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;
    uint32_t element_size = (uint32_t)elements_.size();

    if (0 == element_size)
    {
        LOG(ERROR)("elements is empty.\n");
        return NULL;
    }

    TYPE element = NULL;
    TypeIter ele_iter = elements_.begin();
    TypeIter ele_end_iter = elements_.end();
    TypeIter hit_iter = ele_end_iter;
    uint32_t max_score = (ele_iter->second)->GetScore();
    uint32_t score = (ele_iter->second)->GetScore();
    for (; ele_iter != ele_end_iter; ++ele_iter)
    {
        if (!(ele_iter->second)->IsConnected())
        {
            LOG(ERROR)("score selector, session is not connected. skip this session.");
            continue;
        }
        score = (ele_iter->second)->GetScore();
        if (score >= max_score)
        {
            max_score = score;
            hit_iter = ele_iter;
        }
    }
    
    if (hit_iter != ele_end_iter) 
    {
        element = hit_iter->second;
    } else {
        LOG(ERROR)("find element error.");
    }

    return element;
}


/*****************************************************************************
 函 数 名  : ScoreSelector.AddElement
 功能描述  : 添加元素
 输入参数  : uint32_t element_id  server id
             const TYPE &ele      LibcoNetSession *
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年7月8日
    作    者   : binbinwang
    修改内容   : 新生成函数

*****************************************************************************/
template<class TYPE>
int 
ScoreSelector<TYPE>::AddElement(uint32_t element_id, const TYPE &ele)
{
    uint32_t score = ele->GetScore();
    uint32_t id = ele->GetId();
    uint32_t connect = ele->IsConnected();
    if (elements_.find(element_id) == elements_.end())
    {
        LOG(INFO)("add element, element id:%u, id:%u, score:%u, connect:%u", element_id, id, score, connect);
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
ScoreSelector<TYPE>::RemoveElement(uint32_t element_id)
{
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;

    TypeIter iter = elements_.find(element_id);
    if (elements_.end() != iter)
    {
        uint32_t score = (iter->second)->GetScore();
        uint32_t id = (iter->second)->GetId();
        uint32_t connect = (iter->second)->IsConnected();
        LOG(WARN)("remove element, element id:%u, id:%u, score:%u, connect:%u", element_id, id, score, connect);
        elements_.erase(iter);
    }
    else
    {
        LOG(WARN)("remove element failed, not exist, element id:%u", element_id);
    }
}



template<class TYPE>
void 
ScoreSelector<TYPE>::SetElements(const vector<TYPE> &eles)
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


// 不起作用
template<class TYPE>
void 
ScoreSelector<TYPE>::SetElementId(uint32_t )
{
    LOG(WARN)("ScoreSelector, call useless function!");
}


template<class TYPE>
void 
ScoreSelector<TYPE>::ShowInfo()
{
/*
    LOG(DEBUG)("score selector. elements size:%zu."
        , elements_.size());
    
    typedef typename map<uint32_t, TYPE>::iterator TypeIter;
    TypeIter iter = elements_.begin();
    TypeIter end_iter = elements_.end();
    for (; iter != end_iter; ++iter)
    {
    // TODO: CRASH
        LOG(DEBUG)("element, id:%u, ele:%p, ele score:%u, connect flag:%u"
        , iter->first, iter->second, iter->second->GetScore(), iter->second->IsConnected());
    }
*/
}





#endif // !defined(SCORE_SELECTOR_H_)


