#include "ShortcutDialog.h"
#include "ui_ShortcutDialog.h"

enum ShortcutListItem {
  BUTTON = 0,
  ACTION,
  DATA,
  SUPPRESS,
  _COUNT
};

ShortcutDialog::ShortcutDialog(QWidget *parent) :
  QDialog(parent)
{
  createActions();
  setupTrayIcon();
  setupGui();
  setupUi(this);

  installEventFilter(this);

  uiNewHardware = 1; // needs to be set to 1 so that an initial scan takes place

  bool canSuppress = GlobalShortcutEngine::engine->canSuppress();
  bool canDisable = GlobalShortcutEngine::engine->canDisable();

  qtwShortcuts->setColumnCount(canSuppress ? ShortcutListItem::_COUNT : (ShortcutListItem::_COUNT - 1));
  qtwShortcuts->setItemDelegate(new ShortcutDelegate(qtwShortcuts));

  qtwShortcuts->header()->setSectionResizeMode(ShortcutListItem::ACTION, QHeaderView::Fixed);
  qtwShortcuts->header()->resizeSection(ShortcutListItem::ACTION, 150);
  qtwShortcuts->header()->setSectionResizeMode(ShortcutListItem::BUTTON, QHeaderView::Stretch);
  if (canSuppress){
    qtwShortcuts->header()->setSectionResizeMode(ShortcutListItem::SUPPRESS, QHeaderView::ResizeToContents);
  }

  load(g.s);
}

ShortcutDialog::~ShortcutDialog()
{
}

void ShortcutDialog::createActions() {
  int idx = 1;
  // broadcast mailslot message
  gsPushTalk=new GlobalShortcut(this, idx++, tr("Push-to-Talk", "Global Shortcut"), false);
  gsPushTalk->setObjectName(QLatin1String("PushToTalk"));
  // gsPushTalk->qsToolTip = tr("Push and hold this button to send voice.", "Global Shortcut");
  // gsPushTalk->qsWhatsThis = tr("This configures the push-to-talk button, and as long as you hold this button down, you will transmit voice.", "Global Shortcut");

  // run exe
  gsPushMute=new GlobalShortcut(this, idx++, tr("Push-to-Mute", "Global Shortcut"));
  gsPushMute->setObjectName(QLatin1String("PushToMute"));

  // fake keystroke
  gsMuteSelf=new GlobalShortcut(this, idx++, tr("Mute Self", "Global Shortcut"), false, 0);
  gsMuteSelf->setObjectName(QLatin1String("gsMuteSelf"));

  // send debug message
  gsDeafSelf=new GlobalShortcut(this, idx++, tr("Deafen Self", "Global Shortcut"), false, 0);
  gsDeafSelf->setObjectName(QLatin1String("gsDeafSelf"));
}
void ShortcutDialog::setupTrayIcon() {
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

void ShortcutDialog::toggleWindow() {
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

void ShortcutDialog::setupGui()  {
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

void ShortcutDialog::updateTrayIcon() {
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

void ShortcutDialog::commit() {
  qtwShortcuts->closePersistentEditor(qtwShortcuts->currentItem(), qtwShortcuts->currentColumn());
}

void ShortcutDialog::on_qpbAdd_clicked(bool) {
  commit();
  Shortcut sc;
  sc.iIndex = -1;
  sc.bSuppress = false;
  qlShortcuts << sc;
  reload();
}

void ShortcutDialog::on_qpbRemove_clicked(bool) {
  commit();
  QTreeWidgetItem *qtwi = qtwShortcuts->currentItem();
  if (! qtwi) {
    return;
  }
  int idx = qtwShortcuts->indexOfTopLevelItem(qtwi);
  delete qtwi;
  qlShortcuts.removeAt(idx);
}

void ShortcutDialog::on_qpbSave_clicked(bool) {
  commit();
  save();

  GlobalShortcutEngine::engine->bNeedRemap = true;
  GlobalShortcutEngine::engine->needRemap();
  GlobalShortcutEngine::engine->setEnabled(g.s.bShortcutEnable);
}

void ShortcutDialog::on_qtwShortcuts_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *) {
  qpbRemove->setEnabled(item ? true : false);
}

void ShortcutDialog::on_qtwShortcuts_itemChanged(QTreeWidgetItem *item, int) {
  int idx = qtwShortcuts->indexOfTopLevelItem(item);

  Shortcut &sc = qlShortcuts[idx];
  sc.qlButtons = item->data(ShortcutListItem::BUTTON, Qt::DisplayRole).toList();
  // which action to take
  sc.iIndex = item->data(ShortcutListItem::ACTION, Qt::DisplayRole).toInt();
  sc.bSuppress = item->checkState(ShortcutListItem::SUPPRESS) == Qt::Checked;
  // sc.qvData = item->data(ShortcutListItem::DATA, Qt::DisplayRole);

  const ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);
  // if (gs && sc.qvData.type() != gs->qvDefault.type()) {
  //   item->setData(ShortcutListItem::DATA, Qt::DisplayRole, gs->qvDefault);
  // }
}

QString ShortcutDialog::title() const {
  return tr("Shortcuts");
}

// QIcon ShortcutDialog::icon() const {
//   return QIcon(QLatin1String("skin:config_shortcuts.png"));
// }

void ShortcutDialog::load(const Settings &r) {
  qlShortcuts = r.qlShortcuts;

  reload();
}

void ShortcutDialog::save() const {
  g.s.qlShortcuts = qlShortcuts;
  g.s.save();
}


QTreeWidgetItem *ShortcutDialog::itemForShortcut(const Shortcut &sc) const {
  QTreeWidgetItem *item = new QTreeWidgetItem();
  ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);

  item->setData(ShortcutListItem::BUTTON, Qt::DisplayRole, sc.qlButtons);
  item->setData(ShortcutListItem::ACTION, Qt::DisplayRole, static_cast<unsigned int>(sc.iIndex));
  item->setCheckState(ShortcutListItem::SUPPRESS, sc.bSuppress ? Qt::Checked : Qt::Unchecked);
  // TODO: change to textinput + qstring
  if (sc.qvData.isValid() && gs && (sc.qvData.type() == gs->qvDefault.type())) {
    item->setData(ShortcutListItem::DATA, Qt::DisplayRole, sc.qvData);
  } else if (gs) {
    item->setData(ShortcutListItem::DATA, Qt::DisplayRole, gs->qvDefault);
  }
  item->setFlags(item->flags() | Qt::ItemIsEditable);


  item->setData(ShortcutListItem::BUTTON, Qt::ToolTipRole, tr("Shortcut button combination."));
  item->setData(ShortcutListItem::BUTTON, Qt::WhatsThisRole, tr("<b>This is the global shortcut key combination.</b><br />"
                                         "Click this field and then press the desired key/button combo "
                                         "to rebind. Double-click to clear."));

  // TODO: update this text appropriately
  if (gs) {
    if (! gs->qsToolTip.isEmpty()) {
      item->setData(ShortcutListItem::ACTION, Qt::ToolTipRole, gs->qsToolTip);
    }
    if (! gs->qsWhatsThis.isEmpty()) {
      item->setData(ShortcutListItem::ACTION, Qt::WhatsThisRole, gs->qsWhatsThis);
    }
  }

  item->setData(ShortcutListItem::SUPPRESS, Qt::ToolTipRole, tr("Suppress keys from other applications"));
  item->setData(ShortcutListItem::SUPPRESS, Qt::WhatsThisRole, tr("<b>This hides the button presses from other applications.</b><br />"
                                         "Enabling this will hide the button (or the last button of a multi-button combo) "
                                         "from other applications. Note that not all buttons can be suppressed."));

  return item;
}

void ShortcutDialog::reload() {
  qStableSort(qlShortcuts);
  qtwShortcuts->clear();
  foreach(const Shortcut &sc, qlShortcuts) {
    QTreeWidgetItem *item = itemForShortcut(sc);
    ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);
    qtwShortcuts->addTopLevelItem(item);
  }
}

