#pragma once

#include "router.h"

#include <algorithm>
#include <queue>

namespace graph {
namespace router {
ARITMETIC_T
class dijkstra : public router<T> {
public:
    dijkstra(const graph<T>& graph) : router<T>(graph) {}
    ~dijkstra() {}

    using router<T>::calc;
    router<T>::route calc(types::point<T> start, types::point<T> end) const override {
        auto start_node = this->_graph.node_by_pos(start);
        auto end_node = this->_graph.node_by_pos(end);

        if (!start_node || !end_node)
            return { nullptr, components::weight_t(-1) };
        if (start_node == end_node)
            return { std::make_shared<components::node<T>>(end_node->pos()), 0 };

        using unvisited_node = router<T>::unvisited_node;
        auto current_unvisited_node = unvisited_node{ start_node, 0, nullptr };

        using unvisited_queue = std::priority_queue<
            unvisited_node, std::vector<unvisited_node>, std::greater<unvisited_node>
        >;
        unvisited_queue unvisited_set;
        auto start_un = unvisited_node{ current_unvisited_node.node, 0, nullptr };
        unvisited_set.push(start_un);

        std::set<node_ptr<T>> visited;
        typename router<T>::route::route_visitation_order visitation_order;

        while (unvisited_set.size()) {
            unvisited_set.pop();

            for (auto& e : current_unvisited_node.node->edges()) {
                auto un = unvisited_node{
                    e.node, e.weight, std::make_shared<unvisited_node>(current_unvisited_node)
                };
                if (!visited.contains(un.node)) {
                    unvisited_set.push(un);
                }
            }

            visitation_order.push_back(current_unvisited_node.node);
            visited.insert(current_unvisited_node.node);
            
            if (current_unvisited_node.node == end_node)
                return this->_construct_route(current_unvisited_node, visitation_order);

            current_unvisited_node = unvisited_set.top();
        }

        return { nullptr , components::weight_t(-1) };
    }
};
} // end namespace router
} // end namespace graph