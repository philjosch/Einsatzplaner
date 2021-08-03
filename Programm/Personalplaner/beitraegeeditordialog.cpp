#include "beitraegeeditordialog.h"
#include "ui_beitraegeeditordialog.h"

BeitraegeEditorDialog::BeitraegeEditorDialog(QWidget *parent, ManagerPersonal *manager) :
    QDialog(parent),
    ui(new Ui::BeitraegeEditorDialog)
{
    ui->setupUi(this);
    managerPersonal = manager;

    ui->labelAktiv->setText(Person::toString(Person::AktivenBeitrag));
    ui->labelPassiv->setText(Person::toString(Person::FoerderBeitrag));
    ui->labelErmaessigt->setText(Person::toString(Person::Ermaessigt));
    ui->labelFamilieZahler->setText(Person::toString(Person::FamilienBeitragZahler));
    ui->labelFamilieNutzer->setText(Person::toString(Person::FamilienBeitragNutzer));
    ui->labelFrei->setText(Person::toString(Person::Beitragsfrei));
    ui->labelUnbekannt->setText(Person::toString(Person::BeitragUnbekannt));

    ui->doubleAktiv->setValue(manager->getBeitrag(Person::AktivenBeitrag)/100.f);
    ui->doublePassiv->setValue(manager->getBeitrag(Person::FoerderBeitrag)/100.f);
    ui->doubleErmaessigt->setValue(manager->getBeitrag(Person::Ermaessigt)/100.f);
    ui->doubleFamilieZahler->setValue(manager->getBeitrag(Person::FamilienBeitragZahler)/100.f);
    ui->doubleFamilieNutzer->setValue(manager->getBeitrag(Person::FamilienBeitragNutzer)/100.f);
    ui->doubleFrei->setValue(manager->getBeitrag(Person::Beitragsfrei)/100.f);
    ui->doubleUnbekannt->setValue(manager->getBeitrag(Person::BeitragUnbekannt)/100.f);
}

BeitraegeEditorDialog::~BeitraegeEditorDialog()
{
    delete ui;
}

void BeitraegeEditorDialog::on_buttonBox_accepted()
{
    managerPersonal->setBeitrag(Person::AktivenBeitrag, ui->doubleAktiv->value()*100);
    managerPersonal->setBeitrag(Person::FoerderBeitrag, ui->doublePassiv->value()*100);
    managerPersonal->setBeitrag(Person::Ermaessigt, ui->doubleErmaessigt->value()*100);
    managerPersonal->setBeitrag(Person::FamilienBeitragZahler, ui->doubleFamilieZahler->value()*100);
    managerPersonal->setBeitrag(Person::FamilienBeitragNutzer, ui->doubleFamilieNutzer->value()*100);
    managerPersonal->setBeitrag(Person::Beitragsfrei, ui->doubleFrei->value()*100);
    managerPersonal->setBeitrag(Person::BeitragUnbekannt, ui->doubleUnbekannt->value()*100);
    QDialog::close();
}
