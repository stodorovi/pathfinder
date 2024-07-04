#pragma once

#include "point.h"

#include <vector>
#include <memory>

namespace graph {
namespace components {
ARITMETIC_T class edge;

ARITMETIC_T struct node {
private:
    types::point<T> _pos;

    using edges_v = std::vector<edge<T>>;
    edges_v _edges;

public:
    node() : _pos(), _edges() {}
    node(types::point<T> p) : _pos(p), _edges() {}

    types::point<T> pos() {
        return _pos;
    }

    void pos(types::point<T> p) {
        _pos = p;
    }

    edges_v& edges() {
        return _edges;
    }

    edge<T>& insert_edge(const edge<T>& e) {
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
