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

#include "facebookpostwidget.h"
#include <KDebug>
#include <KLocalizedString>
#include <KAction>
#include <KMenu>
#include <klocalizedstring.h>
#include <KUrl>
#include <mediamanager.h>
#include <textbrowser.h>
#include "facebookutil.h"
#include "facebookviewdialog.h"
#include "facebookwhoiswidget.h"
#include <KPushButton>
#include <QtGui/QtGui>
#include <QtGui/QLayout>
#include <KMessageBox>
#include <kfacebook/getlikesjob.h>
#include <kfacebook/getcommentsjob.h>
#include "facebookinputdialog.h"
#include <kfacebook/postjob.h>
#include <kfacebook/notificationsmarkreadjob.h>
#include "choqokuiglobal.h"

using namespace KFacebook;

const KIcon FacebookPostWidget::unFavIcon(Choqok::MediaManager::convertToGrayScale(KIcon("rating").pixmap(16)) );

FacebookPostWidget::FacebookPostWidget(Choqok::Account* account, Choqok::Post* post, QWidget* parent): PostWidget(account, post, parent)
{
	if(!isNotification())
	{
	  updateUserLike();
    }
	else
	{
		connect(this, SIGNAL(postReaded()), SLOT(markNotificationAsRead()) );
	}  
}

QString FacebookPostWidget::generateSign ()
{
	FacebookPost* post = static_cast<FacebookPost*>(currentPost());
	
    QString ss = "";
 
   ss += isOwnPost() ? "" : "<b>";
    
    ss += "<a href='"+ currentAccount()->microblog()->profileUrl( currentAccount(), post->author.userId ) 
		 +      +"' title=\"" +
    post->author.realName + "\">" ;
		
		 if (post->author.realName.isEmpty())
			ss += "Anonymous";
		else
			ss += removeTags(post->author.realName);
		ss += "</a> - ";
		ss += isOwnPost() ? "" : "</b>";
		ss +=  "via";

    //QStringList list = currentPost()->postId.split("_");
    
    /*ss += "<a href=\"http://www.facebook.com/" + list[0] + "/posts/" + list[1]
	 
	 + "\" title=\""
	 + currentPost().creationDateTime.toString(Qt::DefaultLocaleLongDate) + "\">%1</a>";*/
    
    if( !post->appId.isEmpty())
        ss += " <b> <a href=\"http://www.facebook.com/apps/application.php?id=" + post->appId.toString() + "\">" + removeTags(post->appName) + "</a></b> ";
    else
	    ss += " <b>web</b> ";	
    QString link = (isNotification() ? post->link : currentAccount()->microblog()->postUrl(currentAccount(), post->author.userName, post->postId));
    ss += ", <a href='"
	 + link
 	 + "' title='"
	 + post->creationDateTime.toString(Qt::DefaultLocaleLongDate) + "'>%1</a>";	
    return ss;

}

