#pragma once

#include <QtWidgets/QTableWidget>

class grid : public QTableWidget {
    Q_OBJECT

public:
    grid(QWidget* parent, size_t width, size_t height);
    void wheelEvent(QWheelEvent *event) override;

    using cell_type = int32_t;
};
