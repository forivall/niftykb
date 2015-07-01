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

#ifndef MUMBLE_MUMBLE_SETTINGS_H_
#define MUMBLE_MUMBLE_SETTINGS_H_

#include <QtCore/QVariant>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtCore/QStringList>

// Global helper classes to spread variables around across threads
// especially helpful to initialize things like the stored
// preference for audio transmission, since the GUI elements
// will be created long before the AudioInput object, and the
// latter lives in a separate thread and so cannot touch the
// GUI.

struct Shortcut {
  int iIndex;
  QList<QVariant> qlButtons;
  QVariant qvData;
  bool bSuppress;
  bool operator <(const Shortcut &) const;
  bool isServerSpecific() const;
  bool operator ==(const Shortcut &) const;
};

struct ShortcutTarget {
  bool bUsers;
  QStringList qlUsers;
  QList<unsigned int> qlSessions;
  int iChannel;
  QString qsGroup;
  bool bLinks;
  bool bChildren;
  bool bForceCenter;
  ShortcutTarget();
  bool isServerSpecific() const;
  bool operator <(const ShortcutTarget &) const;
  bool operator ==(const ShortcutTarget &) const;
};

quint32 qHash(const ShortcutTarget &);
quint32 qHash(const QList<ShortcutTarget> &);

QDataStream &operator<<(QDataStream &, const ShortcutTarget &);
QDataStream &operator>>(QDataStream &, ShortcutTarget &);
Q_DECLARE_METATYPE(ShortcutTarget)

struct Settings {
  bool bAskOnQuit;
  bool bHideInTray;
  bool bStateInTray;

  bool bShortcutEnable;
  QList<Shortcut> qlShortcuts;

  // Config updates
  unsigned int uiUpdateCounter;

  Settings();
  void load();
  void load(QSettings*);
  void save();
};

#endif
