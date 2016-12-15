#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

private slots:
    void on_pushSearch_clicked();
    void on_pushDownload_clicked();

    void closeDialogOk();

    void on_pushNotes_clicked();

private:
    Ui::PreferencesDialog *ui;

    void saveSettings();

    QString online;
};

#endif // PREFERENCESDIALOG_H
