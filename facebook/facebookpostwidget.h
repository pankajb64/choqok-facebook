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

#ifndef FACEBOOKPOSTWIDGET_H
#define FACEBOOKPOSTWIDGET_H

#include "facebookaccount.h"
#include "facebookpost.h"

#include <postwidget.h>

#include <KJob>

using namespace KFbAPI;

/*namespace Choqok
{
  namespace UI
  {
	class PostWidget;
  }
}*/

class FacebookPostWidget : public Choqok::UI::PostWidget {

    Q_OBJECT

    public:
    FacebookPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent = 0);

    protected slots:
    void slotImageFetched(const QString& remoteUrl, const QPixmap& pixmap);
    void checkAnchor(const QUrl& link);
    virtual void slotLike();
    virtual void slotViewLikes();
    virtual void slotComment();
    virtual void slotViewComments();
    virtual void slotLiked(KJob* job);
    virtual void slotCommented(KJob* job);
    virtual void slotUpdateLikeCount(KJob* job);
    virtual void slotUpdateLikeString(KJob* job);
    virtual void slotUpdateCommentCount(KJob* job);
    virtual void slotUpdateCommentString(KJob* job);
    virtual void slotUpdateUserLike(KJob* job);
    virtual void markNotificationAsRead();
    virtual void slotMarkNotificationAsRead(KJob* job);
    void commented(FacebookAccount* theAccount, QString message);

    protected:
    virtual QString generateSign ();
    virtual QString prepareStatus( const QString &txt );
    void downloadImage(QString& linkUrl) const;
    void initUi();
    virtual bool isResendAvailable() ;
    virtual bool isRemoveAvailable() ;
    virtual void updateLikeAndCommentCounts();
    virtual void updateUi();
    virtual void setFavorite();
    virtual void updateFavStat();
    virtual void updateLikeCount();
    virtual void updateLikeString();
    virtual void updateCommentCount();
    virtual void updateCommentString();
    virtual void updateUserLike();
    virtual void reloadLikesAndComments();
    virtual bool isOwnPost();
    virtual bool isNotification();
    virtual void showStatusMessage(const QString message, const QString caption);

    static const KIcon unFavIcon;

    private:

    QString prepareLink (QString& link, QString& title, QString& caption, QString& description, QString& type ) const;
    QString likeUrl;
    QString wallStory;
};

#endif
