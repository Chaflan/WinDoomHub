#include "doomhub.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DoomHub w;
    w.show();
    return a.exec();
}
