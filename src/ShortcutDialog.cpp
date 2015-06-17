#include "ShortcutDialog.h"
#include "ui_ShortcutDialog.h"

ShortcutDialog::ShortcutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortcutDialog)
{
    ui->setupUi(this);
}

ShortcutDialog::~ShortcutDialog()
{
    delete ui;
}
