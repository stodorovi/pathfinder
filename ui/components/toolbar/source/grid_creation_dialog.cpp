#include "../include/grid_creation_dialog.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDialogButtonBox>

grid_creation_dialog::grid_creation_dialog()
    : QDialog(nullptr), _layout(new QFormLayout(this)), _values(0, 0),
    _height_label(new QLabel("Height:", this)), _height_input(new QSpinBox(_height_label)),
    _width_label(new QLabel("Width:", this)), _width_input(new QSpinBox(_width_label))
{
    setWindowTitle("Create new grid");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    _height_label->setBuddy(_height_input);
    _width_label->setBuddy(_width_label);
    
    _layout->addRow(_height_label, _height_input);
    _layout->addRow(_width_label, _width_input);
    _width_input->setMaximum(180);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    _layout->addRow(buttons);
    
    connect(buttons, &QDialogButtonBox::accepted, this, &grid_creation_dialog::_on_accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &grid_creation_dialog::_on_cancel);
}

grid_size_t grid_creation_dialog::values() const {
    return _values;
}

void grid_creation_dialog::_on_accept() {
    _values =  { _width_input->value(), _height_input->value() };
    close();
}

void grid_creation_dialog::_on_cancel() {
    close();
}