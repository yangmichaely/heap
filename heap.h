#pragma once
#include <memory>
#include <stdexcept>
#include <utility>
#include <functional>

template <typename T, typename Compare = std::less<T>>
class Heap {
private:
    std::allocator<T> alloc;
    T* data;
    size_t size_;
    size_t capacity_;
    Compare comp;

    void heapifyUp(size_t idx) {
        while (idx > 0) {
            size_t parent = (idx - 1) / 2;
            if (comp(data[parent], data[idx])) {
                std::swap(data[parent], data[idx]);
                idx = parent;
            } else break;
        }
    }

    void heapifyDown(size_t idx) {
        while (true) {
            size_t left = idx * 2 + 1;
            size_t right = idx * 2 + 2;
            size_t largest = idx;

            if (left < size_ && comp(data[largest], data[left])) largest = left;
            if (right < size_ && comp(data[largest], data[right])) largest = right;
            if (largest != idx) {
                std::swap(data[idx], data[largest]);
                idx = largest;
            } else break;
        }
    }

    void grow() {
        size_t newCap = capacity_ == 0 ? 1 : capacity_ * 2;
        T* newData = alloc.allocate(newCap);
        
        size_t constructed = 0;
        try {
            for (size_t i = 0; i < size_; i++) {
                std::allocator_traits<std::allocator<T>>::construct(alloc, &newData[i], std::move(data[i]));
                constructed++;
            }
        } catch (...) {
            for (size_t i = 0; i < constructed; i++) {
                std::allocator_traits<std::allocator<T>>::destroy(alloc, &newData[i]);
            }
            alloc.deallocate(newData, newCap);
            throw;
        }
        
        for (size_t i = 0; i < size_; i++) {
            std::allocator_traits<std::allocator<T>>::destroy(alloc, &data[i]);
        }
        if (data) {
            alloc.deallocate(data, capacity_);
        }
        
        data = newData;
        capacity_ = newCap;
    }

public:
    Heap(size_t initialCap = 0, Compare c = Compare())
        : alloc(),
          data(initialCap ? alloc.allocate(initialCap) : nullptr),
          size_(0),
          capacity_(initialCap),
          comp(c) {}

    ~Heap() {
        for (size_t i = 0; i < size_; i++) {
            std::allocator_traits<std::allocator<T>>::destroy(alloc, &data[i]);
        }
        if (data) {
            alloc.deallocate(data, capacity_);
        }
    }

    Heap(const Heap& other)
        : alloc(),
          data(other.capacity_ ? alloc.allocate(other.capacity_) : nullptr),
          size_(0),
          capacity_(other.capacity_),
          comp(other.comp) {
        try {
            for (size_t i = 0; i < other.size_; i++) {
                std::allocator_traits<std::allocator<T>>::construct(alloc, &data[i], other.data[i]);
                size_++;
            }
        } catch (...) {
            for (size_t i = 0; i < size_; i++) {
                std::allocator_traits<std::allocator<T>>::destroy(alloc, &data[i]);
            }
            if (data) alloc.deallocate(data, capacity_);
            throw;
        }
    }

    Heap& operator=(const Heap& other) {
        if (this == &other) return *this;
        Heap temp(other);
        std::swap(data, temp.data);
        std::swap(size_, temp.size_);
        std::swap(capacity_, temp.capacity_);
        std::swap(comp, temp.comp);
        return *this;
    }

    Heap(Heap&& other) noexcept
        : alloc(std::move(other.alloc)),
          data(other.data),
          size_(other.size_),
          capacity_(other.capacity_),
          comp(std::move(other.comp)) {
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Heap& operator=(Heap&& other) noexcept {
        if (this == &other) return *this;
        for (size_t i = 0; i < size_; i++) {
            std::allocator_traits<std::allocator<T>>::destroy(alloc, &data[i]);
        }
        if (data) {
            alloc.deallocate(data, capacity_);
        }
        alloc = std::move(other.alloc);
        data = other.data;
        size_ = other.size_;
        capacity_ = other.capacity_;
        comp = std::move(other.comp);
        
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        
        return *this;
    }

    void push(const T& value) {
        if (size_ == capacity_) grow();
        std::allocator_traits<std::allocator<T>>::construct(alloc, &data[size_], value);
        heapifyUp(size_);
        size_++;
    }

    void push(T&& value) {
        if (size_ == capacity_) grow();
        std::allocator_traits<std::allocator<T>>::construct(alloc, &data[size_], std::move(value));
        heapifyUp(size_);
        size_++;
    }

    void pop() {
        if (size_ == 0) throw std::out_of_range("Heap is empty");
        data[0] = std::move(data[size_ - 1]);
        std::allocator_traits<std::allocator<T>>::destroy(alloc, &data[size_ - 1]);
        size_--;
        if (size_ > 0) heapifyDown(0);
    }

    T popTop() {
        if (size_ == 0) throw std::out_of_range("Heap is empty");
        T topValue = std::move(data[0]);
        data[0] = std::move(data[size_ - 1]);
        std::allocator_traits<std::allocator<T>>::destroy(alloc, &data[size_ - 1]);
        size_--;
        if (size_ > 0) heapifyDown(0);
        return topValue;
    }

    const T& top() const {
        if (size_ == 0) throw std::out_of_range("Heap is empty");
        return data[0];
    }

    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }
};
