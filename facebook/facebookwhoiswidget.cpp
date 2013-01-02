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

#include "facebookwhoiswidget.h"

#include <KAnimatedButton>
#include <KStatusBar>

FacebookWhoisWidget::FacebookWhoisWidget(FacebookAccount* theAccount, const QString& uId, const Choqok::Post* post, QWidget* parent) : QFrame(parent)
{
	currentAccount = theAccount;
	currentPost = post;
	userId = uId;


	setAttribute(Qt::WA_DeleteOnClose);
    loadUserInfo();

    wid = new KTextBrowser(this);
    setFrameShape(StyledPanel);
    setFrameShadow(Sunken);

    wid->setFrameShape(QFrame::NoFrame);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(wid);
    this->setLayout(layout);
    this->setWindowFlags(Qt::Popup);// | Qt::FramelessWindowHint | Qt::Ta);
    wid->setOpenLinks(false);
    connect(wid,SIGNAL(anchorClicked(const QUrl)),this,SLOT(checkAnchor(const QUrl)));
    setupUi();
}

void FacebookWhoisWidget::show(QPoint pos)
{
    kDebug();
    waitFrame = new QFrame(this);
    waitFrame->setFrameShape(NoFrame);
    waitFrame->setWindowFlags(Qt::Popup);
    KAnimatedButton *waitButton = new KAnimatedButton;
    waitButton->setToolTip(i18n("Please wait..."));
    connect( waitButton, SIGNAL(clicked(bool)), SLOT(slotCancel()) );
    waitButton->setIcons("process-working-kde");
    waitButton->start();

    QVBoxLayout *ly = new QVBoxLayout(waitFrame);
    ly->setSpacing(0);
    ly->setContentsMargins(0, 0, 0, 0);
    ly->addWidget(waitButton);

    waitFrame->move(pos - QPoint(15, 15));
    waitFrame->show();
}

void FacebookWhoisWidget::loadUserInfo()
{
	UserInfoJob* job = new UserInfoJob("/" + userId, currentAccount->accessToken());
	mJob = job;

	connect(job, SIGNAL(result(KJob*)), this, SLOT(userInfoReceived(KJob*)));
	job->start();
}

void FacebookWhoisWidget::userInfoReceived(KJob* job)
{
    kDebug();
    if(job->error()){
        kError()<<"Job Error: "<<job->errorString();
        if( Choqok::UI::Global::mainWindow()->statusBar() )
            Choqok::UI::Global::mainWindow()->statusBar()->showMessage(job->errorString());
        slotCancel();
        return;
    }
    UserInfoJob *ujob = qobject_cast<UserInfoJob *>(job);

    UserInfo userInfo = ujob->userInfo();

    currentUserInfo = &userInfo;

    updateHtml();
    showForm();
    QString imageUrl = QString("http://graph.facebook.com/%1/picture").arg( currentPost->author.userId );
    QPixmap *userAvatar = Choqok::MediaManager::self()->fetchImage( imageUrl, Choqok::MediaManager::Async );

    if(userAvatar) {
        wid->document()->addResource( QTextDocument::ImageResource, QUrl("img://profileImage"), *(userAvatar) );
    } else
    {
        connect( Choqok::MediaManager::self(), SIGNAL( imageFetched(QString,QPixmap)), this, SLOT(avatarFetched(QString, QPixmap) ) );
        connect( Choqok::MediaManager::self(), SIGNAL(fetchError(QString,QString)), this, SLOT(avatarFetchError(QString,QString)) );
    }
 }

void FacebookWhoisWidget::avatarFetched(const QString& remoteUrl, const QPixmap& pixmap)
{
    kDebug();

    QString url = "img://profileImage";
    wid->document()->addResource( QTextDocument::ImageResource, url, pixmap );
    updateHtml();
    disconnect( Choqok::MediaManager::self(), SIGNAL( imageFetched(QString,QPixmap)), this, SLOT(avatarFetched(QString, QPixmap) ) );
    disconnect( Choqok::MediaManager::self(), SIGNAL(fetchError(QString,QString)), this, SLOT(avatarFetchError(QString,QString))  );

}

void FacebookWhoisWidget::avatarFetchError(const QString& remoteUrl, const QString& errMsg)
{
    kDebug();
    Q_UNUSED(errMsg);

    ///Avatar fetching is failed! but will not disconnect to get the img if it fetches later!
    QString url = "img://profileImage";
    wid->document()->addResource( QTextDocument::ImageResource, url, KIcon("image-missing").pixmap(48) );
    updateHtml();

}

void FacebookWhoisWidget::updateHtml()
{
    kDebug();
    QString html;
    if( errorMessage.isEmpty() ) {
		QString url = currentUserInfo->website().isEmpty() ? QString() : QString("<a title='%1' href='%1'>%1</a>").arg(currentUserInfo->website().toString());

        QString mainTable = QString("<table width='100%'><tr>\
        <td width=49><img width=48 height=48 src='img://profileImage'/></td>\
        <td><table width='100%'><tr><td><font size=5><b>%1</b></font></td>\
        <td><a href='choqok://close'><img src='icon://close' title='" + i18n("Close") + "' align='right' /></a></td></tr></table><br/>\
        <b>User Name - %2<br/>UserId - %3</b><br/>\
        <font size=3>%4</font></td></tr></table>")
        .arg(Qt::escape(currentUserInfo->name()))
        .arg(currentUserInfo->username().isEmpty() ?  currentUserInfo->id() : Qt::escape(currentUserInfo->username()))
        .arg(userId)
        .arg(url);

        html = mainTable;

    } else {
        html = i18n("<h3>%1</h3>", errorMessage);
    }
    wid->setHtml(html);
}

void FacebookWhoisWidget::showForm()
{
    kDebug();
    QPoint pos = waitFrame->pos();
    waitFrame->deleteLater();
    wid->resize(320, 200);
    wid->document()->setTextWidth(width()-2);
    wid->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int h = wid->document()->size().toSize().height() + 10;
    wid->setMinimumHeight(h);
    wid->setMaximumHeight(h);
    this->resize(320,h+4);
    int desktopHeight = KApplication::desktop()->height();
    int desktopWidth = KApplication::desktop()->width();
    if( (pos.x() + this->width()) > desktopWidth )
        pos.setX(desktopWidth - width());
    if( (pos.y() + this->height()) > desktopHeight )
        pos.setY(desktopHeight - height());
    move(pos);
    QWidget::show();
}

void FacebookWhoisWidget::slotCancel()
{
    kDebug();
    if(waitFrame)
        waitFrame->deleteLater();
    if(mJob)
        mJob->kill();
    this->close();
}

void FacebookWhoisWidget::checkAnchor( const QUrl url )
{
    kDebug();
    if(url.scheme()=="choqok"){
        if(url.host()=="close"){
            this->close();
        }
    } else {
            Choqok::openUrl(url);
            close();
        }
}

void FacebookWhoisWidget::setupUi()
{
    kDebug();
    wid->document()->addResource( QTextDocument::ImageResource, QUrl("icon://close"),
                            KIcon("dialog-close").pixmap(16) );


    QString style = "color: %1; background-color: %2";
    if ( Choqok::AppearanceSettings::isCustomUi() ) {
        setStyleSheet( style.arg(Choqok::AppearanceSettings::readForeColor().name())
                            .arg(Choqok::AppearanceSettings::readBackColor().name()) );
    } else {
        QPalette p = window()->palette();
        setStyleSheet( style.arg(p.color(QPalette::WindowText).name()).arg(p.color(QPalette::Window).name()) );
    }
}
