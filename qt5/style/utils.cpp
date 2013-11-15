/*****************************************************************************
 *   Copyright 2007 - 2010 Craig Drummond <craig.p.drummond@gmail.com>       *
 *   Copyright 2013 - 2013 Yichao Yu <yyc1992@gmail.com>                     *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU Lesser General Public License as          *
 *   published by the Free Software Foundation; either version 2.1 of the    *
 *   License, or (at your option) version 3, or any later version accepted   *
 *   by the membership of KDE e.V. (or its successor approved by the         *
 *   membership of KDE e.V.), which shall act as a proxy defined in          *
 *   Section 6 of version 3 of the license.                                  *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *   Lesser General Public License for more details.                         *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this library. If not,                                *
 *   see <http://www.gnu.org/licenses/>.                                     *
 *****************************************************************************/

#include <qtcurve-utils/log.h>
#include "utils.h"
#ifdef QTC_ENABLE_X11
#  include <qtcurve-utils/x11utils.h>
#  include <qtcurve-utils/qtutils.h>
#  include <QApplication>
#  include <QDesktopWidget>
#endif

#ifdef QTC_QT5_ENABLE_KDE
#  include <kdeversion.h>
#  include <KDE/KWindowSystem>
#endif

namespace QtCurve {
namespace Utils {

bool compositingActive()
{
#ifndef QTC_QT5_ENABLE_KDE
#ifdef QTC_ENABLE_X11
    return qtcX11CompositingActive();
#else // QTC_ENABLE_X11
    return false;
#endif // QTC_ENABLE_X11
#else
    return KWindowSystem::compositingActive();
#endif
}

bool hasAlphaChannel(const QWidget *widget)
{
#ifdef QTC_ENABLE_X11
    QWidget *window;
    if (!(widget && (window = widget->window())))
        return false;
    if (WId wid = qtcGetQWidgetWid(window)) {
        return qtcX11HasAlpha(wid);
    }
    return window->testAttribute(Qt::WA_TranslucentBackground);
#else
    Q_UNUSED(widget);
    return compositingActive();
#endif
}

}
}
