#ifndef FILESETTINGSDIALOG_H
#define FILESETTINGSDIALOG_H

#include "filesettings.h"
#include "eplfile.h"

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class FileSettingsDialog;
}

class FileSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileSettingsDialog(QWidget *parent, EplFile *dat);
    ~FileSettingsDialog();

    void saveSettings();

private slots:
    void autoUploadAktivieren(bool checked);

    void dialogButtonClicked(QAbstractButton *button);

    void testeServer();

    void passwordAendern();

private:
    Ui::FileSettingsDialog *ui;
    EplFile *datei;

    void loadSettings();
};

#endif // FILESETTINGSDIALOG_H
