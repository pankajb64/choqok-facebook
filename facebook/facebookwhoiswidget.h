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

#ifndef FACEBOOKWHOISWIDGET_H
#define FACEBOOKWHOISWIDGET_H

#include "facebookaccount.h"
#include "facebookmicroblog.h"
#include "facebookutil.h"

#include <choqoktypes.h>
#include <choqokappearancesettings.h>
#include <choqoktools.h>
#include "mediamanager.h"
#include <microblog.h>
#include <notifymanager.h>

#include <libkfbapi/userinfojob.h>
#include <libkfbapi/userinfo.h>

#include <KApplication>
#include <KDebug>
#include <kicon.h>
#include <KIO/Job>
#include <KNotification>
#include <KProcess>
#include <KTextBrowser>
#include <KToolInvocation>
#include <KUrl>
#include <klocalizedstring.h>

#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtGui/QFrame>
#include <QtGui/QDesktopWidget>
#include <QtGui/QVBoxLayout>
#include <QtXml/QDomDocument>

class FacebookWhoisWidget : public QFrame
{
	Q_OBJECT

	public:
    FacebookWhoisWidget( FacebookAccount* theAccount, const QString &userId,
                           const Choqok::Post* post,  QWidget *parent=0 );

    void show(QPoint pos);

protected Q_SLOTS:
    void checkAnchor( const QUrl url );
    void userInfoReceived( KJob *job );
    void slotCancel();
    void avatarFetchError( const QString &remoteUrl, const QString &errMsg );
    void avatarFetched( const QString &remoteUrl, const QPixmap &pixmap );


protected:
    void updateHtml();

private:
    void setupUi();
    void showForm();
    void loadUserInfo( );

    KTextBrowser *wid;
    FacebookAccount *currentAccount;
    QFrame *waitFrame;
    UserInfoJob* mJob;
    const Choqok::Post* currentPost;
    QString userId;
    QString errorMessage;
    UserInfo* currentUserInfo;
};

#endif

