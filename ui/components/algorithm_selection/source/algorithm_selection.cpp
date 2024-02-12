#include "../include/algorithm_selection.h"

algorithm_selection::algorithm_selection(QWidget *parent) : QComboBox(parent)
{
    addItems({
        "Dijkstra",
        "A*"
    });
}