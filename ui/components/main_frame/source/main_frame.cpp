#include "../include/main_frame.h"
#include "../../toolbar/include/toolbar.h"
#include "../../toolbar/include/icon_button.h"
#include "../../toolbar/include/grid_creation_dialog.h"
#include "../../grid/include/grid.h"

#include "../../../../routing/components/include/graph.h"
#include "../../../../routing/components/include/edge_helper.h"
#include "../../../../routing/routers/include/dijkstra.h"
#include "../../../../routing/routers/include/a_star.h"
#include "include/algorithms.h"
#include "ui/components/grid/include/cell.h"

#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QMessageBox>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <utility>
#include <vector>

using grid_type = main_frame::grid_type;
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
    grid_point start { -1, -1 };
    grid_point end { -1, -1 };

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
    return grid_graph(origin_node);
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

std::unique_ptr<graph::router::router<grid_type>> choose_router(const algorithm alg, grid_graph& g) {
    switch (alg) {
        case algorithm::dijkstra:
            return std::make_unique<graph::router::dijkstra<grid_type>>(g);
        case algorithm::a_star:
            return std::make_unique<graph::router::a_star<grid_type>>(g);
        default:
            return nullptr;
    }
    return nullptr;
};

void show_invalid_start_end_message(const bool start_invalid, const bool end_invalid) {
    const auto error_message = [start_invalid, end_invalid]() {
        if (start_invalid && !end_invalid) return QString("Start node not selected!");
        else if (start_invalid && end_invalid) return QString("Neither start nor end node selected!");
        else return QString("End node not selected!");
    }();
    QMessageBox msgbox(QMessageBox::Icon::Critical, "Invalid endpoint", "ERROR: " + error_message);
    msgbox.exec();
}

void show_route_not_found_message() {
    QMessageBox msgbox(QMessageBox::Icon::Critical, "Route not found!", "ERROR: Route was not found.");
    msgbox.exec();
}

void show_empty_grid_error_message() {
    QMessageBox msgbox(QMessageBox::Icon::Critical, "Emptz grid!", "ERROR: The grid is empty. Create a grid to route.");
    msgbox.exec();
}

template<typename Rep, typename Period>
void visualise_visitation_order(
    const graph::route<grid_type>::route_visitation_order& to, grid& g,
    std::chrono::duration<Rep, Period> paint_duration
) {
    const size_t visitation_count = to.size();
    auto to_colour = [visitation_count](const size_t cell_n) {
        uint8_t max_color = 0xff;
        const size_t cells_left = visitation_count - cell_n;
        double visitation_count_d = double(visitation_count);

        int32_t blue = (cells_left / visitation_count_d) * max_color;
        int32_t green = max_color - blue;
        QColor c(0, green, blue);
        
        QBrush brush;
        brush.setColor(c);
        brush.setStyle(Qt::SolidPattern);
        return brush;
    };

    const auto cell_paint_duration = std::chrono::duration_cast<std::chrono::milliseconds>(paint_duration) / visitation_count;
    for (size_t i = 0; i < visitation_count; ++i) {
        auto& node = *to[i];
        const auto pt = node.pos();
        if (auto* cl = (cell*)g.item(pt.y, pt.x); cl) {
            const auto colour = to_colour(i);
            cl->setBackground(colour);
            QThread::sleep(cell_paint_duration);
        }
    }
}

template<typename Rep, typename Period>
void visualise_route(const graph::route<grid_type>& r, grid& g, std::chrono::duration<Rep, Period> paint_duration) {
    auto to_colour = [route_length = r.length](const size_t cell_n) {
        uint8_t max_color = 0xff;
        QBrush brush;
        double red_hardness = (double)cell_n / route_length;
        uint8_t red = max_color * red_hardness;
        brush.setColor(QColor ( red, 0, 0 ));
        brush.setStyle(Qt::SolidPattern);
        return brush;
    };

    const auto cell_paint_duration = std::chrono::duration_cast<std::chrono::milliseconds>(paint_duration) / r.length;
    size_t n = 0;
    for (auto pt : r) {
        if (auto* cl = (cell*)g.item(pt.y, pt.x); cl) {
            cl->setBackground(to_colour(n));
            QThread::sleep(cell_paint_duration);
        }
        ++n;
    }
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

void main_frame::_set_grid_states() {
    _grid_states.clear();

    const size_t rows = _grid->rowCount();
    const size_t columns = _grid->columnCount();

    for (size_t r = 0; r < rows; ++r) {
        _grid_states.emplace_back(std::vector<cell_state>(columns));
        for (size_t c = 0; c < columns; ++c) {
            _grid_states[r][c] = static_cast<cell*>(_grid->item(r, c))->state();
        }
    }
}

void main_frame::_restore_grid_states() {
    if (_grid_states.empty())
        return;

    const size_t rows = _grid->rowCount();
    const size_t columns = _grid->columnCount();

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < columns; ++c) {
            cell* cl = (cell*)_grid->item(r, c);
            cl->state(_grid_states[r][c], true);
        }
    }

    _grid_states.clear();
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
    _grid_states.clear();
}

void main_frame::_on_cell_click(QTableWidgetItem* i) {  
    if (!_grid_states.empty())
        _restore_grid_states();

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
    if (!_grid) {
        show_empty_grid_error_message();
        return;
    }

    static const grid_point invalid_point { -1, -1 };

    auto [graph, start, end] = create_graph(_grid);
    
    const bool invalid_start = start == invalid_point;
    const bool invalid_end = end == invalid_point;
    if (invalid_start || invalid_end) {
        show_invalid_start_end_message(invalid_start, invalid_end);
        return;
    }

    if (const auto router = choose_router(_toolbar->current_algorithm(), graph); router) {
        const auto route = router->calc(start, end);

#if !NDEBUG
        log_route(route);
#endif

    if (!route) {
        show_route_not_found_message();
        return;
    }

    _toolbar->setEnabled(false);
    _grid->setEnabled(false);

    if (!_grid_states.empty())
        _restore_grid_states();
    
    auto _ = QtConcurrent::run([route = std::move(route), this](){
        this->_set_grid_states();
        this->_visualise_algorithm(route);
    });
}
}

void main_frame::_visualise_algorithm(const graph::route<grid_type>& r) {
    visualise_visitation_order(r.visitation_order, *_grid, std::chrono::seconds(5));
    QThread::sleep(std::chrono::seconds(1));
    visualise_route(r, *_grid, std::chrono::seconds(1));
        
    _toolbar->setEnabled(true);
    _grid->setEnabled(true);
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