QString FacebookPostWidget::prepareStatus( const QString &txt ) 
{
	FacebookPost* post = static_cast<FacebookPost*>(currentPost());

    kDebug()<<"content: " << post->content;
    kDebug()<<"title: " << post->title;
    kDebug()<<"caption: " << post->caption;
    kDebug()<<"description: " << post->description;
    kDebug()<<"story: " << post->story;
    kDebug()<<"link: " << post->link;
	
	QString content = Choqok::UI::PostWidget::prepareStatus(post->content); 
	QString title = /*Choqok::UI::PostWidget::prepareStatus*/(post->title);
	QString caption = /*Choqok::UI::PostWidget::prepareStatus*/(post->caption);
	QString description = Choqok::UI::PostWidget::prepareStatus(post->description);
	QString story = Choqok::UI::PostWidget::prepareStatus(post->story);
	QString link = /*Choqok::UI::PostWidget::prepareStatus*/(post->link);
    QString status;
    
    //status += post->likeCount + " , " + post->commentCount + " <br/> ";
    
    if( !wallStory.isEmpty() )
        status += "<b>" + wallStory + "</b> <br/> ";
        
    if( !story.isEmpty() )
        status += "<b>" + story + "</b> <br/> ";
    if( !link.isEmpty() )
        status += prepareLink(link, title, caption, description, post->type) + "<br/>";
    if( !content.isEmpty() )
        status += content;
    
    if(!post->propertyString.isEmpty())
    {
		status += "<br/>" + post->propertyString + "<br/>";    
	}

    
	if (!post->iconUrl.isEmpty())
	{
	  QString iconUrlString = post->iconUrl;
	  QString urlString = "";
	  QUrl iconUrl(iconUrlString);
	  //status += QString("<br/>Host - %1, Path - %2<br/>").arg(iconUrl.host()).arg(iconUrl.path());
	  
	  /// Application Images and Images Outside Fb have to be dealt with seperately because Choqok::MediaManager cant download images from encoded URLS (huh)
	  if (iconUrl.host().contains("www.facebook.com") && iconUrl.path().contains("app_full_proxy.php"))
	  {
		  iconUrlString = QUrl::fromPercentEncoding(iconUrl.queryItemValue("src").toAscii());
	  }
	  if(iconUrl.host().contains("fbexternal-a.akamaihd.net") && iconUrl.path().contains("safe_image.php"))
	  {
		  iconUrlString = QUrl::fromPercentEncoding(iconUrl.queryItemValue("url").toAscii());
	  }
	  
      downloadImage(iconUrlString);
      QString imgUrl = getImageUrl(iconUrlString);
      //status += iconUrlString + "<br/> " + imgUrl + "<br/> ";
      status += QString("<br/><a href = \"%1\" > <img align='left'  src = \"%2\"/> </a><br/>").arg(imgUrl).arg(imgUrl);
    }
    
	  
   //QString status = Choqok::UI::PostWidget::prepareStatus(txt);
   kDebug()<< status;
   return status;
}

QString FacebookPostWidget::prepareLink(QString& link, QString& title, QString& caption, QString& description, QString& type) const
{
    if( title.isEmpty() )
    {
        if( !caption.isEmpty() ){
            title = caption;
            caption.clear();
        } else {
            //title = type;
        }
    }    
    QString link_title = link;
    QUrl url(link);
    url.setScheme("title");
    QString title_link = url.toString();
    if( !caption.isEmpty() )
        link_title = caption;
	QString linkHtml = QString("<a href =\"%1\" title='%3' ><b> %2 </b></a>").arg(title_link).arg(title).arg(link_title);
    if( !description.isEmpty() )
        linkHtml.append(QString("<br/>%1").arg(description));
	return linkHtml;
}

void FacebookPostWidget::downloadImage(QString& linkUrl) const
{
	connect ( Choqok::MediaManager::self(), SIGNAL(imageFetched(QString, QPixmap)), SLOT(slotImageFetched(QString, QPixmap)) );
	
	Choqok::MediaManager::self()->fetchImage(linkUrl, Choqok::MediaManager::Async) ;
}

void FacebookPostWidget::slotImageFetched(const QString& linkUrl, const QPixmap& pixmap)
{
	
	QString imgUrl = getImageUrl(linkUrl);
	
	QPixmap pix = pixmap;
	
	if ( pixmap.width() > 200 )
	    pix = pixmap.scaledToWidth(200);
    else if ( pixmap.height() > 200 )
        pix = pixmap.scaledToHeight(200);
   
   Choqok::UI::PostWidget::mainWidget()->document()->addResource(QTextDocument::ImageResource, imgUrl, pix);
   
   updateUi();
   
   //Choqok::NotifyManager::error(linkUrl, i18n("iconUrlString"));
   /*QString content = currentPost()->content;
   
   content += QString("<a href = \"%1\"> <img src = \"%2\"/> </a>").arg(linkUrl).arg(imgUrl);
   
   currentPost()->content = content;
   
   Choqok::UI::PostWidget::updateUi();       */ 
}


