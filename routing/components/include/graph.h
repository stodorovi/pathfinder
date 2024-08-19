#pragma once

#include "node.h"
#include "edge.h"
#include "point.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace graph {
ARITMETIC_T
using node_ptr = std::shared_ptr<components::node<T>>;

ARITMETIC_T class graph {
public:
    using node_ptr_t = node_ptr<T>;
    using point_t = types::point<T>;

private:
    node_ptr_t _origin_node;

    using node_table_t = std::unordered_map<point_t, node_ptr_t>;
    node_table_t _node_lookup_table;

    void _init_lookup_table() {
        this->_node_lookup_table.clear();
        if (!this->_origin_node) return;

        std::queue<node_ptr_t> next_nodes;
        next_nodes.push(_origin_node);
        std::unordered_set<point_t> visited;
        visited.insert(_origin_node->pos());

        while (next_nodes.size()) {
            auto node = next_nodes.front();
            next_nodes.pop();
            _node_lookup_table[node->pos()] = node;

            for (auto& e : node->edges()) {
                const auto pt = e.node->pos();
                if (!visited.contains(pt)) {
                    next_nodes.emplace(e.node);
                    visited.insert(pt);
                }
            }
        }
    }

public:
    graph(node_ptr_t origin_node) : _origin_node(origin_node) {
        _init_lookup_table();
    }

    void reset(node_ptr_t node) {
        _origin_node = node;
        _init_lookup_table();
    }

    node_ptr_t node_by_pos(point_t pt) const {
        if (_node_lookup_table.contains(pt))
            return _node_lookup_table.at(pt);

        return nullptr;
    }

    std::vector<point_t> points() const {
        std::vector<point_t> rv(_node_lookup_table.size());
        for (auto& [pt, _] : _node_lookup_table)
            rv.push_back(pt);
        return rv;
    }
};
} // end namespace graph