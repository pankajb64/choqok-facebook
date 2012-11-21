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

#include "facebookeditaccountwidget.h"
#include "facebookmicroblog.h"
#include "facebookaccount.h"
#include <KDebug>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <KMessageBox>
#include <QDomDocument>
#include <KToolInvocation>
#include <QProgressBar>
#include <accountmanager.h>
#include <choqoktools.h>
#include <QtOAuth/interface.h>
#include <QtOAuth/qoauth_namespace.h>
#include <kio/accessmanager.h>
#include <QCheckBox>
#include <KInputDialog>
#include <libkfbapi/authenticationdialog.h>
#include <libkfbapi/userinfojob.h>
#include <notifymanager.h>

FacebookEditAccountWidget::FacebookEditAccountWidget(FacebookMicroBlog *microblog, FacebookAccount* account, QWidget* parent) : ChoqokEditAccountWidget(account, parent), mBlog(microblog), mAccount(account)
{
    setupUi(this);
    
    connect(kcfg_authorize, SIGNAL(clicked(bool)), this, SLOT(showAuthenticationDialog()));
    
   if(mAccount) {
        kcfg_alias->setText( mAccount->alias() );
        if(mAccount->accessToken().isEmpty() ) {
          kDebug() << "Account exists, but there aint an accessToken. :S ?! " << account ;  
	  setAuthenticated(false);
        } else {
	  kDebug() << "Account exists with  an accessToken. :) :D " << account;  
            setAuthenticated(true);
            updateUserName();
            token = mAccount->accessToken();   
            username = mAccount->username();
            name = mAccount->name();
            id= mAccount->id();
        }
    } else {
        setAuthenticated(false);
	kDebug() << "The Account was null :(" ;
        QString newAccountAlias = microblog->serviceName();
        QString servName = newAccountAlias;
        int counter = 1;
        while(Choqok::AccountManager::self()->findAccount(newAccountAlias)){
            newAccountAlias = QString("%1%2").arg(servName).arg(counter);
        counter++;
    }
        setAccount( mAccount = new FacebookAccount(microblog, newAccountAlias) );
        kcfg_alias->setText( newAccountAlias );
    }
    loadTimelinesTableState();
    kcfg_alias->setFocus(Qt::OtherFocusReason);
}

FacebookEditAccountWidget::~FacebookEditAccountWidget()
{

}
void FacebookEditAccountWidget::showAuthenticationDialog()
{
  QStringList permissions;
  /*permissions << "offline_access"
	      << "publish_stream"
              << "friends_birthday"
              << "friends_website"
              << "friends_location"
              << "friends_work_history"
              << "friends_relationships"
              << "user_events"
              << "user_notes"
	      << "read_stream";*/
  permissions << "read_stream"
		  << "publish_stream"
		  << "manage_notifications"	
	      << "user_birthday"
	      <<"user_religion_politics"
	      << "user_relationships"
	      << "user_relationship_details"
	      << "user_hometown"
	      << "user_location"
	      << "user_likes"
	      << "user_activities"
	      << "user_interests"
	      << "user_education_history"
	      << "user_work_history"
	      << "user_online_presence"
	      << "user_website"
	      << "user_groups"
	      << "user_events"
	      << "user_photos"
	      << "user_videos"
	      << "user_photo_video_tags"
	      << "user_notes"
	      << "user_about_me"
	      << "user_status"
	      << "friends_birthday"
	      << "friends_religion_politics"
	      << "friends_relationships"
	      << "friends_relationship_details"
	      << "friends_hometown"
	      << "friends_location"
	      << "friends_likes"
	      << "friends_activities"
	      << "friends_interests"
	      << "friends_education_history"
	      << "friends_work_history"
	      << "friends_online_presence"
	      << "friends_website"
	      << "friends_groups"
	      << "friends_events"
	      << "friends_photos"
	      << "friends_videos"
	      << "friends_photo_video_tags"
	      << "friends_notes"
	      << "friends_about_me"
	      << "friends_status";


  AuthenticationDialog * const authDialog = new AuthenticationDialog( this );
  authDialog->setAppId( appID() );
  authDialog->setPermissions( permissions );
  connect( authDialog, SIGNAL(authenticated(QString)), this, SLOT(authenticationDone(QString)) );
  connect( authDialog, SIGNAL(canceled()), this, SLOT(authenticationCancelled()) );
  kcfg_authorize->setEnabled( false );
  authDialog->start();
}

