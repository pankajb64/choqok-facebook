/*
    This file is part of choqok-facebook, a Facebook plugin for Choqok, the KDE micro-blogging client

    Copyright (C) 2012 Pankaj Bhambhani <pankajb64@gmail.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License or (at your option) version 3 or any later version
	accepted by the membership of KDE e.V. (or its successor approved
	by the membership of KDE e.V.), which shall act as a proxy
	defined in Section 14 of version 3 of the license.


	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, see http://www.gnu.org/licenses/
*/

#ifndef FACEBOOKUTIL_H
#define FACEBOOKUTIL_H

#include <QString>
#include "facebookaccount.h"
#include <libkfbapi/likeinfo.h>
#include <libkfbapi/commentinfo.h>
#include <libkfbapi/propertyinfo.h>
#include <algorithm>

using namespace KFbAPI;

QString assignOrNull(QString s);

QString getImageUrl(const QString& linkUrl);

QString createLikeString(const FacebookAccount* account, const LikeInfo &likes) ;

QString createCommentString(const FacebookAccount* account, const CommentInfo &comments) ;

QString createPropertyString(const QList<PropertyInfo> properties);

QString trimName(const QString name);


template <typename T>
QList<T> reverseList( QList<T>&  in ) {
    QList<T> result;
    result.reserve( in.size() ); // reserve is new in Qt 4.7
    std::reverse_copy( in.begin(), in.end(), std::back_inserter( result ) );
    return result;
}

#endif
