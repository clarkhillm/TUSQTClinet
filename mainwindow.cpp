#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileUploader = new TUSUploader;
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
    QString fileName = filePath.split("/").last();

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
    connect(reply, SIGNAL(finished()), SLOT(onLoginFinished()));
}

void MainWindow::onLoginFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << reply->readAll();
    fileUploader->start();
}
