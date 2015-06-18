#include "ShortcutDialog.h"
#include "ui_ShortcutDialog.h"

ShortcutDialog::ShortcutDialog(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);

  installEventFilter(this);

  uiNewHardware = 0;

  bool canSuppress = GlobalShortcutEngine::engine->canSuppress();
  bool canDisable = GlobalShortcutEngine::engine->canDisable();

  qtwShortcuts->setColumnCount(canSuppress ? 4 : 3);
  qtwShortcuts->setItemDelegate(new ShortcutDelegate(qtwShortcuts));

  qtwShortcuts->header()->setSectionResizeMode(0, QHeaderView::Fixed);
  qtwShortcuts->header()->resizeSection(0, 150);
  qtwShortcuts->header()->setSectionResizeMode(2, QHeaderView::Stretch);
  if (canSuppress){
    qtwShortcuts->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  }
}

ShortcutDialog::~ShortcutDialog()
{
}



void ShortcutDialog::commit() {
//    qtwShortcuts->closePersistentEditor(qtwShortcuts->currentItem(), qtwShortcuts->currentColumn());
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

void ShortcutDialog::on_qtwShortcuts_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *) {
  qpbRemove->setEnabled(item ? true : false);
}

void ShortcutDialog::on_qtwShortcuts_itemChanged(QTreeWidgetItem *item, int) {
  int idx = qtwShortcuts->indexOfTopLevelItem(item);

  Shortcut &sc = qlShortcuts[idx];
  sc.iIndex = item->data(0, Qt::DisplayRole).toInt();
  sc.qvData = item->data(1, Qt::DisplayRole);
  sc.qlButtons = item->data(2, Qt::DisplayRole).toList();
  sc.bSuppress = item->checkState(3) == Qt::Checked;

  const ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);
  if (gs && sc.qvData.type() != gs->qvDefault.type()) {
    item->setData(1, Qt::DisplayRole, gs->qvDefault);
  }
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
  // g.s.qlShortcuts = qlShortcuts;
}

QTreeWidgetItem *ShortcutDialog::itemForShortcut(const Shortcut &sc) const {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);

    item->setData(0, Qt::DisplayRole, static_cast<unsigned int>(sc.iIndex));
    if (sc.qvData.isValid() && gs && (sc.qvData.type() == gs->qvDefault.type())) {
      item->setData(1, Qt::DisplayRole, sc.qvData);
    } else if (gs) {
      item->setData(1, Qt::DisplayRole, gs->qvDefault);
    }
    item->setData(2, Qt::DisplayRole, sc.qlButtons);
    item->setCheckState(3, sc.bSuppress ? Qt::Checked : Qt::Unchecked);
    item->setFlags(item->flags() | Qt::ItemIsEditable);


    if (gs) {
      if (! gs->qsToolTip.isEmpty()) {
        item->setData(0, Qt::ToolTipRole, gs->qsToolTip);
      }
      if (! gs->qsWhatsThis.isEmpty()) {
        item->setData(0, Qt::WhatsThisRole, gs->qsWhatsThis);
      }
    }

    item->setData(2, Qt::ToolTipRole, tr("Shortcut button combination."));
    item->setData(2, Qt::WhatsThisRole, tr("<b>This is the global shortcut key combination.</b><br />"
                                           "Click this field and then press the desired key/button combo "
                                           "to rebind. Double-click to clear."));

    item->setData(3, Qt::ToolTipRole, tr("Suppress keys from other applications"));
    item->setData(3, Qt::WhatsThisRole, tr("<b>This hides the button presses from other applications.</b><br />"
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
