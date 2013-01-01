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

#include "facebookmicroblog.h"
#include <KAboutData>
#include <KGenericFactory>
#include "facebookaccount.h"
#include <KMessageBox>
#include <libkfbapi/postinfo.h>
#include <libkfbapi/postjob.h>
#include <libkfbapi/postaddjob.h>
#include <libkfbapi/postslistjob.h>
#include "accountmanager.h"
#include "editaccountwidget.h"
#include "facebookeditaccountwidget.h"
#include "postwidget.h"
#include "facebookpostwidget.h"
#include <application.h>
#include "facebookutil.h"
#include <notifymanager.h>
#include "facebookcomposerwidget.h"
#include <kio/netaccess.h>
#include <kmimetype.h>
#include "mediamanager.h"
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <QtOAuth/qoauth_namespace.h>
#include <QtOAuth/QtOAuth>
#include <KAction>
#include <QMenu>
#include "facebookviewdialog.h"
#include <libkfbapi/notificationslistjob.h>
#include <libkfbapi/userinfojob.h>

K_PLUGIN_FACTORY( MyPluginFactory, registerPlugin < FacebookMicroBlog > (); )
K_EXPORT_PLUGIN( MyPluginFactory( "choqok_facebook" ) )

FacebookMicroBlog::FacebookMicroBlog( QObject *parent, const QList<QVariant> & args ): Choqok::MicroBlog( MyPluginFactory::componentData(), parent)
{
//...
kDebug();
    //KConfigGroup grp(KGlobal::config(), "Facebook");
    //format = grp.readEntry("format", "xml");

    setCharLimit(postCharLimit());
    QStringList timelineTypes;
    timelineTypes<< "Home" << "Profile" << "Notifications" ;
    setTimelineNames(timelineTypes);
    setTimelineInfos();
    setServiceName("Facebook");
    setServiceHomepageUrl("https://www.facebook.com/");
}

void FacebookMicroBlog::setTimelineInfos()
{
    Choqok::TimelineInfo *t = new Choqok::TimelineInfo;
    t->name = i18nc("Timeline Name", "Home");
    t->description = i18nc("Timeline description", "Your Facebook Home");
    t->icon = "user-home";
    mTimelineInfos["Home"] = t;

    t = new Choqok::TimelineInfo;
    t->name = i18nc("Timeline Name", "Profile");
    t->description = i18nc("Timeline description", "Your Facebook Profile Feed");
    t->icon = "system-users";
    mTimelineInfos["Profile"] = t;

    t = new Choqok::TimelineInfo;
    t->name = i18nc("Timeline Name", "Notifications");
    t->description = i18nc("Timeline description", "Your incoming notifications");
    t->icon = "hook-notifier";
    mTimelineInfos["Notifications"] = t;

}

void FacebookMicroBlog::setUserTimelines(FacebookAccount* theAccount, const QStringList& lists)
{
	QStringList tms = theAccount->timelineNames();

    foreach(const QString &name, lists){
        tms.append(name);
        addTimelineName(name);
    }
    tms.removeDuplicates();
    theAccount->setTimelineNames(tms);
}

FacebookMicroBlog::~FacebookMicroBlog()
{
}


Choqok::UI::PostWidget* FacebookMicroBlog::createPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent)
{
    return new FacebookPostWidget(account, post, parent);
}

void FacebookMicroBlog::createPost(Choqok::Account* theAccount, Choqok::Post* post)
{
    kDebug()<<"Creating a new Post for " <<theAccount;
    FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount);
    PostAddJob* job = new PostAddJob(post->content, acc->accessToken());
    mJobsAccount.insert(job, acc);
    mJobsPost.insert(job, post);
    connect( job, SIGNAL(result(KJob*)), this, SLOT(slotCreatePost(KJob*)) );
    job->start();

}

