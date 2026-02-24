#include "lsm/skiplist.hpp"

template<typename K, typename V>
void SkipList<K, V>::insert(K key, V value) {
    Node* new_node = new Node(key, value);
    Node* curr = head.load(std::memory_order_acquire);
    Node* expected = nullptr;
    while (!curr->next[0].compare_exchange_weak(expected, new_node, 
            std::memory_order_release, std::memory_order_relaxed)) {
        curr = curr->next[0].load(std::memory_order_acquire);
        expected = nullptr;
    }
}

template<typename K, typename V>
std::optional<V> SkipList<K, V>::get(K key) {
    Node* curr = head.load(std::memory_order_acquire)->next[0].load(std::memory_order_acquire);
    while (curr) {
        if (curr->key == key) return curr->value;
        curr = curr->next[0].load(std::memory_order_acquire);
    }
    return std::nullopt;
}

template class SkipList<int, int>;