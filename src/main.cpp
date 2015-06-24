#include "Global.h"
#include "ShortcutDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  // init global
  Global::g_global_struct = new Global();
  GlobalShortcutEngine::engine = GlobalShortcutEngine::platformInit();
  g.s.load();
  ShortcutDialog w;
  g.mw = &w;
  w.show();

  return a.exec();
}
