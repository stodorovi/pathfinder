#pragma once

#include <memory>

#ifndef ARITMETIC_T
#define ARITMETIC_T template<typename T> requires(std::is_arithmetic<T>::value)
#endif // !ARITMETIC_T

namespace graph {
namespace components {
ARITMETIC_T struct node;

using weight_t = uint32_t;

ARITMETIC_T struct edge {
using node_ptr_t = std::shared_ptr<node<T>>;
using edge_t = edge<T>;
public:
    weight_t weight;
    node_ptr_t node;
    bool operator==(const edge_t&) const = default;
};

} // end namespace components
} // end namespace graph