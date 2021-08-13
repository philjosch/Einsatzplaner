#ifndef FILESETTINGSDIALOG_H
#define FILESETTINGSDIALOG_H

#include "filesettings.h"

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class FileSettingsDialog;
}

class FileSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileSettingsDialog(QWidget *parent, FileSettings *manager);
    ~FileSettingsDialog();

    void saveSettings();

private slots:
    void on_groupUpload_clicked(bool checked);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_pushCheck_clicked();

    void on_pushPwdChange_clicked();

private:
    Ui::FileSettingsDialog *ui;
    FileSettings *mngr;

    void loadSettings();
};

#endif // FILESETTINGSDIALOG_H
