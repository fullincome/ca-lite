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
    table.CN = ui_cp->nameCertEdit->toPlainText();
    table.O = ui_cp->organizationEdit->toPlainText();
    table.subj = "/CN=" + table.CN;
    if (!table.O.isEmpty()) table.subj += "/O=" + table.O;

    table.key = ui_cp->keycontCertEdit->toPlainText();
    table.suite = "gost";
    table.pem = "need";
    table.table_name = "ca";
    table.serial = "no";
    table.revoke = "no";
    table.issuer = "CA";
    table.condition = "CN = '" + table.CN + "'";
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
