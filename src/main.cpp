#include "lsm/core/skiplist.hpp"
#include "lsm/storage/io_uring_engine.hpp"
#include "lsm/storage/compactor.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    using namespace lsm;
    
    core::SkipList<int, int> memtable;
    storage::IOURingEngine io;
    storage::Compactor compactor(1024 * 1024);

    std::cout << "[LSM] Initializing Storage Engine..." << std::endl;

    for (int i = 0; i < 50000; ++i) {
        memtable.insert(i, i * 2);
    }

    if (memtable.size() > 10000) {
        std::cout << "[LSM] Memtable threshold reached. Triggering async flush..." << std::endl;
        
        int fd = open("level_0.sst", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        auto it = memtable.begin();
        
        std::vector<char> buffer;
        while (it.valid()) {
            std::string entry = std::to_string(it.key()) + ":" + std::to_string(it.value()) + "\n";
            buffer.insert(buffer.end(), entry.begin(), entry.end());
            it.next();
        }

        io.async_write(fd, buffer.data(), buffer.size(), 0, nullptr);
        
        while (io.poll_completions() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        close(fd);
    }

    std::cout << "[LSM] Engine Shutdown." << std::endl;
    return 0;
}