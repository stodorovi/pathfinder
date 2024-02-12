#pragma once

#include <QtWidgets/QWidget>

class QHBoxLayout;
class algorithm_selection;

class toolbar : public QWidget {
    Q_OBJECT
private:
    QHBoxLayout* _layout;

    algorithm_selection* _algorithm_selection;

public:
    explicit toolbar(QWidget* parent = nullptr);
};