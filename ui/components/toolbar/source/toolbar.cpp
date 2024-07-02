#include "../include/toolbar.h"
#include "../include/algorithm_selection.h"
#include "../include/icon_button.h"

#include <QtWidgets/QHBoxLayout>

namespace {

QWidget* create_stretcher(QWidget* parent) {
    auto* seperator = new QWidget(parent);
    seperator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return seperator;
}

} // end anonymous namespace

toolbar::toolbar(QWidget *parent) : QToolBar(parent),
    _new_grid_btn(new icon_button(this, "create_new_grid_button.svg", "Create new grid")),
    _algorithm_selection(new algorithm_selection(this)),
    _run_algorithm_btn(new icon_button(this, "run_algorithm_button.svg", "Run the selected algorithm.")),
    _start_position_btn(new icon_button(this, "start_position_button.svg", "Select the starting position node")),
    _end_position_btn(new icon_button(this, "end_position_button.svg", "Select the ending position node")),
    _untraversable_node_button(new icon_button(this, "untraversable_node_button.svg", "The the node as untraversable"))
{
    addWidget(_new_grid_btn);
    addWidget(_algorithm_selection);
    addWidget(_run_algorithm_btn);
    addWidget(create_stretcher(this));
    addWidget(_start_position_btn);
    addWidget(_end_position_btn);
    addWidget(_untraversable_node_button);
    setMovable(false);
}

icon_button* toolbar::new_grid_btn() const {
    return _new_grid_btn;
}

icon_button *toolbar::run_algorithm_btn() const {
    return _run_algorithm_btn;
}

algorithm toolbar::current_algorithm() const {
    return static_cast<algorithm>(_algorithm_selection->currentIndex());
}
