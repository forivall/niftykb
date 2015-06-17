#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include <QDialog>

namespace Ui {
class ShortcutDialog;
}

class ShortcutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutDialog(QWidget *parent = 0);
    ~ShortcutDialog();

private:
    Ui::ShortcutDialog *ui;
};

#endif // SHORTCUTDIALOG_H
