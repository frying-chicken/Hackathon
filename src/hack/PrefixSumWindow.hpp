#pragma once

#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

#include "RingContainer.hpp"
#include "Types.hpp"
#include "Utility.hpp"

namespace hack
{
    template <typename Key, typename T, size_t Capacity>
    class PrefixSumWindow
    {
        static_assert(std::is_unsigned_v<Key>);
        static_assert(std::is_unsigned_v<T>);
        static_assert(0 < Capacity);

        RingContainer<std::pair<Key, T>, Capacity + 1> _data;

    public:
        PrefixSumWindow() { reset(); }

        constexpr size_t size() const { return _data.size() - 1; }
        constexpr bool empty() const { return size() == 0; }
        constexpr bool full() const { return size() == Capacity; }
        static constexpr size_t capacity() { return Capacity; }

        void reset()
        {
            _data.reset();
            _data.push_back({std::numeric_limits<Key>::lowest(), T{}});
        }

        void push(Key key, T value)
        {
            _data.push_back({key, _data.back().second + value});
        }

        std::optional<T> average(Key begin, Key end) const
        {
            if (empty() || !(begin < end))
                return std::nullopt;

            const size_t begin_index = lower_bound(1, _data.size(), begin);
            const size_t end_index = lower_bound(begin_index, _data.size(), end);

            if (begin_index == end_index)
                return std::nullopt;

            return (_data[end_index - 1].second - _data[begin_index - 1].second) / (end_index - begin_index);
        }

    private:
        size_t lower_bound(size_t begin, size_t end, Key key) const
        {
            Key base = _data[begin].first;
            while (begin < end)
            {
                size_t m = begin + (end - begin) / 2;
                if (_data[m].first - base < key - base)
                {
                    begin = m + 1;
                }
                else
                {
                    end = m;
                }
            }
            return begin;
        }
        // size_t upper_bound(size_t begin, size_t end, Key key) const {
        //     while (begin < end) {
        //         size_t m = begin + (end - begin) / 2;
        //         if (_data[m].first <= key) { begin = m + 1; }
        //         else { end = m; }
        //     }
        //     return begin;
        // }
    };
}
