#pragma once

#include "../../../../include/algorithms.h"
#include <QtWidgets/QToolBar>

class QHBoxLayout;
class algorithm_selection;
class icon_button;

class toolbar : public QToolBar {
    Q_OBJECT

private:
    icon_button* _new_grid_btn;
    algorithm_selection* _algorithm_selection;
    icon_button* _run_algorithm_btn;

    icon_button* _start_position_btn;
    icon_button* _end_position_btn;
    icon_button* _untraversable_node_button;

public:
    explicit toolbar(QWidget* parent = nullptr);

    icon_button* new_grid_btn() const;
    icon_button* run_algorithm_btn() const; 
    algorithm current_algorithm() const; 
    icon_button* start_position_btn() const;
    icon_button* end_position_btn() const;
    icon_button* untraversable_node_button() const;
};
