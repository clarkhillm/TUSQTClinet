#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainLayout = new QVBoxLayout;

    centralWidget()->setLayout(mainLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionupload_triggered()
{
    qDebug() << "upload file ...";
    QString filePath = QFileDialog::getOpenFileName(this, "", "");
    qDebug() << filePath;
    TUSUploader* fileUploader = new TUSUploader;
    uploaderMap[filePath] = fileUploader;

    ProgressWidget* progress = new ProgressWidget;
    progressWidgetMap[filePath] = progress;
    mainLayout->addWidget(progress);

    QString fileName = filePath.split("/").last();
    progress->getNameLabel()->setText(fileName);

    fileUploader->setFilePath(filePath);
    fileUploader->appendMetadata("filename", fileName);
    fileUploader->appendMetadata("dir", "/bigfile/");
    fileUploader->appendMetadata("user", "test");
    fileUploader->appendMetadata("group", "group1");
    fileUploader->appendMetadata("section", "my");
    fileUploader->setPostURL("http://10.12.2.7:8012/files/");

    QNetworkAccessManager* manager = fileUploader->getManager();
    QNetworkRequest request(QUrl("http://10.12.2.7:8012/login?user=test&passwd=111111"));
    QNetworkReply* reply = manager->get(request);
    loginReplyMap[reply] = filePath;
    connect(reply, SIGNAL(finished()), SLOT(onLoginFinished()));
}

void MainWindow::onLoginFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << reply->readAll();
    QString filePath = loginReplyMap[reply];
    qDebug() << "on login finished | get file path from map" << filePath;
    TUSUploader* fileUploader = uploaderMap[filePath];
    fileUploader->start();
    connect(fileUploader, &TUSUploader::uploadProgress, this, &MainWindow::onUploadProgress);
    connect(fileUploader, &TUSUploader::uploadFinished, this, &MainWindow::onUploadFinished);
}
void MainWindow::onUploadFinished(QString filePath)
{
    qDebug() << "cleanup" << filePath;
    ProgressWidget* progress = progressWidgetMap[filePath];
    mainLayout->removeWidget(progress);
    progress->deleteLater();

    TUSUploader* uploader = uploaderMap[filePath];
    uploader->destroyed();
    uploader->deleteLater();

    progressWidgetMap.remove(filePath);
    uploaderMap.remove(filePath);
}

void MainWindow::onUploadProgress(QString filePath, qint64 total, qint64 actualSent, int percent, double speed, QString unit)
{
    ProgressWidget* progress = progressWidgetMap[filePath];
    QProgressBar* progressBar = progress->getProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(total);
    progressBar->setValue(actualSent);
    progress->getSpeedLabel()->setText(QString::number(static_cast<int>(speed)) + " " + unit);
    progress->getPercentageLabel()->setText(QString::number(percent) + "%");
}
