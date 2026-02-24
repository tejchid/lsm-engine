#include "lsm/storage/io_uring_engine.hpp"
#include <stdexcept>
#include <liburing.h>

namespace lsm::storage {

class IOURingEngine {
    struct io_uring ring;
    static constexpr int ENTRIES = 256;

public:
    IOURingEngine() {
        if (io_uring_queue_init(ENTRIES, &ring, 0) < 0) {
            throw std::runtime_error("IO_URING Init Failed");
        }
    }

    ~IOURingEngine() { io_uring_queue_exit(&ring); }

    void async_write(int fd, const void* buf, size_t len, off_t offset, void* user_data) {
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        if (!sqe) {
            io_uring_submit(&ring);
            sqe = io_uring_get_sqe(&ring);
        }
        io_uring_prep_write(sqe, fd, buf, len, offset);
        io_uring_sqe_set_data(sqe, user_data);
        io_uring_submit(&ring);
    }

    int poll_completions() {
        struct io_uring_cqe* cqe;
        unsigned head;
        int count = 0;
        io_uring_for_each_cqe(&ring, head, cqe) {
            count++;
        }
        io_uring_cq_advance(&ring, count);
        return count;
    }
};
}