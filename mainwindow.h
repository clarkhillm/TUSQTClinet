#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "progresswidget.h"
#include "tusuploader.h"
#include <QMainWindow>
#include <QtWidgets>

QT_MESSAGELOGCONTEXT

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    Ui::MainWindow* ui;

    QVBoxLayout* mainLayout;

    QHash<QString, ProgressWidget*> progressWidgetMap;
    QHash<QString, TUSUploader*> uploaderMap;
    QHash<QNetworkReply*, QString> loginReplyMap;

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_actionupload_triggered();
    void onLoginFinished();
    void onUploadFinished(QString filePath);
    void onUploadProgress(QString filePath, qint64 total, qint64 actualSent, int percent, double speed, QString unit);
};

#endif // MAINWINDOW_H