void FacebookMicroBlog::slotCreatePost(KJob* job)
{
    FacebookAccount* acc = mJobsAccount.take(job);
    Choqok::Post* post = mJobsPost.take(job);
    if ( post->isError || job->error() ) {

        kError() << "Server Error:" ;
        Choqok::NotifyManager::error( job->errorString(), i18n("Failed to submit new post "));
        emit errorPost ( acc, post, Choqok::MicroBlog::ServerError, i18n ( "Creating the new post failed, with error" ), MicroBlog::Critical );
   } else {
       Choqok::NotifyManager::success(i18n("New post submitted successfully"));
       emit postCreated ( acc, post );
   }

    //emit postCreated ( acc, post );
}
void FacebookMicroBlog::abortCreatePost(Choqok::Account* theAccount, Choqok::Post* post)
{
    FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount);
    Q_UNUSED(post);

    PostAddJob* job = (PostAddJob *)mJobsAccount.key(acc);

    if(job)
      job->abort();
}

void FacebookMicroBlog::fetchPost(Choqok::Account* theAccount, Choqok::Post* post)
{
    Q_UNUSED(theAccount);
    Q_UNUSED(post);
    KMessageBox::sorry(choqokMainWindow, i18n("Not Supported"));
}

void FacebookMicroBlog::removePost(Choqok::Account* theAccount, Choqok::Post* post)
{
    Q_UNUSED(theAccount);
    Q_UNUSED(post);
    KMessageBox::sorry(choqokMainWindow, i18n("Not Supported"));
}

Choqok::Account* FacebookMicroBlog::createNewAccount(const QString& alias)
{

kDebug() << "Creating a new Facebook Account";
FacebookAccount *acc = qobject_cast<FacebookAccount*>( Choqok::AccountManager::self()->findAccount(alias) );
    if(!acc) {
        return new FacebookAccount(this, alias);
    } else {
        return 0;//If there's an account with this alias, So We can't create a new one
    }
}

ChoqokEditAccountWidget* FacebookMicroBlog::createEditAccountWidget(Choqok::Account* account, QWidget* parent)
{
    kDebug();
    FacebookAccount *acc = qobject_cast<FacebookAccount*>(account);
    if(acc || !account)
        return new FacebookEditAccountWidget(this, acc, parent);
    else{
        kDebug()<<"Account passed here was not a valid Facebook Account!";
        return 0L;
    }
}

Choqok::UI::ComposerWidget* FacebookMicroBlog::createComposerWidget(Choqok::Account* account, QWidget* parent)
{
    return new FacebookComposerWidget(account, parent);
}

Choqok::TimelineInfo* FacebookMicroBlog::timelineInfo(const QString& timelineName)
{
  kDebug() << timelineName;
  if(mTimelineInfos.contains(timelineName))
  {
	if(true || isValidTimeline(timelineName))
	{
		 return mTimelineInfos.value(timelineName);
	}
  }

  if(timelineName.contains("/"))
  {

	 if(mTimelineInfos.contains(timelineName))
     {
         return mTimelineInfos.value(timelineName);
     }

     Choqok::TimelineInfo *info = new Choqok::TimelineInfo;
     info->name = timelineName;
     info->description = QString("%1 - News Feed").arg(timelineName.split("/")[0]);
     info->icon = "format-list-unordered";
     mTimelineInfos.insert(timelineName, info);
     return info;
  }

  return 0;

}

