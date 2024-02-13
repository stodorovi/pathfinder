#include "../include/icon_button.h"
#include "../../../../include/path_utils.h"

icon_button::icon_button(QWidget *parent, const char *icon_name, const char *tooltip) {
    setToolTip(tooltip);

    QIcon icon(path::icon_path(icon_name).c_str());
    assert(icon.isNull() == false);
    setIcon(QIcon(icon));
}