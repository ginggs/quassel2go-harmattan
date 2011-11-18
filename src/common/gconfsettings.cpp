/***************************************************************************
 *   Copyright (C) 2005-09 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <gconfitem.h>
#include <QStringBuilder>

//required for black magic
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include <QBuffer>

#include <QDebug>

#include "gconfsettings.h"

GConfSettings::GConfSettings():basePath("/apps/quassel/") {
}

QVariant GConfSettings::value(const QString &key, const QVariant & defaultValue) const {
  GConfItem i(basePath % group % key);
  QVariant value = i.value(defaultValue);
  if(value == defaultValue || value == QVariant::Invalid) {
    return defaultValue;
  } else if (value.type() != QVariant::String) {
    return value;
  } else {
    if(!value.toString().startsWith("\xFF\xFF\xFF\xFF")) {
      return value;
    } else {
      //black magic happens here
      QString data = value.toString().right(value.toString().size() - 4);
      QByteArray dataArray = data.toAscii();
      dataArray = QByteArray::fromBase64(dataArray);
      QDataStream dataStream(&dataArray, QIODevice::ReadOnly);
      QVariant result;
      dataStream >> result;
      return result;      
    }
  }
}

void GConfSettings::setValue(const QString &key, const QVariant & value) {
  GConfItem i(basePath % group % key);
  QVariant::Type type = value.type();
  if(type == QVariant::String || type == QVariant::Double || type == QVariant::Bool || type == QVariant::Int) {
    i.set(value);
  } else {
    //black magic happens here too
    QByteArray dataArray;
    QByteArray valueArray;
    QBuffer dataBuffer(&dataArray);
    dataBuffer.open(QIODevice::WriteOnly);
    QDataStream valueStream(&valueArray, QIODevice::WriteOnly);
    dataBuffer.write("\xFF\xFF\xFF\xFF");
    valueStream << value;
    valueArray = valueArray.toBase64();
    dataBuffer.write(valueArray);
    dataBuffer.close();
    i.set(QString(dataArray));
  }
}

void GConfSettings::beginGroup(const QString &prefix) {
  group = prefix + "/";
}

void GConfSettings::endGroup() {
  group = "";
}

QStringList GConfSettings::allKeys () const {
  QString root = basePath % group;
  root.chop(1);
  GConfItem i(root);
  QList<QString> entryList = i.listEntries();
  QStringList list;
  
  foreach(QString entry, entryList) {
    list.append(entry.replace(basePath % group, ""));
  }
  
  return list;
}

QStringList GConfSettings::childKeys () const {
  QString root = basePath % group;
  root.chop(1);
  GConfItem i(root);
  QList<QString> entryList = i.listEntries();
  QStringList list;
  int count = (basePath + group).count("/");
  
  QString prefix = basePath % group;
  foreach(QString entry, entryList) {
    if(entry.count("/") == count) {
      list.append(entry.replace(prefix, ""));
    }
  }
  
  foreach(QString tmp, list) {
  }

  return list;
}

void GConfSettings::remove(const QString &key) {
  QList<QString> keysList;
  if(key.isEmpty()) {
    QString root = basePath % group;
    root.chop(1);
    GConfItem i(root);
    keysList = i.listEntries();
  } else {
    GConfItem i(basePath % group % key);
    keysList = i.listEntries();
  }
  
  
  foreach(const QString &key, keysList) {
    GConfItem i(key);
    i.unset();
  }  
}

QStringList GConfSettings::childGroups() const {
  QString root = basePath % group;
  root.chop(1);
  GConfItem i(root);
  QList<QString> dirList = i.listDirs();
  QStringList result;
  
  QString prefix = basePath % group;
  foreach(QString dir, dirList) {
    GConfItem gconfDir(dir);
    if(!gconfDir.listEntries().isEmpty()) {
      result.append(dir.replace(prefix, ""));
    }
  }
  
  foreach(QString res, result) {
  }
  
  return result;
}