QList< Choqok::Post* > FacebookMicroBlog::loadTimeline(Choqok::Account* account, const QString& timelineName)
{

    kDebug()<<timelineName;
    QList< Choqok::Post* > list;
    if (timelineName == "Notifications")
      return list;

    QString fileName = Choqok::AccountManager::generatePostBackupFileName(account->alias(), timelineName);
    kDebug() << "Backup File Name - " << fileName;
    KConfig postsBackup( "choqok/" + fileName, KConfig::NoGlobals, "data" );
    QStringList tmpList = postsBackup.groupList();

    QList<QDateTime> groupList;
    foreach(const QString &str, tmpList)
        groupList.append(QDateTime::fromString(str) );
    qSort(groupList);
    int count = groupList.count();
    if( count ) {
        FacebookPost *st = 0;
        for ( int i = 0; i < count; ++i ) {
            st = new FacebookPost;
            KConfigGroup grp( &postsBackup, groupList[i].toString() );
            st->creationDateTime = grp.readEntry( "creationDateTime", QDateTime::currentDateTime() );
			st->postId = grp.readEntry( "postId", QString() );
            st->replyToPostId = grp.readEntry( "inReplyToPostId", QString() );
            st->title = grp.readEntry( "title", QString() );
            st->source = grp.readEntry( "source", QString() );
            st->link = grp.readEntry( "link", QString() );
            st->content = grp.readEntry( "text", QString() );
			st->type = grp.readEntry( "type", QString() );
			st->replyToUserId = grp.readEntry( "replyToUserId", QString() );
            st->author.userId = grp.readEntry( "authorId", QString() );
            st->author.realName = grp.readEntry( "authorRealName", QString() );
            st->author.profileImageUrl = grp.readEntry( "profileImageUrl", QString() );
            st->caption = grp.readEntry( "caption", QString() );
            st->description = grp.readEntry( "description", QString() );
            st->iconUrl = grp.readEntry( "iconUrl", QString() );
            //st->properties = grp.readEntry( "properties", QList<PropertyInfoPtr>() );
            st->likeCount = grp.readEntry( "likeCount", QString() );
            st->likeString = grp.readEntry( "likeString", QString() );
            st->story = grp.readEntry( "story", QString() );
            st->commentCount = grp.readEntry( "commentCount", QString() );
            st->commentString = grp.readEntry( "commentString", QString() );
            st->propertyString = grp.readEntry( "propertyString", QString() );
            st->appName = grp.readEntry( "appName", QString() );
            st->appId = grp.readEntry( "appId", QString() );
            st->updateDateTime = grp.readEntry( "updateDateTime", QDateTime::currentDateTime() );
            st->isRead = grp.readEntry("isRead", true);
            st->conversationId = grp.readEntry("conversationId", QString());
            st->isFavorited = grp.readEntry("isFavorited", true);
			//Choqok::Post* post = *st;

            list.append( st );
        }

        mTimelineUpdateTime[account][timelineName] = st->updateDateTime.toString(Qt::ISODate);
    }
    return list;
}


void FacebookMicroBlog::saveTimeline(Choqok::Account* account, const QString& timelineName, const QList< Choqok::UI::PostWidget* >& timeline)
{
	if (timelineName != "Notifications")
	{
		kDebug();
		QString fileName = Choqok::AccountManager::generatePostBackupFileName(account->alias(), timelineName);
		KConfig postsBackup( "choqok/" + fileName, KConfig::NoGlobals, "data" );

		///Clear previous data:
		QStringList prevList = postsBackup.groupList();
		int c = prevList.count();
		if ( c > 0 ) {
			for ( int i = 0; i < c; ++i ) {
				postsBackup.deleteGroup( prevList[i] );
			}
		}
		QList< Choqok::UI::PostWidget *>::const_iterator it, endIt = timeline.constEnd();
		for ( it = timeline.constBegin(); it != endIt; ++it ) {
			FacebookPost *post = static_cast<FacebookPost*>((*it)->currentPost());
			KConfigGroup grp( &postsBackup, post->creationDateTime.toString() );
			grp.writeEntry( "creationDateTime", post->creationDateTime );
			grp.writeEntry( "postId", post->postId.toString() );
			grp.writeEntry( "title", post->title );
			grp.writeEntry( "source", post->source );
			grp.writeEntry( "link", post->link );
			grp.writeEntry( "text", post->content );
			grp.writeEntry( "type", post->type );
			grp.writeEntry( "replyToUserId", post->replyToUserId.toString() );
			grp.writeEntry( "inReplyToPostId", post->replyToPostId.toString() );
			grp.writeEntry( "authorId", post->author.userId.toString() );
			grp.writeEntry( "authorRealName", post->author.realName );
			grp.writeEntry( "profileImageUrl", post->author.profileImageUrl );
			grp.writeEntry( "isRead" , post->isRead );
			grp.writeEntry( "conversationId", post->conversationId.toString() );
			grp.writeEntry( "caption", post->caption );
			grp.writeEntry( "description", post->description );
			grp.writeEntry( "iconUrl", post->iconUrl );
			grp.writeEntry( "likeCount", post->likeCount );
			grp.writeEntry( "likeString", post->likeString );
			grp.writeEntry( "story", post->story );
			grp.writeEntry( "commentCount", post->commentCount );
			grp.writeEntry( "commentString", post->commentString );
			grp.writeEntry( "propertyString", post->propertyString );
			grp.writeEntry( "appName", post->appName );
			grp.writeEntry( "appId", post->appId.toString() );
			grp.writeEntry( "updateDateTime", post->updateDateTime );
			grp.writeEntry( "isFavorited", post->isFavorited );
		}
		postsBackup.sync();
	}
	if(Choqok::Application::isShuttingDown()) {
        --countOfTimelinesToSave;
        if(countOfTimelinesToSave < 1)
            emit readyForUnload();
    }
}

