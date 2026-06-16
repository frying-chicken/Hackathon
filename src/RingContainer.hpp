#pragma once

#include <array>
#include <cstddef>

template <typename T, size_t Capacity>
class RingContainer {
    std::array<T, Capacity> _storage = {};

    size_t _front = 0;
    size_t _size = 0;

public:
    T& operator[](size_t index) { return _storage[offset(index)]; }
    const T& operator[](size_t index) const { return _storage[offset(index)]; }

    T& front() { return _storage[_front]; }
    const T& front() const { return _storage[_front]; }

    T& back() { return _storage[offset(_size - 1)]; }
    const T& back() const { return _storage[offset(_size - 1)]; }

    size_t size() const { return _size; }
    bool empty() const { return _size == 0; }

    void reset() {
        _front = 0;
        _size = 0;
    }

    void push_front(T x) {
        _front = decrement(_front);
        _storage[_front] = x;
        if (_size < Capacity) {
            _size++;
        }
    }
    void push_back(T x) {
        _storage[offset(_size)] = x;
        if (_size < Capacity) {
            _size++;
        }
        else {
            _front = increment(_front);
        }
    }
    void pop_front() {
        if (_size == 0) {
            return;
        }
        _front = increment(_front);
        _size--;
    }
    void pop_back() {
        if (_size == 0) {
            return;
        }
        _size--;
    }

private:
    constexpr size_t offset(size_t index) const { return _front + index < Capacity ? _front + index : _front + index - Capacity; }
    static constexpr size_t increment(size_t index) { return ++index == Capacity ? 0 : index; }
    static constexpr size_t decrement(size_t index) { return index == 0 ? Capacity - 1 : index - 1; }
};