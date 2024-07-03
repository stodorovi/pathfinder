#include "../include/main_frame.h"
#include "../../toolbar/include/toolbar.h"
#include "../../toolbar/include/icon_button.h"
#include "../../toolbar/include/grid_creation_dialog.h"
#include "../../grid/include/grid.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

namespace {
auto desktop_centre(const int32_t w, int32_t h) {
    const auto screen = QGuiApplication::primaryScreen()->geometry();

    const auto x = (screen.width() / 2) - (w / 2);
    const auto y = (screen.height() / 2) - (h / 2);

    QRect rv;
    rv.setRect(x, y, w, h);
    return rv;
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
