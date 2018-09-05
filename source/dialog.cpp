#include "dialog.h"
#include "ui_dialog.h"

DbDialog::DbDialog(QWidget *parent) :
    QDialog(parent),
    ui_dl(new Ui::DbDialog)
{
    ui_dl->setupUi(this);
    ui_dl->creatBtn->setEnabled(false);
    setDialogMod("work_dir", "");
}

DbDialog::~DbDialog()
{
    delete ui_dl;
}





//------------------------------------------------------
//----------УНИВЕРСАЛЬНЫЕ ФУНКЦИИ И СЛОТЫ---------------
//------------------------------------------------------
//Выбор мода объекта db_dialog
void DbDialog::setDialogMod(QString dialog_mod, QString path) {

    if (work_path == "")
    {
        work_path = QCoreApplication::applicationDirPath();
    }
    else
    {
        work_path = path;
    }

    if (dialog_mod == "work_dir")
    {
        this->dialog_mod = "work_dir";
        ui_dl->label->setText("Путь к рабочей директории");
        ui_dl->textEdit->setPlainText(work_path);
        ui_dl->creatBtn->setText("Открыть");
    }
    else if (dialog_mod == "import_cert")
    {
        this->dialog_mod = "import_cert";
        this->setWindowTitle("Импорт сертификата");
        ui_dl->label->setText("Путь к импортируемому сертификату");
        ui_dl->textEdit->setPlainText(work_path + "/import_cert.cer");
        ui_dl->creatBtn->setText("Импортировать");
    }
    else if (dialog_mod == "set_config")
    {
        // В случае редактирования конфига используются только кнопки OK/Cancel
        this->dialog_mod = "set_config";
        this->setWindowTitle("Настройка файла конфигурации");
        ui_dl->label->setText("Файл конфигурации");
        ui_dl->fileDialogBtn->close();
        ui_dl->creatBtn->setText("Применить");
    }
}
//Кнопка: Создание рабочей директории ИЛИ импорт сертификата
void DbDialog::on_creatBtn_clicked()
{
    this->close();
    if (dialog_mod == "work_dir")
    {
        emit sendData(ui_dl->textEdit->toPlainText());
    } else if (dialog_mod == "import_cert")
    {
        emit sendData(ui_dl->textEdit->toPlainText());
    } else if (dialog_mod == "set_config")
    {
        emit sendData(ui_dl->textEdit->toPlainText());
    }
}
//Кнопка: отмена
void DbDialog::on_cancelBtn_clicked()
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
void DbDialog::on_textEdit_textChanged()
{
    if (ui_dl->textEdit->toPlainText().size() > 0)
    {
        ui_dl->creatBtn->setEnabled(true);
    }
    else
    {
        ui_dl->creatBtn->setEnabled(false);
    }
}
//Кнопка: "Обзор" - удобный файловый менеджрер
void DbDialog::on_fileDialogBtn_clicked()
{
    QFileDialog file_dialog(this);
    if (dialog_mod == "work_dir")
    {
        QString dir_name = file_dialog.getExistingDirectory(
                    this,
                    tr("Open Directory"),
                    "/",
                    QFileDialog::ShowDirsOnly
                    );
        ui_dl->textEdit->setPlainText(dir_name);
    } else if (dialog_mod == "import_cert")
    {
        QString file_name = file_dialog.getOpenFileName(
                    this,
                    tr("Open File"),
                    QCoreApplication::applicationDirPath() + '/'
                    );
        ui_dl->textEdit->setPlainText(file_name);
    }
}
void DbDialog::getData(QString data)
{
    if (dialog_mod == "set_config")
    {
        ui_dl->textEdit->setPlainText(data);
    }
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------


