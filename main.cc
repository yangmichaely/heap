#include <iostream>
#include <memory>
#include "heap.h"

struct Tracer {
    int value;
    Tracer(int v) : value(v) {
        std::cout << "Ctor(" << value << ")\n";
    }
    Tracer(const Tracer& other) : value(other.value) {
        std::cout << "CopyCtor(" << value << ")\n";
    }
    Tracer(Tracer&& other) noexcept : value(other.value) {
        std::cout << "MoveCtor(" << value << ")\n";
        other.value = -1;
    }
    Tracer& operator=(const Tracer& other) {
        value = other.value;
        std::cout << "CopyAssign(" << value << ")\n";
        return *this;
    }
    Tracer& operator=(Tracer&& other) noexcept {
        value = other.value;
        std::cout << "MoveAssign(" << value << ")\n";
        other.value = -1;
        return *this;
    }
    bool operator<(const Tracer& other) const {
        return value < other.value;
    }
    friend std::ostream& operator<<(std::ostream& os, const Tracer& t) {
        return os << t.value;
    }
};

int main() {
    std::cout << "=== Heap<int> basic test ===\n";
    Heap<int> h;
    h.push(10);
    h.push(5);
    h.push(20);
    std::cout << "Top: " << h.top() << "\n";
    std::cout << "PopTop: " << h.popTop() << "\n";
    std::cout << "New Top: " << h.top() << "\n";

    std::cout << "\n=== Heap<Tracer> copy/move test ===\n";
    Heap<Tracer> ht;
    ht.push(Tracer(1));  // Move into heap
    Tracer t2(2);
    ht.push(t2);         // Copy into heap
    ht.push(Tracer(3));  // Move into heap

    std::cout << "Heap size: " << ht.size() << "\n";
    std::cout << "Top element: " << ht.top() << "\n";
    std::cout << "PopTop: " << ht.popTop() << "\n";
    std::cout << "Heap size after pop: " << ht.size() << "\n";

    std::cout << "\n=== Heap with move-only type ===\n";
    auto ptr_comp = [](const std::unique_ptr<int>& a, const std::unique_ptr<int>& b) {
        return *a < *b;
    };
    Heap<std::unique_ptr<int>, decltype(ptr_comp)> hu(0, ptr_comp);
    hu.push(std::make_unique<int>(42));
    hu.push(std::make_unique<int>(100));

    std::cout << "Popped top: " << *hu.popTop() << "\n";

    return 0;
}
