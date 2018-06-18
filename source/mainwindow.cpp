#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_mw(new Ui::MainWindow)
{
    ui_mw->setupUi(this);
    initWindow();
    if (isRelease) {
        ui_mw->debugLogEdit->close();
        ui_mw->debugLogLabel->close();
        ui_mw->debugCheckContainersBtn->close();
    }
    program = Program("openssl", "ca_cert");
    work_dir = WorkDir(QCoreApplication::applicationDirPath() + "/");
    on_openWorkDirBtn_clicked();
}

MainWindow::~MainWindow()
{
    delete ui_mw;
}





//------------------------------------------------------
//-----------ЗАГРУЗКА РАБОЧЕЙ ДИРЕКТОРИИ----------------
//------------И СОЗДАНИЕ СЕРТИФИКАТА УЦ-----------------
//------------------------------------------------------
// Кнопка: Создать сертификат УЦ
void MainWindow::on_creatCaCertBtn_clicked()
{
    CertParams certParams;
    connect(this, SIGNAL(sendData(QStringList)), &certParams, SLOT(getData(QStringList)));
    connect(this, SIGNAL(closeCertParam(QString)), &certParams, SLOT(closeWindow(QString)));
    connect(&certParams, SIGNAL(readyToCheck(DbTable)), this, SLOT(checkCertParam(DbTable)));
    emit sendData(work_dir.checkContainers());
    certParams.initialise();
    certParams.exec();
}
// Кнопка: Править файл конфигурации
void MainWindow::on_editConfigBtn_clicked()
{
    DbDialog db_dialog;
    connect(&db_dialog, SIGNAL(sendData(QString)), this, SLOT(saveConfig(QString)));
    connect(this, SIGNAL(sendData(QString)), &db_dialog, SLOT(getData(QString)));
    db_dialog.setDialogMod("set_config", work_dir.work_path);
    QString config_data = work_dir.config.readAllFromConfigFile(work_dir.config.name);
    if (config_data == "error") {
        messageError(this, "Не обнаружен файл конфигурации");
    } else {
        emit sendData(config_data);
        db_dialog.exec();
    }
}
// Кнопка: Создать или открыть рабочую директорию
// Выбран путь инициализации ТЕКУЩЕЙ ДИРЕКТОРИИ
void MainWindow::on_openWorkDirBtn_clicked()
{
    if (work_dir.initialiseWorkDir() == "error") {
        if (work_dir.newWorkDir() == "error") {
            messageError(this, "Не удалось инициализировать директорию");
            return;
        }
    }
    if (work_dir.initialiseDatabase() == "error") {
        messageError(this, "Не удалось открыть базу данных");
        return;
    }
    if (work_dir.initialiseConfig() == "error") {
        // пробуем создать сертификат уц
    }
    checkWorkDir(work_dir);

}
// Проверить рабочую директорию
void MainWindow::checkWorkDir(WorkDir work_dir) {
    if (work_dir.isOk && work_dir.config.isOk && work_dir.data_base.isOk) {
        work_dir.ca_cert = work_dir.loadCaCert(work_dir.config);
        QString cert_info = DbTable::getTextFromCert(work_dir.ca_cert.file_name);
        if (cert_info != "error") {
            work_dir.data_base.table.table_name = "cert";
            updateView(work_dir.data_base);
            enableBtn();
            ui_mw->creatCaCertBtn->setDisabled(true);
            ui_mw->exportBtn->setVisible(false);
            //ui_mw->creatCaCertBtn->close();
            ui_mw->creatCaCertBtn->setStyleSheet("color: rgb(0,150,0)");
            ui_mw->caLabel->setText("CA: " + work_dir.ca_cert.CN);
            ui_mw->caLabel->setStyleSheet("color: rgb(0,150,0)");
            ui_mw->dirLabel->setText("Dir: " + work_dir.work_path);
            ui_mw->dirLabel->setStyleSheet("color: rgb(0,150,0)");
            messageSuccess(this, tr("Сертификат УЦ загружен \n\n") + cert_info);
        } else {
            messageError(this, tr("Файл сертификата УЦ поврежден \n\n") + getLastErrorString());
        }
    } else {
        disableBtn();
        ui_mw->caLabel->setText("CA: no certificate");
        ui_mw->caLabel->setStyleSheet("color: rgb(230,0,0)");
        ui_mw->dirLabel->setText("");
        ui_mw->creatCaCertBtn->setStyleSheet("color: rgb(230,0,0)");
        if (work_dir.isOk && work_dir.data_base.isOk) {
            ui_mw->openWorkDirBtn->close();
            ui_mw->creatCaCertBtn->setEnabled(true);
            if (isDebug) ui_mw->debugCheckContainersBtn->setEnabled(true);
        }
        if (!work_dir.isOk) {
            messageError(this, tr("Ошибка в инициализации work_dir"));
        } else if (!work_dir.config.isOk) {
            messageWarning(this, tr("Требуется настройка файла конфигурации или генерация сертификата УЦ"));
        } else {
            messageError(this, tr("Ошибка в инициализации data_base"));
        }
    }
    this->work_dir = work_dir;
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------






//------------------------------------------------------
//-------------------СОЗДАНИЕ CSR-----------------------
//-----------И ПОДПИСЬ CSR СЕРТИФИКАТОМ УЦ--------------
//------------------------------------------------------
//Кнопка: Создать CSR
void MainWindow::on_creatCsrBtn_clicked()
{
    CsrParams csrParams;
    connect(this, SIGNAL(sendData(QStringList)), &csrParams, SLOT(getData(QStringList)));
    connect(this, SIGNAL(closeCertParam(QString)), &csrParams, SLOT(closeWindow(QString)));
    connect(&csrParams, SIGNAL(readyToCheck(DbTable)), this, SLOT(checkCertParam(DbTable)));
    emit sendData(work_dir.checkContainers());
    csrParams.initialise();
    csrParams.exec();
}
//Кнопка: Подписать CSR сертификатом УЦ
void MainWindow::on_signCsrBtn_clicked()
{
    //GOST MOD
    Program prog = Program("openssl", "signing_cert", work_dir.work_path);
    QString csr_CN;
    setSelectedName(csr_CN, ui_mw->certTableView);
    DbTable &table_cert = work_dir.data_base.table;
    table_cert = work_dir.exportCert(csr_CN, work_dir.work_path + prog.csr_filename);
    if (table_cert.suite == "gost") prog.suite = "gost";
    //table_cert.creatSerialToFile(work_dir.files.srl_ca_cert_file);
    //Заполнение параметров программы
    prog.key_in = work_dir.ca_cert.file_key;
    prog.key_out = table_cert.key;
    prog.file_in = work_dir.work_path + prog.csr_filename;
    prog.file_out = work_dir.work_path + prog.cert_filename;
    prog.args = QString("x509 -engine gostengy -req -CA " + work_dir.ca_cert.file_name).split(" ");
    prog.args += QString("-CAkeyform ENGINE -CAkey c:" + work_dir.ca_cert.key).split(" ");
    prog.args += QString("-CAserial " + work_dir.files.srl_ca_cert_file).split(" ");
    prog.args += QString("-in " + prog.file_in + " -out " + prog.file_out).split(" ");
    prog.mod = "cert";
    prog.run();
    //--------------------------------------------------
    messageDebug(prog.output);
    if (!prog.isError) {
        table_cert.table_name = "cert";
        table_cert.pem = table_cert.getPemFromFile(prog.file_out);
        table_cert.serial = table_cert.getSerialFromCert(prog.file_out);
        table_cert.issuer = table_cert.getCNFromCert(work_dir.ca_cert.file_name);
        work_dir.data_base.saveToDb(work_dir.data_base.table, work_dir.data_base.query);
        updateView(work_dir.data_base);
    } else {
        messageWarning(this, "Результат выполениния программы: \n\n" + prog.output);
    }
    prog.clearResult();
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------







//------------------------------------------------------
//----------------ОТЗЫВ СЕРТИФИКАТА---------------------
//--------------И ГЕНИРАЦИЯ СПИСКА CRL------------------
//------------------------------------------------------
//Кнопка: Отзыв сертификата
void MainWindow::on_revokeCertBtn_clicked()
{
    Program prog = Program("openssl", "revoke_cert", work_dir.work_path);
    QString cert_CN;
    setSelectedName(cert_CN, ui_mw->certTableView);
    DbTable &table_cert = work_dir.data_base.table;
    table_cert = work_dir.exportCert(cert_CN, work_dir.work_path + prog.cert_filename);
    if (table_cert.suite == "gost") prog.suite = "gost";
    //Заполнение параметров программы
    prog.file_in = work_dir.work_path + prog.cert_filename;
    prog.file_out = work_dir.work_path + prog.cert_filename;
    prog.key_in = work_dir.ca_cert.key;
    prog.args = QString("ca -engine gostengy -revoke " + prog.file_in).split(" ");
    prog.args += QString("-cert " + work_dir.ca_cert.file_name + " -keyform ENGINE").split(" ");
    prog.args += QString("-keyfile c:" + work_dir.ca_cert.key + " -out " + prog.file_out).split(" ");
    prog.run();
    messageDebug(prog.output);
    if (!prog.isError) {
        table_cert.table_name = "cert";
        table_cert.revoke = "revoked";
        table_cert.pem = table_cert.getPemFromFile(prog.file_out);
        table_cert.serial = table_cert.getSerialFromCert(prog.file_out);
        table_cert.issuer = table_cert.getCNFromCert(work_dir.ca_cert.file_name);
        work_dir.data_base.saveToDb(work_dir.data_base.table, work_dir.data_base.query);
        updateView(work_dir.data_base);
    } else {
        messageWarning(this, "Результат выполениния программы: \n\n" + prog.output);
    }
    prog.clearResult();
}
//Кнопка: Генерация списка CRL
void MainWindow::on_creatCrlBtn_clicked()
{
    Program prog = Program("openssl", "revoke_cert", work_dir.work_path);
    prog.file_in = work_dir.work_path + prog.ca_cert_filename;
    prog.file_out = work_dir.work_path + "crl";
    prog.key_in = work_dir.ca_cert.key;
    prog.args = QString("ca -engine gostengy -gencrl -cert " + prog.file_in).split(" ");
    prog.args += QString("-keyform ENGINE -keyfile c:" + prog.key_in).split(" ");
    prog.args += QString("-out " + prog.file_out).split(" ");
    prog.run();
    messageDebug(prog.output);
    if (!prog.isError) {
        messageSuccess(this, prog.output);
    } else {
        messageWarning(this, "Результат выполениния программы: \n\n" + prog.output);
    }
    prog.clearResult();
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------








//------------------------------------------------------
//-------------КНОПКИ ЭКСПОРТА, ИМПОРТА-----------------
//--------------И ПРОВЕРКИ КОНТЕЙНЕРОВ------------------
//------------------------------------------------------
//Кнопка: Экспорт сертификата
void MainWindow::on_exportBtn_clicked()
{
    QString CN;
    setSelectedName(CN, ui_mw->certTableView);
    if (!CN.isEmpty()) {
        work_dir.exportCert(CN, work_dir.work_path + work_dir.files.export_cert_file);
        if (CN != work_dir.data_base.table.getCNFromCert(work_dir.work_path + work_dir.files.export_cert_file) &&
            CN != work_dir.data_base.table.getCNFromCsr(work_dir.work_path + work_dir.files.export_cert_file)) {
            messageError(this, "Не удалось экспортировать сертификат");
        } else {
            messageSuccess(this, "Сертификат экспортирован: \n\n" + work_dir.work_path + work_dir.files.export_cert_file);
        }
    } else {
        messageWarning(this, "Выберете сертификат для экспорта");
    }
}
// Кнопка: Импорт сертификата
void MainWindow::on_importBtn_clicked()
{
    program.file_in = "";
    DbTable table;
    DbDialog db_dialog;
    connect(&db_dialog, SIGNAL(sendData(QString)), this, SLOT(getData(QString)));
    db_dialog.setDialogMod("import_cert", work_dir.work_path);
    db_dialog.exec();
    if (!program.file_in.isEmpty()) {
        table = work_dir.importCert(program.file_in);
        if (!table.isOk) {
            messageError(this, "Сертификат не импортирован: \n\n" + work_dir.data_base.table.status);
        } else {
            work_dir.data_base.table = table;
            updateView(work_dir.data_base);
            messageSuccess(this, "Сертификат импортирован");
        }
    }
}
// Кнопка: Проверка контейнеров
void MainWindow::on_debugCheckContainersBtn_clicked()
{
    QStringList conts = work_dir.checkContainers();
    if (conts.size() > 0) {
        messageDebug((conts.join("\n")));
    }
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------






//------------------------------------------------------
//------------ГЕНЕРАЦИЯ СЕРТИФИКАТОВ,-------------------
//-------------ПРОВЕРКА И СОХРАНЕНИЕ--------------------
// Сохранение в бд
// TODO рассмотреть фозможность query().lastQuery()
void MainWindow::updateView(DataBase data_base) {
    DbTable table = data_base.table;
    QSqlQuery *query = data_base.query;
    QSqlQueryModel *model_cert = new QSqlQueryModel();
    QSqlQueryModel *model_crl = new QSqlQueryModel();
    QSortFilterProxyModel *sqlproxy_model_cert = new QSortFilterProxyModel();
    //model_csr->
    if (table.table_name == "cert") {
        query->prepare("SELECT CN, pem, serial, revoke, issuer FROM cert");
        if (query->exec()) {
            model_cert->setQuery(*query);
            // прокси модель - в данном случае обертка для sqlQueryModel,
            // нужна, т.к. поддерживает сортировку
            sqlproxy_model_cert->setSourceModel(model_cert);
            ui_mw->certTableView->setModel(sqlproxy_model_cert);
            connect(ui_mw->certTableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
                    this, SLOT(on_certTableView_selectionChanged(QItemSelection, QItemSelection)));
            ui_mw->certTableView->setSelectionMode(QAbstractItemView::SingleSelection);
            ui_mw->certTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
            ui_mw->certTableView->setSortingEnabled(true);
        } else {
            messageDebugUpdate("\n\n CAN NOT SELECT");
        }
    }
    if (table.table_name == "crl") {
//        query->prepare("SELECT CN FROM crl");
//        if (query->exec()) {
//            model_crl->setQuery(*query);
//            ui_mw->certCrlListView->setModel(model_crl);
//        } else {
//            ui_mw->debugLogEdit->setPlainText( ui_mw->debugLogEdit->toPlainText() + "\n\n CAN NOT SELECT");
//        }
    }
}
// Подготовка к сохранению в бд, выполняет проверки полей
bool MainWindow::prepareSaveToDb(Program prog, DbTable table) {
    QStringList file_to_delete;
    // работаем с фалом csr/cert

    if (prog.mod == "ca") {
        if (table.CN == "need") table.getCNFromCert(prog.file_out);
        if (table.subj == "need");
        if (table.suite == "need");
        if (table.table_name == "need");
        if (table.serial == "need") {
            table.serial = table.getSerialFromFile(work_dir.work_path + work_dir.files.srl_ca_cert_file);
            file_to_delete << work_dir.work_path + prog.srl_ca_cert_filename;
        }
        if (table.pem == "need") table.pem = table.getPemFromFile(prog.file_out);
        if (table.revoke == "need");
        if (table.issuer == "need");
        if (table.condition == "need");
        if (table.suite == "gost" ) {
            if (table.key == "need") ;
        }
        QStringList errorField = table.checkErrorFields();
        if (!errorField.isEmpty()) {
            QString message = "Ошибки в полях: \n" + errorField.join("\n");
            messageError(this, message);
            return 0;
        }
        work_dir.config.CAcert = prog.file_out;
        work_dir.config.CAkey = prog.key_in;
        work_dir.saveConfig(work_dir.config);
        work_dir.config = work_dir.loadConfig(work_dir.config.name);
        if (work_dir.config.isOk) {
            work_dir.isOk = true;
        }
    }
    if (prog.mod == "cert" || prog.mod == "csr") {
        if (table.CN == "need") {
            table.CN = table.getCNFromCsr(prog.file_out);
            if (table.CN == "error") table.CN = table.getCNFromCert(prog.file_out);
        }
        if (table.subj == "need");
        if (table.suite == "need");
        if (table.table_name == "need");
        if (table.serial == "need") {
            table.serial = table.getSerialFromFile(work_dir.work_path + prog.srl_ca_cert_filename);
            file_to_delete << work_dir.work_path + prog.srl_ca_cert_filename;
        }
        if (table.pem == "need") table.pem = table.getPemFromFile(prog.file_out);
        if (table.revoke == "need");
        if (table.issuer == "need");
        if (table.condition == "need");
        if (table.suite == "gost" ) {
            if (table.key == "need") ;
        }
        QStringList errorField = table.checkErrorFields();
        if (!errorField.isEmpty()) {
            QString message = "Ошибки в полях: \n" + errorField.join("\n");
            messageError(this, message);
            return 0;
        }

//    //NEEDFIX
      }// else if (prog.mod == "revoke") {

//    }
//    } else if (prog.mod == "crl") {
//        table.CN = table.getCNFromCsr(prog.file_out);
//        table.table_name = "crl";
//        table.suite = prog.suite;
//        table.serial = table.getSerialFromFile(work_dir.work_path + prog.srl_ca_cert_filename);
//        file_to_delete << work_dir.work_path + prog.srl_ca_cert_filename;
//        if (table.suite == "gost" ) {
//            table.key = prog.key_out;
//        }
//        prog.clearResult(file_to_delete);
//    }
    prog.clearResult(file_to_delete);
    work_dir.data_base.table = table;
    return 1;
}
// Основная функция генерации сертификатов
void MainWindow::generateCert(Program prog, DbTable table) {
    prog.run();
    messageDebug(prog.output);
    if (prog.mod == "cert" || prog.mod == "csr") {
        if (prog.isError) {
            messageWarning(this, "Результат выполениния программы: \n\n" + prog.output);
        }
        if (prepareSaveToDb(prog, table)) {
            work_dir.data_base.saveToDb(work_dir.data_base.table, work_dir.data_base.query);
            updateView(work_dir.data_base);
        }
    }
    if (prog.mod == "ca") {
        if (prog.isError) {
            messageWarning(this, "Результат выполениния программы: \n\n" + prog.output);
        }
        if (prepareSaveToDb(prog, table)) {
            if (work_dir.isOk) {
                work_dir.data_base.saveToDb(work_dir.data_base.table, work_dir.data_base.query);
                updateView(work_dir.data_base);
                checkWorkDir(work_dir);
            }
            else {
                messageError(this, "Config is wrong");
            }
        }
    }
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------








//------------------------------------------------------
//--------------ВЫВОД И ПЕЧАТЬ СООБЩЕНИЙ----------------
//------------------------------------------------------
// Вывод на дебагавое окно, затирает текущей текст
void MainWindow::messageDebug(QString message) {
    if (isDebug) ui_mw->debugLogEdit->setPlainText(message);
}
// Вывод на дебаговое окно, конкатенация
void MainWindow::messageDebugUpdate(QString messageUpdate) {
    if (isDebug) ui_mw->debugLogEdit->setPlainText( ui_mw->debugLogEdit->toPlainText() + messageUpdate);
}
// Сообщение об ошибке
void MainWindow::messageError(QWidget *window, QString message)
{
    QMessageBox::critical(window, tr("Ошибка"), message);
}
// Сообщение предупреждение
void MainWindow::messageWarning(QWidget *window, QString message)
{
    QMessageBox::warning(window, tr("Предупреждение"), message);
}
// Сообщение об успехе
void MainWindow::messageSuccess(QWidget *window, QString message)
{
    QMessageBox::information(window, tr("Успешно"), message);
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------






//------------------------------------------------------
//---------ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ И СЛОТЫ--------------
//------------------------------------------------------
// Настройки окна при инициализации
void MainWindow::initWindow() {
    ui_mw->openWorkDirBtn->setDisabled(true);
    ui_mw->openWorkDirBtn->close();
    disableBtn();
    ui_mw->caLabel->setStyleSheet("color: rgb(230,0,0)");
    ui_mw->creatCaCertBtn->setStyleSheet("color: rgb(230,0,0)");
    ui_mw->signCsrBtn->setVisible(false);
    ui_mw->revokeCertBtn->setVisible(false);
}
// Слот, принимающий конфиг
void MainWindow::saveConfig(QString data) {
    if (work_dir.config.writeToConfigFile(work_dir.config.name, data) == "error") {
        messageError(this, "Не удалось записать в файл конфигурации");
    } else {
        on_openWorkDirBtn_clicked();
    }
}
// Проверяем поля из ca_cert и пробуем генерировать сертификат
void MainWindow::checkCertParam(DbTable table) {
    Program prog;
    QStringList args_cur;
    prog.file_out = work_dir.work_path + work_dir.files.ca_cert_file;
    prog.key_in = table.key;
    if (table.table_name == "ca") {
        prog = Program("openssl", "ca", work_dir.work_path);
    } else {
        prog = Program("openssl", "csr", work_dir.work_path);
    }
    //RSA MOD
    //args_cur = QString("req -x509 -newkey rsa:2048").split(" ");
    //args_cur += QString("-keyout " + prog.key_out + " -nodes -out " + prog.file_out).split(" ");
    //args_cur += QString("-subj " + cert.subj).split(" ");
    //args_cur = QString("req -x509 -newkey rsa:2048").split(" ");

    //GOST MOD
    if (prog.mod == "ca") {
        args_cur = QString("req -engine gostengy -x509 -keyform ENGINE -key c:" + table.key).split(" ");
        args_cur += QString("-nodes -out " + prog.file_out).split(" ");
        args_cur += QString("-subj " + table.subj).split(" ");
    } else {
        args_cur = QString("req -engine gostengy -new -keyform ENGINE -key c:" + table.key).split(" ");
        args_cur += QString("-subj " + table.subj).split(" ");
        args_cur += QString("-out " + prog.file_out).split(" ");
    }
    prog.args = args_cur;
    generateCert(prog, table);
    emit closeCertParam("ok");
}
// Слот, принимающие данные от сигналов
void MainWindow::getData(QString data) {
    program.file_in = data;
}
// Получение данных CN по выбранному в таблице сертификату
void MainWindow::setSelectedName(QString &select_name, QTableView *table_view) {
    const QModelIndex &index = table_view->selectionModel()->currentIndex();
    //QString selectedData = table_view->model()->data(index).toString();
    //select_name = table_view->model()->data(index, 0).toString();
     select_name = index.sibling(index.row(), 0).data().toString();
}
// Отключение всех кнопок
void MainWindow::disableBtn() {
    ui_mw->creatCaCertBtn->setEnabled(false);
    ui_mw->creatCsrBtn->setEnabled(false);
    ui_mw->creatCrlBtn->setEnabled(false);
    ui_mw->signCsrBtn->setEnabled(false);
    ui_mw->revokeCertBtn->setEnabled(false);
    if (isDebug) ui_mw->debugCheckContainersBtn->setEnabled(false);
    ui_mw->importBtn->setEnabled(false);
    ui_mw->exportBtn->setEnabled(false);
}
//Включение всех кнопок
void MainWindow::enableBtn() {
    ui_mw->creatCaCertBtn->setEnabled(true);
    ui_mw->creatCsrBtn->setEnabled(true);
    ui_mw->creatCrlBtn->setEnabled(true);
    ui_mw->signCsrBtn->setEnabled(true);
    ui_mw->revokeCertBtn->setEnabled(true);
    if (isDebug) ui_mw->debugCheckContainersBtn->setEnabled(true);
    ui_mw->importBtn->setEnabled(true);
    ui_mw->exportBtn->setEnabled(true);
}
// Реакция на выбор нескольких строк
void MainWindow::on_certTableView_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    if (selected.indexes().count() == 5) {
        QString CN = selected.indexes()[0].data().toString();
        QString pem = selected.indexes()[1].data().toString();
        QString serial = selected.indexes()[2].data().toString();
        QString revoked = selected.indexes()[3].data().toString();
        QString issuer = selected.indexes()[4].data().toString();

        ui_mw->exportBtn->setVisible(true);
        if (serial == "no") ui_mw->signCsrBtn->setVisible(true);
        else ui_mw->signCsrBtn->setVisible(false);
        if (revoked == "no" && issuer != "no") ui_mw->revokeCertBtn->setVisible(true);
        else ui_mw->revokeCertBtn->setVisible(false);
        if (issuer != "no" && issuer != work_dir.ca_cert.CN) {
            ui_mw->revokeCertBtn->setVisible(false);
            ui_mw->revokeCertBtn->setVisible(false);
        }
    }
}
// Инфо сертификата по кнопке Enter
void MainWindow::on_certTableView_activated(const QModelIndex &index)
{
    work_dir.exportCert(index.sibling(index.row(), 0).data().toString(), work_dir.files.cert_file);
    QMessageBox::information(this, tr("Информация о сертификате"), DbTable::getTextFromAny(work_dir.files.cert_file));
    program.clearResult();
}
//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------



