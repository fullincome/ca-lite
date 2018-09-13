#include "cert_params.h"
#include "ui_cert_params.h"

CertParams::CertParams(QWidget *parent) :
    QDialog(parent),
    ui_cp(new Ui::CertParams)
{
    ui_cp->setupUi(this);
    ui_cp->creatBtn->setEnabled(false);

    ui_cp->authorityKeyIdentifierEdit->setDisabled(true);
    ui_cp->authorityKeyIdentifierEdit->setStyleSheet("color: rgb(100,100,100)");
    ui_cp->authorityKeyIdentifierEdit->setPlainText("Field available only on sign");

    ui_cp->nameCertLabel->setStyleSheet("color: rgb(230,0,0)");

    // Если контейнеры есть, то сработает
    // on_containersBox_currentIndexChanged
    // и поле сразу заполнится, поэтому проверяем
    if (ui_cp->keycontCertEdit->toPlainText().size() > 0) {
        ui_cp->keycontCertLabel->setStyleSheet("color: rgb(0,150,0)");
    }
    else {
        ui_cp->keycontCertLabel->setStyleSheet("color: rgb(230,0,0)");
    }
}

CertParams::~CertParams()
{
    delete ui_cp;
}


//------------------------------------------------------
//------------------СОЗДАНИЕ CSR------------------------
//------------------------------------------------------
//Инициализация детерменированных параметров
void CertParams::initialise(QString table_name)
{
    table.table_name = table_name;
    for (int i = 0; i < conts_info.size(); ++i)
    {
        ui_cp->containersBox->addItem(conts_info[i]);
    }
    ui_cp->basicConstraintsContBox->addItems(
                QStringList() << "critical"
                              << "CA:FALSE");

    ui_cp->authorityKeyIdentifierContBox->addItems(
                QStringList() << "keyid:always");

    ui_cp->subjectKeyIdentifierContBox->addItems(
                QStringList() << "hash");

    ui_cp->keyUsageContBox->addItems(
                QStringList() << "digitalSignature"
                              << "keyEncipherment"
                              << "nonRepudiation");

    ui_cp->nsCertTypeContBox->addItems(
                QStringList() << "client"
                              << "server");

    ui_cp->subjectAltNameContBox->addItems(
                QStringList() << "email:calite@calite.ru"
                              << "DNS:calite.ru"
                              << "DNS:onemore.calite.ru");

    ui_cp->extendedKeyUsageContBox->addItems(
                QStringList() << "1.3.6.1.5.5.7.3.1"
                              << "1.3.6.1.5.5.7.3.2");
}
//Установка выбранных параметров
void CertParams::setParams() {
    QString str_field;

    str_field = ui_cp->nameCertEdit->toPlainText();
    if (!str_field.isEmpty()) table.CN = str_field;

    str_field = ui_cp->organizationEdit->toPlainText();
    if (!str_field.isEmpty()) table.O = str_field;

    str_field = ui_cp->countryEdit->toPlainText();
    if (!str_field.isEmpty()) table.C = str_field;

    str_field = ui_cp->localityEdit->toPlainText();
    if (!str_field.isEmpty()) table.L = str_field;

    str_field = ui_cp->daysValidEdit->toPlainText();
    if (!str_field.isEmpty()) table.days_valid = str_field;

    str_field = ui_cp->keycontCertEdit->toPlainText();
    if (!str_field.isEmpty()) table.key = str_field;

    str_field = "CN = '" + table.CN + "'";
    if (!str_field.isEmpty()) table.condition = str_field;

    // Секция расширений
    str_field = ui_cp->basicConstraintsEdit->toPlainText();
    if (!str_field.isEmpty()) table.cert_extension.basicConstraints = str_field;

    if (ui_cp->authorityKeyIdentifierEdit->isEnabled())
    {
        str_field = ui_cp->authorityKeyIdentifierEdit->toPlainText();
        if (!str_field.isEmpty()) table.cert_extension.authorityKeyIdentifier = str_field;
    }

    str_field = ui_cp->subjectKeyIdentifierEdit->toPlainText();
    if (!str_field.isEmpty()) table.cert_extension.subjectKeyIdentifier = str_field;

    str_field = ui_cp->keyUsageEdit->toPlainText();
    if (!str_field.isEmpty()) table.cert_extension.keyUsage = str_field;

    str_field = ui_cp->nsCertTypeEdit->toPlainText();
    if (!str_field.isEmpty()) table.cert_extension.nsCertType = str_field;

    str_field = ui_cp->subjectAltNameEdit->toPlainText();
    if (!str_field.isEmpty()) table.cert_extension.subjectAltName = str_field;

    str_field = ui_cp->extendedKeyUsageEdit->toPlainText();
    if (!str_field.isEmpty()) table.cert_extension.extendedKeyUsage = str_field;


}
//Кнопка: Создать CSR
void CertParams::on_creatBtn_clicked()
{
    setParams();
    emit readyToCheck(table);
}
//Кнопка: Отмена
void CertParams::on_cancelBtn_clicked()
{
    this->close();
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------









//------------------------------------------------------
//---------------СЛОТЫ ПРИЕМА ДАННЫХ--------------------
//------------------------------------------------------
//Прием work_dir
void CertParams::getData(QStringList conts_info)
{
    this->conts_info = conts_info;
}
void CertParams::closeWindow(QString rc)
{
    this->close();
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------







//------------------------------------------------------
//---------ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ И СЛОТЫ--------------
//------------------------------------------------------
//Блокировка на ввод пустого текста
void CertParams::on_nameCertEdit_textChanged()
{
    if (ui_cp->nameCertEdit->toPlainText().size() > 0)
    {
        ui_cp->nameCertLabel->setStyleSheet("color: rgb(0,150,0)");
        ui_cp->creatBtn->setEnabled(true);
    }
    else
    {
        ui_cp->nameCertLabel->setStyleSheet("color: rgb(230,0,0)");
        ui_cp->creatBtn->setDisabled(true);
    }
}
//Блокировка на ввод пустого текста
void CertParams::on_keycontCertEdit_textChanged()
{
    if (ui_cp->keycontCertEdit->toPlainText().size() > 0)
    {
        ui_cp->keycontCertLabel->setStyleSheet("color: rgb(0,150,0)");
        ui_cp->creatBtn->setEnabled(true);
    }
    else
    {
        ui_cp->keycontCertLabel->setStyleSheet("color: rgb(230,0,0)");
        ui_cp->creatBtn->setDisabled(true);
    }
}
//Обработка выбранного контейнера в containersBox
void CertParams::on_containersBox_currentIndexChanged(const QString &arg1)
{
    QRegExp regexp("Name: (\\w+) \\(len = \\d+\\)");
    if (regexp.indexIn(arg1, 0) == -1)
    {
        //ui_mw->debugLogEdit->setPlainText(ui_mw->debugLogEdit->toPlainText() + prog.output
        //                                    + "\n В " + prog.file_out + " не удалось определить CN");
    }
    else
    {
        ui_cp->keycontCertEdit->setPlainText(regexp.cap(1));
    }
}
//Обработка выбранного поля в basicConstraintsContBox
void CertParams::on_basicConstraintsContBox_activated(const QString &arg1)
{
    if (ui_cp->basicConstraintsEdit->toPlainText().isEmpty())
    {
        ui_cp->basicConstraintsEdit->setPlainText(arg1);
    }
    else
    {
        ui_cp->basicConstraintsEdit->setPlainText(
                    ui_cp->basicConstraintsEdit->toPlainText() + ", " + arg1);
    }
}

//Обработка выбранного поля в authorityKeyIdentifierContBox
void CertParams::on_authorityKeyIdentifierContBox_activated(const QString &arg1)
{
    if (ui_cp->authorityKeyIdentifierEdit->isEnabled())
    {
        if (ui_cp->authorityKeyIdentifierEdit->toPlainText().isEmpty())
        {
            ui_cp->authorityKeyIdentifierEdit->setPlainText(arg1);
        }
        else
        {
            ui_cp->authorityKeyIdentifierEdit->setPlainText(
                        ui_cp->authorityKeyIdentifierEdit->toPlainText() + ", " + arg1);
        }
    }
}
//Обработка выбранного поля в subjectKeyIdentifierContBox
void CertParams::on_subjectKeyIdentifierContBox_activated(const QString &arg1)
{
    if (ui_cp->subjectKeyIdentifierEdit->toPlainText().isEmpty())
    {
        ui_cp->subjectKeyIdentifierEdit->setPlainText(arg1);
    }
    else
    {
        ui_cp->subjectKeyIdentifierEdit->setPlainText(
                    ui_cp->subjectKeyIdentifierEdit->toPlainText() + ", " + arg1);
    }
}
//Обработка выбранного поля в keyUsageContBox
void CertParams::on_keyUsageContBox_activated(const QString &arg1)
{
    if (ui_cp->keyUsageEdit->toPlainText().isEmpty())
    {
        ui_cp->keyUsageEdit->setPlainText(arg1);
    }
    else
    {
        ui_cp->keyUsageEdit->setPlainText(
                    ui_cp->keyUsageEdit->toPlainText() + ", " + arg1);
    }
}
//Обработка выбранного поля в nsCertTypeContBox
void CertParams::on_nsCertTypeContBox_activated(const QString &arg1)
{
    if (ui_cp->nsCertTypeEdit->toPlainText().isEmpty())
    {
        ui_cp->nsCertTypeEdit->setPlainText(arg1);
    }
    else
    {
        ui_cp->nsCertTypeEdit->setPlainText(
                    ui_cp->nsCertTypeEdit->toPlainText() + ", " + arg1);
    }
}
//Обработка выбранного поля в subjectAltNameContBox
void CertParams::on_subjectAltNameContBox_activated(const QString &arg1)
{
    if (ui_cp->subjectAltNameEdit->toPlainText().isEmpty())
    {
        ui_cp->subjectAltNameEdit->setPlainText(arg1);
    }
    else
    {
        ui_cp->subjectAltNameEdit->setPlainText(
                    ui_cp->subjectAltNameEdit->toPlainText() + ", " + arg1);
    }
}
//Обработка выбранного поля в extendedKeyUsageContBox
void CertParams::on_extendedKeyUsageContBox_activated(const QString &arg1)
{
    if (ui_cp->extendedKeyUsageEdit->toPlainText().isEmpty())
    {
        ui_cp->extendedKeyUsageEdit->setPlainText(arg1);
    }
    else
    {
        ui_cp->extendedKeyUsageEdit->setPlainText(
                    ui_cp->extendedKeyUsageEdit->toPlainText() + ", " + arg1);
    }
}

//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------




