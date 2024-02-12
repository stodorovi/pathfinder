#include "../include/run_algorithm_button.h"

run_algorithm_button::run_algorithm_button(QWidget *parent) : QPushButton(parent)
{
    setToolTip("Run the selected algorithm.");
    QIcon icon("play_button.svg");
    assert(icon.isNull() == false);
    setIcon(QIcon(icon));
}