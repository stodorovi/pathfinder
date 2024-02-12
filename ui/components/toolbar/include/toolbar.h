#pragma once

#include <QtWidgets/QWidget>

class QHBoxLayout;
class algorithm_selection;
class run_algorithm_button;

class toolbar : public QWidget {
    Q_OBJECT
private:
    QHBoxLayout* _layout;

    algorithm_selection* _algorithm_selection;
    run_algorithm_button* _run_algorithm_btn;

public:
    explicit toolbar(QWidget* parent = nullptr);
};