/*  This file is part of KDevelop
    Copyright 2009 Aleix Pol <aleixpol@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
#ifndef QTHELPNETWORK_H
#define QTHELPNETWORK_H

#include "debug.h"

#include <QNetworkReply>
#include <QTimer>
#include <QHelpEngine>
#include <QMimeDatabase>
#include <QMimeType>

class HelpNetworkReply : public QNetworkReply
{
	public:
		HelpNetworkReply(const QNetworkRequest &request, const QByteArray &fileData, const QString &mimeType);
		
		void abort() override {}
		qint64 bytesAvailable() const override { return data.length() + QNetworkReply::bytesAvailable(); }
		
	protected:
		qint64 readData(char *data, qint64 maxlen) override;
		
	private:
		QByteArray data;
		qint64 origLen;
};

HelpNetworkReply::HelpNetworkReply(const QNetworkRequest &request,
        const QByteArray &fileData, const QString &mimeType)
    : data(fileData), origLen(fileData.length())
{
    setRequest(request);
    setOpenMode(QIODevice::ReadOnly);

    // Instantly finish processing if data is empty. Without this code the loadFinished()
    // signal will never be emitted by the corresponding QWebView.
    if (!origLen) {
        qCDebug(QTHELP) << "Empty data for" << request.url().toDisplayString();
        QTimer::singleShot(0, this, SIGNAL(finished()));
    }

    // Fix broken CSS images (tested on Qt 5.5.1 and 5.7.0)
    if (request.url().fileName() ==  QStringLiteral("offline.css")) {
        data.replace("../images", "images");
    }

    setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
    setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(origLen));
    QTimer::singleShot(0, this, SIGNAL(metaDataChanged()));
    QTimer::singleShot(0, this, SIGNAL(readyRead()));
}

qint64 HelpNetworkReply::readData(char *buffer, qint64 maxlen)
{
	qint64 len = qMin(qint64(data.length()), maxlen);
	if (len) {
		memcpy(buffer, data.constData(), len);
		data.remove(0, len);
	}
	if (!data.length())
		QTimer::singleShot(0, this, SIGNAL(finished()));
	return len;
}

class HelpNetworkAccessManager : public QNetworkAccessManager
{
	public:
		explicit HelpNetworkAccessManager(QHelpEngineCore *engine, QObject *parent = nullptr)
			: QNetworkAccessManager(parent), m_helpEngine(engine) {}

	protected:
		virtual QNetworkReply *createRequest(Operation op,
			const QNetworkRequest &request, QIODevice *outgoingData = nullptr) override;

	private:
		QHelpEngineCore *m_helpEngine;
};

QNetworkReply *HelpNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	QString scheme = request.url().scheme();
	if (scheme == QLatin1String("qthelp") || scheme == QLatin1String("about")) {
		QString mimeType = QMimeDatabase().mimeTypeForUrl(request.url()).name();
		if (mimeType == "application/x-extension-html") {
			// see also: https://bugs.kde.org/show_bug.cgi?id=288277
			// firefox seems to add this bullshit mimetype above
			// which breaks displaying of qthelp documentation :(
			mimeType = "text/html";
		}
		return new HelpNetworkReply(request, m_helpEngine->fileData(request.url()), mimeType);
	}
	return QNetworkAccessManager::createRequest(op, request, outgoingData);
}

#endif
