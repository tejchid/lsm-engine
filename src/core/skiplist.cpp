#include "lsm/core/skiplist.hpp"

namespace lsm::core {

template<typename K, typename V>
void SkipList<K, V>::insert(K key, V value) {
    Node* update[MAX_HEIGHT];
    Node* curr = head_.load(std::memory_order_acquire);

    // Navigate the levels to find insertion points
    for (int i = max_height_.load(std::memory_order_relaxed) - 1; i >= 0; i--) {
        while (Node* next = curr->next[i].load(std::memory_order_acquire)) {
            if (next->key < key) {
                curr = next;
            } else if (next->key == key) {
                // Key exists: Atomic update of the value
                next->value.store(value, std::memory_order_release);
                return;
            } else {
                break;
            }
        }
        update[i] = curr;
    }

    int height = generate_random_height();
    int current_max = max_height_.load(std::memory_order_relaxed);
    
    while (height > current_max) {
        if (max_height_.compare_exchange_weak(current_max, height, 
            std::memory_order_release, std::memory_order_relaxed)) {
            for (int i = current_max; i < height; ++i) {
                update[i] = head_.load(std::memory_order_relaxed);
            }
            break;
        }
    }

    Node* new_node = new Node(key, value, height);
    for (int i = 0; i < height; ++i) {
        while (true) {
            Node* next = update[i]->next[i].load(std::memory_order_acquire);
            new_node->next[i].store(next, std::memory_order_relaxed);
            if (update[i]->next[i].compare_exchange_weak(next, new_node, 
                std::memory_order_release, std::memory_order_relaxed)) {
                break;
            }
            // If CAS fails, someone else inserted; re-find position for this level
            Node* find_pos = update[i];
            while (Node* n = find_pos->next[i].load(std::memory_order_acquire)) {
                if (n->key < key) find_pos = n;
                else break;
            }
            update[i] = find_pos;
        }
    }
    count_.fetch_add(1, std::memory_order_relaxed);
}

template<typename K, typename V>
std::optional<V> SkipList<K, V>::get(const K& key) const {
    Node* curr = head_.load(std::memory_order_acquire);
    for (int i = max_height_.load(std::memory_order_relaxed) - 1; i >= 0; i--) {
        while (Node* next = curr->next[i].load(std::memory_order_acquire)) {
            if (next->key < key) curr = next;
            else if (next->key == key) return next->value.load(std::memory_order_acquire);
            else break;
        }
    }
    return std::nullopt;
}

template class SkipList<int, int>;
template class SkipList<std::string, std::string>;
} // namespace lsm::core