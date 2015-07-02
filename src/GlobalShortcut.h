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

#ifndef NIFTYKB_GLOBALSHORTCUT_H_
#define NIFTYKB_GLOBALSHORTCUT_H_

#include "Settings.h"
#include "Timer.h"

#include <QThread>

class GlobalShortcut : public QObject {
    friend class GlobalShortcutEngine;
    friend class GlobalShortcutConfig;
  private:
    Q_OBJECT
    Q_DISABLE_COPY(GlobalShortcut)
  protected:
    QList<QPair<QVariant,QVariant>> qlActive;
  signals:
    void down(QVariant);
    void triggered(bool, QVariant, QVariant);
  public:
    QString qsToolTip;
    QString qsWhatsThis;
    QString name;
    QVariant qvPressDefault;
    QVariant qvReleaseDefault;
    int idx;

    GlobalShortcut(QObject *parent, int index, QString qsName, QVariant defPress = QVariant(), QVariant defRelease = QVariant());
    ~GlobalShortcut() Q_DECL_OVERRIDE;

    bool active() const {
      return ! qlActive.isEmpty();
    }
};

struct ShortcutKey {
  Shortcut s;
  int iNumUp;
  GlobalShortcut *gs;
};

/**
 * Creates a background thread which handles global shortcut behaviour. This class inherits
 * a system unspecific interface and basic functionality to the actually used native backend
 * classes (GlobalShortcutPlatform).
 *
 * @see GlobalShortcutX
 * @see GlobalShortcutMac
 * @see GlobalShortcutWin
 */
class GlobalShortcutEngine : public QThread {
  private:
    Q_OBJECT
    Q_DISABLE_COPY(GlobalShortcutEngine)
  public:
    bool bNeedRemap;
    Timer tReset;

    static GlobalShortcutEngine *engine;
    static GlobalShortcutEngine *platformInit();

    QHash<int, GlobalShortcut *> qmShortcuts;
    QList<QVariant> qlActiveButtons;
    QList<QVariant> qlDownButtons;
    QList<QVariant> qlSuppressed;

    QList<QVariant> qlButtonList;
    QList<QList<ShortcutKey *> > qlShortcutList;

    GlobalShortcutEngine(QObject *p = NULL);
    ~GlobalShortcutEngine() Q_DECL_OVERRIDE;
    void resetMap();
    void remap();
    virtual void needRemap();
    void run();

    bool handleButton(const QVariant &, bool);
    static void add(GlobalShortcut *);
    static void remove(GlobalShortcut *);
    static QString buttonText(const QList<QVariant> &);
    virtual QString buttonName(const QVariant &) = 0;
    virtual bool canSuppress();

    virtual void setEnabled(bool b);
    virtual bool enabled();
    virtual bool canDisable();

    virtual void prepareInput();
  signals:
    void buttonPressed(bool last);
};

#endif // NIFTYKB_GLOBALSHORTCUT_H_
