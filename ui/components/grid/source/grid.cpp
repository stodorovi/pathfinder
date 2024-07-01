#include "../include/grid.h"

#include <QtWidgets/QHeaderView>
    
namespace {
void setup_header(QHeaderView& header, int32_t cell_size) {
    header.setSectionResizeMode(QHeaderView::Fixed);
    header.setDefaultSectionSize(30);
    header.hide();
}

void create_table(grid& g, uint8_t width, uint8_t height) {
    g.setColumnCount(width);
    g.setRowCount(height);
    g.setEditTriggers(QAbstractItemView::NoEditTriggers);
    g.setFocusPolicy(Qt::NoFocus);
    g.setSelectionMode(QAbstractItemView::NoSelection);

    setup_header(*g.verticalHeader(), 30);
    setup_header(*g.horizontalHeader(), 30);

    for (size_t row = 0; row < height; ++row)
        for (size_t column = 0; column < width; ++column) {
            auto* cell = new QTableWidgetItem();
            g.setItem(row, column, cell);
            cell->setBackground(Qt::gray);
        }
}
} // end anonymous namespace

grid::grid(QWidget* parent, size_t width, size_t height) : QTableWidget(parent) {
    create_table(*this, width, height);
}
