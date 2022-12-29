/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dfmgenericpluginobject.h"
#include <QWidget>

DFMGenericPluginObject::DFMGenericPluginObject(QObject *parent)
    : QObject(parent)
{
}

QList<QIcon> DFMGenericPluginObject::fileAdditionalIcon(const DAbstractFileInfoPointer &fileInfo)
{
    QList<QIcon> list;

     list << QIcon("://emblem-default.png");
    return list;
}
