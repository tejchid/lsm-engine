#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace lsm::storage {

class BlockBuilder {
    std::vector<char> buffer_;
    std::vector<uint32_t> restarts_; // Restart points for prefix compression
    int counter_;
    bool finished_;

public:
    BlockBuilder();
    void add(const std::string& key, const std::string& value);
    void reset();
    std::string finish();
    size_t current_size_estimate() const;
    bool empty() const { return buffer_.empty(); }
};

class BlockReader {
    const char* data_;
    size_t size_;
    uint32_t num_restarts_;
    uint32_t restart_offset_;

public:
    BlockReader(const char* data, size_t size);
    bool seek(const std::string& target, std::string* value);
};

} // namespace lsm::storage