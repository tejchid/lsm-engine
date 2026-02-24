#include "lsm/io_engine.hpp"
#include <stdexcept>
#include <cstring>

IOEngine::IOEngine() {
    if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0) {
        throw std::runtime_error("Failed to initialize io_uring");
    }
}

IOEngine::~IOEngine() {
    io_uring_queue_exit(&ring);
}

void IOEngine::submit_write(int fd, const void* buf, size_t len, off_t offset) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) return;
    io_uring_prep_write(sqe, fd, buf, len, offset);
    io_uring_sqe_set_data(sqe, (void*)(uintptr_t)fd);
    io_uring_submit(&ring);
}

int IOEngine::wait_for_completion() {
    struct io_uring_cqe *cqe;
    int ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) return ret;
    int bytes_written = cqe->res;
    io_uring_cqe_seen(&ring, cqe);
    return bytes_written;
}