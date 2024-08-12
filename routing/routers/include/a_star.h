#pragma once

#include "router.h"
#include "dijkstra.h"

#include <queue>

namespace graph {
namespace router {
ARITMETIC_T
class a_star : public router<T> {
using route_t = route<T>;
protected:
    dijkstra<T> _dr;

    components::weight_t _heuristic_weight(
        components::node_ptr<T> node, components::node_ptr<T> end_node, components::edge<T>& e
    ) {
        auto route = _dr.calc(e.node->pos(), end_node->pos());
        return e.weight + route.length;
    }

public:
    a_star(graph<T>& graph) : router<T>(graph), _dr(graph)
    {}

    using router<T>::calc;
    route_t calc(types::point<T> start, types::point<T> end) override {
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
        typename route_t::route_visitation_order visitation_order;

        while (unvisited_set.size()) {
            unvisited_set.pop();

            for (auto& e : current_unvisited_node.node->edges()) {
                auto weight = this->_heuristic_weight(current_unvisited_node.node, end_node, e);
                auto un = unvisited_node{
                    e.node, weight + current_unvisited_node.weight , std::make_shared<unvisited_node>(current_unvisited_node)
                };
                if (!visited.contains(un.node)) {
                    unvisited_set.push(un);
                }
            }

            visitation_order.push_back(current_unvisited_node.node);
            visited.insert(current_unvisited_node.node);

            if (current_unvisited_node.node == end_node)
                return this->_construct_route(current_unvisited_node, visitation_order);

            while(visited.contains(unvisited_set.top().node))
                unvisited_set.pop();

            current_unvisited_node = unvisited_set.top();
        }

        return { nullptr , components::weight_t(-1) };
    }
};
} // end namespace router
} // end namespace graph