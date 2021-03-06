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

#include "common.h"

#include "GlobalShortcut.h"

#include "Global.h"

/**
 * Used to save the global, unique, platform specific GlobalShortcutEngine.
 */
GlobalShortcutEngine *GlobalShortcutEngine::engine = NULL;

GlobalShortcutEngine::GlobalShortcutEngine(QObject *p) : QThread(p) {
  bNeedRemap = true;
  needRemap();
}

GlobalShortcutEngine::~GlobalShortcutEngine() {
  QSet<ShortcutKey *> qs;
  foreach(const QList<ShortcutKey*> &ql, qlShortcutList) {
    qs += ql.toSet();
  }
  foreach(ShortcutKey *sk, qs) {
    delete sk;
  }
}

void GlobalShortcutEngine::remap() {
  bNeedRemap = false;

  QSet<ShortcutKey *> qs;
  foreach(const QList<ShortcutKey*> &ql, qlShortcutList) {
    qs += ql.toSet();
  }

  foreach(ShortcutKey *sk, qs) {
    delete sk;
  }

  qlButtonList.clear();
  qlShortcutList.clear();
  qlDownButtons.clear();

  foreach(const Shortcut &sc, g.s.qlShortcuts) {
    GlobalShortcut *gs = qmShortcuts.value(sc.iIndex);
    if (gs && ! sc.qlButtons.isEmpty()) {
      ShortcutKey *sk = new ShortcutKey;
      sk->s = sc;
      sk->iNumUp = sc.qlButtons.count();
      sk->gs = gs;

      foreach(const QVariant &button, sc.qlButtons) {
        int idx = qlButtonList.indexOf(button);
        if (idx == -1) {
          qlButtonList << button;
          qlShortcutList << QList<ShortcutKey *>();
          idx = qlButtonList.count() - 1;
        }
        qlShortcutList[idx] << sk;
      }
    }
  }
}

void GlobalShortcutEngine::run() {
}

bool GlobalShortcutEngine::canSuppress() {
  return false;
}

void GlobalShortcutEngine::setEnabled(bool) {
}

bool GlobalShortcutEngine::enabled() {
  return true;
}

bool GlobalShortcutEngine::canDisable() {
  return false;
}

void GlobalShortcutEngine::resetMap() {
  tReset.restart();
  qlActiveButtons.clear();
}

void GlobalShortcutEngine::needRemap() {
}

/**
 * This function gets called internally to update the state
 * of a button.
 *
 * @return True if button is suppressed, otherwise false
*/
bool GlobalShortcutEngine::handleButton(const QVariant &button, bool down) {
  bool already = qlDownButtons.contains(button);
  QList<QVariant> &qlButton = button.toList();
  unsigned int keyid = qlButton.at(0).toUInt();
  QUuid &GUID_SysKeyboard = qlButton.at(1).toUuid();
  if (already == down) {
    return qlSuppressed.contains(button);
  }
  if (down) {
    qlDownButtons << button;
  } else {
    qlDownButtons.removeAll(button);
  }

  if (tReset.elapsed() > 100000) {
    if (down) {
      qlActiveButtons.removeAll(button);
      qlActiveButtons << button;
    }
    emit buttonPressed(! down);
  }

  int idx = qlButtonList.indexOf(button);
  if (idx == -1) {
    return false;
  }

  bool suppress = false;

  foreach(ShortcutKey *sk, qlShortcutList.at(idx)) {
    if (down) {
      sk->iNumUp--;
      if (sk->iNumUp == 0) {
        GlobalShortcut *gs = sk->gs;
        if (sk->s.bSuppress) {
          suppress = true;
          qlSuppressed << button;
        }
        auto dataPair = sk->s.getDataPair();
        if (! gs->qlActive.contains(dataPair)) {
          gs->qlActive << dataPair;
          emit gs->triggered(true, sk->s.qvPressData, sk->s.qvReleaseData);
          emit gs->down(sk->s.qvPressData);
        }
      } else if (sk->iNumUp < 0) {
        sk->iNumUp = 0;
      }
    } else {
      if (qlSuppressed.contains(button)) {
        suppress = true;
        qlSuppressed.removeAll(button);
      }
      sk->iNumUp++;
      if (sk->iNumUp == 1) {
        GlobalShortcut *gs = sk->gs;
        auto dataPair = sk->s.getDataPair();
        if (gs->qlActive.contains(dataPair)) {
          gs->qlActive.removeAll(dataPair);
          emit gs->triggered(false, sk->s.qvPressData, sk->s.qvReleaseData);
        }
      } else if (sk->iNumUp > sk->s.qlButtons.count()) {
        sk->iNumUp = sk->s.qlButtons.count();
      }
    }
  }
  return suppress;
}

void GlobalShortcutEngine::add(GlobalShortcut *gs) {
  if (! GlobalShortcutEngine::engine) {
    GlobalShortcutEngine::engine = GlobalShortcutEngine::platformInit();
    GlobalShortcutEngine::engine->setEnabled(g.s.bShortcutEnable);
  }

  GlobalShortcutEngine::engine->qmShortcuts.insert(gs->idx, gs);
  GlobalShortcutEngine::engine->bNeedRemap = true;
  GlobalShortcutEngine::engine->needRemap();
}

void GlobalShortcutEngine::remove(GlobalShortcut *gs) {
  engine->qmShortcuts.remove(gs->idx);
  engine->bNeedRemap = true;
  engine->needRemap();
  if (engine->qmShortcuts.isEmpty()) {
    delete engine;
    GlobalShortcutEngine::engine = NULL;
  }
}

QString GlobalShortcutEngine::buttonText(const QList<QVariant> &list) {
  QStringList keys;

  foreach(QVariant button, list) {
    QString id = GlobalShortcutEngine::engine->buttonName(button);
    if (! id.isEmpty()) {
      keys << id;
    }
  }
  return keys.join(QLatin1String(" + "));
}

void GlobalShortcutEngine::prepareInput() {
}

GlobalShortcut::GlobalShortcut(QObject *p, int index, QString qsName, QVariant defPress, QVariant defRelease) : QObject(p) {
  idx = index;
  name=qsName;
  qvPressDefault = defPress;
  qvReleaseDefault = defRelease;
  GlobalShortcutEngine::add(this);
}

GlobalShortcut::~GlobalShortcut() {
  GlobalShortcutEngine::remove(this);
}
