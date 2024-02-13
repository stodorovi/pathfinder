#include "../include/toolbar.h"
#include "../algorithm_selection/include/algorithm_selection.h"
#include "../icon_button/include/icon_button.h"

#include <QtWidgets/QHBoxLayout>

toolbar::toolbar(QWidget *parent) : QWidget(parent),
    _layout(new QHBoxLayout(this)), _algorithm_selection(new algorithm_selection(this)),
    _run_algorithm_btn(new icon_button(this, "run_algorithm_button.svg", "Run the selected algorithm.")),
    _start_position_btn(new icon_button(this, "start_position_button.svg", "Select the starting position node")),
    _end_position_btn(new icon_button(this, "end_position_button.svg", "Select the ending position node")),
    _untraversable_node_button(new icon_button(this, "untraversable_node_button.svg", "The the node as untraversable"))
{
    setLayout(_layout);
    _layout->addWidget(_algorithm_selection);
    _layout->addWidget(_run_algorithm_btn);
    _layout->addStretch();
    _layout->addWidget(_start_position_btn);
    _layout->addWidget(_end_position_btn);
    _layout->addWidget(_untraversable_node_button);
}