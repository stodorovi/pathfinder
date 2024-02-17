#include "../include/main_frame.h"
#include "../../toolbar/include/toolbar.h"
#include "../../grid/include/grid.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

namespace {
auto desktop_centre(const int32_t w, int32_t h) {
    const auto screen = QGuiApplication::primaryScreen()->geometry();

    const auto x = (screen.width() / 2) - (w / 2);
    const auto y = (screen.height() / 2) - (h / 2);

    QRect rv;
    rv.setRect(x, y, w, h);
    return rv;
}
} // end anonymous namespace

main_frame::main_frame() : QMainWindow(),
    _toolbar(new toolbar())
{
    this->addToolBar(_toolbar);
    setGeometry(desktop_centre(640, 480));
    setWindowTitle("Pathfinding Visualiser");
    setCentralWidget(new grid(this));
}
