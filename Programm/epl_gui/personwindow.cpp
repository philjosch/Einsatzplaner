#include "personwindow.h"
#include "ui_personwindow.h"

#include "export.h"

#include <QDesktopServices>
#include <QMessageBox>

PersonWindow::PersonWindow(CoreMainWindow *parent, Person *p) :
    QMainWindow(parent), ui(new Ui::PersonWindow)
{
    ui->setupUi(this);
    ui->buttonGeschlecht->setId(ui->radioUnbekannt,  Person::Geschlecht::GeschlechtUnbekannt);
    ui->buttonGeschlecht->setId(ui->radioJuristisch, Person::Geschlecht::Juristisch);
    ui->buttonGeschlecht->setId(ui->radioMaennlich,  Person::Geschlecht::Maennlich);
    ui->buttonGeschlecht->setId(ui->radioWeiblich,   Person::Geschlecht::Weiblich);
    ui->buttonGeschlecht->setId(ui->radioDivers,     Person::Geschlecht::Divers);

    person = p;

    connect(this, &PersonWindow::loeschen, parent, &CoreMainWindow::loeschenPerson);
    connect(ui->actionMail, &QAction::triggered, this, &PersonWindow::sendMail);
    connect(ui->actionLoeschen, &QAction::triggered, this, &PersonWindow::deleteTriggered);
    connect(ui->actionEinzelPDF, &QAction::triggered, this, &PersonWindow::exportSinglePdf);
    connect(ui->actionEinzelDrucken, &QAction::triggered, this, &PersonWindow::exportSinglePrint);

    connect(ui->lineVorname, &QLineEdit::textChanged, this, &PersonWindow::changedFirstname);
    connect(ui->lineNachname, &QLineEdit::textChanged, this, &PersonWindow::changedFamilyname);

    connect(ui->lineID, &QLineEdit::textChanged, this, &PersonWindow::changedID);
    connect(ui->pushAutoID, &QPushButton::clicked, this, &PersonWindow::autoID);

    connect(ui->dateGeburtstag, &QDateEdit::dateChanged, this, &PersonWindow::changedBirthday);
    connect(ui->checkGeburtstag, &QCheckBox::clicked, this, &PersonWindow::changedBirthdayStatus);

    connect(ui->dateEintritt, &QDateEdit::dateChanged, this, &PersonWindow::changedJoiningDate);
    connect(ui->checkEintritt, &QCheckBox::clicked, this, &PersonWindow::changedJoiningStatus);

    connect(ui->checkAktiv, &QCheckBox::clicked, this, &PersonWindow::changedMembershipStatus);

    connect(ui->spinKm, QOverload<int>::of(&QSpinBox::valueChanged), this, &PersonWindow::changedDistance);

    connect(ui->lineJob, &QLineEdit::textChanged, this, &PersonWindow::changedJob);
    connect(ui->lineStrasse, &QLineEdit::textChanged, this, &PersonWindow::changedStreet);
    connect(ui->linePLZ, &QLineEdit::textChanged, this, &PersonWindow::changedPostalCode);
    connect(ui->lineOrt, &QLineEdit::textChanged, this, &PersonWindow::changedPlace);

    connect(ui->linePhone, &QLineEdit::textChanged, this, &PersonWindow::changedPhoneFirst);
    connect(ui->linePhone2, &QLineEdit::textChanged, this, &PersonWindow::changedPhoneSecond);
    connect(ui->checkPhone, &QCheckBox::clicked, this, &PersonWindow::changedPhoneStatus);

    connect(ui->lineMail, &QLineEdit::textChanged, this, &PersonWindow::changedMail);
    connect(ui->checkMail, &QCheckBox::clicked, this, &PersonWindow::changedMailStatus);

    connect(ui->checkTf, &QCheckBox::clicked, this, &PersonWindow::changedTfStatus);
    connect(ui->checkZf, &QCheckBox::clicked, this, &PersonWindow::changedZfStatus);
    connect(ui->checkRangierer, &QCheckBox::clicked, this, &PersonWindow::changedRangiererStatus);

    connect(ui->dateDienst, &QDateEdit::dateChanged, this, &PersonWindow::changedFitnessDate);
    connect(ui->checkDienst, &QCheckBox::clicked, this, &PersonWindow::changedFitnessStatus);

    connect(ui->plainBemerkung, &QPlainTextEdit::textChanged, this, &PersonWindow::changedCommentsGeneral);

    connect(ui->checkAustritt, &QCheckBox::clicked, this, &PersonWindow::changedResignationStatus);
    connect(ui->dateAustritt, &QDateEdit::dateChanged, this, &PersonWindow::changedResignationDate);

    connect(ui->comboAnrede, &QComboBox::currentTextChanged, this, &PersonWindow::changedTitle);
    connect(ui->buttonGeschlecht, &QButtonGroup::idClicked, this, &PersonWindow::changedGender);

    connect(ui->comboBeitragsart, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PersonWindow::changedContributionType);
    connect(ui->lineIBAN, &QLineEdit::textChanged, this, &PersonWindow::changedIBAN);
    connect(ui->lineBank, &QLineEdit::textChanged, this, &PersonWindow::changedBank);
    connect(ui->lineKontoinhaber, &QLineEdit::textChanged, this, &PersonWindow::changedDepositor);

    connect(ui->plainBetrieb, &QPlainTextEdit::textChanged, this, &PersonWindow::changedCommentsOperation);
    connect(ui->plainAusbildung, &QPlainTextEdit::textChanged, this, &PersonWindow::changedCommentsEducation);

    enabled = false;

    ui->lineID->setText(QString::number(p->getNummer()));
    ui->lineVorname->setText(p->getVorname());
    ui->lineNachname->setText(p->getNachname());

    // Persoenliches
    ui->checkGeburtstag->setChecked(p->getGeburtstag().isNull());
    ui->dateGeburtstag->setEnabled(p->getGeburtstag().isValid());
    ui->dateGeburtstag->setDate(p->getGeburtstag());
    ui->lineJob->setText(p->getBeruf());
    ui->comboAnrede->setCurrentText(p->getAnrede());
    ui->buttonGeschlecht->button(p->getGeschlecht())->click();


    // Mitgliedschaft
    ui->checkEintritt->setChecked(p->getEintritt().isNull());
    ui->dateEintritt->setEnabled(p->getEintritt().isValid());
    ui->dateEintritt->setDate(p->getEintritt());
    ui->checkAktiv->setChecked(p->getAktiv());
    ui->checkAustritt->setChecked(p->getAustritt().isValid());
    ui->dateAustritt->setEnabled(p->getAustritt().isValid());
    ui->dateAustritt->setDate(p->getAustritt());

    ui->comboBeitragsart->setCurrentIndex(p->getBeitragsart());
    changedContributionType(ui->comboBeitragsart->currentIndex());
    ui->lineIBAN->setText(p->getIban());
    ui->lineBank->setText(p->getBank());
    ui->lineKontoinhaber->setText(p->getKontoinhaber());
    if (p->getBeitragsart() == Person::Beitragsart::FamilienBeitragNutzer) {
        ui->labelKonto->setText("Zahler");
        ui->lineIBAN->setEnabled(false);
        ui->lineBank->setEnabled(false);
    }
    ui->lineBeitrag->setText(tr("%L1 €").arg(p->getBeitrag()/100.f, 0, 'f', 2));
    if (person->getBeitragNachzahlung() != 0)
        ui->lineBeitrag->setText(tr("%L1 € (+ %L2 €)")
                                 .arg(person->getBeitrag()/100.f, 0, 'f', 2)
                                 .arg(person->getBeitragNachzahlung()/100.f, 0, 'f', 2));

    // Kontakt
    ui->lineStrasse->setText(p->getStrasse());
    ui->linePLZ->setText(p->getPLZ());
    ui->lineOrt->setText(p->getOrt());
    ui->spinKm->setValue(p->getStrecke());

    ui->linePhone->setText(p->getTelefon());
    ui->linePhone2->setText(p->getTelefon2());
    ui->lineMail->setText(p->getMail());

    // Betriebsdienst
    ui->checkTf->setChecked(p->getAusbildungTf());
    ui->checkZf->setChecked(p->getAusbildungZf());
    ui->checkRangierer->setChecked(p->getAusbildungRangierer());

    ui->checkDienst->setChecked(p->getTauglichkeit().isNull());
    ui->dateDienst->setEnabled(p->getTauglichkeit().isValid());
    ui->dateDienst->setDate(p->getTauglichkeit());

    ui->plainBetrieb->setPlainText(p->getSonstigeBetrieblich().replace("<br/>", "\n"));
    ui->plainAusbildung->setPlainText(p->getSonstigeAusbildung().replace("<br/>", "\n"));

    // Sonstiges
    ui->checkMail->setChecked(p->getMailOK());
    ui->checkPhone->setChecked(p->getTelefonOK());

    ui->plainBemerkung->setPlainText(p->getBemerkungen().replace("<br/>","\n"));

    enabled = true;

    setWindowTitle(person->getName());
}