void ShortcutDialog::accept() const {
  GlobalShortcutEngine::engine->bNeedRemap = true;
  GlobalShortcutEngine::engine->needRemap();
  GlobalShortcutEngine::engine->setEnabled(g.s.bShortcutEnable);
}

bool ShortcutDialog::nativeEvent(const QByteArray &, void *message, long *) {
  MSG *msg = reinterpret_cast<MSG *>(message);
  if (msg->message == WM_DEVICECHANGE && msg->wParam == DBT_DEVNODES_CHANGED) {
    uiNewHardware++;
  }

  return false;
}


void ShortcutDialog::on_PushToTalk_triggered(bool down, QVariant) {
  qDebug("Push to talk triggered!");
  // g.iPrevTarget = 0;
  // if (down) {
  //   g.uiDoublePush = g.tDoublePush.restart();
  //   g.iPushToTalk++;
  // } else if (g.iPushToTalk > 0) {
  //   QTimer::singleShot(g.s.uiPTTHold, this, SLOT(pttReleased()));
  // }
}

// void ShortcutDialog::pttReleased() {
//   if (g.iPushToTalk > 0) {
//     g.iPushToTalk--;
//   }
// }

void ShortcutDialog::on_PushToMute_triggered(bool down, QVariant) {
  qDebug("Push to mute triggered!");
}


void ShortcutDialog::on_gsMuteSelf_down(QVariant v) {
  qDebug("Mute self down!");
}

void ShortcutDialog::on_gsDeafSelf_down(QVariant v) {
  qDebug("Deaf self down!");
}
