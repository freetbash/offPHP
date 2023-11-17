#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    opApp opapp;
    opapp.show();
    return app.exec();
}