void FacebookMicroBlog::updateTimelines(Choqok::Account * theAccount)
{
  kDebug();

    foreach ( const QString &tm, theAccount->timelineNames() ) {
		if (tm == "Notifications")
		  requestNotification(theAccount);
		else
          requestTimeline( theAccount, tm, mTimelineUpdateTime[theAccount][tm] );
    }
}

void FacebookMicroBlog::requestTimeline(Choqok::Account *theAccount, QString timelineName, QString sinceTime)
{
	FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount);

    if(!acc){
        kError()<<"FacebookMicroBlog::updateTimelines: acc is not an FacebookAccount";
        return;
    }

    PostsListJob * job;
    if (timelineName == "Home")
      job = new PostsListJob(acc->accessToken());
    else if (timelineName == "Profile")
      job = new PostsListJob(acc->id(), acc->accessToken());
    else
      {
		  QString userId = timelineName.split("/")[1];
		  job = new PostsListJob(userId, acc->accessToken());
	  }
    job->addQueryItem("since", sinceTime);
    mJobsAccount.insert(job, acc);
    mJobsTimeline.insert(job, timelineName);
    connect( job, SIGNAL(result(KJob*)), this, SLOT(slotTimeLineLoaded(KJob*)) );
    job->start();

}
void FacebookMicroBlog::slotTimeLineLoaded(KJob *job)
{
  kDebug();

    FacebookAccount* acc = mJobsAccount.take(job);
    if ( job->error() ) {
        kDebug() << "Job Error: " << job->errorString();
        emit error( acc, CommunicationError, i18n("Timeline update failed, %1", job->errorString()), Low );

    }else {
	QString tm = mJobsTimeline.take(job);
	   QList<Choqok::Post*>list = toChoqokPost(acc, ((PostsListJob *)job)->posts() );
	   //QList<Choqok::Post>& l = list;
        emit timelineDataReceived( acc, tm, reverseList(list));
    }
}

void FacebookMicroBlog::requestNotification(Choqok::Account* theAccount)
{
	FacebookAccount* acc = qobject_cast<FacebookAccount*>(theAccount);

    if(!acc){
        kError()<<"FacebookMicroBlog::updateTimelines: acc is not an FacebookAccount";
        return;
    }

    NotificationsListJob* job = new NotificationsListJob(acc->accessToken());
    mJobsAccount.insert(job, acc);
    connect( job, SIGNAL(result(KJob*)), this, SLOT(slotNotificationLoaded(KJob*)) );
    job->start();
}

void FacebookMicroBlog::slotNotificationLoaded(KJob* job)
{
	FacebookAccount* acc = mJobsAccount.take(job);

	if ( job->error() ) {
        kDebug() << "Job Error: " << job->errorString();
        emit error( acc, CommunicationError, i18n("Notification update failed, %1", job->errorString()), Low );
    }
    else {
	   QString tm = "Notifications";
	   QList<Choqok::Post*>list = toChoqokPost(acc, ((NotificationsListJob *)job)->notifications() );
       emit timelineDataReceived( acc, tm, reverseList(list));
    }
}

void FacebookMicroBlog::aboutToUnload()
{
    countOfTimelinesToSave = 0;
    foreach(Choqok::Account* acc, Choqok::AccountManager::self()->accounts()){
        if(acc->microblog() == this){
//             acc->writeConfig();
            countOfTimelinesToSave += acc->timelineNames().count();
        }
    }
    emit saveTimelines();
}

QString FacebookMicroBlog::profileUrl (Choqok::Account* acc, const QString& userId) const
{
	FacebookAccount* account = qobject_cast<FacebookAccount*>(acc);

    if(!account){
        kError()<<"FacebookMicroBlog::profileUrl: acc is not an FacebookAccount";
        return QString();
    }

	return QString("user://%1").arg(userId);
}

