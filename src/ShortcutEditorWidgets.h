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

#ifndef NIFTYKB_SHORTCUTEDITORWIDGETS_H_
#define NIFTYKB_SHORTCUTEDITORWIDGETS_H_

#include <QToolButton>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QComboBox>
#include <QDialog>

/**
 * Widget used to define and key combination for a shortcut. Once it gains
 * focus it will listen for a button combination until it looses focus.
 */
class ShortcutKeyWidget : public QLineEdit {
  private:
    Q_OBJECT
    Q_DISABLE_COPY(ShortcutKeyWidget)
    Q_PROPERTY(QList<QVariant> shortcut READ getShortcut WRITE setShortcut USER true)
  protected:
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual bool eventFilter(QObject *, QEvent *);
  public:
    QList<QVariant> qlButtons;
    bool bModified;
    ShortcutKeyWidget(QWidget *p = NULL);
    QList<QVariant> getShortcut() const;
    void displayKeys(bool last = true);
  public slots:
    void updateKeys(bool last);
    void setShortcut(const QList<QVariant> &shortcut);
  signals:
    void keySet(bool, bool);
};

/**
 * Combo box widget used to define the kind of action a shortcut triggers. Then
 * entries get auto-generated from the GlobalShortcutEngine::qmShortcuts store.
 *
 * @see GlobalShortcutEngine
 */
class ShortcutActionWidget : public QComboBox {
  private:
    Q_OBJECT
    Q_DISABLE_COPY(ShortcutActionWidget)
    Q_PROPERTY(unsigned int index READ index WRITE setIndex USER true)
  public:
    ShortcutActionWidget(QWidget *p = NULL);
    unsigned int index() const;
    void setIndex(int);
};

class ShortcutToggleWidget : public QComboBox {
  private:
    Q_OBJECT
    Q_DISABLE_COPY(ShortcutToggleWidget)
    Q_PROPERTY(int index READ index WRITE setIndex USER true)
  public:
    ShortcutToggleWidget(QWidget *p = NULL);
    int index() const;
    void setIndex(int);
};

/**
 * Used to get custom display and edit behaviour for the model used in GlobalShortcutConfig::qtwShortcuts.
 * It registers custom handlers which link specific types to custom ShortcutXWidget editors and also
 * provides a basic textual representation for them when they are not edited.
 *
 * @see GlobalShortcutConfig
 * @see ShortcutKeyWidget
 * @see ShortcutActionWidget
 * @see ShortcutTargetWidget
 */
class ShortcutDelegate : public QStyledItemDelegate {
    Q_OBJECT
    Q_DISABLE_COPY(ShortcutDelegate)
  public:
    ShortcutDelegate(QObject *);
    ~ShortcutDelegate() Q_DECL_OVERRIDE;
    QString displayText(const QVariant &, const QLocale &) const Q_DECL_OVERRIDE;
};

#endif // NIFTYKB_SHORTCUTEDITORWIDGETS_H_
