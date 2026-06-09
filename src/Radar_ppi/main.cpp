#include "MainWindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char** argv)
{
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    MainWindow window;
    window.resize(1280, 900);
    window.show();

    return app.exec();
}