QString FacebookMicroBlog::postUrl(Choqok::Account*, const QString& username, const QString& postId) const
{
    QStringList list = postId.split("_");
    return QString ( "http://www.facebook.com/%1/posts/%2" ).arg ( list.at(0) ).arg ( list.at(1) );
}

QString FacebookMicroBlog::facebookUrl(Choqok::Account* acc, const QString& userId) const
{
	FacebookAccount* account = qobject_cast<FacebookAccount*>(acc);

    if(!account){
        kError()<<"FacebookMicroBlog::facebookUrl: acc is not an FacebookAccount";
        return QString();
    }
	return QString("http://www.facebook.com/profile.php?id=%1").arg(userId);
}


QList<Choqok::Post *> FacebookMicroBlog::toChoqokPost(FacebookAccount* account, QList<PostInfo> mPosts) const
{

  QList<Choqok::Post*> list ;
  PostInfo p;

  foreach ( p, mPosts)
  {
	  PostInfo * postInfo = &p;
	  FacebookPost * post = new FacebookPost ();
	  post->postId = assignOrNull(postInfo->id());
	  post->author = toChoqokUser( account, postInfo->from());
	  post->author.profileImageUrl = "https://graph.facebook.com/" + postInfo->from().id() + "/picture" ;
	  post->content = assignOrNull(postInfo->message());
	  post->link = assignOrNull(postInfo->link().toString());
	  post->title = assignOrNull(postInfo->name());
	  post->caption = assignOrNull(postInfo->caption());
	  post->description = assignOrNull(postInfo->description());
	  post->iconUrl = assignOrNull(postInfo->pictureUrl().toString()); //assignOrNull(postInfo->icon());
	  //post->properties = postInfo->properties();
	  post->type = assignOrNull(postInfo->type());
	  post->source = assignOrNull(postInfo->sourceUrl().toString());
      post->likeCount = QString::number(postInfo->likes().count()); //+ " likes";
	  QString likeString =  createLikeString(account, postInfo->likes());
	  likeString += " Click to see who all like this";
	  post->likeString = likeString;
	  post->story = assignOrNull(postInfo->story());
	  post->commentCount = QString::number(postInfo->comments().count()); // + " comments";
	  QString commentString = createCommentString(account, postInfo->comments());
	  commentString += 	" Click to see all comments";
	  post->commentString = commentString;
	  post->propertyString = createPropertyString(postInfo->properties());
	  post->appId = postInfo->application().id();
	  post->appName =  postInfo->application().name();
	  post->creationDateTime = postInfo->createdTime().dateTime();
	  post->updateDateTime = postInfo->updatedTime().dateTime();
	  post->isRead = (postInfo->from().id() == account->id());

      //post->content = prepareStatus(post);

	  list.append(post);
  }

  return list;
}

Choqok::User FacebookMicroBlog::toChoqokUser(FacebookAccount* account, UserInfo userInfo) const
{
	Choqok::User * user = new Choqok::User();

	user->userId = userInfo.id();
	user->userName = userInfo.username().isNull() ? userInfo.id() : userInfo.username() ;
	user->realName = (account->id() == userInfo.id()) ? "You" : userInfo.name();

	return *user;
}


QList<Choqok::Post*> FacebookMicroBlog::toChoqokPost(FacebookAccount* account, QList<NotificationInfo> notifications) const
{
  QList<Choqok::Post*> list ;
  NotificationInfo n;

  foreach ( n, notifications)
  {
	  NotificationInfo * notificationInfo = &n;
	  FacebookPost * post = new FacebookPost ();
	  post->postId = assignOrNull(notificationInfo->id());
	  post->author = toChoqokUser( account, notificationInfo->from());
	  post->author.profileImageUrl = "https://graph.facebook.com/" + notificationInfo->from().id() + "/picture" ;
	  post->title = assignOrNull(notificationInfo->title());
	  post->link = assignOrNull(notificationInfo->link().toString());
	  post->type = "notification";
	  post->appId = notificationInfo->application().id();
	  post->appName = notificationInfo->application().name();
	  post->creationDateTime = notificationInfo->createdTime().dateTime();
	  post->updateDateTime = notificationInfo->updatedTime().dateTime();
	  post->isRead = !(notificationInfo->unread());

      //post->content = prepareStatus(post);

	  list.append(post);
  }

  return list;
}

