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

#ifndef FACEBOOKMICROBLOG_H
#define FACEBOOKMICROBLOG_H
#include "microblog.h"
#include <libkfbapi/facebookjobs.h>
#include <libkfbapi/postinfo.h>
#include <libkfbapi/userinfo.h>
#include <libkfbapi/notificationinfo.h>
#include "facebookpost.h"
#include <QStringList>
#include "facebookinputdialog.h"

using namespace KFbAPI;

class FacebookAccount;
//class KJob;
class FacebookMicroBlog : public Choqok::MicroBlog
{
  Q_OBJECT

  public :
    FacebookMicroBlog(QObject * parent,/*const char *name,*/ const QVariantList& args);
    virtual ~FacebookMicroBlog();

    virtual ChoqokEditAccountWidget* createEditAccountWidget(Choqok::Account* account, QWidget* parent);
    virtual Choqok::UI::PostWidget* createPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent);
    virtual Choqok::UI::ComposerWidget * createComposerWidget( Choqok::Account *account, QWidget *parent );
    virtual void createPost(Choqok::Account* theAccount, Choqok::Post* post);
    virtual void abortCreatePost(Choqok::Account* theAccount, Choqok::Post* post = 0);
    virtual void fetchPost(Choqok::Account* theAccount, Choqok::Post* post);
    virtual void removePost(Choqok::Account* theAccount, Choqok::Post* post);
    virtual void saveTimeline(Choqok::Account* account, const QString& timelineName, const QList< Choqok::UI::PostWidget* >& timeline);
    virtual QList< Choqok::Post* > loadTimeline(Choqok::Account* account, const QString& timelineName);
    virtual Choqok::Account* createNewAccount(const QString& alias);
    virtual void updateTimelines(Choqok::Account* theAccount);
    virtual void requestTimeline(Choqok::Account* theAccount, QString timelineName, QString sinceTime);
    virtual void requestNotification(Choqok::Account* theAccount);
    virtual Choqok::TimelineInfo* timelineInfo(const QString& timelineName);
    virtual void setTimelineInfos();
    virtual void setUserTimelines(FacebookAccount* theAccount, const QStringList& lists);
    virtual void aboutToUnload();
    virtual QList <Choqok::Post * > toChoqokPost(FacebookAccount* account, QList<PostInfo> mPosts) const;
    virtual QList <Choqok::Post * > toChoqokPost(FacebookAccount* account, QList<NotificationInfo> notifications) const;
    virtual Choqok::User toChoqokUser(FacebookAccount* account, UserInfo userInfo) const;
    virtual QString profileUrl(Choqok::Account* account, const QString& username) const;
    virtual QString postUrl(Choqok::Account* account, const QString& username, const QString& postId) const;
    virtual QString facebookUrl(Choqok::Account* account, const QString& username) const;
    virtual uint postCharLimit() const;
    void createPostWithAttachment(Choqok::Account *theAccount, Choqok::Post *post, const QString &mediumToAttach = QString());
    virtual QMenu* createActionsMenu(Choqok::Account* theAccount, QWidget* parent = Choqok::UI::Global::mainWindow());

protected  slots :
  void slotCreatePost(KJob* job);
  void slotTimeLineLoaded(KJob *job);
  void slotNotificationLoaded(KJob* job);
  virtual void showPrivateMessageDialog( FacebookAccount *theAccount = 0, const QString &toUsername = QString() );
  virtual void showAddTimelineDialog(FacebookAccount *theAccount=0, const QString &toUsername=QString());
  virtual void slotInputEntered(FacebookAccount *theAccount, QString id);
  virtual void userInfoJobDone(KJob* job);

  private:
     QMap<KJob*, FacebookAccount*> mJobsAccount;
     QMap<KJob*, Choqok::Post*> mJobsPost;
     QMap<QString, Choqok::TimelineInfo*> mTimelineInfos;//timelineName, Info
     QHash< Choqok::Account*, QMap<QString, QString> > mTimelineUpdateTime;//TimelineType, UpdateTime
     QMap<KJob*, QString> mJobsTimeline;
     int countOfTimelinesToSave;
     FacebookInputDialog* dialog;

};

#endif