void FacebookPostWidget::checkAnchor(const QUrl &link)
{
	QString scheme = link.scheme();
	
	if (scheme == "img")
		{
			QString postId = currentPost()->postId;
			QStringList list = postId.split("_");
			postId = list[1];
			QString userId = list[0];
			QString urlString = QString("https://www.facebook.com/%1/posts/%2").arg(userId).arg(postId);
			QUrl url(urlString);
			FacebookViewDialog* fdialog = new FacebookViewDialog(url, this);
			fdialog->start();
		}
	
	else if(scheme == "user")
	 {
        KMenu menu;
        QString is = isOwnPost() ? "are" : "is";
        KAction * info = new KAction( KIcon("user-identity"), i18nc("Who is user", "Who %2 %1", currentPost()->author.realName, is), &menu );
        KAction * openInBrowser = new KAction(KIcon("applications-internet"), i18nc("Open profile page in browser", "Open profile in browser"), &menu);

        menu.addAction(info);
        menu.addAction(openInBrowser);
        
        QAction * ret = menu.exec(QCursor::pos());
        if(ret == 0)
            return;
        if(ret == info) {
			QStringList list = currentPost()->postId.split("_");
			
			FacebookAccount* acc = qobject_cast<FacebookAccount *> (currentAccount());

            FacebookWhoisWidget *wd = new FacebookWhoisWidget(acc, link.host(),  currentPost(), /*ShowType::UserInfo ,*/ this);
            wd->show(QCursor::pos());
            return;
        } else if(ret == openInBrowser){
			FacebookMicroBlog* blog = qobject_cast < FacebookMicroBlog * > ( currentAccount()->microblog());
            Choqok::openUrl( QUrl( blog->facebookUrl(currentAccount(), link.host()) ) );
            return;
        }
	} else if ( scheme == "property" || scheme == "title")
	{
		QUrl url(link.toString());
		url.setScheme("http");
		FacebookViewDialog* fdialog = new FacebookViewDialog(url, this);
		fdialog->start();
	}
	else
        Choqok::UI::PostWidget::checkAnchor(link);
}

void FacebookPostWidget::initUi()
{
    Choqok::UI::PostWidget::initUi();
    
    if( ! isNotification() )
    {
		KPushButton *btnLike = addButton( "btnLike",i18nc( "@info:tooltip", "Like" ), "rating" );
		KPushButton *btnViewLikes = addButton( "btnViewLikes",i18nc( "@info:tooltip", "View Likes on this post" ), "" );
		KPushButton *btnComment = addButton( "btnComment",i18nc( "@info:tooltip", "Comment" ), "edit-undo" );
		KPushButton *btnViewComments = addButton( "btnViewComments",i18nc( "@info:tooltip", "View Comments on this post" ), "" );

		updateLikeAndCommentCounts();
		
		updateFavStat();
		
		connect ( btnLike , SIGNAL(clicked(bool)), SLOT(slotLike()) );
		connect ( btnViewLikes, SIGNAL( clicked(bool)), SLOT(slotViewLikes()) );
		connect ( btnComment, SIGNAL(clicked(bool)), SLOT(slotComment()) );
		connect ( btnViewComments, SIGNAL(clicked(bool)), SLOT(slotViewComments()) );
    }

}

void FacebookPostWidget::updateUi()
{
	Choqok::UI::PostWidget::updateUi();
	
	if ( ! isNotification() )
	  updateLikeAndCommentCounts();
}

void FacebookPostWidget::updateUserLike()
{
	FacebookAccount * acc = qobject_cast<FacebookAccount *> (currentAccount());
	GetLikesJob* getJob = new GetLikesJob(currentPost()->postId, acc->accessToken());
	connect( getJob, SIGNAL(result(KJob*)), this, SLOT(slotUpdateUserLike(KJob*)) );
    getJob->start();
}