uint FacebookMicroBlog::postCharLimit() const
{
	return 63206;
}
void FacebookMicroBlog::createPostWithAttachment(Choqok::Account* theAccount, Choqok::Post* post,
                                                 const QString& mediumToAttach)
{
    if( mediumToAttach.isEmpty() ){
        createPost(theAccount, post);
    } else {
        QByteArray picData;
        QString tmp;
        KUrl picUrl(mediumToAttach);
        KIO::TransferJob *picJob = KIO::get( picUrl, KIO::Reload, KIO::HideProgressInfo);
        if( !KIO::NetAccess::synchronousRun(picJob, 0, &picData) ){
            kError()<<"Job error: " << picJob->errorString();
            KMessageBox::detailedError(Choqok::UI::Global::mainWindow(),
                                       i18n( "Uploading medium failed: cannot read the medium file." ),
            picJob->errorString() );
            return;
        }
        if ( picData.count() == 0 ) {
            kError() << "Cannot read the media file, please check if it exists.";
            KMessageBox::error( Choqok::UI::Global::mainWindow(),
                                i18n( "Uploading medium failed: cannot read the medium file." ) );
            return;
        }
        ///Documentation: https://developers.facebook.com/docs/reference/api/photo/, https://developers.facebook.com/docs/reference/api/video/
        FacebookAccount* account = qobject_cast<FacebookAccount*>(theAccount);

        QString mimeType = KMimeType::findByUrl( picUrl, 0, true )->name();
        QByteArray fileContentType = mimeType.toUtf8();;

        QString uploadUrl;
        if ( mimeType.contains("video"))
          uploadUrl = QString("https://graph-video.facebook.com/%1/videos");
        else
          uploadUrl = QString("https://graph.facebook.com/%1/photos");

        QString me = post->author.userId.compare("") == 0 ? QString("me") : post->author.userId;
        uploadUrl = uploadUrl.arg(me);
        KUrl url(uploadUrl);


        /*QUrl url(mediumToAttach);
		KFileItem item(KFileItem::Unknown, KFileItem::Unknown, url, true);
		mimeType = item.mimetype();*/

        QMap<QString, QByteArray> formdata;
        formdata["message"] = post->content.toUtf8();
        //formdata["description"] = post->content.toUtf8();
        formdata["access_token"] = account->accessToken().toUtf8();
        //formdata["source"] = picData;

        QMap<QString, QByteArray> mediafile;
        mediafile["name"] = "source";
        mediafile["filename"] = picUrl.fileName().toUtf8();
        mediafile["mediumType"] = fileContentType;
        mediafile["medium"] = picData;
        QList< QMap<QString, QByteArray> > listMediafiles;
        listMediafiles.append(mediafile);

        QByteArray data = Choqok::MediaManager::createMultipartFormData(formdata, listMediafiles);

        KIO::StoredTransferJob *job = KIO::storedHttpPost(data, url, KIO::HideProgressInfo) ;
        if ( !job ) {
            kError() << "Cannot create a http POST request!";
            return;
        }
        job->addMetaData( "content-type", "Content-Type: multipart/form-data; boundary=AaB03x" );
        //job->addMetaData("customHTTPHeader", "Authorization: " + authorizationHeader(account, url, QOAuth::POST));
        mJobsPost.insert(job, post);
        mJobsAccount.insert(job, account);
        connect( job, SIGNAL( result( KJob* ) ),
                 SLOT( slotCreatePost(KJob*) ) );
        job->start();
    }
}

QMenu* FacebookMicroBlog::createActionsMenu(Choqok::Account* theAccount, QWidget* parent)
{
    QMenu * menu = MicroBlog::createActionsMenu(theAccount, parent);

    KAction *directMessge = new KAction( KIcon("mail-message-new"), i18n("Send Private Message..."), menu );
    directMessge->setData( theAccount->alias() );
    connect( directMessge, SIGNAL(triggered(bool)), SLOT(showPrivateMessageDialog()) );
    menu->addAction(directMessge);

    KAction *addTimline = new KAction( KIcon("appointment-new"), i18n("Add a new Timeline...."), menu );
    addTimline->setData( theAccount->alias() );
    connect( addTimline, SIGNAL(triggered(bool)), SLOT(showAddTimelineDialog()) );
    menu->addAction(addTimline);

    return menu;
}

