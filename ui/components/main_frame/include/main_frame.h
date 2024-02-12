#pragma once

#include <QtWidgets/QWidget>

class QVBoxLayout;
class toolbar;

class main_frame : public QWidget {
    Q_OBJECT
    
private:
    QVBoxLayout* _main_layout;
    toolbar* _toolbar;

public:
    main_frame();
};
