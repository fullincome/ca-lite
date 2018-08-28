#include "ca_cert_params.h"
#include "ui_ca_cert_params.h"

CertParams::CertParams(QWidget *parent) :
    QDialog(parent),
    ui_cp(new Ui::CertParams)
{
    ui_cp->setupUi(this);
    ui_cp->creatBtn->setEnabled(false);
}

CertParams::~CertParams()
{
    delete ui_cp;
}

//------------------------------------------------------
//-------------СОЗДАНИЕ СЕРТИФИКАТА УЦ------------------
//------------------------------------------------------
//Инициализация детерменированных параметров
void CertParams::initialise() {
    for (int i = 0; i < conts_info.size(); ++i) {
        ui_cp->containersBox->addItem(conts_info[i]);
    }
}
//Установка выбранных параметров
void CertParams::setParams() {
    QString str_field;
    table.table_name = "ca";
    str_field = ui_cp->nameCertEdit->toPlainText();
    if (!str_field.isEmpty()) table.CN = str_field;

    str_field = ui_cp->organizationEdit->toPlainText();
    if (!str_field.isEmpty()) table.O = str_field;

    str_field = ui_cp->daysValidEdit->toPlainText();
    if (!str_field.isEmpty()) table.days_valid = str_field;

    str_field = ui_cp->keycontCertEdit->toPlainText();
    if (!str_field.isEmpty()) table.key = str_field;

    str_field = "1.3.6.1.5.5.7.3.1";
    if (!str_field.isEmpty()) table.cert_extension.extendedKeyUsage = str_field;

    str_field = "CN = '" + table.CN + "'";
    if (!str_field.isEmpty()) table.condition = str_field;
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
//Прием результата WorkDir::checkContainers()
void CertParams::getData(QStringList conts_info) {
    this->conts_info = conts_info;
}
void CertParams::closeWindow(QString rc) {
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
    if (ui_cp->nameCertEdit->toPlainText().size() > 0) {
        ui_cp->creatBtn->setEnabled(true);
    }
    else {
        ui_cp->creatBtn->setEnabled(false);
    }
}
//Обработка выбранного контейнера в containersBox
void CertParams::on_containersBox_currentIndexChanged(const QString &arg1)
{
    QRegExp regexp("Name: (\\w+) \\(len = \\d+\\)");
    if (regexp.indexIn(arg1, 0) == -1) {
        //ui_mw->debugLogEdit->setPlainText(ui_mw->debugLogEdit->toPlainText() + prog.output
        //                                    + "\n В " + prog.file_out + " не удалось определить CN");
    } else {
        ui_cp->keycontCertEdit->setPlainText(regexp.cap(1));
    }
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
