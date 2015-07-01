/* Copyright (C) 2005-2011, Thorvald Natvig <thorvald@natvig.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Mumble Developers nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/* Modifications (c) 2015 Jordan Klassen, GPLv3 or later license */

#include "common.h"

#include "ShortcutEditorWidgets.h"

#include "GlobalShortcut.h"

ShortcutKeyWidget::ShortcutKeyWidget(QWidget *p) : QLineEdit(p) {
  setReadOnly(true);
  clearFocus();
  bModified = false;
  displayKeys();
}

QList<QVariant> ShortcutKeyWidget::getShortcut() const {
  return qlButtons;
}

void ShortcutKeyWidget::setShortcut(const QList<QVariant> &buttons) {
  qlButtons = buttons;
  displayKeys();
}

void ShortcutKeyWidget::focusInEvent(QFocusEvent *) {
  setText(tr("Press Shortcut"));

  QPalette pal=parentWidget()->palette();
  pal.setColor(QPalette::Base, pal.color(QPalette::Base).dark(120));
  setPalette(pal);

  setForegroundRole(QPalette::Button);
  GlobalShortcutEngine::engine->resetMap();
  connect(GlobalShortcutEngine::engine, SIGNAL(buttonPressed(bool)), this, SLOT(updateKeys(bool)));
  installEventFilter(this);
}

void ShortcutKeyWidget::focusOutEvent(QFocusEvent *e) {
  if ((e->reason() == Qt::TabFocusReason) || (e->reason() == Qt::BacktabFocusReason)) {
    return;
  }
  setPalette(parentWidget()->palette());
  clearFocus();
  disconnect(GlobalShortcutEngine::engine, SIGNAL(buttonPressed(bool)), this, SLOT(updateKeys(bool)));
  displayKeys();
  removeEventFilter(this);
}

bool ShortcutKeyWidget::eventFilter(QObject *, QEvent *evt) {
  if ((evt->type() == QEvent::KeyPress) || (evt->type() == QEvent::MouseButtonPress))
    return true;
  return false;
}

void ShortcutKeyWidget::mouseDoubleClickEvent(QMouseEvent *) {
  bModified = true;
  qlButtons.clear();
  clearFocus();
  displayKeys();
}

void ShortcutKeyWidget::updateKeys(bool last) {
  qlButtons = GlobalShortcutEngine::engine->qlActiveButtons;
  bModified = true;

  if (qlButtons.isEmpty()) {
    return;
  }

  if (last) {
    clearFocus();
  } else {
    displayKeys(false);
  }
}

void ShortcutKeyWidget::displayKeys(bool last) {
  QStringList keys;

  foreach(QVariant button, qlButtons) {
    QString id = GlobalShortcutEngine::engine->buttonName(button);
    if (! id.isEmpty()) {
      keys << id;
    }
  }
  setText(keys.join(QLatin1String(" + ")));
  emit keySet(keys.count() > 0, last);
}

ShortcutActionWidget::ShortcutActionWidget(QWidget *p) : QComboBox(p) {
  int idx = 0;

  insertItem(idx, tr("Unassigned"));
  setItemData(idx, -1);
  setSizeAdjustPolicy(AdjustToContents);

  idx++;

  foreach(GlobalShortcut *gs, GlobalShortcutEngine::engine->qmShortcuts) {
    insertItem(idx, gs->name);
    setItemData(idx, gs->idx);
    if (! gs->qsToolTip.isEmpty())
      setItemData(idx, gs->qsToolTip, Qt::ToolTipRole);
    if (! gs->qsWhatsThis.isEmpty())
      setItemData(idx, gs->qsWhatsThis, Qt::WhatsThisRole);
    idx++;
  }

}

void ShortcutActionWidget::setIndex(int idx) {
  setCurrentIndex(findData(idx));
}

unsigned int ShortcutActionWidget::index() const {
  return itemData(currentIndex()).toUInt();
}

ShortcutToggleWidget::ShortcutToggleWidget(QWidget *p) : QComboBox(p) {
  int idx = 0;

  insertItem(idx, tr("Off"));
  setItemData(idx, -1);
  idx++;

  insertItem(idx, tr("Toggle"));
  setItemData(idx, 0);
  idx++;

  insertItem(idx, tr("On"));
  setItemData(idx, 1);
  idx++;
}

void ShortcutToggleWidget::setIndex(int idx) {
  setCurrentIndex(findData(idx));
}

int ShortcutToggleWidget::index() const {
  return itemData(currentIndex()).toInt();
}

ShortcutDelegate::ShortcutDelegate(QObject *p) : QStyledItemDelegate(p) {
  QItemEditorFactory *factory = new QItemEditorFactory;

  factory->registerEditor(QVariant::List, new QStandardItemEditorCreator<ShortcutKeyWidget>());
  factory->registerEditor(QVariant::UInt, new QStandardItemEditorCreator<ShortcutActionWidget>());
  factory->registerEditor(QVariant::Int, new QStandardItemEditorCreator<ShortcutToggleWidget>());
  factory->registerEditor(QVariant::String, new QStandardItemEditorCreator<QLineEdit>());
  factory->registerEditor(QVariant::Invalid, new QStandardItemEditorCreator<QWidget>());
  setItemEditorFactory(factory);
}

ShortcutDelegate::~ShortcutDelegate() {
  delete itemEditorFactory();
  setItemEditorFactory(NULL);
}

/**
 * Provides textual representations for the mappings done for the edit behaviour.
 */
QString ShortcutDelegate::displayText(const QVariant &item, const QLocale &loc) const {
  if (item.type() == QVariant::List) {
    return GlobalShortcutEngine::buttonText(item.toList());
  } else if (item.type() == QVariant::Int) {
    int v = item.toInt();
    if (v > 0) {
      return tr("On");
    } else if (v < 0) {
      return tr("Off");
    } else {
      return tr("Toggle");
    }
  } else if (item.type() == QVariant::UInt) {
    GlobalShortcut *gs = GlobalShortcutEngine::engine->qmShortcuts.value(item.toInt());
    if (gs) {
      return gs->name;
    } else {
      return tr("Unassigned");
    }
  }

  qWarning("ShortcutDelegate::displayText Unknown type %d", item.type());

  return QStyledItemDelegate::displayText(item,loc);
}
