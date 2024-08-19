#pragma once

#include "a_star.h"
#include "../../components/include/point.h"
#include "../../components/include/rect.h"
#include "router.h"
#include <array>
#include <cassert>
#include <memory>
#include <numeric>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace graph {
namespace router {
ARITMETIC_T
class hpa_star : public a_star<T> {
using router_t = router<T>;
using route_t = route<T>;
using point_t = types::point<T>;
using rect_t = types::rect<T>;
using clusters_t = std::array<rect_t, 4>;
using entrances_t = std::vector<rect_t>;
using transition_point = std::pair<point_t, point_t>;
using cluster_t = std::pair<const rect_t, std::vector<transition_point>>;
using clusters_map = std::unordered_map<typename cluster_t::first_type, typename cluster_t::second_type>;
using node_t = components::node<T>;
using graph_t = graph<T>;
using a_star_t = a_star<T>;
using node_ptr_t = node_ptr<T>;
using visitation_order_v = std::vector<node_ptr_t>;

private:
// tl, tr, br, bl
clusters_map _clusters;
graph_t _abstract_graph;

clusters_t _create_clusters() {
    const rect_t bounds = [this]() {
        rect_t rv = rect_t::inside_out();
        for (const auto& pt : router_t::_graph.points())
            rv |= pt;
        return rv;
    }();

    // always only 4 clusters ¯\_(ツ)_/¯
    return [&]() -> clusters_t {
        clusters_t rv;
        auto& [tl, tr, br, bl ] = rv;
        const point_t midpoint = { std::midpoint(bounds.left, bounds.right), std::midpoint(bounds.bottom, bounds.top) };
        tl = rect_t { bounds.left,      bounds.top,     midpoint.x,     midpoint.y + 1 };
        tr = rect_t { midpoint.x + 1,   bounds.top,     bounds.right,   midpoint.y + 1 };
        br = rect_t { midpoint.x + 1,   midpoint.y,     bounds.right,   bounds.bottom  };
        bl = rect_t { bounds.left,      midpoint.y,     midpoint.x,     bounds.bottom  };
        return rv;
    }();
}

enum class entrance_side { left, top, right, bottom, none };
entrances_t _entrances_between_clusters(const rect_t& c1, const rect_t& c2, entrance_side e) const {
    auto [ begin, end, variable_axis, static_axis, c1_border, c2_border ] = [&] ()
        -> std::tuple<T rect_t::*, T rect_t::*, T point_t::*, T point_t::*, T rect_t::*, T rect_t::*>
    {
        switch (e) {
            case entrance_side::left:
            return { &rect_t::bottom, &rect_t::top, &point_t::y, &point_t::x, &rect_t::left, &rect_t::right };
            case entrance_side::top:
            return { &rect_t::left, &rect_t::right, &point_t::x, &point_t::y, &rect_t::top, &rect_t::bottom };
            case entrance_side::right:
            return { &rect_t::bottom, &rect_t::top, &point_t::y, &point_t::x, &rect_t::right, &rect_t::left };
            case entrance_side::bottom:
            return { &rect_t::left, &rect_t::right, &point_t::x, &point_t::y, &rect_t::bottom, &rect_t::top };
            default: {
                assert("Not possible to to find entrances with no shared border");
                return {};
            }
        }
    }();

    auto& graph = router_t::_graph;
    
    entrances_t rv;
    rect_t entrance_rect = rect_t::inside_out();

    for (T pos = c1.*begin; pos <= c1.*end; ++pos) {
        point_t first_pt;   first_pt.*variable_axis = pos;  first_pt.*static_axis = c1.*c1_border;
        point_t second_pt;  second_pt.*variable_axis = pos; second_pt.*static_axis = c2.*c2_border;

        auto first_node = graph.node_by_pos(first_pt);
        auto second_node = graph.node_by_pos(second_pt);
        if (!first_node || !second_node) continue;
        const bool connected = first_node->connected_to(second_node) && second_node->connected_to(first_node);
        if (!connected) {
            if (entrance_rect != rect_t::inside_out()) rv.push_back(entrance_rect);
            entrance_rect = rect_t::inside_out();
            continue;
        }
        entrance_rect |= first_pt;
        entrance_rect |= second_pt;
    }
    if (entrance_rect != rect_t::inside_out()) rv.push_back(entrance_rect);
    return rv;
}

std::vector<transition_point> _all_cluster_entrances(const rect_t& c, const clusters_t& clusters) const {
    std::vector<transition_point> rv;
    auto& graph = router_t::_graph;

    auto to_transition_point = [](const rect_t& r, const entrance_side& border) {
        if (r.width() > r.height()) {
            const auto x = std::midpoint(r.left, r.right);
            point_t top { x, r.top };
            point_t bottom { x, r.bottom };
            return border == entrance_side::bottom
                ? transition_point{ top, bottom }
                : transition_point{ bottom, top }
            ;
        }
        const auto y = std::midpoint(r.top, r.bottom);
        point_t left { r.left, y };
        point_t right{ r.right, y };
        return  border == entrance_side::right
            ? transition_point{ left, right }
            : transition_point{ right, left } 
        ;
    };

    for (const auto& cluster : clusters) {
        if (c == cluster) continue;

        const auto border = [&c, &cluster]() {
            if (c.top == cluster.top) {
                if      (c.left == cluster.right + 1) return entrance_side::left;
                else if (c.right == cluster.left - 1) return entrance_side::right;
            }
            else if (c.left == cluster.left) {
                if      (c.top == cluster.bottom - 1) return entrance_side::top;
                else if (c.bottom == cluster.top + 1) return entrance_side::bottom;
            }
            return entrance_side::none;
        }();
        if (border == entrance_side::none) continue;

        const auto entrances = _entrances_between_clusters(c, cluster, border);
        for (auto& e : entrances)
            rv.push_back(to_transition_point(e, border));
    }

    return rv;
}

using router<T>::calc;
node_ptr_t _create_abstract_graph() {
    std::unordered_map<point_t, node_ptr_t> nodes;
    auto get_node = [&](const point_t& pt) mutable {
        if (!nodes.contains(pt))
            nodes[pt] = std::make_shared<node_t>(pt);
        return nodes[pt];
    };

    auto& graph = router_t::_graph;

    for (auto& [cluster, transition_points] : _clusters) {
        for (auto [inside, exit ] : transition_points) {
            for (auto& [inside_second, _] : transition_points) {
                if (inside == inside_second) continue;
                const auto route = a_star_t::calc(inside, inside_second);
                if (!route) continue;
                auto begin_node = get_node(inside);
                auto end_node = get_node(inside_second);
                begin_node->insert_edge(edge_helper::create_edge(route.length, end_node));
            }
            
            auto entry_node = get_node(inside);
            auto exit_node = get_node(exit);
            entry_node->insert_edge(edge_helper::create_edge(1, exit_node));
        }
    }

    return nodes.begin()->second;
}

clusters_map _init_clusters() {
    clusters_map rv;
    const auto clusters = _create_clusters();
    for (auto& cluster : clusters)
        rv[cluster] = _all_cluster_entrances(cluster, clusters);
    return rv;
}

const cluster_t* find_cluster(const point_t& pt) {
    for (const auto& cluster : _clusters)
        if (cluster.first.contains(pt)) return &cluster;
    return nullptr;
}

struct visitation_order_pair_iterator_sentinel {};
struct visitation_order_pair_iterator {
    private:
    const visitation_order_v& _visitation_order;
    visitation_order_v::const_iterator _it;

