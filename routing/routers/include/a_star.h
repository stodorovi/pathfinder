#pragma once

#include "router.h"

#include <cmath>
#include <queue>

namespace graph {
namespace router {
ARITMETIC_T
class a_star : public router<T> {
using route_t = route<T>;
protected:

    components::weight_t _heuristic_weight(
        components::node_ptr<T> node, components::node_ptr<T> end_node, components::edge<T>& e
    ) {
        const auto n_pt = e.node->pos();
        const auto end_pt = end_node->pos();
        
        const double a = abs(n_pt.x - end_pt.x);
        const double b = abs(n_pt.y - end_pt.y);
        const double res = sqrt((a * a) + (b * b));
        return res * 1000;
    }

public:
    a_star(graph<T>& graph) : router<T>(graph)
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
        unvisited_queue unvisited_q;
        auto start_un = unvisited_node{ current_unvisited_node.node, 0, nullptr };
        unvisited_q.push(start_un);

        std::set<node_ptr<T>> visited;
        typename route_t::route_visitation_order visitation_order;

        while (unvisited_q.size()) {
            unvisited_q.pop();

            for (auto& e : current_unvisited_node.node->edges()) {
                const auto weight = this->_heuristic_weight(current_unvisited_node.node, end_node, e);
                auto un = unvisited_node{
                    e.node, weight, std::make_shared<unvisited_node>(current_unvisited_node)
                };
                if (!visited.contains(un.node)) {
                    unvisited_q.push(un);
                }
            }

            visitation_order.push_back(current_unvisited_node.node);
            visited.insert(current_unvisited_node.node);

            if (current_unvisited_node.node == end_node)
                return this->_construct_route(current_unvisited_node, visitation_order);

            while(visited.contains(unvisited_q.top().node))
                unvisited_q.pop();

            current_unvisited_node = unvisited_q.top();
        }

        return { nullptr , components::weight_t(-1) };
    }
};
} // end namespace router
} // end namespace graph