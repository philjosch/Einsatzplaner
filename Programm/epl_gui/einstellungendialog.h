#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "coreapplication.h"

#include <QDialog>

namespace Ui {
class EinstellungenDialog;
}

class EinstellungenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EinstellungenDialog(QWidget *parent = nullptr);
    ~EinstellungenDialog();

private slots:
    void searchUpdate();

    void closeDialogOk();

    void showNotes();

private:
    void saveSettings();

    Ui::EinstellungenDialog *ui;
    Version online;
};

#endif // PREFERENCESDIALOG_H
