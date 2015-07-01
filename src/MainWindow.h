#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
  QIcon qiIcon;
  GlobalShortcut *gsPushTalk, *gsPushMute, *gsMuteSelf, *gsDeafSelf;

  void toggleWindow();
  bool nativeEvent(const QByteArray &, void *, long *) Q_DECL_OVERRIDE;
private:
  void createActions();
  void setupGui();
  void setupTrayIcon();
  void updateTrayIcon();
signals:

public slots:
  void on_PushToTalk_triggered(bool, QVariant);
  void on_PushToMute_triggered(bool, QVariant);
  void on_gsMuteSelf_down(QVariant);
  void on_gsDeafSelf_down(QVariant);
};

#endif // MAINWINDOW_H
