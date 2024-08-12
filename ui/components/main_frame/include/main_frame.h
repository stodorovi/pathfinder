#pragma once

#include <QtWidgets/QMainWindow>
#include "../../grid/include/grid.h"
#include "../../grid/include/cell.h"
#include "../../../routing/routers/include/router.h"

class toolbar;

class main_frame : public QMainWindow {
    Q_OBJECT
    
public:
using grid_type = grid::cell_type;

private:
    toolbar* _toolbar;
    grid* _grid;

    cell_state _cell_set_state;
    cell* _start_cell;
    cell* _end_cell;

    std::vector<std::vector<cell_state>> _grid_states;
    void _set_grid_states();
    void _restore_grid_states();

    void _create_new_grid();
    void _on_cell_click(QTableWidgetItem*);

    void _run_algorithm();
    void _visualise_algorithm(const graph::route<grid_type>& r);

    void _register_connections();

    void _on_start_position_btn_click();
    void _on_end_position_btn_click();
    void _on_untraversable_node_button_click();

public:
    main_frame();
};