    public:
    visitation_order_pair_iterator(const visitation_order_v& visitation_order)
        : _visitation_order(visitation_order), _it(_visitation_order.begin())
    {}
    
    void operator++() {
        ++_it;
    }

    bool operator==(visitation_order_pair_iterator_sentinel) const {
        return (_it + 1) == _visitation_order.end();
    }

    std::pair<point_t, point_t> operator*() const {
        return { (*_it)->pos(), (*(_it+1))->pos() };
    }
    
};

struct visitation_order_pairs {
    const visitation_order_v& visitation_order;
    visitation_order_pair_iterator begin() const {
        return visitation_order_pair_iterator(visitation_order);
    }
    visitation_order_pair_iterator_sentinel end() const {
        return visitation_order_pair_iterator_sentinel{};
    }
};

public:
    hpa_star(graph_t& graph) : a_star_t(graph), _clusters(_init_clusters()), _abstract_graph(_create_abstract_graph())
    {}

    route_t calc(point_t start, point_t end) override {
        const auto start_cluster = find_cluster(start);
        const auto end_cluster = find_cluster(end);
        if (!start_cluster || !end_cluster) return {};

        if (*start_cluster == *end_cluster) 
            return a_star_t::calc(start, end);

        auto start_node = std::make_shared<node_t>(start);
        for (auto& [ inside, exit ] : start_cluster->second) {
            const auto route = a_star_t::calc(start, inside);
            if (route)  {
                auto inside_node = _abstract_graph.node_by_pos(inside);
                if (inside_node) start_node->insert_edge(edge_helper::create_edge(route.length, inside_node));
            }
        }

        auto end_node = std::make_shared<node_t>(end);
        for (auto& [ inside, exit ] : end_cluster->second) {
            const auto route = a_star_t::calc(inside, end);
            if (route)  {
                auto inside_node = _abstract_graph.node_by_pos(inside);
                if (inside_node) inside_node->insert_edge(edge_helper::create_edge(route.length, end_node));
            }
        }

        _abstract_graph.reset(start_node);
        a_star_t astar_abstract(_abstract_graph);
        const auto abstract_route = astar_abstract.calc(start, end);
        if (!abstract_route) return {};


        std::vector<route_t> cluster_routes; cluster_routes.reserve(abstract_route.visitation_order.size() - 1);
        for (const auto& [ cluster_start, cluster_end ] : visitation_order_pairs { abstract_route.visitation_order }) {
            cluster_routes.emplace_back(a_star_t::calc(cluster_start, cluster_end));
        }

        auto last_node_in_route = [](const route_t& r) {
            node_ptr_t node = r.node;
            while (!node->edges().empty()) node = node->edges().front().node;
            return node;
        };

        auto merged_route = [&]() {
            route_t merged_route = cluster_routes.front();
            for (auto it = cluster_routes.cbegin() + 1; it != cluster_routes.cend(); ++it) {
                auto last_node = last_node_in_route(merged_route);
                auto& next_route = *it;
                auto next_node = next_route.node;

                // traversal point, skip as next route continues the path
                if (last_node->pos() == next_node->pos()) continue;
                last_node->insert_edge(edge_helper::create_edge(1, next_node));

                merged_route.visitation_order.insert(merged_route.visitation_order.end(),
                    std::make_move_iterator(next_route.visitation_order.begin()),
                    std::make_move_iterator(next_route.visitation_order.end())
                );
                merged_route.length += next_route.length;
            }
            return merged_route;
        }();

        return merged_route;;
    }

    const auto& clusters() const {
        return _clusters;
    }

};

} // end namespace router
} // end namespace midpoint