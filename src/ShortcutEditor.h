#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include "common.h"
#include "Settings.h"
#include "Global.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "ui_ShortcutEditor.h"

class ShortcutEditor : public QWidget, public Ui::ShortcutEditor
{
  private:
    Q_OBJECT
  public:
    explicit ShortcutEditor(QWidget *parent = 0);

    QList<Shortcut> qlShortcuts;

    ~ShortcutEditor();
  public slots:
    void accept() const;
    void save() const;
    void load(const Settings &r);
    void reload();
    void commit();
    void on_qpbAdd_clicked(bool);
    void on_qpbRemove_clicked(bool);
    void on_qpbSave_clicked(bool);
    void on_qtwShortcuts_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void on_qtwShortcuts_itemChanged(QTreeWidgetItem *, int);
  protected:
    QTreeWidgetItem *itemForShortcut(const Shortcut &) const;
  private:
};

#endif // SHORTCUTDIALOG_H
