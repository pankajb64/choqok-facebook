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

#ifndef FACEBOOKCOMPOSERWIDGET_H
#define FACEBOOKCOMPOSERWIDGET_H

#include <KPushButton>

#include <QHBoxLayout>
#include <qlayout.h>
#include <QLabel>
#include <QPointer>

#include <account.h>
#include <composerwidget.h>
#include <microblog.h>

class FacebookComposerWidget : public Choqok::UI::ComposerWidget
{
	Q_OBJECT

public:
    explicit FacebookComposerWidget(Choqok::Account* account, QWidget* parent = 0);

protected slots:
    virtual void submitPost(const QString& text);
    virtual void slotPostMediaSubmitted(Choqok::Account *theAccount, Choqok::Post* post);
    virtual void selectMediumToAttach();
    virtual void cancelAttachMedium();

private:
    QString mediumToAttach;
    QString mimeType;
    KPushButton *btnAttach;
    QPointer<QLabel> mediumName;
    QPointer<KPushButton> btnCancel;
    QGridLayout *editorLayout;
};

#endif
