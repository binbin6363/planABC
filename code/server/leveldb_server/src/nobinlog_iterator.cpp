/**
 * @filedesc: 
 * nobinlog_iterator.cpp, iterator for kv except binlog's kv
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/19 18:02:59
 * @modify:
 *
**/
#include "nobinlog_iterator.h"
#include "leveldb/iterator.h"
#include "log.h"


NoBinlogIterator::NoBinlogIterator(leveldb::Iterator *it,
		const std::string &end,
		uint64_t limit,
		Direction direction)
{
	this->it_ = it;
	this->end = end;
	this->limit = limit;
	this->is_first = true;
	this->direction = direction;
}

NoBinlogIterator::~NoBinlogIterator(){
	delete it_;
}

leveldb::Slice NoBinlogIterator::key(){
    if (!it_->Valid()){
        LOG(ERROR)("NoBinlogIterator::key, iterator is invalid.");
        return leveldb::Slice("", 0);
    }
	return it_->key();
}

leveldb::Slice NoBinlogIterator::val(){
    if (!it_->Valid()){
        LOG(ERROR)("NoBinlogIterator::val, iterator is invalid.");
        return leveldb::Slice("", 0);
    }
	return it_->value();
}

bool NoBinlogIterator::skip(uint64_t offset){
	while(offset-- > 0){
		if(this->next() == false){
			return false;
		}
	}
	return true;
}

bool NoBinlogIterator::next(){
	if(limit == 0){
		return false;
	}
	if(is_first){
		is_first = false;
	}else{
		if(direction == FORWARD){
            do {
    			it_->Next();
                // 跳过包含特殊字符'~'的key，包含'~'的key是binlog的key
                if (it_->Valid() && it_->key()[0] == '~') {
                    continue;
                }
                break;
            }while(true);
            
		}else{
            do {
    			it_->Prev();
                // 跳过包含特殊字符'~'的key，包含'~'的key是binlog的key
                if (it_->key()[0] == '~' && it_->Valid()) {
                    continue;
                }
                break;
            }while(true);
		}
	}

	if(!it_->Valid()){
		// make next() safe to be called after previous return false.
		limit = 0;
		return false;
	}
	if(direction == FORWARD){
		if(!end.empty() && it_->key().compare(end) > 0){
			limit = 0;
			return false;
		}
	}else{
		if(!end.empty() && it_->key().compare(end) < 0){
			limit = 0;
			return false;
		}
	}
	limit --;
	return true;
}


