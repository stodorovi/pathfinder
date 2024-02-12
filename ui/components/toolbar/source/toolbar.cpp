#include "../include/toolbar.h"
#include "../../algorithm_selection/include/algorithm_selection.h"

#include <QtWidgets/QHBoxLayout>

#include <QtWidgets/qpushbutton.h>

toolbar::toolbar(QWidget *parent) : QWidget(parent),
    _layout(new QHBoxLayout(this)), _algorithm_selection(new algorithm_selection(this))
{
    setLayout(_layout);
    _layout->addWidget(_algorithm_selection);
}