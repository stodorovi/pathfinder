#pragma once

#include <QtWidgets/QMainWindow>
#include "../../grid/include/grid.h"
#include "../../grid/include/cell.h"

class toolbar;

class main_frame : public QMainWindow {
    Q_OBJECT
    
private:
    toolbar* _toolbar;
    grid* _grid;

    cell_state _cell_set_state;
    cell* _start_cell;
    cell* _end_cell;

    void _create_new_grid();
    void _on_cell_click(QTableWidgetItem*);

    void _run_algorithm();

    void _register_connections();

    void _on_start_position_btn_click();
    void _on_end_position_btn_click();
    void _on_untraversable_node_button_click();

public:
    main_frame();
};
