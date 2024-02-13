#pragma once

#include <QtWidgets/QWidget>

class QHBoxLayout;
class algorithm_selection;
class icon_button;

class toolbar : public QWidget {
    Q_OBJECT
private:
    QHBoxLayout* _layout;

    algorithm_selection* _algorithm_selection;
    icon_button* _run_algorithm_btn;
    icon_button* _start_position_btn;
    icon_button* _end_position_btn;

public:
    explicit toolbar(QWidget* parent = nullptr);
};
