/* Copyright (C) 2005-2011, Thorvald Natvig <thorvald@natvig.com>
 * Copyright (C) 2009-2011, Stefan Hacker <dd0t@users.sourceforge.net>
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
#include "Settings.h"

#include "Global.h"

bool Shortcut::operator < (const Shortcut &other) const {
  return (iIndex < other.iIndex);
}

bool Shortcut::operator == (const Shortcut &other) const {
  return (iIndex == other.iIndex) && (qlButtons == other.qlButtons) && (qvData == other.qvData) && (bSuppress == other.bSuppress);
}

ShortcutTarget::ShortcutTarget() {
  bUsers = true;
  iChannel = -3;
  bLinks = bChildren = bForceCenter = false;
}

bool ShortcutTarget::operator == (const ShortcutTarget &o) const {
  if ((bUsers != o.bUsers) || (bForceCenter != o.bForceCenter)) {
    return false;
  }
  if (bUsers) {
    return (qlUsers == o.qlUsers) && (qlSessions == o.qlSessions);
  } else {
    return (iChannel == o.iChannel) && (bLinks == o.bLinks) && (bChildren == o.bChildren) && (qsGroup == o.qsGroup);
  }
}

quint32 qHash(const ShortcutTarget &t) {
  quint32 h = t.bForceCenter ? 0x55555555 : 0xaaaaaaaa;
  if (t.bUsers) {
    foreach(unsigned int u, t.qlSessions) {
      h ^= u;
    }
  } else {
    h ^= t.iChannel;
    if (t.bLinks) {
      h ^= 0x80000000;
    }
    if (t.bChildren) {
      h ^= 0x40000000;
    }
    h ^= qHash(t.qsGroup);
    h = ~h;
  }
  return h;
}

quint32 qHash(const QList<ShortcutTarget> &l) {
  quint32 h = l.count();
  foreach(const ShortcutTarget &st, l) {
    h ^= qHash(st);
  }
  return h;
}

QDataStream &operator<< (QDataStream &qds, const ShortcutTarget &st) {
  qds << st.bUsers << st.bForceCenter;

  if (st.bUsers) {
    return qds << st.qlUsers;
  } else {
    return qds << st.iChannel << st.qsGroup << st.bLinks << st.bChildren;
  }
}

QDataStream &operator>> (QDataStream &qds, ShortcutTarget &st) {
  qds >> st.bUsers >> st.bForceCenter;
  if (st.bUsers) {
    return qds >> st.qlUsers;
  } else {
    return qds >> st.iChannel >> st.qsGroup >> st.bLinks >> st.bChildren;
  }
}



Settings::Settings() {
  qRegisterMetaType<ShortcutTarget> ("ShortcutTarget");
  qRegisterMetaTypeStreamOperators<ShortcutTarget> ("ShortcutTarget");
  qRegisterMetaType<QVariant> ("QVariant");

  bAskOnQuit = true;
  bHideInTray = true;
  bStateInTray = true;

  // Config updates
  uiUpdateCounter = 0;

  bShortcutEnable = true;
}

void Settings::load() {
  load(g.qs);
}

#define LOAD(var,name) var = qvariant_cast<BOOST_TYPEOF(var)>(settings_ptr->value(QLatin1String(name), var))
void Settings::load(QSettings* settings_ptr) {
  // Config updates
  LOAD(bShortcutEnable, "shortcut/enable");

  int nshorts = settings_ptr->beginReadArray(QLatin1String("shortcuts"));
  for (int i=0; i<nshorts; i++) {
    settings_ptr->setArrayIndex(i);
    Shortcut s;

    s.iIndex = -2;

    LOAD(s.iIndex, "index");
    LOAD(s.qlButtons, "keys");
    LOAD(s.bSuppress, "suppress");
    s.qvData = settings_ptr->value(QLatin1String("data"));
    if (s.iIndex >= -1) {
      qlShortcuts << s;
    }
  }
  settings_ptr->endArray();
  qWarning("Loaded Settings from %s", qPrintable(settings_ptr->fileName()));
}
#undef LOAD

#define SAVE(var,name) if (var != def.var) settings_ptr->setValue(QLatin1String(name), var); else settings_ptr->remove(QLatin1String(name))
void Settings::save() {
  QSettings* settings_ptr = g.qs;
  Settings def;

  SAVE(bShortcutEnable, "shortcut/enable");

  settings_ptr->beginWriteArray(QLatin1String("shortcuts"));
  int idx = 0;
  foreach(const Shortcut &s, qlShortcuts) {
    settings_ptr->setArrayIndex(idx++);
    settings_ptr->setValue(QLatin1String("index"), s.iIndex);
    settings_ptr->setValue(QLatin1String("keys"), s.qlButtons);
    settings_ptr->setValue(QLatin1String("suppress"), s.bSuppress);
    settings_ptr->setValue(QLatin1String("data"), s.qvData);
  }
  settings_ptr->endArray();
  settings_ptr->sync();
  qWarning("Saved Settings to %s", qPrintable(settings_ptr->fileName()));
}
#undef SAVE
