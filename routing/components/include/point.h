#pragma once

#include <type_traits>
#include <utility>

namespace graph {
namespace types {

#ifndef ARITMETIC_T
#define ARITMETIC_T template<typename T> requires(std::is_arithmetic<T>::value)
#endif // !ARITMETIC_T

ARITMETIC_T struct point {
private:
    // Bottom-left to top-right
    auto _compare(const point& r) const {
        if (x < r.x || (x == r.x && y < r.y)) return -1;
        else if (x > r.x || (x == r.x && y < r.y)) return 1;
        else return 0;
    }

public:
    T x;
    T y;

    point() : x(0), y(0) {}
    point(T x, T y) : x(x), y(y) {}

    point(const point& p) : x(p.x), y(p.y) {}
    point(point&& p) noexcept : x(std::move(p.x)), y(std::move(p.y)) {}

    point& operator=(point p) {
        this->x = p.x;
        this->y = p.y;
        return *this;
    }
    point& operator=(point&& p) {
        this->x = std::move(p.x);
        this->y = std::move(p.y);
        return *this;
    }

    auto operator<=>(const point& p) const {
        return _compare(p);
    }
    bool operator==(const point& p) const {
        return (this->x == p.x && this->y == p.y);
    }

    point operator+(const point& p) {
        return point(this->x + p.x, this->y + p.y);
    }
    point operator-(const point& p) {
        return point(this->x - p.x, this->y - p.y);
    }

    point& operator+=(const point& p) {
        this->x += p.x;
        this->y += p.y;
        return *this;
    }
    point& operator-=(const point& p) {
        this->x -= p.x;
        this->y -= p.y;
        return *this;
    }

    point operator*(const point& p) {
        return point(this->x * p.x, this->y * p.y);
    }
    point operator/(const point& p) {
        return point(this->x / p.x, this->y / p.y);
    }

    point& operator*=(const point& p) {
        this->x *= p.x;
        this->y *= p.y;
        return *this;
    }

    point& operator /=(const point& p) {
        this->x /= p.x;
        this->y /= p.y;
        return *this;
    }
};
} // end namespace types
} // end namespace graph

template<typename T>
struct std::hash<graph::types::point<T>> {
    std::size_t operator()(const graph::types::point<T>& pt) const noexcept {
        std::size_t h1 = std::hash<T>{}(pt.x);
        std::size_t h2 = std::hash<T>{}(pt.y);
        return h1 ^ (h2 << 1);
    }
};
