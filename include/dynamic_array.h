#pragma once
#include <memory>
#include <iterator>
#include <stdexcept>
#include <new>
#include <cstdlib>

using namespace std;

template <class T, class Allocator>
class DynamicArray;

template <class T, class Allocator>
class ArrayIterator {
public:
    using iterator_category = forward_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;

private:
    DynamicArray<T, Allocator>* array_ptr;
    size_t current_index;

public:
    ArrayIterator(DynamicArray<T, Allocator>* array_ptr, size_t start_index) 
        : array_ptr(array_ptr), current_index(start_index) {}

    reference operator*() const {
        return (*array_ptr)[current_index];
    }

    pointer operator->() const {
        return &(*array_ptr)[current_index];
    }

    ArrayIterator& operator++() {
        ++current_index;
        return *this;
    }

    ArrayIterator operator++(int) {
        ArrayIterator temp = *this;
        ++current_index;
        return temp;
    }

    bool operator==(const ArrayIterator& other) const {
        return array_ptr == other.array_ptr && current_index == other.current_index;
    }

    bool operator!=(const ArrayIterator& other) const {
        return !(*this == other);
    }
};

template <class T, class Allocator = pmr::polymorphic_allocator<T>>
class DynamicArray {
private:
    T** blocks; //ptr array
    size_t capacity_;
    size_t size_;
    Allocator allocator;

    void resize_blocks() {
        size_t new_capacity = capacity_ == 0 ? 2 : capacity_ * 2;
        cout << "reallocate bloks: " << capacity_ << " -> " << new_capacity << endl;
        
        T** new_blocks = static_cast<T**>(malloc(new_capacity * sizeof(T*)));
        if (!new_blocks) throw bad_alloc();
        
        for (size_t i = 0; i < size_; ++i) {
            new_blocks[i] = blocks[i];
        }
        if (blocks) {
            free(blocks);
        }
        
        blocks = new_blocks;
        capacity_ = new_capacity;
    }

public:
    using value_type = T;
    using iterator = ArrayIterator<T, Allocator>;

    DynamicArray(const Allocator& alloc = {})
        : blocks(nullptr), capacity_(0), size_(0), allocator(alloc) {
        cout << "created DynamicArray" << endl;
    }

    DynamicArray(size_t initial_capacity, const Allocator& alloc = {})
        : blocks(nullptr), capacity_(initial_capacity), size_(0), allocator(alloc) {
        if (capacity_ > 0) {
            blocks = static_cast<T**>(malloc(capacity_ * sizeof(T*)));
            if (!blocks) {
                throw bad_alloc();
            }
        }
        cout << "DynamicArray's capacity " << capacity_ << endl;
    }

    void push_back(const T& value) {
        cout << "add element (size: " << size_ << ", capacity: " << capacity_ << ")..." << endl;
        
        if (size_ >= capacity_) {
            resize_blocks();
        }
        
        T* new_block = allocator.allocate(1);
        cout << "add new block for element: " << new_block << endl;
        
        try {
            allocator_traits<Allocator>::construct(allocator, new_block, value);
        } catch (...) {
            allocator.deallocate(new_block, 1);
            throw;
        }
        
        blocks[size_] = new_block;
        size_++;
        
        cout << "element added. size: " << size_ << endl;
    }

    void push_back(T&& value) {
        cout << "add new element" << endl;
        
        if (size_ >= capacity_) {
            resize_blocks();
        }
        
        T* new_block = allocator.allocate(1);
        cout << "add new block for element: " << new_block << endl;
        
        try {
            allocator_traits<Allocator>::construct(allocator, new_block, std::move(value));
        } catch (...) {
            allocator.deallocate(new_block, 1);
            throw;
        }
        
        blocks[size_] = new_block;
        size_++;
        
        cout << "element added. size: " << size_ << endl;
    }

    void pop_back() {
        if (size_ > 0) {
            T* last_block = blocks[size_ - 1];
            allocator_traits<Allocator>::destroy(allocator, last_block);
            allocator.deallocate(last_block, 1);
            size_--;
            cout << "element deleted. size: " << size_ << endl;
        }
    }

    T& operator[](size_t index) {
        if (index >= size_) {
            throw out_of_range("error: out of range\n");
        }
        return *blocks[index];
    }

    const T& operator[](size_t index) const {
        if (index >= size_) {
            throw out_of_range("error: out of range\n");
        }
        return *blocks[index];
    }

    size_t size() const { 
        return size_; 
    }

    bool empty() const { 
        return size_ == 0; 
    }

    size_t capacity() const { 
        return capacity_;
    }

    iterator begin() { 
        return iterator(this, 0); 
    }

    iterator end() { 
        return iterator(this, size_); 
    }

    ~DynamicArray() {
        cout << "clean up " << size_ << " elements" << endl;
        
        for (size_t i = 0; i < size_; ++i) {
            if (blocks[i]) {
                allocator_traits<Allocator>::destroy(allocator, blocks[i]);
                allocator.deallocate(blocks[i], 1);
            }
        }
        if (blocks) {
            free(blocks);
        }
    }

    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;
};
