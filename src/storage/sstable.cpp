#include "lsm/storage/sstable.hpp"
#include <fstream>

namespace lsm::storage {

SSTable::SSTable(std::string filename) : filename_(std::move(filename)) {

}

bool SSTable::get(const std::string& key, std::string* value) {
    if (filter_ && !filter_->might_contain(key)) {
        return false; 
    }
   
    return false; 
}

} // namespace lsm::storage