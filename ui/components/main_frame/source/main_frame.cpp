#include "../include/main_frame.h"
#include "../../toolbar/include/toolbar.h"
#include "../../toolbar/include/icon_button.h"
#include "../../toolbar/include/grid_creation_dialog.h"
#include "../../grid/include/grid.h"

#include "../../../../routing/components/include/graph.h"
#include "../../../../routing/components/include/edge_helper.h"
#include "../../../../routing/routers/include/dijkstra.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <queue>

using grid_point = graph::types::point<int32_t>;
using grid_node = graph::components::node<int32_t>;
using grid_node_ptr = graph::node_ptr<int32_t>;
using grid_graph = graph::graph<int32_t>;

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

std::vector<std::vector<grid_node_ptr>> create_nodes(grid const * const g) {
    const int32_t rows = g->rowCount();
    std::vector<std::vector<grid_node_ptr>> rv(rows);

    const int32_t columns = g->columnCount();
    for (int32_t r = 0; r < rows; ++r) {
        rv[r] = std::vector<grid_node_ptr>(columns);

        for (int32_t c = 0; c < columns; ++c) {
            if (const auto* cl = (cell*)g->item(r, c); cl && cl->state() != cell_state::untraversable)
                rv[r][c] = std::make_unique<grid_node>(grid_point{ c, r });
        }
    }

    return rv;
}

grid_graph connect_graph(grid const * const g, std::vector<std::vector<grid_node_ptr>> nodes) {
    grid_graph rv(nullptr);

    grid_node_ptr origin_node = nullptr;

    for (auto& row : nodes) {
        for (auto& node : row) {
            if (!node) continue;
            if (!origin_node) origin_node = node;

            auto p = node->pos();
            for (auto sn : get_surrounding_cells(g, p.y, p.x)) {
                grid_point pt { sn->column(), sn->row() };
                node->insert_edge(edge_helper::create_edge(1, nodes[sn->row()][sn->column()]));
            }
        }
    }

    rv.reset(origin_node);
    return rv;
}

grid_graph create_graph(grid const * const g) {
    return connect_graph(g, create_nodes(g));
}

} // end anonymous namespace

main_frame::main_frame() : QMainWindow(),
    _toolbar(new toolbar()), _grid(nullptr), _cell_set_state(cell_state::empty)
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
}

void main_frame::_on_cell_click(QTableWidgetItem* i) {
    static cell* start_cell = nullptr;
    static cell* end_cell = nullptr;

    cell* c = (cell*)i;
    if (_cell_set_state == cell_state::start) {
        if (start_cell && start_cell != c) start_cell->state(cell_state::empty);
        start_cell = c;
    }

    if (_cell_set_state == cell_state::end) {
        if (end_cell && end_cell != c) end_cell->state(cell_state::empty);
        end_cell = c;
    }

    c->state(_cell_set_state);
}

void main_frame::_run_algorithm() {
    grid_graph graph = create_graph(_grid);
    graph::router::dijkstra dr(graph);
}

void main_frame::_register_connections() {
    connect(_toolbar->new_grid_btn(), &QPushButton::released, this, &main_frame::_create_new_grid);
    connect(_toolbar->run_algorithm_btn(), &QPushButton::released, this, &main_frame::_run_algorithm);

    connect(_toolbar->start_position_btn(), &QPushButton::released, this, &main_frame::_on_start_position_btn_click);
    connect(_toolbar->end_position_btn(), &QPushButton::released, this, &main_frame::_on_end_position_btn_click);
    connect(_toolbar->untraversable_node_button(), &QPushButton::released, this, &main_frame::_on_untraversable_node_button_click);
}

void main_frame::_on_start_position_btn_click() {
    _cell_set_state = toggle_state(_cell_set_state, cell_state::start);
}

void main_frame::_on_end_position_btn_click() {
    _cell_set_state = toggle_state(_cell_set_state, cell_state::end);
}

void main_frame::_on_untraversable_node_button_click() {
    _cell_set_state = toggle_state(_cell_set_state, cell_state::untraversable);
}
