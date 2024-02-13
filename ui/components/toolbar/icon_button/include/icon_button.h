#pragma once

#include <QtWidgets/QPushButton>

class icon_button : public QPushButton {
    Q_OBJECT

public:
    icon_button(QWidget* parent = nullptr, const char* icon_name = nullptr, const char* tooltip = nullptr);
};