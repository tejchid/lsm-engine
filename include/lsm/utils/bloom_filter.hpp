#pragma once
#include <vector>
#include <string>
#include <cmath>

namespace lsm::utils {
class BloomFilter {
    std::vector<bool> bits_;
    uint8_t num_hashes_;
public:
    BloomFilter(size_t num_items, double false_positive_rate);
    void add(const std::string& key);
    bool might_contain(const std::string& key) const;
    
    // Serialization for disk storage
    std::vector<uint8_t> serialize() const;
    void deserialize(const std::vector<uint8_t>& data);
};
}