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
public:
    weight_t weight;
    std::shared_ptr<node<T>> node;
};

} // end namespace components
} // end namespace graph