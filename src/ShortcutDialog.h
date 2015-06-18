#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include "common.h"
#include "Settings.h"
#include "GlobalShortcut.h"
#include "Global.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "ui_ShortcutDialog.h"

class ShortcutDialog : public QDialog, public Ui::ShortcutDialog
{
private:
    Q_OBJECT
protected:
    QTreeWidgetItem *itemForShortcut(const Shortcut &) const;
public:
    explicit ShortcutDialog(QWidget *parent = 0);

    QList<Shortcut> qlShortcuts;
    unsigned int uiNewHardware;

    QString title() const;
    bool nativeEvent(const QByteArray &, void *, long *);

    ~ShortcutDialog();
public slots:
    void accept() const;
    void save() const;
    void load(const Settings &r);
    void reload();
    void commit();
    void on_qpbAdd_clicked(bool);
    void on_qpbRemove_clicked(bool);
    void on_qtwShortcuts_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void on_qtwShortcuts_itemChanged(QTreeWidgetItem *, int);
private:
};

#endif // SHORTCUTDIALOG_H
