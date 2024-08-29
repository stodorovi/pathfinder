#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iomanip>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>
#include <vector>

#include "../routers/include/dijkstra.h"
#include "../routers/include/a_star.h"
#include "../routers/include/hpa_star.h"

ARITMETIC_T
std::string points_print(const std::vector<graph::types::point<T>>& points) {
    std::stringstream ss;
    if (points.empty()) return ss.str();

    ss << "[";
    for (size_t i = 0; i < points.size(); ++i) {
        ss << "(" << points[i].x << "," << points[i].y << ")";
        if (i != points.size() - 1) ss << ", ";
    }
    ss << "]";

    return ss.str();
}

ARITMETIC_T
void results_print(
    const std::vector<graph::types::point<T>>& expected,
    const std::vector<graph::types::point<T>>& actual, const std::string alg
) {
    auto result = actual == expected ? "SUCCESS" : "FAIL";
    std::cout << result << "...(" << alg << ")"
        << "\n\tactual:" << std::setw(14) << std::setfill('.') << "| " << points_print(actual)
        << "\n\texpected:" << std::setw(12) << std::setfill('.') << "| " << points_print(expected)
        << "\n\n";
}

template<typename comparator>
void results_print(size_t l1, size_t l2, const std::string& tst, const std::string condition) {
    comparator comp;
    auto comp_res = comp(l1, l2);
    auto result = comp_res ? "SUCCESS" : "FAIL";
    auto outcome = comp_res ? "TRUE" : "FALSE";
    std::cout << result << "...(" << tst << ")"
        << "\n\tactual:" << std::setw(14) << std::setfill('.') << "| " << outcome
        << "\n\tcondition:" << std::setw(11) << std::setfill('.') << "| " << condition
        << "\n\n";
}

ARITMETIC_T
auto route_to_point_v(graph::components::node_ptr<T> route) {
    std::vector<graph::types::point<T>> route_pts;

    graph::components::node_ptr<T> current_node = route;
    while (current_node) {
        route_pts.push_back(current_node->pos());
        if (current_node->edges().empty()) break;
        current_node = current_node->edges()[0].node;
    }

    return route_pts;
}

ARITMETIC_T
void test_route(
    graph::components::node_ptr<T> route, std::vector<graph::types::point<T>> expected, const std::string& alg
) {
    graph::components::node_ptr<T> current_node = route;

    auto actual = route_to_point_v(route);
    results_print(expected, actual, alg);
}

ARITMETIC_T
void test_route(
    graph::components::node_ptr<T> route, graph::components::node_ptr<T> route2, const std::string& alg
) {
    auto actual = route_to_point_v(route);
    auto expected = route_to_point_v(route2);
    results_print(expected, actual, alg);
}

template<typename comparator>
void test_len(const size_t len1, const size_t len2, const std::string& tst, const std::string condition) {
    results_print<comparator>(len1, len2, tst, condition);
}

/*
        10 / (10,10) \ 22
(0,0) <                   > (22,22)
        11 \ (11,11) / 6
*/
auto small_graph() {
    auto n = std::make_shared<graph::components::node<int>>();
    auto first = n->insert_edge(edge_helper::create_edge(10, graph::types::point(10, 10)));
    auto second = n->insert_edge(edge_helper::create_edge(11, graph::types::point(11, 11)));

    auto third = first.node->insert_edge(edge_helper::create_edge(22, graph::types::point(22, 22)));
    auto fourth = third;
    fourth.weight = 6;
    second.node->insert_edge(fourth);

    return graph::graph<int>(n);
}

std::optional<std::pair<int, int>> valid_point(
    const int r, const int c, const int r_max, const int c_max
) {
    const int valid_r = r >= 0 && r < r_max;
    const int valid_c = c >= 0 && c < c_max;

    if (valid_c && valid_r) return std::make_pair(r, c);
    return std::nullopt;
}

