#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include <QtWidgets>

namespace Ui {
class ProgressWidget;
}

class ProgressWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget* parent = 0);
    ~ProgressWidget();

    QString getFilePath();
    void setFilePath(QString filePath);
    QProgressBar* getProgressBar();
    QLabel* getSpeedLabel();
    QLabel* getNameLabel();
    QLabel* getPercentageLabel();

private:
    Ui::ProgressWidget* ui;
    QString filePath;
};

#endif // PROGRESSWIDGET_H