void FacebookPostWidget::slotUpdateUserLike(KJob* job)
{
	GetLikesJob *getJob = dynamic_cast<GetLikesJob *>( job );
	
	if ( getJob-> error() )
	{
		showStatusMessage(getJob->errorString(), i18n("Failed to Update User Like Status"));
	}
	
	else 
	{
		currentPost()->isFavorited = getJob->userLikes();
		likeUrl = getJob->href();
		updateFavStat();
	}
}

void FacebookPostWidget::updateLikeAndCommentCounts()
{
	FacebookPost* post = static_cast<FacebookPost*>(currentPost());
	
	KPushButton * btnViewLikes = buttons().value("btnViewLikes");
	KPushButton * btnViewComments = buttons().value("btnViewComments");
	
	QString likeCount = post->likeCount;
    QString commentCount = post->commentCount;
    
    if (btnViewLikes)
    {
		btnViewLikes->setText(likeCount + " ");
		btnViewLikes->setIconSize(QSize(0,0));
		btnViewLikes->setMinimumSize(QSize(likeCount.length() * 10, 20));
		btnViewLikes->setToolTip(post->likeString);
    }
    
    if ( btnViewComments)
    {
		btnViewComments->setText(commentCount + " ");
		btnViewComments->setIconSize(QSize(0,0));    
		btnViewComments->setMinimumSize(QSize(commentCount.length() * 10, 20));
		btnViewComments->setToolTip(post->commentString);
	}
}
void FacebookPostWidget::slotLike()
{
	
	setReadWithSignal();
	QString postId = currentPost()->postId;
	QString path = QString("/%1/likes").arg(postId);
	FacebookAccount * acc = qobject_cast<FacebookAccount *> (currentAccount());
	
	FacebookJob * job;
	
	if ( ! currentPost() -> isFavorited )
		 job = new FacebookAddJob(path, acc->accessToken());
	else
	   	 job = new FacebookDeleteJob(path, acc->accessToken());
	
	connect( job, SIGNAL(result(KJob*)), this, SLOT(slotLiked(KJob*)) );
    
    job->start();
    
	//KMessageBox::sorry(choqokMainWindow, i18n("Not Supported"));
}

void FacebookPostWidget::slotLiked( KJob* job)
{
	FacebookJob * fJob ;
	
	if ( ! currentPost()->isFavorited )
		fJob = dynamic_cast<FacebookAddJob *>( job );
	else
		fJob = dynamic_cast<FacebookDeleteJob *>( job ); 	
	
	if ( !fJob-> error() || fJob->error() == KJob::UserDefinedError   )
	{
		setFavorite();
		reloadLikesAndComments();
	}
	
	else 
	{
		showStatusMessage(fJob->errorString(), i18n("Failed to Like Post"));
	}
}

void FacebookPostWidget::setFavorite()
{
    currentPost()->isFavorited = !currentPost()->isFavorited;
    updateFavStat();
}


void FacebookPostWidget::updateFavStat()
{
	KPushButton* btnLike = buttons().value("btnLike");
	
	if (btnLike)
	{
		if(currentPost()->isFavorited){
			btnLike->setChecked(true);
			btnLike->setIcon(KIcon("rating"));
			btnLike->setToolTip("Unlike this post");
		} else {
			btnLike->setChecked(false);
			btnLike->setIcon(unFavIcon);
			btnLike->setToolTip("Like this post");
		}
	}
}

void FacebookPostWidget::updateLikeCount()
{
	FacebookAccount * acc = qobject_cast<FacebookAccount *> (currentAccount());
	GetLikesJob* getJob = new GetLikesJob(currentPost()->postId, acc->accessToken());
	connect( getJob, SIGNAL(result(KJob*)), this, SLOT(slotUpdateLikeCount(KJob*)) );
    getJob->start();
	
}

