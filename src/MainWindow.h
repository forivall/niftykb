/* (c) 2015 Jordan Klassen, GPLv3 or later license */
#ifndef NIFTYKB_MAINWINDOW_H
#define NIFTYKB_MAINWINDOW_H

#include <QCommandLineParser>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>

#include "GlobalShortcut.h"
#include "ShortcutEditor.h"

#include "Ui_MainWindow.h"

class MainWindow : public QMainWindow, public Ui_MainWindow
{
private:
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = 0, QCommandLineParser *args = 0);

  unsigned int uiNewHardware;

  ShortcutEditor *shortcutEditor;

  QSystemTrayIcon *qstiIcon;
  QMenu *qmTray;
  QIcon qiTrayIcon;
  QIcon qiWindowIcon;
  GlobalShortcut *gsMailslotMessage, *gsRunProgram, *gsKeyMapping, *gsDebugMessage;

  bool nativeEvent(const QByteArray &, void *, long *) Q_DECL_OVERRIDE;
private:
  void createActions();
  void setupGui();
  void setupTrayIcon();
  void updateTrayIcon();
signals:

public slots:
  void toggleWindow();
  void on_gsMailslotMessage_triggered(bool, QVariant, QVariant);
  void on_gsRunProgram_down(QVariant);
  void on_gsKeyMapping_triggered(bool, QVariant, QVariant);
  void on_gsDebugMessage_triggered(bool, QVariant, QVariant);
};

#endif // NIFTYKB_MAINWINDOW_H
