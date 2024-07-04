#pragma once

#include "node.h"
#include "edge.h"
#include "point.h"

namespace edge_helper {
using namespace graph;
ARITMETIC_T
components::edge<T> create_edge(components::weight_t weight, components::node_ptr<T> n) {
    return components::edge<T>(weight, n);
}

ARITMETIC_T
components::edge<T> create_edge(uint32_t weight, types::point<T> pt) {
    return create_edge(weight, std::make_shared<components::node<T>>(pt));
}
} // end namespace edge_helper