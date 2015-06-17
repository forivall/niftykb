#include "ShortcutDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ShortcutDialog w;
    w.show();

    return a.exec();
}
