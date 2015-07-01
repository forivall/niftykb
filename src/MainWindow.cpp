#include "common.h"

#include "MainWindow.h"
#include "ShortcutEditor.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  uiNewHardware = 1; // needs to be set to 1 so that an initial scan takes place

  createActions();
  setupTrayIcon();
  setupGui();

  QMetaObject::connectSlotsByName(this);

  setCentralWidget(new ShortcutEditor(this));
}

void MainWindow::createActions() {
  int idx = 1;
  // broadcast mailslot message
  gsPushTalk = new GlobalShortcut(this, idx++, tr("Push-to-Talk", "Global Shortcut"), false);
  gsPushTalk->setObjectName(QLatin1String("PushToTalk"));
  // gsPushTalk->qsToolTip = tr("Push and hold this button to send voice.", "Global Shortcut");
  // gsPushTalk->qsWhatsThis = tr("This configures the push-to-talk button, and as long as you hold this button down, you will transmit voice.", "Global Shortcut");

  // run exe
  gsPushMute = new GlobalShortcut(this, idx++, tr("Push-to-Mute", "Global Shortcut"));
  gsPushMute->setObjectName(QLatin1String("PushToMute"));

  // fake keystroke
  gsMuteSelf = new GlobalShortcut(this, idx++, tr("Mute Self", "Global Shortcut"), false, 0);
  gsMuteSelf->setObjectName(QLatin1String("gsMuteSelf"));

  // send debug message
  gsDeafSelf = new GlobalShortcut(this, idx++, tr("Deafen Self", "Global Shortcut"), false, 0);
  gsDeafSelf->setObjectName(QLatin1String("gsDeafSelf"));
}

void MainWindow::setupTrayIcon() {
  qiIcon.addFile(":/assets/monkey_face_16x16.png");
  qstiIcon = new QSystemTrayIcon(qiIcon, this);
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
    qstiIcon->setIcon(qiIcon);
  }
}

bool MainWindow::nativeEvent(const QByteArray &, void *message, long *) {
  MSG *msg = reinterpret_cast<MSG *>(message);
  if (msg->message == WM_DEVICECHANGE && msg->wParam == DBT_DEVNODES_CHANGED) {
    uiNewHardware++;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
//                                 HANDLERS                                  //
///////////////////////////////////////////////////////////////////////////////

void MainWindow::on_PushToTalk_triggered(bool down, QVariant) {
  qDebug("Push to talk triggered!");
  // g.iPrevTarget = 0;
  // if (down) {
  //   g.uiDoublePush = g.tDoublePush.restart();
  //   g.iPushToTalk++;
  // } else if (g.iPushToTalk > 0) {
  //   QTimer::singleShot(g.s.uiPTTHold, this, SLOT(pttReleased()));
  // }
}

// void MainWindow::pttReleased() {
//   if (g.iPushToTalk > 0) {
//     g.iPushToTalk--;
//   }
// }

void MainWindow::on_PushToMute_triggered(bool down, QVariant) {
  qDebug("Push to mute triggered!");
}


void MainWindow::on_gsMuteSelf_down(QVariant v) {
  qDebug("Mute self down!");
}

void MainWindow::on_gsDeafSelf_down(QVariant v) {
  qDebug("Deaf self down!");
}
