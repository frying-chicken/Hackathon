#pragma once

#include <limits>
#include <utility>

#include "RingContainer.hpp"

template <typename Key, typename T, size_t Capacity>
class PrefixSumWindow {
    static_assert(0 < Capacity, "PrefixSumWindow Capacity must be greater than zero");

    static constexpr T kMod = std::numeric_limits<T>::max() / 2;
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

        const size_t begin_index = lower_bound(1, _data.size(), begin);
        const size_t end_index = lower_bound(begin_index, _data.size(), end);

        if (begin_index == end_index) {
            return T{};
        }

        return sub_mod(_data[end_index - 1].second, _data[begin_index - 1].second) / (end_index - begin_index);
    }

private:
    static T add_mod(T lhs, T rhs) {
        const T sum = lhs + rhs;
        return kMod <= sum ? sum - kMod : sum;
    }
    static T sub_mod(T lhs, T rhs) {
        return lhs >= rhs ? lhs - rhs : lhs + kMod - rhs;
    }

    size_t lower_bound(size_t begin, size_t end, Key key) const {
        while (begin < end) {
            size_t m = begin + (end - begin) / 2;
            if (_data[m].first <= key) { begin = m + 1; }
            else { end = m; }
        }
        return begin;
    }
    size_t upper_bound(size_t begin, size_t end, Key key) const {
        while (begin < end) {
            size_t m = begin + (end - begin) / 2;
            if (_data[m].first <= key) { begin = m + 1; }
            else { end = m; }
        }
        return begin;
    }
};