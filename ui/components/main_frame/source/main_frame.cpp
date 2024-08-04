#include "../include/main_frame.h"
#include "../../toolbar/include/toolbar.h"
#include "../../toolbar/include/icon_button.h"
#include "../../toolbar/include/grid_creation_dialog.h"
#include "../../grid/include/grid.h"

#include "../../../../routing/components/include/graph.h"
#include "../../../../routing/components/include/edge_helper.h"
#include "../../../../routing/routers/include/dijkstra.h"
#include "ui/components/grid/include/cell.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <format>
#include <utility>

using grid_type = int32_t;
using grid_point = graph::types::point<grid_type>;
using grid_node = graph::components::node<grid_type>;
using grid_node_ptr = graph::node_ptr<grid_type>;
using grid_graph = graph::graph<grid_type>;

namespace {

auto desktop_centre(const int32_t w, int32_t h) {
    const auto screen = QGuiApplication::primaryScreen()->geometry();

    const auto x = (screen.width() / 2) - (w / 2);
    const auto y = (screen.height() / 2) - (h / 2);

    QRect rv;
    rv.setRect(x, y, w, h);
    return rv;
}

const std::vector<const cell*> get_surrounding_cells(
    grid const * const g, const int32_t row, const int32_t column
) {
    std::vector<const cell*> rv;
    if (const auto* c = (cell*)g->item(row - 1, column);     c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row - 1, column + 1); c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row,     column + 1); c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row + 1, column + 1); c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row + 1, column);     c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row + 1, column - 1); c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row,     column - 1); c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    if (const auto* c = (cell*)g->item(row - 1, column - 1); c && c->state() != cell_state::untraversable)
        rv.push_back(c);
    return rv;
}

std::tuple<std::vector<std::vector<grid_node_ptr>>, grid_point, grid_point>
create_nodes(grid const * const g) {
    const int32_t rows = g->rowCount();
    std::vector<std::vector<grid_node_ptr>> nodes(rows);
    grid_point start;
    grid_point end;

    const int32_t columns = g->columnCount();
    for (int32_t r = 0; r < rows; ++r) {
        nodes[r] = std::vector<grid_node_ptr>(columns);

        for (int32_t c = 0; c < columns; ++c) {
            if (const auto* cl = (cell*)g->item(r, c); cl && cl->state() != cell_state::untraversable) {
                auto n = std::make_unique<grid_node>(grid_point{ c, r });
                const grid_point npos = n->pos();
                if (cl->state() == cell_state::start)
                    start = npos;
                else if (cl->state() == cell_state::end)
                    end = npos;
                nodes[r][c] = std::move(n);
            }
        }
    }

    return std::make_tuple(nodes, start, end);
}

grid_graph connect_graph(grid const * const g, std::vector<std::vector<grid_node_ptr>> nodes) {
    grid_graph rv(nullptr);

    grid_node_ptr origin_node = nullptr;

    for (auto& row : nodes) {
        for (auto& node : row) {
            if (!node) continue;
            if (!origin_node) origin_node = node;

            auto p = node->pos();
            const auto sns = get_surrounding_cells(g, p.y, p.x);
            for (auto n : sns) {
                grid_point pt { n->column(), n->row() };
                node->insert_edge(edge_helper::create_edge(1, nodes[n->row()][n->column()]));
            }
        }
    }

    rv.reset(origin_node);
    return rv;
}

std::tuple<grid_graph, grid_point, grid_point> create_graph(grid const * const g) {
    auto [nodes, start, end] = create_nodes(g);
    return std::make_tuple(connect_graph(g, nodes), start, end);
}

void log_route(auto& route) {
    static std::string log;
    log = "Route: ";
    for (auto n : route) {
        log.append(std::format("({},{}) -> ", n.x, n.y));
    }
    printf("%s", log.append(std::format("LEN: {}\n", route.length)).c_str());
}

