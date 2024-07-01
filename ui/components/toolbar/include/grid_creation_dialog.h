#pragma once

#include <QtWidgets/QDialog>

class QFormLayout;
class QSpinBox;
class QLabel;

using grid_size_t = std::pair<int, int>;

class grid_creation_dialog : public QDialog {
    Q_OBJECT

private:
    QFormLayout* _layout;
    grid_size_t _values;

    QLabel* _width_label;
    QSpinBox* _width_input;
    
    QLabel* _height_label;
    QSpinBox* _height_input;

    void _on_accept();
    void _on_cancel();

public:
    grid_creation_dialog();
    grid_size_t values() const;
};
