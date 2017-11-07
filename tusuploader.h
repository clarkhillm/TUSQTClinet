#ifndef TUSUPLOADER_H
#define TUSUPLOADER_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

enum TUSState {
    start,
    stop,
    finish
};

class TUSUploader : public QObject {
    Q_OBJECT

    QString postURL;
    QString patchURL;
    QString filePath;
    TUSState state;

    qint64 size;
    qint64 sent;

    QFile* file;
    QTime time; //用来计算上传速度

    QNetworkAccessManager* netManager;
    QNetworkReply* patchReply;

    QString metaData;
    QByteArray fileLocation;

public:
    explicit TUSUploader(QObject* parent = nullptr);

    void setPostURL(QString url);
    void setFilePath(QString filePath);
    void appendMetadata(QString key, QString value);

    void start();
    void stop();
    void terminate();
    TUSState getState();

    QNetworkAccessManager* getManager();

signals:
    void uploadProgress(QString filePath, qint64 total, qint64 actualSent, int percent, double speed, QString unit);
    void uploadFinished(QString filePath);

private slots:
    void onCreateFinished();
    void onPatchFinished();
    void onNetworkError(QNetworkReply::NetworkError code);
    void onProgress(qint64 bytesSent, qint64 bytesTotal);
};

#endif // TUSUPLOADER_H
