/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DFMGENERICPLUGINOBJECT_H
#define DFMGENERICPLUGINOBJECT_H

#include <QObject>
#include <QIcon>
#include <QQueue>

#include <dabstractfileinfo.h>

// 不同版本libdde-file-manager-dev，此头文件的路径可能不一样
#include "dde-file-manager/dfmgenericplugin.h"


class DFMGenericPluginObject : public QObject
{
    Q_OBJECT

public:
    explicit DFMGenericPluginObject(QObject *parent = 0);

public slots:
    QList<QIcon> fileAdditionalIcon(const DAbstractFileInfoPointer &fileInfo);
};

#endif // DFMGENERICPLUGINOBJECT_H
