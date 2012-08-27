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

#include "facebookutil.h"
#include <KUrl>
#include <QUrl>

QString assignOrNull(QString s)
{
	return s.isNull() ? "" : s;
}

QString getImageUrl(const QString& linkUrl)
{
	QUrl url(linkUrl);
	url.setScheme("img");
	QString imgUrl = url.toString();
	
	return imgUrl;
}



QString createLikeString(const FacebookAccount* account, const LikeInfoPtr likes) 
{
	int count = likes->count();
	QString string = "";
	
	int ct = 0;
	
	QList<UserInfoPtr> users = likes->data();
	
	if (!users.isEmpty())
	{
		foreach ( UserInfoPtr user, users)
		{
			string += (account->id() == user->id()) ? "You" : trimName(user->name());
			if ( user == users[users.length() - 1] )
			   string += "";
			else if (users.length() > 1 && user == users[users.length() - 2] && count == users.length() )
			   string += " and ";
			else
			   string  += ", ";
			
			ct++;
			
			if (ct == 4) ///max 4 suggestions
			   break;   
		}
	}
	int diff = count - ct;
	
	if ( diff > 0)
	{
		
		string += QString(" and %1 other%2").arg(diff).arg(diff > 1 ? "s " : " ");
	}
	
	if (count > 0)
	{
		string += " like";
		
		if (( diff == 1 || count == 1) && !string.startsWith("You" ) )
		   string += "s";
		
		string += " this.";
	}	

	return string;
}

QString createCommentString(const FacebookAccount* account, const CommentInfoPtr comments) 
{
	int count = comments->count();
	QString string = "";
	
	QList<CommentDataPtr> list = comments->data();
	int ct = 0;
	
	if (!list.isEmpty())
	{
		foreach ( CommentDataPtr comment, list)
		{
			string += (account->id() == comment->from()->id()) ? "You" : trimName(comment->from()->name());
			
			if ( comment == list[list.length() - 1] )
			   string += "";
			else if (list.length() > 1 && comment == list[list.length() - 2] && count == list.length() )
			   string += " and ";
			else
			   string  += ", ";
			ct++;
			
			if ( ct == 4) ///max 4 suggestions
			  break;   
		}
	}
	int diff = count - ct;
	
	if ( diff > 0)
	{
		if (ct != 0)
		  string += QString(" and %1 other%2").arg(diff).arg(diff > 1 ? "s " : " ");
		else
		  string += QString(" %1 people ").arg(diff);
	}
	
	if ( count > 0)
	  string += " commented on this.";
	  
	return string;
}


QString createPropertyString(const QList<PropertyInfoPtr> properties)
{
	QString string = "";
	
	foreach(PropertyInfoPtr property, properties)
	{
		QUrl url(property->href());
		url.setScheme("property");
		QString href = url.toString();
		QString name  = property->name().trimmed();
		string += QString("%1 %4 <a href=\"%3\">%2</a><br/>").arg(name).arg(property->text()).arg(href).arg(name.isEmpty() ? "" : " : " );
	}
	return string;
}

QString trimName(const QString name)
{
	QString trimName = name;
	
	if (name.length() > 20 )
	{
		trimName = name.mid(17) + "...";
	}
	
	return trimName;
}
