#pragma once
#include <vector>
#include <string>
#include <queue>
#include "lsm/storage/sstable.hpp"

namespace lsm::storage {

struct MergeEntry {
    std::string key;
    std::string value;
    size_t sstable_index;

    bool operator>(const MergeEntry& other) const {
        return key > other.key;
    }
};

class Compactor {
public:
    Compactor(size_t max_threshold);
    void run_compaction(const std::vector<std::string>& input_files, const std::string& output_path);

private:
    size_t threshold_;
};

}