#pragma once
#include "lsm/storage/block.hpp"
#include "lsm/utils/bloom_filter.hpp"
#include <string>
#include <memory>

namespace lsm::storage {

class SSTable {
    std::string filename_;
    std::unique_ptr<utils::BloomFilter> filter_;
    uint64_t index_offset_;
    uint64_t filter_offset_;

public:
    SSTable(std::string filename);
    
    // Static method to build an SSTable from a Memtable iterator
    static void build_from_memtable(const std::string& path, auto& iterator);
    
    bool get(const std::string& key, std::string* value);
};

} // namespace lsm::storage