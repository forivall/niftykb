/* (c) 2015 Jordan Klassen, GPLv3 or later license */
#include "common.h"

#include "MainWindow.h"

#include "Global.h"
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
  gsMailslotMessage = new GlobalShortcut(this, idx++, tr("Send Mailslot Message", "Global Shortcut"), "", "");
  gsMailslotMessage->setObjectName(QLatin1String("gsMailslotMessage"));
  // gsMailslotMessage->qsToolTip = tr("Push and hold this button to send voice.", "Global Shortcut");
  // gsMailslotMessage->qsWhatsThis = tr("This configures the push-to-talk button, and as long as you hold this button down, you will transmit voice.", "Global Shortcut");

  idx++;
  // // run exe
  // gsRunProgram = new GlobalShortcut(this, idx++, tr("Run Program", "Global Shortcut"));
  // gsRunProgram->setObjectName(QLatin1String("gsRunProgram"));

  idx++;
  // // fake keystroke
  // gsKeyMapping = new GlobalShortcut(this, idx++, tr("Map to other Key", "Global Shortcut"), 0);
  // gsKeyMapping->setObjectName(QLatin1String("gsKeyMapping"));

  // send debug message
  gsDebugMessage = new GlobalShortcut(this, idx++, tr("Send Debug Message", "Global Shortcut"), "", "");
  gsDebugMessage->setObjectName(QLatin1String("gsDebugMessage"));
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

///////////////////////////////////////////////////////////////////////////////
//                                 HANDLERS                                  //
///////////////////////////////////////////////////////////////////////////////

#define MAILSLOT_PATH "\\\\.\\mailslot\\niftykb"

void MainWindow::on_gsMailslotMessage_triggered(bool down, QVariant press, QVariant release) {
  qDebug("Mailslot Message triggered! %d", down);
  // TODO: cache and share mailslot handle
  HANDLE slotHandle;
  QString qsMessage;
  QByteArray qbaMessage;
  LPTSTR message;
  DWORD writtenBytes;

  slotHandle = CreateFile(MAILSLOT_PATH,
    GENERIC_WRITE,
    FILE_SHARE_READ,
    (LPSECURITY_ATTRIBUTES) NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    (HANDLE) NULL);

  if (slotHandle == INVALID_HANDLE_VALUE) {
    auto err = GetLastError();
    qWarning("Couldn't connect to mailslot! %d", err);
    return;
  }

  qsMessage = down ? press.toString() : release.toString();
  // message = const_cast<LPTSTR>(reinterpret_cast<LPCTSTR>(qsMessage.utf16()));
  qbaMessage = qsMessage.toLatin1();
  message = const_cast<LPTSTR>(reinterpret_cast<LPCTSTR>(qbaMessage.data()));

  BOOL ok = WriteFile(slotHandle, message,
    (DWORD)(lstrlen(message) + 1)*sizeof(TCHAR),
    &writtenBytes,
    (LPOVERLAPPED) NULL);

  if (!ok) {
    auto err = GetLastError();
    qWarning("Couldn't write mailslot message! %d", err);
    return;
  }
}


void MainWindow::on_gsRunProgram_triggered(bool down, QVariant, QVariant) {
  qDebug("Push to mute triggered!");
}


void MainWindow::on_gsKeyMapping_down(QVariant v) {
  qDebug("Mute self down!");
}

void MainWindow::on_gsDebugMessage_triggered(bool down, QVariant press, QVariant release) {
  if (down) {
    if (press.canConvert(QMetaType::QString)) {
      qDebug("%s", qPrintable(press.toString()));
    } else {
      qDebug() << press;
    }
  } else {
    if (release.canConvert(QMetaType::QString)) {
      qDebug("%s", qPrintable(release.toString()));
    } else {
      qDebug() << release;
    }
  }
}
