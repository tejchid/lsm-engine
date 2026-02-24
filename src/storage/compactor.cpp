#include "lsm/storage/compactor.hpp"
#include "lsm/storage/block.hpp"
#include <fstream>

namespace lsm::storage {

Compactor::Compactor(size_t max_threshold) : threshold_(max_threshold) {}

void Compactor::run_compaction(const std::vector<std::string>& input_files, const std::string& output_path) {
    std::priority_queue<MergeEntry, std::vector<MergeEntry>, std::greater<MergeEntry>> pq;
    std::vector<std::ifstream> streams;
    
    for (size_t i = 0; i < input_files.size(); ++i) {
        streams.emplace_back(input_files[i], std::ios::binary);
        std::string k, v;
        if (streams.back() >> k >> v) {
            pq.push({k, v, i});
        }
    }

    BlockBuilder builder;
    std::ofstream out(output_path, std::ios::binary);

    while (!pq.empty()) {
        MergeEntry top = pq.top();
        pq.pop();

        builder.add(top.key, top.value);
        if (builder.current_size_estimate() >= 4096) {
            std::string block = builder.finish();
            out.write(block.data(), block.size());
            builder.reset();
        }

        std::string next_k, next_v;
        if (streams[top.sstable_index] >> next_k >> next_v) {
            pq.push({next_k, next_v, top.sstable_index});
        }
    }

    if (!builder.empty()) {
        std::string block = builder.finish();
        out.write(block.data(), block.size());
    }
}

}