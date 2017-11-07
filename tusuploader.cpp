#include "tusuploader.h"
#include <QBuffer>

TUSUploader::TUSUploader(QObject* parent)
    : QObject(parent)
{
    netManager = new QNetworkAccessManager;
    metaData = "";
}

void TUSUploader::setPostURL(QString url)
{
    this->postURL = url;
}

void TUSUploader::setFilePath(QString filePath)
{
    this->filePath = filePath;
}

void TUSUploader::start()
{

    sent = 0;
    state = TUSState::start;

    QFile* file = new QFile(filePath);
    file->open(QIODevice::ReadOnly);
    size = file->size();
    file->close();
    file->deleteLater();

    QNetworkRequest request(postURL);
    request.setRawHeader("Tus-Resumable", QByteArray("1.0.0"));
    request.setRawHeader("Upload-Metadata", metaData.toUtf8());

    qDebug() << request.rawHeader("Upload-Metadata");

    QByteArray fileLength;
    fileLength.setNum(size);
    request.setRawHeader("Upload-Length", fileLength);

    qDebug() << "first post request to " << postURL;
    QNetworkReply* reply = netManager->post(request, "");

    connect(reply, &QNetworkReply::finished, this, &TUSUploader::onCreateFinished);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onNetworkError(QNetworkReply::NetworkError)));
}

void TUSUploader::appendMetadata(QString key, QString value)
{
    metaData = metaData + key + " " + value.toUtf8().toBase64() + ",";
}

void TUSUploader::onCreateFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (code == 201) {
        fileLocation = reply->rawHeader("Location");
        qDebug() << fileLocation;
        patchURL = QString::fromUtf8(fileLocation);

        qDebug() << "open file" << filePath;
        file = new QFile(filePath);
        file->open(QIODevice::ReadOnly);
        size = file->size();

        QByteArray fileLength;
        fileLength.setNum(size);
        qDebug() << "set file size byte array" << fileLength;

        QByteArray offset;
        offset.setNum(0);

        QNetworkRequest request(patchURL);
        request.setRawHeader("Tus-Resumable", QByteArray("1.0.0"));
        request.setRawHeader("Connection", "keep-alive");
        request.setRawHeader("Upload-Offset", offset);
        request.setRawHeader("Content-Length", fileLength);
        request.setRawHeader("Content-Type", "application/offset+octet-stream");
        qDebug() << "create request obj";

        uchar* buf = file->map(0, size);
        QBuffer* buffer = new QBuffer();
        buffer->setData(reinterpret_cast<const char*>(buf), size);
        buffer->close();

        qDebug() << "2nd request use patch to" << patchURL;
        QNetworkReply* reply = netManager->sendCustomRequest(request, "PATCH", buffer);
        time.start();

        connect(reply, &QNetworkReply::finished, this, &TUSUploader::onPatchFinished);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onNetworkError(QNetworkReply::NetworkError)));
        connect(reply, &QNetworkReply::uploadProgress, this, &TUSUploader::onProgress);
        file->unmap(buf);

    } else {
        qDebug() << "upload error" << reply->readAll();
    }
}

void TUSUploader::onPatchFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "patch finished" << reply->readAll();
    file->close();
    file->deleteLater();
}

void TUSUploader::onNetworkError(QNetworkReply::NetworkError code)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << code << reply->errorString();
    if (file != nullptr) {
        file->close();
        file->deleteLater();
    }
}
void TUSUploader::stop() {}
void TUSUploader::terminate() {}
TUSState TUSUploader::getState()
{
    return this->state;
}

QNetworkAccessManager* TUSUploader::getManager()
{
    return this->netManager;
}

void TUSUploader::onProgress(qint64 bytesSent, qint64 bytesTotal)
{
    //    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    qint64 actualSent = sent + bytesSent;

    double speed = actualSent * 1000.0 / time.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024 * 1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024 * 1024;
        unit = "MB/s";
    }
    int percent = actualSent * 100 / size;

    qDebug() << bytesTotal << actualSent << percent << speed << unit;
    emit uploadProgress(bytesTotal, actualSent, percent, speed, unit);
}
