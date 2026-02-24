#include <iostream>
#include <fcntl.h>
#include "lsm/skiplist.hpp"
#include "lsm/io_engine.hpp"

int main() {
    SkipList<int, int> memtable;
    IOEngine io;


    std::cout << "Inserting keys into lock-free memtable..." << std::endl;
    for(int i = 0; i < 100; ++i) {
        memtable.insert(i, i * 10);
    }


    int fd = open("sstable_0.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char* data = "Simulated SSTable Data Block";
    std::cout << "Submitting async write via io_uring..." << std::endl;
    io.submit_write(fd, data, strlen(data), 0);

    int result = io.wait_for_completion();
    std::cout << "Write completed: " << result << " bytes." << std::endl;

    close(fd);
    return 0;
}