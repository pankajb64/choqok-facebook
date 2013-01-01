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

#include "facebookaccount.h"
#include "facebookmicroblog.h"
#include <KDebug>
#include <libkfbapi/userinfojob.h>

class FacebookAccount::Private
{
public:
    QString accessToken;
    QString id;
    QString name;
    QStringList timelineNames;
};

FacebookAccount::FacebookAccount(FacebookMicroBlog* parent, const QString& alias) : Account(parent, alias), d(new Private)
{
    kDebug()<<alias;
   d->accessToken = configGroup()->readEntry("AccessToken", QString());
   d->id = configGroup()->readEntry("Id", QString());
   d->name = configGroup()->readEntry("Name", QString());
   d->timelineNames = configGroup()->readEntry("Timelines", QStringList());

   if( d->timelineNames.isEmpty() ){
        QStringList list = parent->timelineNames();
        list.removeOne("Profile");
        d->timelineNames = list;
    }

    QStringList lists;

    foreach(const QString & tm, timelineNames()){
        if(tm.contains('/'))
            lists.append(tm);
    }

    if(!lists.isEmpty())
        parent->setUserTimelines(this, lists);
}

FacebookAccount::~FacebookAccount()
{
  delete d;
}

void FacebookAccount::writeConfig()
{
    configGroup()->writeEntry ("AccessToken", d->accessToken);
    configGroup()->writeEntry ("Name", d->name);
    configGroup()->writeEntry ("Id", d->id);
    configGroup()->writeEntry("Timelines", d->timelineNames);
    Choqok::Account::writeConfig();
    //configGroup()->sync();
    //emit modified(this);
}

void FacebookAccount::setAccessToken (const QString& accessToken)
{
  d->accessToken = accessToken;
}

QString FacebookAccount::accessToken() const
{
  return d->accessToken;
}

void FacebookAccount::setName (const QString& name)
{
  d->name = name;
}

QString FacebookAccount::name() const
{
  return d->name;
}
void FacebookAccount::setId (const QString& id)
{
  d->id = id;
}

QString FacebookAccount::id() const
{
  return d->id;
}

QStringList FacebookAccount::timelineNames() const
{
    return d->timelineNames;
}

void FacebookAccount::setTimelineNames(const QStringList& list)
{
    d->timelineNames.clear();
    foreach(const QString &name, list){
        if(microblog()->timelineNames().contains(name))
            d->timelineNames<<name;
    }
}

#include "facebookaccount.moc"
