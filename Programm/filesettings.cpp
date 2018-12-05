#include "filesettings.h"
#include "ui_filesettings.h"

FileSettings::FileSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSettings)
{
    ui->setupUi(this);
}

FileSettings::~FileSettings()
{
    delete ui;
}

void FileSettings::on_checkEnable_clicked(bool checked)
{
    ui->checkAuto->setEnabled(checked);
    ui->lineServer->setEnabled(checked);
    ui->linePath->setEnabled(checked);
    ui->lineID->setEnabled(checked);
    ui->pushCheck->setEnabled(checked);
    ui->comboFrom->setEnabled(checked);
    ui->comboTo->setEnabled(checked);
    ui->checkActivity->setEnabled(checked);
}
