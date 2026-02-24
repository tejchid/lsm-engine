#pragma once
#include <liburing.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

class IOEngine {
    struct io_uring ring;
    static constexpr uint32_t QUEUE_DEPTH = 64;

public:
    IOEngine();
    ~IOEngine();
    void submit_write(int fd, const void* buf, size_t len, off_t offset);
    int wait_for_completion();
};