#include "csr_params.h"
#include "ui_csr_params.h"

CsrParams::CsrParams(QWidget *parent) :
    QDialog(parent),
    ui_cp(new Ui::CsrParams)
{
    ui_cp->setupUi(this);
    ui_cp->creatBtn->setEnabled(false);
    ui_cp->authorityKeyIdentifierEdit->setDisabled(true);
    ui_cp->authorityKeyIdentifierEdit->setStyleSheet("color: rgb(100,100,100)");
    ui_cp->authorityKeyIdentifierEdit->setPlainText("Field available only on sign");
}

CsrParams::~CsrParams()
{
    delete ui_cp;
}


//------------------------------------------------------
//------------------СОЗДАНИЕ CSR------------------------
//------------------------------------------------------
//Инициализация детерменированных параметров
void CsrParams::initialise() {
    for (int i = 0; i < conts_info.size(); ++i) {
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
void CsrParams::setParams() {
    QString str_field;
    table.table_name = "cert";

    str_field = ui_cp->nameCsrEdit->toPlainText();
    if (!str_field.isEmpty()) table.CN = str_field;

    str_field = ui_cp->organizationEdit->toPlainText();
    if (!str_field.isEmpty()) table.O = str_field;

    str_field = ui_cp->daysValidEdit->toPlainText();
    if (!str_field.isEmpty()) table.days_valid = str_field;

    str_field = ui_cp->keycontCsrEdit->toPlainText();
    if (!str_field.isEmpty()) table.key = str_field;

    str_field = "CN = '" + table.CN + "'";
    if (!str_field.isEmpty()) table.condition = str_field;

    // Секция расширений
    str_field =  ui_cp->basicConstraintsEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.basicConstraints = str_field;

    str_field =  ui_cp->authorityKeyIdentifierEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.authorityKeyIdentifier = str_field;

    str_field =  ui_cp->subjectKeyIdentifierEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.subjectKeyIdentifier = str_field;

    str_field =  ui_cp->keyUsageEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.keyUsage = str_field;

    str_field =  ui_cp->nsCertTypeEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.nsCertType = str_field;

    str_field =  ui_cp->subjectAltNameEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.subjectAltName = str_field;

    str_field =  ui_cp->extendedKeyUsageEdit->toPlainText();;
    if (!str_field.isEmpty()) table.cert_extension.extendedKeyUsage = str_field;


}
//Кнопка: Создать CSR
void CsrParams::on_creatBtn_clicked()
{
    setParams();
    emit readyToCheck(table);
}
//Кнопка: Отмена
void CsrParams::on_cancelBtn_clicked()
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
void CsrParams::getData(QStringList conts_info) {
    this->conts_info = conts_info;
}
void CsrParams::closeWindow(QString rc)
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
void CsrParams::on_nameCsrEdit_textChanged()
{
    if (ui_cp->nameCsrEdit->toPlainText().size() > 0) {
        ui_cp->creatBtn->setEnabled(true);
    }
    else {
        ui_cp->creatBtn->setEnabled(false);
    }
}
//Обработка выбранного контейнера в containersBox
void CsrParams::on_containersBox_currentIndexChanged(const QString &arg1)
{
    QRegExp regexp("Name: (\\w+) \\(len = \\d+\\)");
    if (regexp.indexIn(arg1, 0) == -1) {
        //ui_mw->debugLogEdit->setPlainText(ui_mw->debugLogEdit->toPlainText() + prog.output
        //                                    + "\n В " + prog.file_out + " не удалось определить CN");
    } else {
        ui_cp->keycontCsrEdit->setPlainText(regexp.cap(1));
    }
}
//Обработка выбранного поля в basicConstraintsContBox
void CsrParams::on_basicConstraintsContBox_activated(const QString &arg1)
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
void CsrParams::on_authorityKeyIdentifierContBox_activated(const QString &arg1)
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
void CsrParams::on_subjectKeyIdentifierContBox_activated(const QString &arg1)
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
void CsrParams::on_keyUsageContBox_activated(const QString &arg1)
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
void CsrParams::on_nsCertTypeContBox_activated(const QString &arg1)
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
void CsrParams::on_subjectAltNameContBox_activated(const QString &arg1)
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
void CsrParams::on_extendedKeyUsageContBox_activated(const QString &arg1)
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







