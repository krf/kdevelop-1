/***************************************************************************
 *   Copyright (C) 1999-2001 by Bernd Gehrmann                             *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MAKEVIEWPART_H_
#define _MAKEVIEWPART_H_

#include <qguardedptr.h>

#include "kdevmakefrontend.h"
#include "KDevMakeFrontendIface.h"


class MakeWidget;

class MakeViewPart : public KDevMakeFrontend
{
    Q_OBJECT

public:
    MakeViewPart( KDevApi *api, QObject *parent=0, const char *name=0 );
    ~MakeViewPart();

protected:
    virtual void startMakeCommand(const QString &dir, const QString &command);
    virtual bool isRunning();

private:
    QGuardedPtr<MakeWidget> m_widget;
    KDevMakeFrontendIface *m_dcop;
    friend class MakeWidget;
};

#endif
