#pragma once

#include "../../components/include/graph.h"
#include "../../components/include/edge_helper.h"

#include <algorithm>
#include <optional>
#include <set>

namespace graph {
namespace router {
ARITMETIC_T class router {
public:
struct route {

    struct route_iterator_sentinel{};
    class route_iterator {
    private:
        components::node_ptr<T> _node;

    public:
        route_iterator(components::node_ptr<T> n) : _node(std::move(n)) {}
        bool operator==(route_iterator_sentinel) const { return _node == nullptr; }
        route_iterator& operator++() {
            _node = (_node && !_node->edges().empty())
                ? _node->edges().front().node
                : nullptr
            ;
            return *this;
        }
        types::point<T> operator*() { return _node->pos(); }
    };

    components::node_ptr<T> node;
    components::weight_t length;
    using route_visitation_order = std::vector<components::node_ptr<T>>;
    route_visitation_order visitation_order;

    route_iterator begin() const { return { node }; }
    route_iterator_sentinel end() const { return {}; }
    
    explicit operator bool() const {
        return node != nullptr;
    }
};

protected:
    graph<T>& _graph;

    struct unvisited_node {
        components::node_ptr<T> node;
        components::weight_t weight;
        std::shared_ptr<unvisited_node> previous_node;

        auto operator<=>(const unvisited_node& e) const {
            if (this->weight < e.weight) return -1;
            if (this->weight > e.weight) return 1;
            return 0;
        }
        bool operator==(const unvisited_node& e) const {
            return this->node == e.node;
        }
    };

    std::optional<components::edge<T>> _find_edge(
        const components::node_ptr<T> p, const components::node_ptr<T> searched_p
    ) const {
        for (auto& e : p->edges())
            if (e.node == searched_p)
                return e;

        return std::nullopt;
    }

    components::weight_t _route_len(components::node_ptr<T> n) const {
        components::weight_t len = 0;

        auto curr_node = n;
        while (!curr_node->edges().empty()) {
            auto e = curr_node->edges().at(0);
            len += e.weight;
            curr_node = e.node;
        }

        return len;
    }

    route _construct_route(unvisited_node n, route::route_visitation_order vo) const {
        std::vector<components::node_ptr<T>> route_nodes;
        unvisited_node* current_node_ptr = &n;
        do {
            route_nodes.push_back(current_node_ptr->node);
            current_node_ptr = current_node_ptr->previous_node.get();
        } while (current_node_ptr);

        std::reverse(route_nodes.begin(), route_nodes.end());

        auto first_node = std::make_shared<components::node<T>>(route_nodes.front()->pos());
        auto current_node = first_node;
        for (size_t i = 0; i < route_nodes.size() - 1; ++i) {
            auto n = route_nodes[i];
            auto next_n = route_nodes[i+1];
            
            if (auto e = _find_edge(n, next_n); e) {
                auto new_edge = current_node->insert_edge(edge_helper::create_edge(e->weight, e->node->pos()));
                current_node = new_edge.node;
            }
            else {
                return { nullptr, 0 };
            }
        }

        return { first_node, this->_route_len(first_node), std::move(vo) };
    }

public:
    router(graph<T>& graph) : _graph(graph) {};
    virtual ~router() {};

    virtual route calc(types::point<T> start, types::point<T> end) = 0;

    void reset(graph<T>& graph) {
        _graph = graph;
    }
};
} // end namespace router
} // end namespace graph