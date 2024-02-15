#pragma once

#include <QtWidgets/QToolBar>

class QHBoxLayout;
class algorithm_selection;
class icon_button;

class toolbar : public QToolBar {
    Q_OBJECT

private:
    algorithm_selection* _algorithm_selection;
    icon_button* _run_algorithm_btn;
    icon_button* _start_position_btn;
    icon_button* _end_position_btn;
    icon_button* _untraversable_node_button;

public:
    explicit toolbar(QWidget* parent = nullptr);
};
