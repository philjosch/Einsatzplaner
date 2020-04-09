#include "minimumhourseditordialog.h"
#include "ui_minimumhourseditordialog.h"

#include "aactivity.h"

MinimumHoursEditorDialog::MinimumHoursEditorDialog(ManagerPersonal *manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MinimumHoursEditorDialog)
{
    ui->setupUi(this);
    QDialog::setWindowModality(Qt::WindowModal);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(abstractButtonClickedSlot(QAbstractButton*)));
    this->manager = manager;

    ui->lineTf->setText(stringFromTime(manager->getMinimumHours(Category::Tf)));
    ui->lineZf->setText(stringFromTime(manager->getMinimumHours(Category::Zf)));
    ui->lineService->setText(stringFromTime(manager->getMinimumHours(Category::Service)));
    ui->lineZub->setText(stringFromTime(manager->getMinimumHours(Category::Zub)));
    ui->lineBuero->setText(stringFromTime(manager->getMinimumHours(Category::Buero)));
    ui->lineWerkstatt->setText(stringFromTime(manager->getMinimumHours(Category::Werkstatt)));
    ui->lineZugVorbereiten->setText(stringFromTime(manager->getMinimumHours(Category::ZugVorbereiten)));
    ui->lineAusbildung->setText(stringFromTime(manager->getMinimumHours(Category::Ausbildung)));
    ui->lineSonstiges->setText(stringFromTime(manager->getMinimumHours(Category::Sonstiges)));
    ui->lineGesamt->setText(stringFromTime(manager->getMinimumHours(Gesamt)));
}

MinimumHoursEditorDialog::~MinimumHoursEditorDialog()
{
    delete ui;
}

void MinimumHoursEditorDialog::setDefault()
{
    ui->lineTf->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Tf)));
    ui->lineZf->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Zf)));
    ui->lineService->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Service)));
    ui->lineZub->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Zub)));
    ui->lineBuero->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Buero)));
    ui->lineWerkstatt->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Werkstatt)));
    ui->lineZugVorbereiten->setText(stringFromTime(manager->getMinimumHoursDefault(Category::ZugVorbereiten)));
    ui->lineAusbildung->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Ausbildung)));
    ui->lineSonstiges->setText(stringFromTime(manager->getMinimumHoursDefault(Category::Sonstiges)));
    ui->lineGesamt->setText(stringFromTime(manager->getMinimumHoursDefault(Gesamt)));
}

void MinimumHoursEditorDialog::abstractButtonClickedSlot(QAbstractButton *button)
{
    if (button == (QAbstractButton*)ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)) {
        setDefault();
    } else if (button == (QAbstractButton*)ui->buttonBox->button(QDialogButtonBox::Ok)) {
        save();
    } else if (button == (QAbstractButton*)ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        QDialog::close();
    }
}

void MinimumHoursEditorDialog::save()
{
    manager->setMinimumHours(Category::Tf, timeFromString(ui->lineTf->text()));
    manager->setMinimumHours(Category::Zf, timeFromString(ui->lineZf->text()));
    manager->setMinimumHours(Category::Service, timeFromString(ui->lineService->text()));
    manager->setMinimumHours(Category::Zub, timeFromString(ui->lineZub->text()));
    manager->setMinimumHours(Category::Buero, timeFromString(ui->lineBuero->text()));
    manager->setMinimumHours(Category::Werkstatt, timeFromString(ui->lineWerkstatt->text()));
    manager->setMinimumHours(Category::ZugVorbereiten, timeFromString(ui->lineZugVorbereiten->text()));
    manager->setMinimumHours(Category::Ausbildung, timeFromString(ui->lineAusbildung->text()));
    manager->setMinimumHours(Category::Sonstiges, timeFromString(ui->lineSonstiges->text()));
    manager->setMinimumHours(Category::Gesamt, timeFromString(ui->lineGesamt->text()));
    QDialog::accept();
}

int MinimumHoursEditorDialog::timeFromString(QString arg)
{
    if (arg.contains(":")) {
        QStringList l = arg.split(":");
        return l.at(0).toInt()*60+l.at(1).toInt();
    }
    return arg.toInt();
}

QString MinimumHoursEditorDialog::stringFromTime(int arg)
{
    return QString("%1:%2").arg(int(arg/60), 4, 10, QLatin1Char('0')).arg(arg % 60, 2, 10,QLatin1Char('0'));
}
