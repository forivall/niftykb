#include "ShortcutDialog.h"
#include "ui_ShortcutDialog.h"

enum {
  // SHORTCUT_LIST_ITEM_ACTION = 0,
  SHORTCUT_LIST_ITEM_BUTTON = 0,
  SHORTCUT_LIST_ITEM_MESSAGE,
  SHORTCUT_LIST_ITEM_SUPPRESS,
  SHORTCUT_LIST_ITEMS_COUNT
};

ShortcutDialog::ShortcutDialog(QWidget *parent) :
  QDialog(parent)
{
  setupUi(this);

  installEventFilter(this);

  uiNewHardware = 1; // needs to be set to 1 so that an initial scan takes place

  bool canSuppress = GlobalShortcutEngine::engine->canSuppress();
  bool canDisable = GlobalShortcutEngine::engine->canDisable();

  qtwShortcuts->setColumnCount(canSuppress ? SHORTCUT_LIST_ITEMS_COUNT : (SHORTCUT_LIST_ITEMS_COUNT - 1));
  qtwShortcuts->setItemDelegate(new ShortcutDelegate(qtwShortcuts));

  qtwShortcuts->header()->setSectionResizeMode(SHORTCUT_LIST_ITEM_MESSAGE, QHeaderView::Fixed);
  qtwShortcuts->header()->resizeSection(SHORTCUT_LIST_ITEM_MESSAGE, 150);
  qtwShortcuts->header()->setSectionResizeMode(SHORTCUT_LIST_ITEM_BUTTON, QHeaderView::Stretch);
  if (canSuppress){
    qtwShortcuts->header()->setSectionResizeMode(SHORTCUT_LIST_ITEM_SUPPRESS, QHeaderView::ResizeToContents);
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
  sc.qlButtons = item->data(SHORTCUT_LIST_ITEM_BUTTON, Qt::DisplayRole).toList();
  sc.iIndex = item->data(SHORTCUT_LIST_ITEM_MESSAGE, Qt::DisplayRole).toInt();
  sc.bSuppress = item->checkState(SHORTCUT_LIST_ITEM_SUPPRESS) == Qt::Checked;
  // sc.qvData = item->data(SHORTCUT_LIST_ITEM_ACTION, Qt::DisplayRole);

  const ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);
  // if (gs && sc.qvData.type() != gs->qvDefault.type()) {
  //   item->setData(SHORTCUT_LIST_ITEM_ACTION, Qt::DisplayRole, gs->qvDefault);
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
  // g.s.qlShortcuts = qlShortcuts;
}


QTreeWidgetItem *ShortcutDialog::itemForShortcut(const Shortcut &sc) const {
  QTreeWidgetItem *item = new QTreeWidgetItem();
  ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);

  item->setData(SHORTCUT_LIST_ITEM_BUTTON, Qt::DisplayRole, sc.qlButtons);
  // TODO: change to textinput + qstring
  item->setData(SHORTCUT_LIST_ITEM_MESSAGE, Qt::DisplayRole, static_cast<unsigned int>(sc.iIndex));
  item->setCheckState(SHORTCUT_LIST_ITEM_SUPPRESS, sc.bSuppress ? Qt::Checked : Qt::Unchecked);
  // TODO: remove all references
  // if (sc.qvData.isValid() && gs && (sc.qvData.type() == gs->qvDefault.type())) {
  //   item->setData(SHORTCUT_LIST_ITEM_ACTION, Qt::DisplayRole, sc.qvData);
  // } else if (gs) {
  //   item->setData(SHORTCUT_LIST_ITEM_ACTION, Qt::DisplayRole, gs->qvDefault);
  // }
  item->setFlags(item->flags() | Qt::ItemIsEditable);


  item->setData(SHORTCUT_LIST_ITEM_BUTTON, Qt::ToolTipRole, tr("Shortcut button combination."));
  item->setData(SHORTCUT_LIST_ITEM_BUTTON, Qt::WhatsThisRole, tr("<b>This is the global shortcut key combination.</b><br />"
                                         "Click this field and then press the desired key/button combo "
                                         "to rebind. Double-click to clear."));

  // TODO: update this text appropriately
  if (gs) {
    if (! gs->qsToolTip.isEmpty()) {
      item->setData(SHORTCUT_LIST_ITEM_MESSAGE, Qt::ToolTipRole, gs->qsToolTip);
    }
    if (! gs->qsWhatsThis.isEmpty()) {
      item->setData(SHORTCUT_LIST_ITEM_MESSAGE, Qt::WhatsThisRole, gs->qsWhatsThis);
    }
  }

  item->setData(SHORTCUT_LIST_ITEM_SUPPRESS, Qt::ToolTipRole, tr("Suppress keys from other applications"));
  item->setData(SHORTCUT_LIST_ITEM_SUPPRESS, Qt::WhatsThisRole, tr("<b>This hides the button presses from other applications.</b><br />"
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
