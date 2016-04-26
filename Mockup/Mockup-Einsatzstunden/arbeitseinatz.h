#ifndef ARBEITSEINATZ_H
#define ARBEITSEINATZ_H

#include <QMainWindow>

namespace Ui {
class Arbeitseinatz;
}

class Arbeitseinatz : public QMainWindow
{
    Q_OBJECT

public:
    explicit Arbeitseinatz(QWidget *parent = 0);
    ~Arbeitseinatz();

private:
    Ui::Arbeitseinatz *ui;
};

#endif // ARBEITSEINATZ_H
