#pragma once

#include "point.h"
#include <limits>

namespace graph {
namespace types {

#ifndef ARITMETIC_T
#define ARITMETIC_T template<typename T> requires(std::is_arithmetic<T>::value)
#endif // !ARITMETIC_T

ARITMETIC_T struct rect {
using point_t = point<T>;
using rect_t = rect<T>;

    union { T x1; T left; };
    union { T y2; T top; };
    union { T x2; T right; };
    union { T y1; T bottom; };
    
    rect() : x1(0), y1(0), x2(0), y2(0) {}
    rect(T left, T top, T right, T bottom) : x1(left), y2(top), x2(right), y1(bottom) {}
    rect(point_t pt) : x1(pt.x), x2(pt.x), y1(pt.y), y2(pt.y) {}
    
    rect_t& operator|=(const point_t& pt) {
        if (pt.x < left)    left = pt.x;
        if (pt.x > right)   right = pt.x;
        if (pt.y < bottom)  bottom = pt.y;
        if (pt.y > top)     top = pt.y;
        return *this;
    }

    bool operator==(const rect_t& other) const {
        return left == other.left && top == other.top && right == other.right && bottom == other.bottom;
    }

    T height() const {
        return std::abs(top - bottom);
    }
    T width() const {
        return std::abs(right - left);
    }

    bool contains(const point_t& pt) const {
        return pt.x >= left && pt.x <= right && pt.y >= bottom && pt.y <= top;
    }

    static rect_t inside_out() {
        using limits_t = std::numeric_limits<T>;
        return rect_t { limits_t::max(), limits_t::lowest(), limits_t::lowest(), limits_t::max() };
    }
};
} // end namespace types
} // end namespace graph

template<typename T>
struct std::hash<const graph::types::rect<T>> {
    std::size_t operator()(const graph::types::rect<T>& r) const noexcept {
        std::size_t hl = std::hash<T>{}(r.left);
        std::size_t ht = std::hash<T>{}(r.top);
        std::size_t hr = std::hash<T>{}(r.right);
        std::size_t hb = std::hash<T>{}(r.bottom);
        return hl ^ (ht << 1) ^ (hr << 2) ^ (hb << 3);
    }
};
