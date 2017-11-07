#include "progresswidget.h"
#include "ui_progresswidget.h"

ProgressWidget::ProgressWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ProgressWidget)
{
    ui->setupUi(this);
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

QString ProgressWidget::getFilePath()
{
    return filePath;
}

void ProgressWidget::setFilePath(QString filePath)
{
    this->filePath = filePath;
}

QProgressBar* ProgressWidget::getProgressBar()
{
    return ui->progressBar;
}
QLabel* ProgressWidget::getSpeedLabel()
{
    return ui->label;
}
QLabel* ProgressWidget::getNameLabel()
{
    return ui->nameLabel;
}
QLabel* ProgressWidget::getPercentageLabel()
{
    return ui->percentageLabel;
}
