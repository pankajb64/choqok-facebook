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
#include "facebookcomposerwidget.h"
#include "facebookmicroblog.h"

#include "composerwidget.h"
#include "choqoktextedit.h"
#include <shortenmanager.h>
#include <notifymanager.h>

#include <KDebug>
#include <KFileDialog>
#include <KFileItem>
#include <klocalizedstring.h>

FacebookComposerWidget::FacebookComposerWidget(Choqok::Account* account, QWidget* parent)
    : Choqok::UI::ComposerWidget(account, parent), btnAttach(0), mediumName(0), btnCancel(0)
{
    editorLayout = qobject_cast<QGridLayout*>(editorContainer()->layout());
    btnAttach = new KPushButton(editorContainer());
    btnAttach->setIcon(KIcon("image-x-generic"));
    btnAttach->setToolTip(i18n("Attach a photo/Video"));
    btnAttach->setMaximumWidth(btnAttach->height());

    connect(btnAttach, SIGNAL(clicked(bool)), this, SLOT(selectMediumToAttach()));
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(btnAttach);
    vLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::MinimumExpanding));
    editorLayout->addItem(vLayout, 0, 1, 1, 1);
}

void FacebookComposerWidget::submitPost(const QString& txt)
{
    if( mediumToAttach.isEmpty() ){
        Choqok::UI::ComposerWidget::submitPost(txt);
    } else {
        kDebug();
        editorContainer()->setEnabled(false);
        QString text = txt;
        if( currentAccount()->microblog()->postCharLimit() &&
            text.size() > (int)currentAccount()->microblog()->postCharLimit() )
            text = Choqok::ShortenManager::self()->parseText(text);
        setPostToSubmit(0L);
        setPostToSubmit( new Choqok::Post );
        postToSubmit()->content = text;
        if( !replyToId.isEmpty() ) {
            postToSubmit()->replyToPostId = replyToId;
        }
        connect( currentAccount()->microblog(), SIGNAL(postCreated(Choqok::Account*,Choqok::Post*)),
                SLOT(slotPostMediaSubmitted(Choqok::Account*,Choqok::Post*)) );
        connect(currentAccount()->microblog(),
                SIGNAL(errorPost(Choqok::Account*,Choqok::Post*,Choqok::MicroBlog::ErrorType,
                                         QString,Choqok::MicroBlog::ErrorLevel)),
                SLOT(slotErrorPost(Choqok::Account*,Choqok::Post*)));
        btnAbort = new KPushButton(KIcon("dialog-cancel"), i18n("Abort"), this);
        layout()->addWidget(btnAbort);
        connect( btnAbort, SIGNAL(clicked(bool)), SLOT(abort()) );
        FacebookMicroBlog *mBlog = qobject_cast<FacebookMicroBlog*>(currentAccount()->microblog());
        mBlog->createPostWithAttachment( currentAccount(), postToSubmit(), mediumToAttach );
    }
}

void FacebookComposerWidget::slotPostMediaSubmitted(Choqok::Account* theAccount, Choqok::Post* post)
{
    kDebug();
    if( currentAccount() == theAccount && post == postToSubmit() ) {
        kDebug()<<"Accepted";
        disconnect(currentAccount()->microblog(), SIGNAL(postCreated(Choqok::Account*,Choqok::Post*)),
                   this, SLOT(slotPostMediaSubmitted(Choqok::Account*,Choqok::Post*)) );
        disconnect(currentAccount()->microblog(),
                    SIGNAL(errorPost(Choqok::Account*,Choqok::Post*,Choqok::MicroBlog::ErrorType,
                                    QString,Choqok::MicroBlog::ErrorLevel)),
                    this, SLOT(slotErrorPost(Choqok::Account*,Choqok::Post*)));
        if(btnAbort){
            btnAbort->deleteLater();
        }
        Choqok::NotifyManager::success(i18n("New post submitted successfully"));
        editor()->clear();
        replyToId.clear();
        editorContainer()->setEnabled(true);
        setPostToSubmit( 0L );
        cancelAttachMedium();
        currentAccount()->microblog()->updateTimelines(currentAccount());
    }
}

void FacebookComposerWidget::selectMediumToAttach()
{
    kDebug();
    mediumToAttach = KFileDialog::getOpenFileName( KUrl("kfiledialog:///image?global"),
                                                      QString(), this,
                                                      i18n("Select Media to Upload") );
    if( mediumToAttach.isEmpty() )
        return;

    QString fileName = KUrl(mediumToAttach).fileName();
    if( !mediumName ){
        kDebug()<<fileName;
        mediumName = new QLabel(editorContainer());
        btnCancel = new KPushButton(editorContainer());
        btnCancel->setIcon(KIcon("list-remove"));
        btnCancel->setToolTip(i18n("Discard Attachment"));
        btnCancel->setMaximumWidth(btnCancel->height());
        connect( btnCancel, SIGNAL(clicked(bool)), SLOT(cancelAttachMedium()) );

        editorLayout->addWidget(mediumName, 1, 0);
        editorLayout->addWidget(btnCancel, 1, 1);
    }

    mediumName->setText(i18n("Attaching <b>%1</b> ", fileName));
    editor()->setFocus();
}

void FacebookComposerWidget::cancelAttachMedium()
{
    kDebug();
    delete mediumName;
    mediumName = 0;
    delete btnCancel;
    btnCancel = 0;
    mediumToAttach.clear();
}
