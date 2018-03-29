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
    QStringList data_list = work_dir.checkContainers();
    for (int i = 0; i < data_list.size(); ++i) {
        ui_cp->containersBox->addItem(data_list[i]);
    }
    prog = Program("openssl", "cert", work_dir.work_path);
}
//Установка выбранных параметров
void CertParams::setParams() {
    DbTable &table = work_dir.data_base.table;

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

    prog.mod = "ca";
    prog.file_out = work_dir.work_path + work_dir.files.ca_cert_file;
    prog.key_in = table.key;

    QStringList args_cur;
    //RSA MOD
    //args_cur = QString("req -x509 -newkey rsa:2048").split(" ");
    //args_cur += QString("-keyout " + prog.key_out + " -nodes -out " + prog.file_out).split(" ");
    //args_cur += QString("-subj " + cert.subj).split(" ");
    //args_cur = QString("req -x509 -newkey rsa:2048").split(" ");

    //GOST MOD
    args_cur = QString("req -engine gostengy -x509 -keyform ENGINE -key c:" + table.key).split(" ");
    args_cur += QString("-nodes -out " + prog.file_out).split(" ");
    args_cur += QString("-subj " + table.subj).split(" ");
    prog.args = args_cur;
}
//Кнопка: Создать CSR
void CertParams::on_creatBtn_clicked()
{
    setParams();
    this->close();
    emit readyRunProgram(prog, work_dir.data_base.table);
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
void CertParams::getData(WorkDir work_dir) {
    this->work_dir = work_dir;
    initialise();
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
