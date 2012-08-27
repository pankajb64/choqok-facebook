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

#ifndef FACEBOOKPOST_H
#define FACEBOOKPOST_H

#include <kfacebook/postinfo.h>
#include <choqoktypes.h>

using namespace KFacebook;

class FacebookPost : public Choqok::Post
{
	
	public :
	
	FacebookPost() {}
	~FacebookPost() {}
	//FacebookPost(Choqok::Post post) : Choqok::Post::Post() {}
	QString title;
	QString caption;
	QString description;
	QString iconUrl;
	QString likeCount;
	QString likeString;
	QString story;
	QString commentCount;
	QString commentString;
	QString propertyString;
	QString appName;
	ChoqokId appId;
	QDateTime updateDateTime;
	
	//PostInfoPtr post;
};

#endif
