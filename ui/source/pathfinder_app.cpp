#include "../include/pathfinder_app.h"
#include "../include/path_utils.h"

#include <string_view>

namespace {
std::string_view folder_dir(const char* app_dir) {
    std::string_view sv(app_dir);
    const auto slash = sv.find_last_of('/');
    if (slash == sv.npos)
        return sv;
    return std::string_view(app_dir, slash);
} 
} // end anonymous namespace

pathfinder_app::pathfinder_app(int &argc, char **argv) 
    : _app(argc, argv), _main_frame()
{}

void pathfinder_app::run() {
    _main_frame.show();
    _app.exec();
}

int main(int argc, char** argv) {
    path::app_dir() = folder_dir(argv[0]);
    pathfinder_app app(argc, argv);
    app.run();
}