void FacebookMicroBlog::showPrivateMessageDialog( FacebookAccount *theAccount, const QString &toUsername)
{
    kDebug();
    if( !theAccount ) {
        KAction *act = qobject_cast<KAction *>(sender());
        theAccount = qobject_cast<FacebookAccount*>( Choqok::AccountManager::self()->findAccount( act->data().toString() ) );
    }
    QString appId = FacebookEditAccountWidget::appID();
    QString urlString = QString("https://www.facebook.com/dialog/send?app_id=%1&to=%2&link=http://choqok.gnufolks.org&redirect_uri=http://choqok.gnufolks.org/").arg(appId).arg(toUsername);
    QUrl url(urlString);
    //QUrl choqokUrl("http://choqok.gnufolks.org");
    FacebookViewDialog * dialog = new FacebookViewDialog(url, Choqok::UI::Global::mainWindow(), "http://choqok.gnufolks.org");
    dialog->start();

}

void FacebookMicroBlog::showAddTimelineDialog(FacebookAccount *theAccount, const QString &toUsername)
{
	if( !theAccount ) {
        KAction *act = qobject_cast<KAction *>(sender());
        theAccount = qobject_cast<FacebookAccount*>(
                                    Choqok::AccountManager::self()->findAccount( act->data().toString() ) );
    }

	dialog = new FacebookInputDialog(theAccount, "Enter the \"Facebook Username\" OR \"Facebook Id\" of the Person/Group/Page whose feed you want to show on Choqok (Please note that we do not support lists yet)", "Add New Timeline", "Enter input here and press return or click OK", false, Choqok::UI::Global::mainWindow());
	connect(dialog, SIGNAL(inputEntered(FacebookAccount*, QString)), this, SLOT(slotInputEntered(FacebookAccount*, QString)));
	dialog->show();
}

void FacebookMicroBlog::slotInputEntered(FacebookAccount *theAccount, QString id)
{
	disconnect(dialog, SIGNAL(inputEntered(FacebookAccount*, QString)), this, SLOT(slotInputEntered(FacebookAccount*, QString)));

	if (id.length() < 5 || id.contains(QRegExp("[^a-z0-9\\.]", Qt::CaseInsensitive ) ) )
	{
		KMessageBox::sorry(choqokMainWindow, i18n("Invalid Facebook Id. You can enter only alphanumeric character and a period"));
		return;
	}

	if ( id.compare(theAccount->id(), Qt::CaseInsensitive) == 0  )
	{
		KMessageBox::information(choqokMainWindow, i18n("This is your own Facebook Id. No Timelines added."));
	}

	else
	{
		id = id.toLower();
		UserInfoJob *job = new UserInfoJob(id, theAccount->accessToken());
		connect(job, SIGNAL(result(KJob*)), this, SLOT(userInfoJobDone( KJob*)) );
		mJobsAccount.insert(job, theAccount);
		job->start();
	}
}

void FacebookMicroBlog::userInfoJobDone(KJob* job)
{


	if (job->error())
	{
		KMessageBox::sorry(choqokMainWindow, i18n("Error retrievind Facebook User Info. The server said : %1").arg(job->errorString()));
		return;
	}
	UserInfoJob* uJob = dynamic_cast<UserInfoJob*>(job);

	FacebookAccount* theAccount = mJobsAccount.take(job);

	UserInfo userInfo = uJob->userInfo();

	if (theAccount->id().compare(userInfo.id(), Qt::CaseInsensitive) == 0)
	{
		KMessageBox::information(choqokMainWindow, i18n("This is your own Facebook Id. No Timelines added."));
	}

	else
	{
		QStringList tms = theAccount->timelineNames();
		QString timelineName = QString("%1/%2").arg(userInfo.name(),  userInfo.id());
		tms.append(timelineName);
		addTimelineName(timelineName);
		theAccount->setTimelineNames(tms);
		theAccount->writeConfig();
		updateTimelines(theAccount);
	}
}
#include "facebookmicroblog.moc"
