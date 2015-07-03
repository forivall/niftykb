/* (c) 2015 Jordan Klassen, GPLv3 or later license */
#include "common.h"

#include "MainWindow.h"

#include "Global.h"
#include "ShortcutEditor.h"

MainWindow::MainWindow(QWidget *parent, QCommandLineParser *args) :
  QMainWindow(parent)
{
  uiNewHardware = 1; // needs to be set to 1 so that an initial scan takes place

  createActions();
  setupTrayIcon();
  setupGui();

  QMetaObject::connectSlotsByName(this);

  setCentralWidget(new ShortcutEditor(this));
}

void MainWindow::setupTrayIcon() {
  qiTrayIcon.addFile(":/assets/monkey_face_16x16.png");
  qstiIcon = new QSystemTrayIcon(qiTrayIcon, this);
  qstiIcon->setToolTip(tr("NiftyKb"));
  qstiIcon->setObjectName(QLatin1String("Icon"));

  connect(qstiIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
    if (reason != QSystemTrayIcon::Trigger && reason != QSystemTrayIcon::DoubleClick)
      return;

    toggleWindow();
  });

#ifndef Q_OS_MAC
  qstiIcon->show();
#endif
}

void MainWindow::toggleWindow() {
  if (!isVisible()) {
    show();
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
    activateWindow();
  } else {
    if (qstiIcon) {
      hide();
    }
  }
}

void MainWindow::setupGui()  {
  setWindowTitle(tr("NiftyKb"));// -- %1").arg(QLatin1String(MUMBLE_RELEASE)));

  // if (! g.s.qbaMainWindowGeometry.isNull())
  //   restoreGeometry(g.s.qbaMainWindowGeometry);

  qmTray = new QMenu(this);
  qmTray->clear();
  // qmTray->addAction(qaAudioMute);
  // qmTray->addAction(qaAudioDeaf);
  qmTray->addSeparator();
  // qmTray->addAction(qaQuit);
  qstiIcon->setContextMenu(qmTray);

  updateTrayIcon();


  qiWindowIcon.addFile(":/assets/monkey_face_64x64.png");
  setWindowIcon(qiWindowIcon);
  resize(600, 450);
}

void MainWindow::updateTrayIcon() {
  if (false && g.s.bStateInTray) {
    // switch (p->tsState) {
    // case Settings::Talking:
    //   qstiIcon->setIcon(qiTalkingOn);
    //   break;
    // case Settings::Whispering:
    //   qstiIcon->setIcon(qiTalkingWhisper);
    //   break;
    // case Settings::Shouting:
    //   qstiIcon->setIcon(qiTalkingShout);
    //   break;
    // case Settings::Passive:
    // default:
    //   qstiIcon->setIcon(qiTalkingOff);
    //   break;
    // }
  } else {
    qstiIcon->setIcon(qiTrayIcon);
  }
}

bool MainWindow::nativeEvent(const QByteArray &, void *message, long *) {
  MSG *msg = reinterpret_cast<MSG *>(message);
  if (msg->message == WM_DEVICECHANGE && msg->wParam == DBT_DEVNODES_CHANGED) {
    uiNewHardware++;
  }

  return false;
}
