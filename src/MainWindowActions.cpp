/* (c) 2015 Jordan Klassen, GPLv3 or later license */
#include "common.h"

#include "MainWindow.h"

#define MAILSLOT_PATH "\\\\.\\mailslot\\niftykb"

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

void MainWindow::on_gsMailslotMessage_triggered(bool down, QVariant press, QVariant release) {
  // qDebug("Mailslot Message triggered! %d", down);
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

void MainWindow::on_gsRunProgram_down(QVariant) {
  qDebug("Run Program down!");
}

void MainWindow::on_gsKeyMapping_triggered(bool down, QVariant press, QVariant) {
  qDebug("Key mapping triggered!");
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