PersonWindow::~PersonWindow()
{
    delete ui;
}

void PersonWindow::sendMail()
{
    if (person->getMail() != "") {
        QDesktopServices::openUrl(QUrl("mailto:"+person->getMail()));
    }
}

void PersonWindow::deleteTriggered()
{
    emit loeschen(person);
}

void PersonWindow::exportSinglePdf()
{
    person->exportMemberdataAsHtml(ExportHtmlPdf::generate(this, "Stammdaten-"+person->getName(), QPageLayout::Orientation::Portrait));
}
void PersonWindow::exportSinglePrint()
{
    person->exportMemberdataAsHtml(ExportHtmlPaper::generate(this, QPageLayout::Orientation::Portrait));
}


void PersonWindow::changedFirstname(const QString &arg1)
{
    if (enabled) {
        if (person->setVorname(arg1)) {
            setWindowTitle(person->getName());
            ui->lineKontoinhaber->setPlaceholderText(person->getName());
        } else {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        }
    }
}
void PersonWindow::changedFamilyname(const QString &arg1)
{
    if (enabled) {
        if (person->setNachname(arg1)) {
            setWindowTitle(person->getName());
            ui->lineKontoinhaber->setPlaceholderText(person->getName());
        } else {
            QMessageBox::information(this, tr("Name doppelt vergeben"), tr("Der eingegebene Namen ist bereits im System registriert.\nSomit kann keine zweite Personen den gleichen Namen haben!"));
        }
    }
}

