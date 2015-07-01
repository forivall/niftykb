/* (c) 2015 Jordan Klassen, GPLv3 or later license */
#include "common.h"

#include "ShortcutEditor.h"

#include "Global.h"
#include "ShortcutEditorWidgets.h"

#include "ui_ShortcutEditor.h"

enum ShortcutListItem {
  BUTTON = 0,
  ACTION,
  DATA,
  SUPPRESS,
  _COUNT
};

ShortcutEditor::ShortcutEditor(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);
  installEventFilter(this);

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

ShortcutEditor::~ShortcutEditor() {}

void ShortcutEditor::commit() {
  qtwShortcuts->closePersistentEditor(qtwShortcuts->currentItem(), qtwShortcuts->currentColumn());
}

void ShortcutEditor::on_qpbAdd_clicked(bool) {
  commit();
  Shortcut sc;
  sc.iIndex = -1;
  sc.bSuppress = false;
  qlShortcuts << sc;
  reload();
}

void ShortcutEditor::on_qpbRemove_clicked(bool) {
  commit();
  QTreeWidgetItem *qtwi = qtwShortcuts->currentItem();
  if (! qtwi) {
    return;
  }
  int idx = qtwShortcuts->indexOfTopLevelItem(qtwi);
  delete qtwi;
  qlShortcuts.removeAt(idx);
}

void ShortcutEditor::on_qpbSave_clicked(bool) {
  commit();
  save();

  GlobalShortcutEngine::engine->bNeedRemap = true;
  GlobalShortcutEngine::engine->needRemap();
  GlobalShortcutEngine::engine->setEnabled(g.s.bShortcutEnable);
}

void ShortcutEditor::on_qtwShortcuts_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *) {
  qpbRemove->setEnabled(item ? true : false);
}

void ShortcutEditor::on_qtwShortcuts_itemChanged(QTreeWidgetItem *item, int) {
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


// QIcon ShortcutEditor::icon() const {
//   return QIcon(QLatin1String("skin:config_shortcuts.png"));
// }

void ShortcutEditor::load(const Settings &r) {
  qlShortcuts = r.qlShortcuts;

  reload();
}

void ShortcutEditor::save() const {
  g.s.qlShortcuts = qlShortcuts;
  g.s.save();
}


QTreeWidgetItem *ShortcutEditor::itemForShortcut(const Shortcut &sc) const {
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

void ShortcutEditor::reload() {
  qStableSort(qlShortcuts);
  qtwShortcuts->clear();
  foreach(const Shortcut &sc, qlShortcuts) {
    QTreeWidgetItem *item = itemForShortcut(sc);
    ::GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(sc.iIndex);
    qtwShortcuts->addTopLevelItem(item);
  }
}

void ShortcutEditor::accept() const {
  GlobalShortcutEngine::engine->bNeedRemap = true;
  GlobalShortcutEngine::engine->needRemap();
  GlobalShortcutEngine::engine->setEnabled(g.s.bShortcutEnable);
}
