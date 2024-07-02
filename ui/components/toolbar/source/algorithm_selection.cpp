#include "../include/algorithm_selection.h"
#include "../../../../include/algorithms.h"

namespace {
const QStringList& available_algorithms() {
    static QStringList rv = {
        QString::fromStdString(to_string(algorithm::dijkstra)),
        QString::fromStdString(to_string(algorithm::a_star)),
        QString::fromStdString(to_string(algorithm::d_star_lite)),
        QString::fromStdString(to_string(algorithm::hpa_star))
    };
    return rv;
}
} // end anonzmous namespace 

algorithm_selection::algorithm_selection(QWidget *parent) : QComboBox(parent)
{
    addItems(available_algorithms());
    setToolTip("Select an algorithm to run.");
}