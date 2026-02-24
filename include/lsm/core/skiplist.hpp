#ifndef LSM_SKIPLIST_HPP
#define LSM_SKIPLIST_HPP

#include <atomic>
#include <vector>
#include <random>
#include <optional>
#include <concepts>
#include <memory>

namespace lsm::core {

template<typename K, typename V>
class SkipList {
public:
    static constexpr int MAX_HEIGHT = 20;
    static constexpr float PROBABILITY = 0.25;

    struct Node {
        const K key;
        std::atomic<V> value;
        const int height;
        std::atomic<Node*> next[MAX_HEIGHT];

        Node(K k, V v, int h) : key(k), value(v), height(h) {
            for (int i = 0; i < MAX_HEIGHT; ++i) {
                next[i].store(nullptr, std::memory_order_relaxed);
            }
        }
    };

private:
    std::atomic<Node*> head_;
    std::atomic<int> max_height_;
    std::atomic<size_t> count_;

    int generate_random_height() {
        static thread_local std::mt19937 gen(std::random_device{}());
        static thread_local std::uniform_real_distribution<float> dist(0, 1);
        int h = 1;
        while (dist(gen) < PROBABILITY && h < MAX_HEIGHT) h++;
        return h;
    }

public:
    explicit SkipList() : max_height_(1), count_(0) {
        head_.store(new Node(K{}, V{}, MAX_HEIGHT), std::memory_order_relaxed);
    }

    ~SkipList() {
        Node* curr = head_.load(std::memory_order_relaxed);
        while (curr) {
            Node* next_node = curr->next[0].load(std::memory_order_relaxed);
            delete curr;
            curr = next_node;
        }
    }

    void insert(K key, V value);
    std::optional<V> get(const K& key) const;
    bool contains(const K& key) const;
    size_t size() const { return count_.load(std::memory_order_relaxed); }
    
    // Iterators for SSTable flushing
    class Iterator {
        Node* current;
    public:
        explicit Iterator(Node* node) : current(node) {}
        bool valid() const { return current != nullptr; }
        void next() { current = current->next[0].load(std::memory_order_acquire); }
        K key() const { return current->key; }
        V value() const { return current->value.load(std::memory_order_relaxed); }
    };
    
    Iterator begin() const { return Iterator(head_.load(std::memory_order_acquire)->next[0].load(std::memory_order_acquire)); }
};

} // namespace lsm::core

#endif