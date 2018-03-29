#include "csr_params.h"
#include "ui_csr_params.h"

CsrParams::CsrParams(QWidget *parent) :
    QDialog(parent),
    ui_cp(new Ui::CsrParams)
{
    ui_cp->setupUi(this);
    ui_cp->creatBtn->setEnabled(false);
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
    QStringList data_list = work_dir.checkContainers();
    for (int i = 0; i < data_list.size(); ++i) {
        ui_cp->containersBox->addItem(data_list[i]);
    }
    prog = Program("openssl", "cert", work_dir.work_path);
}
//Установка выбранных параметров
void CsrParams::setParams() {
    DbTable &table = work_dir.data_base.table;

    table.CN = ui_cp->nameCsrEdit->toPlainText();
    table.O = ui_cp->organizationEdit->toPlainText();
    table.subj = "/CN=" + table.CN;
    if (!table.O.isEmpty()) table.subj += "/O=" + table.O;

    table.key = ui_cp->keycontCsrEdit->toPlainText();
    table.suite = "gost";
    table.pem = "need";
    table.table_name = "cert";
    table.serial = "no";
    table.revoke = "no";
    table.issuer = "no";
    table.condition = "CN = '" + table.CN + "'";

    prog.mod = "csr";
    prog.file_out = work_dir.work_path + work_dir.files.csr_file;

    QStringList args_cur;
    args_cur = QString("req -engine gostengy -new -keyform ENGINE -key c:" + table.key).split(" ");
    args_cur += QString("-subj " + table.subj).split(" ");
    args_cur += QString("-out " + prog.file_out).split(" ");
    prog.args = args_cur;
}
//Кнопка: Создать CSR
void CsrParams::on_creatBtn_clicked()
{
    setParams();
    this->close();
    emit readyRunProgram(prog, work_dir.data_base.table);
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
void CsrParams::getData(WorkDir work_dir) {
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
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------




