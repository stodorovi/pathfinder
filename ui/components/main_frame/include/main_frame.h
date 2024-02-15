#pragma once

#include <QtWidgets/QMainWindow>

class toolbar;

class main_frame : public QMainWindow {
    Q_OBJECT
    
private:
    toolbar* _toolbar;

public:
    main_frame();
};
