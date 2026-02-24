#include "lsm/storage/block.hpp"
#include <algorithm>
#include <cstring>

namespace lsm::storage {

BlockBuilder::BlockBuilder() : counter_(0), finished_(false) {
    restarts_.push_back(0); 
}

void BlockBuilder::add(const std::string& key, const std::string& value) {

    uint32_t klen = key.size();
    uint32_t vlen = value.size();
    
    char metadata[8];
    std::memcpy(metadata, &klen, 4);
    std::memcpy(metadata + 4, &vlen, 4);
    
    buffer_.insert(buffer_.end(), metadata, metadata + 8);
    buffer_.insert(buffer_.end(), key.begin(), key.end());
    buffer_.insert(buffer_.end(), value.begin(), value.end());
    
    counter_++;
    if (counter_ >= 16) { // New restart point every 16 entries
        restarts_.push_back(static_cast<uint32_t>(buffer_.size()));
        counter_ = 0;
    }
}

std::string BlockBuilder::finish() {
    for (uint32_t r : restarts_) {
        char buf[4];
        std::memcpy(buf, &r, 4);
        buffer_.insert(buffer_.end(), buf, buf + 4);
    }
    
    uint32_t num_restarts = restarts_.size();
    char buf[4];
    std::memcpy(buf, &num_restarts, 4);
    buffer_.insert(buffer_.end(), buf, buf + 4);
    
    finished_ = true;
    return std::string(buffer_.data(), buffer_.size());
}

size_t BlockBuilder::current_size_estimate() const {
    return buffer_.size() + restarts_.size() * 4 + 4;
}

}