auto surrounding_nodes(
    const int r, const int c, const std::vector<std::vector<graph::node_ptr<int>>>& nodes
) {
    std::vector<graph::node_ptr<int>> rv;

    const int rows = nodes.size();
    const int columns = nodes.front().size();

    if (auto pt = valid_point(r - 1, c, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r - 1, c + 1, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r, c + 1, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r + 1, c + 1, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r + 1, c, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r - 1, c - 1, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r, c - 1, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);
    if (auto pt = valid_point(r - 1, c - 1, rows, columns); pt)
        rv.push_back(nodes[pt->first][pt->second]);

    return rv;
}

/*
    o - o - o - o - o - o
    |   |   |   |   |   |
    o - o - o - o - o - o
    |   |   |   |   |   |
    o - o - o - o - o - o
    |   |   |   |   |   |
    o - o - o - o - o - o
    |   |   |   |   |   |
    o - o - o - o - o - o
    |   |   |   |   |   |
    o - o - o - o - o - o
*/
auto grid_graph(const int grid_size = 6) {
    std::vector<std::vector<graph::node_ptr<int>>> nodes(grid_size);
    for (int r = 0; r < nodes.size(); ++r) {
        nodes[r] = std::vector<graph::node_ptr<int>>(grid_size);
        for (int c = 0; c < nodes[r].size(); ++c) {
            nodes[r][c] = std::make_shared<graph::components::node<int>>(c, r);
        }
    }

    for (int r = 0; r < nodes.size(); ++r) {
        for (int c = 0; c < nodes[r].size(); ++c) {
            for (auto& n : surrounding_nodes(r, c, nodes))
                nodes[r][c]->insert_edge(edge_helper::create_edge(1, n));
        }
    }

    return graph::graph<int>(nodes[0][0]);
}

void test_dijkstra() {
    auto sg = small_graph();

    graph::router::dijkstra<int> dr(sg);
    auto route = dr.calc(graph::types::point(0, 0), graph::types::point(22, 22));
    test_route(route.node, {
        graph::types::point(0, 0),
        graph::types::point(11, 11),
        graph::types::point(22, 22) },
        "Dijkstra | small graph"
    );

    auto gg = grid_graph();
    dr.reset(gg);
    route = dr.calc(graph::types::point(0, 0), graph::types::point(3, 3));
    test_route(route.node, {
        graph::types::point(0, 0),
        graph::types::point(1, 1),
        graph::types::point(2, 2),
        graph::types::point(3, 3) },
        "Dijkstra | grid graph"
    );

}

void test_a_star() {
    auto g = grid_graph();

    graph::router::dijkstra<int> dr(g);
    graph::router::a_star<int> ar(g);

    auto dijkstra_route = dr.calc(graph::types::point(0, 0), graph::types::point(5, 5));
    auto a_star_route = ar.calc(graph::types::point(0, 0), graph::types::point(5, 5));

    test_route(a_star_route.node, dijkstra_route.node, "A* == Dijkstra route | grid graph"
    );
    test_len<std::less<size_t>>(
        a_star_route.visitation_order.size(), dijkstra_route.visitation_order.size(),
        "A* smaller visitation size than Dijkstra | grid graph", "<"
    );
}

void test_hpa_start() {
    auto g = grid_graph();
    graph::router::hpa_star<int> hpar(g);
    graph::router::a_star<int> ar(g);

    const auto hpa_route = hpar.calc(graph::types::point(0, 0), graph::types::point(5, 5));
    const auto a_route = ar.calc(graph::types::point(0, 0), graph::types::point(5, 5));

    test_len<std::greater<size_t>>(hpa_route.visitation_order.size(), a_route.visitation_order.size(),
        "HPA* greater visitation size than A* because it traverses in between clusters instead of diagonaly", ">"
    );
}

template<typename Router>
double measure_calculation_time(Router& r,  const graph::types::point<int>& start, const graph::types::point<int>& end) {
    uint64_t duration_sum = 0;

    const int loop_count = 100;
    for (size_t i = 0; i < loop_count; ++i) {
        const auto start_time = std::chrono::high_resolution_clock::now();
        const auto route = r.calc(start, end);
        assert(route.node != nullptr);
        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        duration_sum += time.count();
    }

    return duration_sum / (double)loop_count / 1000;

}

struct hpa_star_caching_t : public graph::router::hpa_star<int> {
    hpa_star_caching_t(graph::graph<int>& g) : graph::router::hpa_star<int>(g) {}

    graph::route<int> calc(graph::types::point<int> start, graph::types::point<int> end) override {
        using a_star_t = graph::router::a_star<int>;

        const auto start_cluster = _find_cluster(start);
        const auto end_cluster = _find_cluster(end);
        
        // connect start and exit nodes
        auto start_node = std::make_shared<graph::components::node<int>>(start);
        for (auto& [ inside, exit ] : start_cluster->second) {
            const auto route = a_star_t::calc(start, inside);
        }
        auto end_node = std::make_shared<graph::components::node<int>>(end);
        for (auto& [ inside, exit ] : end_cluster->second) {
            const auto route = a_star_t::calc(inside, end);
        }

        // pretend the cluster routes are cached
        a_star_t astar_abstract(_abstract_graph);
        return astar_abstract.calc(start_cluster->second.front().first, end_cluster->second.front().first);
    }
};

void algs_run_time(int grid_size) {
    auto g = grid_graph(grid_size);
    const auto start_point = graph::types::point<int> { 0, 0 };
    const auto end_point = graph::types::point<int> { grid_size - 1, grid_size - 1 };

    graph::router::dijkstra<int> dijkstra(g);
    graph::router::a_star<int> a_star(g);
    graph::router::hpa_star<int> hpa_star(g);
    hpa_star_caching_t hpa_star_caching(g);

    const auto dijkstra_run_time = measure_calculation_time(dijkstra, start_point, end_point);
    const auto a_star_run_time = measure_calculation_time(a_star, start_point, end_point);
    const auto hpa_star_run_time = measure_calculation_time(hpa_star, start_point, end_point);
    const auto hpa_cached_star_run_time = measure_calculation_time(hpa_star_caching, start_point, end_point);

    std::cout << "Testing algorithms run time (grid size: " << grid_size << "x" << grid_size << ")"
        << "\n\tDijkstra:"          << std::setw(14) << std::setfill('.') << "| " << dijkstra_run_time << " ms"
        << "\n\tA*:"                << std::setw(20) << std::setfill('.') << "| " << a_star_run_time << " ms"
        << "\n\tHPA* (no caching):" << std::setw(5)  << std::setfill('.') << "| " << hpa_star_run_time << " ms"
        << "\n\tHPA* (caching):"    << std::setw(8)  << std::setfill('.') << "| " << hpa_cached_star_run_time << " ms"
        << "\n\n";
}

void test_run_time() {
    algs_run_time(10);
    algs_run_time(20);
    algs_run_time(40);
    algs_run_time(80);
    algs_run_time(160);
    algs_run_time(320);
}

int main() {
    test_dijkstra();
    test_a_star();
    test_hpa_start();
    test_run_time();
}