void FacebookPostWidget::slotUpdateLikeCount(KJob * job)
{
	GetLikesJob *getJob = dynamic_cast<GetLikesJob *>( job );
	
	if ( getJob-> error() )
	{
		showStatusMessage(getJob->errorString(), i18n("Failed to Update Like Count"));
	}
	
	else 
	{
		FacebookPost* post = static_cast<FacebookPost*>(currentPost());
		
		post->likeCount = QString::number(getJob->likeCount());
		updateLikeAndCommentCounts();
	}
	
}

void FacebookPostWidget::slotViewLikes()
{
	setReadWithSignal();
	QUrl url(likeUrl);
	FacebookViewDialog* fdialog = new FacebookViewDialog(url, this);
	fdialog->start();
	reloadLikesAndComments();
}

void FacebookPostWidget::slotComment()
{
	setReadWithSignal();
	FacebookAccount* acc = qobject_cast<FacebookAccount*>(currentAccount());
	FacebookInputDialog* dialog = new FacebookInputDialog(acc,"Enter text for your comment below", "Comment on the Post", "Enter comment and press return or click OK", true, this);
	connect (dialog, SIGNAL( inputEntered(FacebookAccount*, QString)), this ,SLOT( commented(FacebookAccount*, QString)));
	dialog->show();
}

void FacebookPostWidget::commented(FacebookAccount* theAccount, QString message)
{
	QString postId = currentPost()->postId;
	QString path = QString("/%1/comments").arg(postId);
	
	FacebookAccount * acc = theAccount;
	
	FacebookJob * job = new FacebookAddJob(path, acc->accessToken());;
	
	job->addQueryItem("message", message);
	
    connect( job, SIGNAL(result(KJob*)), this, SLOT(slotCommented(KJob*)) );
    
    job->start();
    
}

void FacebookPostWidget::slotCommented(KJob* job)
{
	FacebookJob * fJob = dynamic_cast<FacebookAddJob *>( job ) ;

	if ( !fJob-> error() || fJob->error() == KJob::UserDefinedError   )
	{
		reloadLikesAndComments();
		
	}
	
	else 
	{
		showStatusMessage(fJob->errorString(), i18n("Failed to Comment on Post"));
	}
}

void FacebookPostWidget::updateCommentCount()
{
	FacebookAccount * acc = qobject_cast<FacebookAccount *> (currentAccount());
	GetCommentsJob* getJob = new GetCommentsJob(currentPost()->postId, acc->accessToken());
	connect( getJob, SIGNAL(result(KJob*)), this, SLOT(slotUpdateCommentCount(KJob*)) );
    getJob->start();
}

void FacebookPostWidget::slotUpdateCommentCount(KJob * job)
{
	GetCommentsJob *getJob = dynamic_cast<GetCommentsJob *>( job );
	
	if ( getJob-> error() )
	{
		showStatusMessage(getJob->errorString(), i18n("Failed to Update Comment Count"));
	}
	
	else 
	{
		FacebookPost* post = static_cast<FacebookPost*>(currentPost());
		
		post->commentCount = QString::number(getJob->commentCount());
		updateLikeAndCommentCounts();
	}
	
}

void FacebookPostWidget::updateLikeString()
{
	FacebookAccount* acc = qobject_cast<FacebookAccount*>(currentAccount());
	PostJob* job = new PostJob(currentPost()->postId, acc->accessToken());
	job->addQueryItem("fields", "likes");
	connect(job, SIGNAL(result(KJob*)), this, SLOT(slotUpdateLikeString(KJob*)) );
	job->start();
}

void FacebookPostWidget::slotUpdateLikeString(KJob * job)
{
	PostJob *postJob = dynamic_cast<PostJob *>( job );
	
	if ( postJob-> error() )
	{
		showStatusMessage(postJob->errorString(), i18n("Failed to Update Like String"));
	}
	
	else 
	{
		FacebookPost* post = static_cast<FacebookPost*>(currentPost());
		FacebookAccount* acc = qobject_cast<FacebookAccount*>(currentAccount());
		
		LikeInfoPtr likes = postJob->postInfo()[0]->likes();
		QString likeString = likes.isNull() ? "" : createLikeString(acc, likes);
		likeString += " Click to see who all like this";
		post->likeString =  likeString;
		updateLikeAndCommentCounts();
	}
	
}

