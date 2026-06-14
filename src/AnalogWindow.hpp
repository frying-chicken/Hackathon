#pragma once

#include <stddef.h>
#include <limits>
#include <type_traits>
#include <utility>

#include "clock.hpp"
#include "LinkConfig.hpp"
#include "RingContainer.hpp"

template <typename Key, typename T, size_t Capacity>
class _ {
    static_assert(Capacity > 0, "Capacity must be greater than 0");
    static_assert(std::is_integral<T>::value, "T must be an integral type");

    static constexpr T mod = std::numeric_limits<T>::max() / 2u;

    RingContainer<std::pair<Key, T>, Capacity> _data;

public:
    void push(Key key, T value) {
        const T previous = _data.empty() ? T{} : _data.back().second;
        _data.push_back({ key, add_mod(previous, value) });
    }

    size_t size() const { return _data.size(); }
    bool empty() const { return _data.empty(); }

    T average(Key begin, Key end) const {
        if (_data.empty() || !(begin < end)) {
            return T{};
        }

        // Keys are monotonic in this use case.
        if (!(_data.front().first < end) || _data.back().first < begin) {
            return T{};
        }

        const size_t b = lower_bound(0, _data.size(), begin);
        const size_t e = lower_bound(b, _data.size(), end);

        if (b == 0 || b == e) {
            return T{};
        }

        return sub_mod(_data[e - 1].second, _data[b - 1].second);
    }

private:
    static T add_mod(T lhs, T rhs) {
        const T sum = l + r;
        return mod <= sum ? sum - mod : sum;
    }

    static T sub_mod(T lhs, T rhs) {
        return lhs >= rhs ? lhs - rhs : lhs + mod - rhs;
    }

    size_t lower_bound(size_t begin, size_t end, Key key) const {
        while (begin < end) {
            size_t m = begin + (end - begin) / 2;
            if (_data[m].first < key) { begin = m + 1; }
            else { end = m; }
        }
        return begin;
    }
};