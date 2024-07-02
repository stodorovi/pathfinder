#include "../include/grid.h"
#include "../include/cell.h"

#include <QtWidgets/QHeaderView>
#include <QWheelEvent>
    
namespace {
auto cell_limits() {
    static const int32_t min_size = 10;
    static const int32_t max_size = 100;
    return std::make_pair(min_size, max_size);
}

int32_t checked_cell_size(int32_t cs) {
    static const auto [ min_size, max_size ] = cell_limits();
    return std::clamp(cs, min_size, max_size);
}

void zoom_grid(QHeaderView& h, QHeaderView& v, int32_t direction) {
    const auto current_size = h.sectionSize(0);

    const auto columns = h.model()->columnCount();
    const auto rows = v.model()->rowCount();

    for (size_t i = 0; i < columns; ++i)
        h.resizeSection(i, checked_cell_size(h.sectionSize(i) + direction));

    for (size_t i = 0; i < rows; ++i)
        v.resizeSection(i, checked_cell_size(v.sectionSize(i) + direction));
}

void setup_header(QHeaderView& header, int32_t cell_size) {
    static const auto [ min_size, max_size ] = cell_limits();
    const auto size = checked_cell_size(cell_size);

    header.setSectionResizeMode(QHeaderView::Fixed);
    header.setDefaultSectionSize(size);
    header.setMinimumSectionSize(min_size);
    header.setMaximumSectionSize(max_size);
    header.hide();
}

void setup_headers(QHeaderView& h, QHeaderView& v, int32_t cell_size) {
    setup_header(h, cell_size);
    setup_header(v, cell_size);
}

void create_table(grid& g, uint8_t width, uint8_t height) {
    g.setColumnCount(width);
    g.setRowCount(height);
    g.setEditTriggers(QAbstractItemView::NoEditTriggers);
    g.setFocusPolicy(Qt::NoFocus);
    g.setSelectionMode(QAbstractItemView::NoSelection);

    setup_headers(*g.verticalHeader(), *g.horizontalHeader(), 30);

    for (size_t row = 0; row < height; ++row)
        for (size_t column = 0; column < width; ++column) {
            auto* c = new cell();
            c->state(cell_state::empty);
            g.setItem(row, column, c);
        }
}
} // end anonymous namespace

grid::grid(QWidget* parent, size_t width, size_t height) : QTableWidget(parent) {
    create_table(*this, width, height);
}

void grid::wheelEvent(QWheelEvent *event) {
    const int y = event->angleDelta().y();
    int direction = -y / std::abs(y);
    zoom_grid(*this->horizontalHeader(), *this->verticalHeader(), direction);
}