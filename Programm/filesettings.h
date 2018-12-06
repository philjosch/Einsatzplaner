#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include "managerfilesettings.h"

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class FileSettings;
}

class FileSettings : public QDialog
{
    Q_OBJECT

public:
    explicit FileSettings(QWidget *parent, ManagerFileSettings *manager);
    ~FileSettings();

    void getSettings(ManagerFileSettings *mgr);

private slots:
    void on_checkEnable_clicked(bool checked);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_pushCheck_clicked();

private:
    Ui::FileSettings *ui;
    ManagerFileSettings *mngr;

    void loadSettings();
};

#endif // FILESETTINGS_H
