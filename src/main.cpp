#include "Global.h"
#include "MainWindow.h"
#include <QtSingleApplication>
#include <QCommandLineParser>
#include <QTranslator>

int main(int argc, char *argv[])
{
  QtSingleApplication app(argc, argv);
  QApplication::setApplicationName("niftykb");
  QApplication::setApplicationVersion("0.1");

  // process command line arguments
  QCommandLineParser parser;
  parser.addVersionOption();
  parser.addHelpOption();
  parser.addOptions({
    {"hide", QCommandLineParser::tr("Hide window on start. Overrides any settings")},
    {"show", QCommandLineParser::tr("Show window on start. Overrides any settings")},
  });
  parser.process(app);

  // Only run single instance
  if (app.isRunning()) {
    // TODO: hide/show the running instance
    // TODO: allow multiple with command line arguments
    return !app.sendMessage("show");
  }

  // Init global struct
  Global::g_global_struct = new Global();
  GlobalShortcutEngine::engine = GlobalShortcutEngine::platformInit();
  g.s.load();
  MainWindow w(0, &parser);
  g.mw = &w;
  app.setActivationWindow(&w);

  if (!parser.isSet("hide")) {
    w.show();
  }

  return app.exec();
}
