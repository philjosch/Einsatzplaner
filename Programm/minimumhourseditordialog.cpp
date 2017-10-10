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

    ui->doubleTf->setValue(manager->getMinimumHours(Category::Tf));
    ui->doubleZf->setValue(manager->getMinimumHours(Category::Zf));
    ui->doubleService->setValue(manager->getMinimumHours(Category::Service));
    ui->doubleZub->setValue(manager->getMinimumHours(Category::Zub));
    ui->doubleBuero->setValue(manager->getMinimumHours(Category::Buero));
    ui->doubleWerkstatt->setValue(manager->getMinimumHours(Category::Werkstatt));
    ui->doubleZugVorbereiten->setValue(manager->getMinimumHours(Category::ZugVorbereiten));
    ui->doubleSonstiges->setValue(manager->getMinimumHours(Category::Sonstiges));
    ui->doubleGesamt->setValue(manager->getMinimumHours());
}

MinimumHoursEditorDialog::~MinimumHoursEditorDialog()
{
    delete ui;
}

void MinimumHoursEditorDialog::setDefault()
{
    ui->doubleTf->setValue(manager->getMinimumHoursDefault(Category::Tf));
    ui->doubleZf->setValue(manager->getMinimumHoursDefault(Category::Zf));
    ui->doubleService->setValue(manager->getMinimumHoursDefault(Category::Service));
    ui->doubleZub->setValue(manager->getMinimumHoursDefault(Category::Zub));
    ui->doubleBuero->setValue(manager->getMinimumHoursDefault(Category::Buero));
    ui->doubleWerkstatt->setValue(manager->getMinimumHoursDefault(Category::Werkstatt));
    ui->doubleZugVorbereiten->setValue(manager->getMinimumHoursDefault(Category::ZugVorbereiten));
    ui->doubleSonstiges->setValue(manager->getMinimumHoursDefault(Category::Sonstiges));
    ui->doubleGesamt->setValue(manager->getMinimumHoursDefault());
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
    manager->setMinimumHours(Category::Tf, ui->doubleTf->value());
    manager->setMinimumHours(Category::Zf, ui->doubleZf->value());
    manager->setMinimumHours(Category::Service, ui->doubleService->value());
    manager->setMinimumHours(Category::Zub, ui->doubleZub->value());
    manager->setMinimumHours(Category::Buero, ui->doubleBuero->value());
    manager->setMinimumHours(Category::Werkstatt, ui->doubleWerkstatt->value());
    manager->setMinimumHours(Category::ZugVorbereiten, ui->doubleZugVorbereiten->value());
    manager->setMinimumHours(Category::Sonstiges, ui->doubleSonstiges->value());
    manager->setMinimumHours(ui->doubleGesamt->value());
    QDialog::accept();
}
