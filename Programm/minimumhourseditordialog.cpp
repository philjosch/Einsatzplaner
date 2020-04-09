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

    ui->lineTf->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Tf)));
    ui->lineZf->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Zf)));
    ui->lineService->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Service)));
    ui->lineZub->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Zub)));
    ui->lineBuero->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Buero)));
    ui->lineWerkstatt->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Werkstatt)));
    ui->lineZugVorbereiten->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::ZugVorbereiten)));
    ui->lineAusbildung->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Ausbildung)));
    ui->lineSonstiges->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Category::Sonstiges)));
    ui->lineGesamt->setText(stringForDurationEditorFromMinutes(manager->getMinimumHours(Gesamt)));
}

MinimumHoursEditorDialog::~MinimumHoursEditorDialog()
{
    delete ui;
}

void MinimumHoursEditorDialog::setDefault()
{
    ui->lineTf->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Tf)));
    ui->lineTf->update();
    ui->lineZf->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Zf)));
    ui->lineZf->update();
    ui->lineService->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Service)));
    ui->lineService->update();
    ui->lineZub->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Zub)));
    ui->lineZub->update();
    ui->lineBuero->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Buero)));
    ui->lineBuero->update();
    ui->lineWerkstatt->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Werkstatt)));
    ui->lineWerkstatt->update();
    ui->lineZugVorbereiten->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::ZugVorbereiten)));
    ui->lineZugVorbereiten->update();
    ui->lineAusbildung->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Ausbildung)));
    ui->lineAusbildung->update();
    ui->lineSonstiges->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Category::Sonstiges)));
    ui->lineSonstiges->update();
    ui->lineGesamt->setText(stringForDurationEditorFromMinutes(manager->getMinimumHoursDefault(Gesamt)));
    ui->lineGesamt->update();
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
    manager->setMinimumHours(Category::Tf, minutesFromStringForDurationEditor(ui->lineTf->text()));
    manager->setMinimumHours(Category::Zf, minutesFromStringForDurationEditor(ui->lineZf->text()));
    manager->setMinimumHours(Category::Service, minutesFromStringForDurationEditor(ui->lineService->text()));
    manager->setMinimumHours(Category::Zub, minutesFromStringForDurationEditor(ui->lineZub->text()));
    manager->setMinimumHours(Category::Buero, minutesFromStringForDurationEditor(ui->lineBuero->text()));
    manager->setMinimumHours(Category::Werkstatt, minutesFromStringForDurationEditor(ui->lineWerkstatt->text()));
    manager->setMinimumHours(Category::ZugVorbereiten, minutesFromStringForDurationEditor(ui->lineZugVorbereiten->text()));
    manager->setMinimumHours(Category::Ausbildung, minutesFromStringForDurationEditor(ui->lineAusbildung->text()));
    manager->setMinimumHours(Category::Sonstiges, minutesFromStringForDurationEditor(ui->lineSonstiges->text()));
    manager->setMinimumHours(Category::Gesamt, minutesFromStringForDurationEditor(ui->lineGesamt->text()));
    QDialog::accept();
}
