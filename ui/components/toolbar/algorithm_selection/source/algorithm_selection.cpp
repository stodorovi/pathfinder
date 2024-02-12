#include "../include/algorithm_selection.h"

algorithm_selection::algorithm_selection(QWidget *parent) : QComboBox(parent)
{
    addItems({
        "Dijkstra",
        "A*",
        "D* Lite",
        "HPA*"
    });
    setToolTip("Select an algorithm to run.");
}