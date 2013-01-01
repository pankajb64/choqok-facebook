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

#ifndef FACEBOOKEDITACCOUNT_H
#define FACEBOOKEDITACCOUNT_H

#include "editaccountwidget.h"
#include <QWidget>
#include "ui_facebookeditaccount_base.h"

namespace QOAuth {
class Interface;
}

class KJob;
class QProgressBar;
class FacebookAccount;
class FacebookMicroBlog;


class FacebookEditAccountWidget : public ChoqokEditAccountWidget, public Ui::FacebookEditAccountBase
{
    Q_OBJECT
public:
    FacebookEditAccountWidget(FacebookMicroBlog *microblog, FacebookAccount* account, QWidget *parent);

    /**
    * Destructor
    */
    virtual ~FacebookEditAccountWidget();


    virtual bool validateData();
    /**
    * Create a new account if we are in the 'add account wizard',
    * otherwise update the existing account.
    * @Return new or modified account. OR 0L on failure.
    */
    virtual Choqok::Account *apply();

    /**
    * Returns the API Key, APP ID and APP SECRET
    */

    static QString appID() ;
    static QString apiKey();
    static QString appSecret();

protected slots:
    virtual void userInfoJobDone(KJob* job);
    virtual void authenticationDone(const QString& accessToken);
    virtual void showAuthenticationDialog();
    virtual void authenticationCancelled();

protected:

    virtual void updateUserName();
    void loadTimelinesTableState();
    void saveTimelinesTableState();
    void setAuthenticated(bool authenticated);

    bool isAuthenticated;
    bool mTriggerSync;
    FacebookMicroBlog *mBlog;
    FacebookAccount *mAccount;
    QString username;
    QString token;
    QString name;
    QString id;
};

#endif