void PersonWindow::changedID(const QString &arg1)
{
    if (enabled) {
        if (arg1.toInt() > 0) {
            if (! person->setNummer(arg1.toInt())) {
                QMessageBox::information(this, tr("Fehler"), tr("Die Mitgliedsnummer konnte nicht geändert werden, da sie bereits vergeben ist. Bitte wählen Sie eine andere Nummer!"));
            }
        }
    }
}
void PersonWindow::autoID()
{
    if (enabled) {
        enabled=false;
        ui->lineID->setText(QString::number(person->setAutoNummer()));
        enabled=true;
    }
}

void PersonWindow::changedBirthday(const QDate &date)
{
    if (enabled) {
        person->setGeburtstag(date);
    }
}
void PersonWindow::changedBirthdayStatus(bool checked)
{
    if (enabled) {
        ui->dateGeburtstag->setEnabled(!checked);
        if (checked) {
            person->setGeburtstag(QDate());
        } else {
            ui->dateGeburtstag->setDate(QDate::currentDate());
            person->setGeburtstag(ui->dateGeburtstag->date());
        }
    }
}

void PersonWindow::changedJoiningDate(const QDate &date)
{
    if (enabled) {
        person->setEintritt(date);
    }
}
void PersonWindow::changedJoiningStatus(bool checked)
{
    if (enabled) {
        ui->dateEintritt->setEnabled(!checked);
        if (checked) {
            person->setEintritt(QDate());
        } else {
            ui->dateEintritt->setDate(QDate::currentDate());
            person->setEintritt(ui->dateEintritt->date());
        }
    }
}

void PersonWindow::changedMembershipStatus(bool checked)
{
    if (enabled) {
        person->setAktiv(checked);
    }
}

void PersonWindow::changedDistance(int arg1)
{
    if (enabled) {
        person->setStrecke(arg1);
    }
}

void PersonWindow::changedJob(const QString &arg1)
{
    if (enabled) {
        person->setBeruf(arg1);
    }
}

void PersonWindow::changedStreet(const QString &arg1)
{
    if (enabled) {
        person->setStrasse(arg1);
    }
}
void PersonWindow::changedPostalCode(const QString &arg1)
{
    if (enabled) {
        person->setPLZ(arg1);
    }
}
void PersonWindow::changedPlace(const QString &arg1)
{
    if (enabled) {
        person->setOrt(arg1);
    }
}

void PersonWindow::changedPhoneFirst(const QString &arg1)
{
    if (enabled) {
        person->setTelefon(arg1);
    }
}
void PersonWindow::changedPhoneStatus(bool checked)
{
    if (enabled) {
        person->setTelefonOK(checked);
    }
}