void toggle_cell_state_buttons(toolbar& t, cell_state& current, cell_state to_set) {
    static auto state_to_btn = [&t](cell_state s) -> icon_button* {
        switch (s) {
            case cell_state::start: return t.start_position_btn();
            case cell_state::end: return t.end_position_btn();
            case cell_state::untraversable: return t.untraversable_node_button();
            case cell_state::empty: return nullptr;
        }
    };

    icon_button* before = state_to_btn(current);
    current = toggle_state(current, to_set);
    icon_button* after = state_to_btn(current);
    
    if  (before) before->setEnabled(true);
    after->setEnabled(false);
}

} // end anonymous namespace

main_frame::main_frame() : QMainWindow(),
    _toolbar(new toolbar()), _grid(nullptr), _cell_set_state(cell_state::empty),
    _start_cell(nullptr), _end_cell(nullptr)
{
    this->addToolBar(_toolbar);
    setGeometry(desktop_centre(640, 480));
    setWindowTitle("Pathfinding Visualiser");

    _register_connections();
}

void main_frame::_create_new_grid() {
    grid_creation_dialog d;
    d.exec();
    auto [x, y] = d.values();

    const bool valid_grid = x && y;
    _grid = valid_grid ? new grid(this, x, y) : nullptr;
    setCentralWidget(_grid);

    if (_grid) connect(_grid, &grid::itemClicked, this, &main_frame::_on_cell_click);
    _start_cell = nullptr;
    _end_cell = nullptr;
}

void main_frame::_on_cell_click(QTableWidgetItem* i) {   
    cell* c = (cell*)i;
    
    if (_cell_set_state == cell_state::start) {
        if (_start_cell) _start_cell->state(cell_state::empty);
        _start_cell = c;
        if (c == _end_cell) _end_cell = nullptr;
    }
    else if (_cell_set_state == cell_state::end) {
        if (_end_cell) _end_cell->state(cell_state::empty);
        if (c == _start_cell) _start_cell = nullptr;
        _end_cell = c;
    }
    else {
        if (c == _start_cell) _start_cell = nullptr;
        else if (c == _end_cell) _end_cell = nullptr;
    }

    c->state(toggle_state(c->state(), _cell_set_state));
}

void main_frame::_run_algorithm() {
    static auto choose_router = [](const algorithm alg, grid_graph& g) 
        -> std::unique_ptr<graph::router::router<grid_type>>
    {
        switch (alg) {
            case algorithm::dijkstra:
                return std::make_unique<graph::router::dijkstra<grid_type>>(g);
            default:
                return nullptr;
        }
        return nullptr;
    };

    auto [graph, start, end] = create_graph(_grid);
    const auto router = choose_router(_toolbar->current_algorithm(), graph);
    if (!router) return;

    const auto route = router->calc(start, end);
    log_route(route);
    for (auto n : route) {
    }
}

void main_frame::_register_connections() {
    connect(_toolbar->new_grid_btn(), &QPushButton::released, this, &main_frame::_create_new_grid);
    connect(_toolbar->run_algorithm_btn(), &QPushButton::released, this, &main_frame::_run_algorithm);

    connect(_toolbar->start_position_btn(), &QPushButton::released, this, &main_frame::_on_start_position_btn_click);
    connect(_toolbar->end_position_btn(), &QPushButton::released, this, &main_frame::_on_end_position_btn_click);
    connect(_toolbar->untraversable_node_button(), &QPushButton::released, this, &main_frame::_on_untraversable_node_button_click);
}

void main_frame::_on_start_position_btn_click() {
    toggle_cell_state_buttons(*_toolbar, _cell_set_state, cell_state::start);
}

void main_frame::_on_end_position_btn_click() {
    toggle_cell_state_buttons(*_toolbar, _cell_set_state, cell_state::end);
}

void main_frame::_on_untraversable_node_button_click() {
    toggle_cell_state_buttons(*_toolbar, _cell_set_state, cell_state::untraversable);
}
