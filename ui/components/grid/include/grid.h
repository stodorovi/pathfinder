#pragma once

#include <QtWidgets/QTableWidget>

class grid : public QTableWidget {
    Q_OBJECT

public:
    grid(QWidget* parent = nullptr);
};
