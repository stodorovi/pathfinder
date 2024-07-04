#pragma once

#include "node.h"
#include "edge.h"

#include <map>
#include <queue>

namespace graph {
ARITMETIC_T
using node_ptr = std::shared_ptr<components::node<T>>;

ARITMETIC_T class graph {
private:
    node_ptr<T> _origin_node;

    using node_table_t = std::map<types::point<T>, node_ptr<T>>;
    node_table_t _node_lookup_table;

    void init() {
        this->_node_lookup_table.clear();
        if (!this->_origin_node) return;

        std::queue<node_ptr<T>> next_nodes;
        next_nodes.push(_origin_node);

        while (next_nodes.size()) {
            auto top_node = next_nodes.front();

            bool contains = _node_lookup_table.contains(top_node->pos());
            if (!_node_lookup_table.contains(top_node->pos()))
                _node_lookup_table[top_node->pos()] = top_node;

            for (auto& e : top_node->edges()) {
                auto node = e.node;
                if (_node_lookup_table.contains(node->pos())) continue;
                    next_nodes.push(e.node);
            }

            next_nodes.pop();
        }
    }

public:
    graph(node_ptr<T> origin_node) : _origin_node(origin_node) {
        init();
    }

    void reset(node_ptr<T> node) {
        _origin_node = node;
        init();
    }

    node_ptr<T> node_by_pos(types::point<T> pt) const {
        if (_node_lookup_table.contains(pt))
            return _node_lookup_table.at(pt);

        return nullptr;
    }
};
} // end namespace graph