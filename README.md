# High-Performance LSM Storage Engine

## Why I built this
Standard database implementations often bottle-neck on disk I/O and mutex contention in the Memtable. I wanted to see how much performance I could reclaim by:
1. Moving away from standard POSIX blocking writes.
2. Implementing a Memtable that doesn't rely on a global lock.
3. Managing data in a structured, block-based format optimized for cache locality.

## Technical Architecture

### 1. Lock-Free Memtable
I implemented a multi-level SkipList using C++20 atomics. Instead of a `std::mutex`, it uses **Acquire-Release memory ordering** and **Compare-And-Swap (CAS)** for node linkage. This allows multiple threads to ingest data simultaneously without the heavy overhead of context switching between threads.

### 2. Kernel Bypass with io_uring
Most storage engines spend too much time waiting for the kernel. I used **liburing** to implement an asynchronous submission/completion loop. This allows the engine to keep pushing data to the NVMe drive while the CPU stays busy processing the next batch of writes.

### 3. Block-Based SSTables
Data on disk is organized into 4KB blocks. 
* **Index Blocks:** Fast binary search for keys within a file.
* **Bloom Filters:** A probabilistic check that prevents "ghost reads" from hitting the disk.
* **K-Way Compaction:** A background process that uses a priority queue to merge multiple SSTable segments, cleaning up shadowed keys and reducing fragmentation.

## Performance Considerations
* **Memory Management:** The project utilizes page-aligned buffers to ensure `O_DIRECT` compatibility where possible.
* **Cache Locality:** The BlockBuilder ensures that keys are laid out contiguously to minimize TLB misses during high-speed lookups.

## Build Requirements
- Linux (for io_uring support)
- Clang 15+ or GCC 12+ (C++20 features)
- `liburing-dev`

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./lsm_engine
