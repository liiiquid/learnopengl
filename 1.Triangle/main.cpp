#include "GLWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GLWindow w;
    w.resize(512, 512);
    w.setAnimating(true);
    w.show();
    return a.exec();
}
