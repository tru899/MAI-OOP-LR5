#pragma once
#include <memory_resource>
#include <map>
#include <iostream>
#include <cstdlib>

using namespace std;

class CustomMemoryResource : public pmr::memory_resource {
private:
    map<void*, size_t> allocated_blocks;
    map<size_t, vector<void*>> free_blocks_by_size;

public:
    void* do_allocate(size_t bytes, size_t alignment) override {
        if (bytes == 0) {
            bytes = 1;
        }

        cout << "req on allocate: " << bytes << " bytes" << endl;

        auto it = free_blocks_by_size.find(bytes);
        if (it != free_blocks_by_size.end() && !it->second.empty()) {
            void* ptr = it->second.back();  //get block
            it->second.pop_back();
            allocated_blocks[ptr] = bytes;
            cout << "overused block: " << ptr << endl;
            return ptr;
        }

        void* ptr = malloc(bytes);  //if isn't free block => alloc new 
        if (!ptr) {
            cout << "error: allocate" << bytes << " bytes" << endl;
            throw bad_alloc();
        }

        allocated_blocks[ptr] = bytes;
        cout << "allocate new block: " << ptr << " size " << bytes << " bytes" << endl;
        return ptr;
    }

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        if (ptr == nullptr) {
            return;
        }

        cout << "clean up block: " << ptr << endl;

        auto it = allocated_blocks.find(ptr);
        if (it == allocated_blocks.end()) {
            throw logic_error("error: cleaning up unallocated memory\n");
        }

        allocated_blocks.erase(it);
        free_blocks_by_size[bytes].push_back(ptr);  //save to mem
    }

    bool do_is_equal(const pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    ~CustomMemoryResource() {
        cout << "clean up CustomMemoryResource: " 
                  << allocated_blocks.size() << " allocated blocks, "
                  << free_blocks_by_size.size() << " free blocks" << endl;
        
        for (auto& [ptr, size] : allocated_blocks) {
            free(ptr);
        }
        
        for (auto& [size, blocks] : free_blocks_by_size) {
            for (void* ptr : blocks) {
                free(ptr);
            }
        }
    }
};
