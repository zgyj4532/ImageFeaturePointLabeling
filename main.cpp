#include "dedistortion.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dedistortionImageWindow w;
    w.show();
    return a.exec();
}
