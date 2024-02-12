#include "../include/run_algorithm_button.h"
#include "../../../../include/path_utils.h"

#include <format>

run_algorithm_button::run_algorithm_button(QWidget *parent) : QPushButton(parent)
{
    setToolTip("Run the selected algorithm.");

    QIcon icon(path::icon_path("run_algorithm_button.svg").c_str());
    assert(icon.isNull() == false);
    setIcon(QIcon(icon));
}