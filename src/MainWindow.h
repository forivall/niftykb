/* (c) 2015 Jordan Klassen, GPLv3 or later license */
#ifndef NIFTYKB_MAINWINDOW_H
#define NIFTYKB_MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>

#include "GlobalShortcut.h"

class MainWindow : public QMainWindow
{
private:
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = 0);

  unsigned int uiNewHardware;

  QSystemTrayIcon *qstiIcon;
  QMenu *qmTray;
  QIcon qiTrayIcon;
  QIcon qiWindowIcon;
  GlobalShortcut *gsMailslotMessage, *gsRunProgram, *gsKeyMapping, *gsDebugMessage;

  void toggleWindow();
  bool nativeEvent(const QByteArray &, void *, long *) Q_DECL_OVERRIDE;
private:
  void createActions();
  void setupGui();
  void setupTrayIcon();
  void updateTrayIcon();
signals:

public slots:
  void on_gsMailslotMessage_triggered(bool, QVariant, QVariant);
  void on_gsRunProgram_down(QVariant);
  void on_gsKeyMapping_triggered(bool, QVariant, QVariant);
  void on_gsDebugMessage_triggered(bool, QVariant, QVariant);
};

#endif // NIFTYKB_MAINWINDOW_H
