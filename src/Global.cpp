/* Copyright (C) 2005-2011, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "Global.h"

Global *Global::g_global_struct;

Global::Global() {

  bQuit = false;

  QStringList qsl;
  qsl << QCoreApplication::instance()->applicationDirPath();
  QString appdata;
  // wchar_t appDataWchar[MAX_PATH];
  char appDataWchar[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataWchar))) {
    appdata = QDir::fromNativeSeparators(QString::fromUtf8(appDataWchar));

    if (!appdata.isEmpty()) {
      appdata.append(QLatin1String("/nilpostman"));
      qsl << appdata;
    }
  }
  qsl << QStandardPaths::writableLocation(QStandardPaths::DataLocation);

  qs = NULL;
  mw = NULL;

  foreach(const QString &dir, qsl) {
    QFile inifile(QString::fromLatin1("%1/nilpostman.ini").arg(dir));
    if (inifile.exists() && inifile.permissions().testFlag(QFile::WriteUser)) {
      qdBasePath = dir;
      qs = new QSettings(inifile.fileName(), QSettings::IniFormat);
      break;
    }
  }

  if (!qs) {
    if (! appdata.isEmpty()) {
      qdBasePath.setPath(appdata);
    }
    qs = new QSettings(QString::fromLatin1("%1/nilpostman.ini").arg(qdBasePath.path()), QSettings::IniFormat);
    qdBasePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (! qdBasePath.exists()) {
      QDir::root().mkpath(qdBasePath.absolutePath());
      if (! qdBasePath.exists()) {
        qdBasePath = QDir::home();
      }
    }
  }

  qs->setIniCodec("UTF-8");
}

Global::~Global() {
  delete qs;
}
