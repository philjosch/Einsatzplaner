#ifndef FILESETTINGS_H
#define FILESETTINGS_H

#include <QDialog>

namespace Ui {
class FileSettings;
}

class FileSettings : public QDialog
{
    Q_OBJECT

public:
    explicit FileSettings(QWidget *parent = nullptr);
    ~FileSettings();

private slots:
    void on_checkEnable_clicked(bool checked);

private:
    Ui::FileSettings *ui;
};

#endif // FILESETTINGS_H
