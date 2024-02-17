#pragma once

#include "../ui/components/main_frame/include/main_frame.h"
#include <QtWidgets/QApplication>

class pathfinder_app {
private:
    QApplication _app;
    main_frame _main_frame;

public:
    pathfinder_app(int& argc, char** argv);
    void run();
};
