/***************************************************************************
 *   Copyright (C) 2000-2001 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _GREPVIEWFACTORY_H_
#define _GREPVIEWFACTORY_H_

#include "kdevfactory.h"


class GrepViewFactory : public KDevFactory
{
    Q_OBJECT

public:
    GrepViewFactory( QObject *parent=0, const char *name=0 );
    ~GrepViewFactory();

    virtual KDevPart *createPartObject(KDevApi *api, QObject *parent, const QStringList &args);
    static KInstance *instance();

private:
    static KInstance *s_instance;
};

#endif