void PersonWindow::changedMail(const QString &arg1)
{
    if (enabled) {
        person->setMail(arg1);
    }
}
void PersonWindow::changedMailStatus(bool checked)
{
    if (enabled) {
        person->setMailOK(checked);
    }
}

void PersonWindow::changedTfStatus(bool checked)
{
    if (enabled) {
        person->setAusbildungTf(checked);
    }
}
void PersonWindow::changedZfStatus(bool checked)
{
    if (enabled) {
        person->setAusbildungZf(checked);
    }
}
void PersonWindow::changedRangiererStatus(bool checked)
{
    if (enabled) {
        person->setAusbildungRangierer(checked);
    }
}

void PersonWindow::changedFitnessDate(const QDate &date)
{
    if (enabled) {
        person->setTauglichkeit(date);
    }
}
void PersonWindow::changedFitnessStatus(bool checked)
{
    if (enabled) {
        ui->dateDienst->setEnabled(!checked);
        if (checked) {
            person->setTauglichkeit(QDate());
        } else {
            ui->dateDienst->setDate(QDate::currentDate());
            person->setTauglichkeit(ui->dateDienst->date());
        }
    }
}

void PersonWindow::changedCommentsGeneral()
{
    if (enabled) {
        person->setBemerkungen(ui->plainBemerkung->toPlainText().replace("\n","<br/>"));
    }
}

void PersonWindow::changedResignationDate(const QDate &date)
{
    if (enabled) {
        person->setAustritt(date);
    }
}
void PersonWindow::changedResignationStatus(bool checked)
{
    if (enabled) {
        ui->dateAustritt->setEnabled(checked);
        if (checked) {
            ui->dateAustritt->setDate(QDate::currentDate());
            person->setAustritt(ui->dateAustritt->date());
        } else {
            person->setAustritt(QDate());
        }
    }
}

void PersonWindow::changedTitle(const QString &arg1)
{
    if (enabled)
        person->setAnrede(arg1);
}

void PersonWindow::changedGender(int button)
{
    if (enabled)
        person->setGeschlecht(static_cast<Person::Geschlecht>(button));
}

void PersonWindow::changedContributionType(int index)
{
    Person::Beitragsart ba = static_cast<Person::Beitragsart>(index);
    if (enabled)
        person->setBeitragsart(ba);

    if (ba == Person::Beitragsart::FamilienBeitragNutzer) {
        ui->lineIBAN->setEnabled(false);
        ui->lineBank->setEnabled(false);
        ui->labelKonto->setText(tr("Zahler"));
        ui->lineKontoinhaber->setPlaceholderText("");
        ui->lineKontoinhaber->setToolTip("");
        ui->lineKontoinhaber->setStatusTip(tr("Name der Person, die den Beitrag bezahlt."));
    } else {
        ui->lineIBAN->setEnabled(true);
        ui->lineBank->setEnabled(true);
        ui->labelKonto->setText(tr("Kontoinhaber"));
        ui->lineKontoinhaber->setPlaceholderText(person->getName());
        ui->lineKontoinhaber->setToolTip(tr("Name des Kontoinhabers, sofern abweichend vom Namen der Person."));
        ui->lineKontoinhaber->setStatusTip(tr("Abweichender Kontoinhaber"));
    }
    ui->lineBeitrag->setText(tr("%1 €").arg(person->getBeitrag()/100.f, 0, 'f', 2));
    if (person->getBeitragNachzahlung() != 0)
        ui->lineBeitrag->setText(tr("%1 € (+ %2 €)")
                                 .arg(person->getBeitrag()/100.f, 0, 'f', 2)
                                 .arg(person->getBeitragNachzahlung()/100.f, 0, 'f', 2));
}

void PersonWindow::changedIBAN(const QString &arg1)
{
    if (enabled)
        person->setIban(arg1);
}

void PersonWindow::changedBank(const QString &arg1)
{
    if (enabled)
        person->setBank(arg1);
}

void PersonWindow::changedDepositor(const QString &arg1)
{
    if (enabled)
        person->setKontoinhaber(arg1);
}

void PersonWindow::changedPhoneSecond(const QString &arg1)
{
    if (enabled)
        person->setTelefon2(arg1);
}

void PersonWindow::changedCommentsOperation()
{
    if (enabled)
        person->setSonstigeBetrieblich(ui->plainBetrieb->toPlainText().replace("\n","<br/>"));
}

void PersonWindow::changedCommentsEducation()
{
    if (enabled)
        person->setSonstigeAusbildung(ui->plainAusbildung->toPlainText().replace("\n","<br/>"));
}
