#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "coreapplication.h"

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

private slots:
    void on_pushSearch_clicked();
    void on_pushDownload_clicked();

    void closeDialogOk();

    void on_pushNotes_clicked();

private:
    void saveSettings();

    Ui::PreferencesDialog *ui;
    CoreApplication::Version online;
};

#endif // PREFERENCESDIALOG_H