void FacebookEditAccountWidget::authenticationCancelled()
{
  kcfg_authorize->setEnabled( true );
}

void FacebookEditAccountWidget::authenticationDone(const QString& accessToken)
{

  mAccount->setAccessToken( accessToken );
  token = accessToken;
  apply();
  kDebug() << "Access Token is :" << mAccount->accessToken() <<" , i.e. " <<accessToken;
  setAuthenticated(true);
  updateUserName();
}

void FacebookEditAccountWidget::updateUserName()
{
    UserInfoJob *  job = new UserInfoJob( mAccount->accessToken() );
    connect( job, SIGNAL(result(KJob*)), this, SLOT(userInfoJobDone(KJob*)) );
    job->start();
}

void FacebookEditAccountWidget::userInfoJobDone( KJob* job )
{
  UserInfoJob *  userInfoJob = dynamic_cast<UserInfoJob*>( job );
  Q_ASSERT( userInfoJob );
  if ( !userInfoJob->error() ) {
    mAccount->setUsername( userInfoJob->userInfo().username() );
    mAccount->setId(userInfoJob->userInfo().id() );
    mAccount->setName (userInfoJob->userInfo().name() );
    username = mAccount->username();
    name = mAccount->name();
    id = mAccount->id();
    setAuthenticated(true);
    mAccount->writeConfig();
  } else {
    
    kWarning() << "Can't get user info: " << userInfoJob->errorText();
  }
  
}

bool FacebookEditAccountWidget::validateData()
{
    if(kcfg_alias->text().isEmpty() || !isAuthenticated )
        return false;
    else
        return true;
}


Choqok::Account* FacebookEditAccountWidget::apply()
{
    kDebug();
    mAccount->setAlias(kcfg_alias->text());
    //mAccount->setUsername( username );
    mAccount->setAccessToken( token );
    saveTimelinesTableState();
    mAccount->writeConfig();
    return mAccount;
}


void FacebookEditAccountWidget::setAuthenticated(bool authenticated)
{
    isAuthenticated = authenticated;
    if(authenticated){
        kcfg_authorize->setIcon(KIcon("object-unlocked"));
        kcfg_authenticateLed->on();
	if ( mAccount->username().isEmpty() ) {
	    kcfg_authenticateStatus->setText( i18n( "Authenticated." ) );
    } else {
      kcfg_authenticateStatus->setText( i18n( "Authenticated as <b>%1</b>.", mAccount->username() ) );
    }
        //kcfg_authenticateStatus->setText(i18n("Authenticated"));
    } else {
        kcfg_authorize->setIcon(KIcon("object-locked"));
        kcfg_authenticateLed->off();
        kcfg_authenticateStatus->setText(i18n("Not Authenticated"));
    }
}



void FacebookEditAccountWidget::loadTimelinesTableState()
{
    foreach(const QString &timeline, mAccount->microblog()->timelineNames()){
        int newRow = timelinesTable->rowCount();
        timelinesTable->insertRow(newRow);
        timelinesTable->setItem(newRow, 0, new QTableWidgetItem(timeline));

        QCheckBox *enable = new QCheckBox ( timelinesTable );
        enable->setChecked ( mAccount->timelineNames().contains(timeline) );
        timelinesTable->setCellWidget ( newRow, 1, enable );
    }
}

void FacebookEditAccountWidget::saveTimelinesTableState()
{
    QStringList timelines;
    int rowCount = timelinesTable->rowCount();
    for(int i=0; i<rowCount; ++i){
        QCheckBox *enable = qobject_cast<QCheckBox*>(timelinesTable->cellWidget(i, 1));
        if(enable && enable->isChecked())
            timelines<<timelinesTable->item(i, 0)->text();
    }
    timelines.removeDuplicates();
    mAccount->setTimelineNames(timelines);
    mAccount->writeConfig();
    mBlog->updateTimelines(mAccount);
}




QString FacebookEditAccountWidget::apiKey() 
{
  return "698f95e6ad2c477359f75f344d12b899";
}

QString FacebookEditAccountWidget::appID() 
{
  return "161169767282198";
}

QString FacebookEditAccountWidget::appSecret()
{
  return "343ea4a345c8926c1e00183e82ea9542";
}
#include "facebookeditaccountwidget.moc"
