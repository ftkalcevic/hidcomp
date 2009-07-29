#include "stdafx.h"
#include "hidconfig.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("hidconfig");
    hidconfig w;
    w.show();
    w.ProcessCommandline();
    return a.exec();
}


