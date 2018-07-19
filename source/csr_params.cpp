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
    for (int i = 0; i < conts_info.size(); ++i) {
        ui_cp->containersBox->addItem(conts_info[i]);
    }
}
//Установка выбранных параметров
void CsrParams::setParams() {
    QString str_field;

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
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------




