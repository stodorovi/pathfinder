#include "../include/grid.h"

#include <QtWidgets/QHeaderView>
    
namespace {
void setup_header(QHeaderView& header, int32_t cell_size) {
    header.setSectionResizeMode(QHeaderView::Fixed);
    header.setDefaultSectionSize(30);
    header.hide();
}

void create_table(grid& g) {
    g.setColumnCount(40);
    g.setRowCount(25);
    g.setEditTriggers(QAbstractItemView::NoEditTriggers);
    g.setFocusPolicy(Qt::NoFocus);
    g.setSelectionMode(QAbstractItemView::NoSelection);

    setup_header(*g.verticalHeader(), 30);
    setup_header(*g.horizontalHeader(), 30);
}
} // end anonymous namespace

grid::grid(QWidget* parent) : QTableWidget(parent) {
    create_table(*this);
}
