#include "Global.h"
#include "ShortcutDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  // init global
  Global::g_global_struct = new Global();
  GlobalShortcutEngine::engine = GlobalShortcutEngine::platformInit();
  // g.s = new Settings();
  QApplication a(argc, argv);
  ShortcutDialog w;
  g.mw = &w;
  w.show();

  return a.exec();
}
