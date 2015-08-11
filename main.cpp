#include "../AAMlib/cvimagewidget.h"
#include "aameditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AAMed w;
    w.show();

    return a.exec();
}