void FacebookPostWidget::updateCommentString()
{
	FacebookAccount* acc = qobject_cast<FacebookAccount*>(currentAccount());
	PostJob* job = new PostJob(currentPost()->postId, acc->accessToken());
	job->addQueryItem("fields", "comments");
	connect(job, SIGNAL(result(KJob*)), this, SLOT(slotUpdateCommentString(KJob*)) );
	job->start();
}

void FacebookPostWidget::slotUpdateCommentString(KJob * job)
{
	PostJob *postJob = dynamic_cast<PostJob *>( job );
	
	if ( postJob-> error() )
	{
		showStatusMessage(postJob->errorString(), i18n("Failed to Update Comment String"));
	}
	
	else 
	{
		FacebookPost* post = static_cast<FacebookPost*>(currentPost());
		FacebookAccount* acc = qobject_cast<FacebookAccount*>(currentAccount());
		
		CommentInfoPtr comments = postJob->postInfo()[0]->comments();
		QString commentString =  comments.isNull()  ? "" : createCommentString(acc, comments);
		commentString += " Click to see who all comments"; 
		post->commentString =  commentString   ;
		updateLikeAndCommentCounts();
	}
	
}
void FacebookPostWidget::slotViewComments ()
{
	setReadWithSignal();
	QStringList list = currentPost()->postId.split("_");
	QString postId = list[1];
	QString userId = list[0];
	QString urlString = QString("https://www.facebook.com/%1/posts/%2#contentArea").arg(userId).arg(postId);
	QUrl url(urlString);
	FacebookViewDialog* fdialog = new FacebookViewDialog(url, this);
	fdialog->start();
	reloadLikesAndComments();
}

void FacebookPostWidget::reloadLikesAndComments()
{
	updateLikeCount();
	updateLikeString();
	updateCommentCount();
	updateCommentString();
}

bool FacebookPostWidget::isRemoveAvailable() 
{
	return false;
}

bool FacebookPostWidget::isResendAvailable() 
{
	return false;
}

bool FacebookPostWidget::isOwnPost()
{
	FacebookAccount* account = qobject_cast<FacebookAccount*>(currentAccount());
	return account->id().compare( currentPost()->author.userId, Qt::CaseInsensitive ) == 0;
}

bool FacebookPostWidget::isNotification()
{
	return currentPost()->type.compare("notification") == 0;
}


void FacebookPostWidget::markNotificationAsRead()
{
	if (isNotification() )
	{
		FacebookAccount* acc = qobject_cast<FacebookAccount*>(currentAccount());
		
		NotificationsMarkReadJob* job = new NotificationsMarkReadJob(currentPost()->postId, acc->accessToken());
		connect(job, SIGNAL(result(KJob*)), this, SLOT(slotMarkNotificationAsRead(KJob*)) );
		job->start();
	}
}

void FacebookPostWidget::slotMarkNotificationAsRead(KJob* job)
{
	if ( !job-> error() || job->error() == KJob::UserDefinedError   )
	{
		Choqok::UI::PostWidget::setRead(true);
		Choqok::NotifyManager::success(i18n("Notification  Marked As Read"));
	}
	
	else 
	{
		showStatusMessage(job->errorString(), i18n("Failed to Mark Notification As Read"));
	}
}

void FacebookPostWidget::showStatusMessage(const QString message, const QString caption)
{
	QString status = caption + " : " + message;
	
	if(Choqok::UI::Global::mainWindow()->isActiveWindow()){
        choqokMainWindow->showStatusMessage(message);
    }
	
}
