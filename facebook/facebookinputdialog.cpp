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

#include "facebookinputdialog.h"
#include <KLocale>
#include <QVBoxLayout>

FacebookInputDialog::FacebookInputDialog (FacebookAccount* theAccount, QString labelText, QString captionText, QString placeholderText, bool closeWithSignal, QWidget* parent) : KDialog(parent)
{
	mLabelText = labelText;
	mCaptionText = captionText;
	mPlaceholderText = placeholderText;
	mCloseWithSignal = closeWithSignal;
	mAccount = theAccount;
	initUi();
}

void FacebookInputDialog::initUi()
{
	setButtons( KDialog::Ok );
	setCaption(  mCaptionText  );
    setAttribute( Qt::WA_DeleteOnClose, true );
    QWidget * const widget = new QWidget( this );
    QVBoxLayout * const layout = new QVBoxLayout( widget );
	label = new QLabel(mLabelText);
	label->setWordWrap(true);
	lineEdit = new KLineEdit(this);
	lineEdit->setPlaceholderText(mPlaceholderText);

	connect( this, SIGNAL(okClicked()), SLOT(slotSubmitInput()) );
	connect( lineEdit, SIGNAL(returnPressed()), this, SLOT(slotSubmitInput()) );
	layout->addWidget(label);
	layout->addWidget(lineEdit);

	setMainWidget(widget);
}

void FacebookInputDialog::slotSubmitInput()
{
	QString message = lineEdit->text();
	
	if( ! message.isEmpty() )
	{
		emit inputEntered(mAccount, message);
	}
	
	if (mCloseWithSignal)
	  close();
}
