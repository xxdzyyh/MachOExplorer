//
// Created by everettjf on 2017/7/7.
//

#include "binary.h"

MOEX_NAMESPACE_BEGIN

Binary::Binary(const std::string & filepath)
        : filepath_(filepath){
    using namespace boost::interprocess;

    if(filepath_.length() == 0){
        throw NodeException("invalid file path");
    }

    // Mapping file
    const char * file_name = filepath_.c_str();

    file_mapping tmp_mapping(file_name,read_only);
    mapping_ = std::move(tmp_mapping);
    mapped_region tmp_region(mapping_,read_only);
    region_ = std::move(tmp_region);

    void *addr = region_.get_address();
    std::size_t size = region_.get_size();

    // Set member
    memory_ = addr;
    memorysize_ = size;

    // Context
    NodeContextPtr context = std::make_shared<NodeContext>();
    context->file_start = memory_;

    // Header
    magic_.Parse(memory_);
    if(!magic_.IsValid()){
        throw NodeException("Not a MachO file");
    }

    if(magic_.IsFat()){
        fath_ = std::make_shared<FatHeader>();
        fath_->init(memory_,context);
    }else{
        mh_ = std::make_shared<MachHeader>();
        mh_->init(memory_,context);
    }
}


MOEX_NAMESPACE_END