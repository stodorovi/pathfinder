#pragma once

#include "point.h"

#include <vector>
#include <memory>

namespace graph {
namespace components {
ARITMETIC_T class edge;

ARITMETIC_T struct node {
private:
    using node_t = node<T>;
    using node_ptr_t = std::shared_ptr<node_t>;

    using point_t = types::point<T>;
    point_t _pos;

    using edge_t = edge<T>;
    using edges_v = std::vector<edge_t>;
    edges_v _edges;

public:
    node() : _pos(), _edges() {}
    node(point_t p) : _pos(p), _edges() {}
    node(T x, T y) : _pos(types::point(x, y)), _edges() {}

    bool operator==(const node_t& other) const = default;

    point_t pos() const {
        return _pos;
    }

    void pos(point_t p) {
        _pos = p;
    }

    edges_v& edges() {
        return _edges;
    }

    bool connected_to(const node_ptr_t& n) const {
        for (auto& edge : _edges)
            if (edge.node == n) return true;
        return false;
    }

    edge_t& insert_edge(const edge_t& e) {
        _edges.push_back(e);
        return _edges.back();
    }

    void clear_edges() {
        _edges.clear();
    }
};

ARITMETIC_T
using node_ptr = std::shared_ptr<node<T>>;

} // end namespace components
} // end namespace graph
