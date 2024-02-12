#include "../include/toolbar.h"
#include "../algorithm_selection/include/algorithm_selection.h"
#include "../run_algorithm_button/include/run_algorithm_button.h"

#include <QtWidgets/QHBoxLayout>

toolbar::toolbar(QWidget *parent) : QWidget(parent),
    _layout(new QHBoxLayout(this)), _algorithm_selection(new algorithm_selection(this)),
    _run_algorithm_btn(new run_algorithm_button(this))
{
    setLayout(_layout);
    _layout->addWidget(_algorithm_selection);
    _layout->addWidget(_run_algorithm_btn);
    _layout->addStretch();
}