#pragma once

#include <QtWidgets/QMainWindow>

class toolbar;

class main_frame : public QMainWindow {
    Q_OBJECT
    
private:
    toolbar* _toolbar;

    void _create_new_grid();

public:
    main_frame();
};
