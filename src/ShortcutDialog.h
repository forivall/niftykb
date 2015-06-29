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
  public:
    explicit ShortcutDialog(QWidget *parent = 0);

    QSystemTrayIcon *qstiIcon;
    QMenu *qmTray;
    QIcon qiIcon;
    GlobalShortcut *gsPushTalk, *gsPushMute, *gsMuteSelf, *gsDeafSelf;

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
    void on_qpbSave_clicked(bool);
    void on_qtwShortcuts_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void on_qtwShortcuts_itemChanged(QTreeWidgetItem *, int);

    void on_PushToTalk_triggered(bool, QVariant);
    void on_PushToMute_triggered(bool, QVariant);
    void on_gsMuteSelf_down(QVariant);
    void on_gsDeafSelf_down(QVariant);
  protected:
    QTreeWidgetItem *itemForShortcut(const Shortcut &) const;
    void createActions();
    void setupGui();
    void updateTrayIcon();
  private:
};

#endif // SHORTCUTDIALOG_H
