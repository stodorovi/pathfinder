#pragma once

class QWidget;
class QVBoxLayout;

class main_frame {
private:
    QWidget* _main_window;
    QVBoxLayout* _main_layout;

public:
    main_frame();
    void show();
};
