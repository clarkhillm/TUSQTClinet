#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tusuploader.h"
#include <QMainWindow>

QT_MESSAGELOGCONTEXT

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_actionupload_triggered();
    void onLoginFinished();

private:
    Ui::MainWindow* ui;

    TUSUploader* fileUploader;
};

#endif // MAINWINDOW_H
