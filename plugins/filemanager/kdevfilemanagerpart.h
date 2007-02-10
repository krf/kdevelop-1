/***************************************************************************
 *   Copyright (C) 2006 by Alexander Dymo                                  *
 *   adymo@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#ifndef KDEVFILEMANAGERPART_H
#define KDEVFILEMANAGERPART_H

#include <iplugin.h>

class FileManager;

class KDevFileManagerPart: public KDevelop::IPlugin {
    Q_OBJECT
public:
    KDevFileManagerPart(QObject *parent, const QStringList &args);
    ~KDevFileManagerPart();

    // KDevelop::Plugin methods
    virtual Qt::DockWidgetArea dockWidgetAreaHint() const;
    virtual void unload();

private slots:
    void init();

private:
    class KDevFileManagerViewFactory *m_factory;

};

#endif

//kate: space-indent on; indent-width 4; replace-tabs on; auto-insert-doxygen on; indent-mode cstyle;
