#include "activitywindow.h"
#include "ui_activitywindow.h"

ActivityWindow::ActivityWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ActivityWindow)
{
    ui->setupUi(this);
}

ActivityWindow::~ActivityWindow()
{
    delete ui;
}

void ActivityWindow::on_buttonInsert_clicked()
{
    ui->tablePersonen->insertRow(0);
}

void ActivityWindow::on_buttonRemove_clicked()
{
    ui->tablePersonen->removeRow(ui->tablePersonen->currentRow());
}