#pragma once
#include <atomic>
#include <vector>
#include <optional>

template<typename K, typename V>
class SkipList {
    static constexpr int MAX_LEVEL = 16;
    struct Node {
        K key;
        V value;
        std::atomic<Node*> next[MAX_LEVEL];
        Node(K k, V v) : key(k), value(v) {
            for (int i = 0; i < MAX_LEVEL; ++i) next[i].store(nullptr, std::memory_order_relaxed);
        }
    };
    std::atomic<Node*> head;

public:
    SkipList() {
        head.store(new Node(K{}, V{}), std::memory_order_relaxed);
    }
    void insert(K key, V value);
    std::optional<V> get(K key);
};