#include "../include/pathfinder_app.h"

pathfinder_app::pathfinder_app(int &argc, char **argv) 
    : _app(argc, argv), _main_frame()
{}

void pathfinder_app::run() {
    _main_frame.show();
    _app.exec();
}

int main(int argc, char** argv) {
    pathfinder_app app(argc, argv);
    app.run();
}