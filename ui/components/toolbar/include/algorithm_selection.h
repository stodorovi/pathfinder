#pragma once

#include <QtWidgets/QComboBox>

class algorithm_selection : public QComboBox {
    Q_OBJECT

public:
    algorithm_selection(QWidget* parent = nullptr);
};
