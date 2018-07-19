#include "work_dir.h"

DbTable::DbTable()
{
    CN = "CN_calite";
    O = "O_calite";
    C = "RU";
    email = "calite@calite.ru";
    sun = "email:calite@calite.ru";
    subj = "/CN=CN_calite/O=O_calite/C=C_calite";
    days_valid = "356";
    key = "need";
    pem = "need";
    table_name = "cert";
    suite = "gost";
    serial = "no";
    revoke = "no";
    issuer = "no";
    condition = "CN = CN_calite";
    status = "ok";
}

QStringList DbTable::checkErrorFields() {
    QStringList errorFild;
    if (CN == "error" || CN == "need") errorFild << "CN = " + CN;
    if (subj == "error" || subj == "need") errorFild << "subj = " + subj;
    if (key == "error" || key == "need") errorFild << "key = " + key;
    if (pem == "error" || pem == "need") errorFild << "pem = " + pem;
    if (table_name == "error" || table_name == "need") errorFild << "table_name = " + table_name;
    if (suite == "error" || suite == "need") errorFild << "suite = " + suite;
    if (serial == "error" || serial == "need") errorFild << "serial = " + serial;
    if (revoke == "error" || revoke == "need") errorFild << "revoke = " + revoke;
    if (issuer == "error" || issuer == "need") errorFild << "issuer = " + issuer;
    if (condition == "error" || condition == "need") errorFild << "condition = " + condition;
    return errorFild;
}
QString DbTable::getPemFromFile(QString file_name) {
    QString pem;
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream in(&file);
    if (!in.atEnd()) {
        pem = in.readAll();
    }
    file.close();
    return pem;
}

QString DbTable::getTextFromCsr(QString file_in) {
    Program prog = Program("openssl", "csr");
    prog.args = QString("req -in " + file_in + " -noout -text").split(" ");
    prog.run();
    return prog.output;
}

BOOL_ERR DbTable::getTextFromCert(QString file_in, QString &cert_info) {
    Program prog = Program("openssl", "ca");
    prog.args = QString("x509 -in " + file_in + " -noout -text").split(" ");
    prog.run();
    if (prog.isError) {
        setErrorString(prog.output);
        return FAIL;
    } else {
        cert_info = prog.output;
        return OK;
    }
}

QString DbTable::getTextFromAny(QString file_in, QString work_path) {
    Program prog = Program("openssl", "ca", work_path);
    prog.args = QString("x509 -in " + file_in + " -noout -text").split(" ");
    prog.run();
    if (prog.isError) {
        prog.args = QString("req -in " + file_in + " -noout -text").split(" ");
        prog.run();
    }
    return prog.output;
}

QString DbTable::getCNFromCsr(QString file_in) {
    QString CN;
    QRegExp regexp(RegexpPatternWorkDir::CN);

    if (regexp.indexIn(getTextFromCsr(file_in), 0) == -1) {
        return "error";
    } else {
        CN = regexp.cap(1);
    }
    return CN;
}
QString DbTable::getCNFromCert(QString file_in) {
    QString CN;
    QString textCert;
    QRegExp regexp(RegexpPatternWorkDir::CN);

    getTextFromCert(file_in, textCert);
    if (regexp.indexIn(textCert, 0) == -1) {
        return "error";
    } else {
        CN = regexp.cap(1);
    }
    return CN;
}
QString DbTable::getIssuerFromCert(QString file_in) {
    QString issuer;
    QString textCert;
    QRegExp regexp(RegexpPatternWorkDir::Issuer);

    getTextFromCert(file_in, textCert);
    if (regexp.indexIn(textCert, 0) == -1) {
        return "error";
    } else {
        issuer = regexp.cap(1);
    }
    return issuer;
}
QString DbTable::creatSerialToFile(QString file_name) {
    QFile file_srl(file_name);
    if (!file_srl.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream stream_srl(&file_srl);
    stream_srl << "AF";
    file_srl.close();
    return "ok";
}
QString DbTable::getSerialFromFile(QString file_name) {
    QString serial;
    QFile file_srl(file_name);
    if (!file_srl.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream stream_srl(&file_srl);
    serial = stream_srl.readAll();
    file_srl.close();
    return serial;
}
QString DbTable::getSerialFromCert(QString file_name) {
    QString serial;
    QString textCert;
    QRegExp regexp(RegexpPatternWorkDir::Serial);

    getTextFromCert(file_name, textCert);
    if (regexp.indexIn(textCert, 0) == -1) {
        return "error";
    } else {
        serial = regexp.cap(1);
    }
    return serial;
}




//------------------------------------------------------
//--------------------DataBase--------------------------
//------------------------------------------------------
bool DataBase::load_db(QString name) {
    sql_db = QSqlDatabase::addDatabase("QSQLITE");
    sql_db.setDatabaseName(name);
    return sql_db.open() ? true : false;
}
void DataBase::newTables(QSqlQuery *query)
{
    query->exec("CREATE TABLE ca ("
        "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`CN`       TEXT,"
        "`pem`      TEXT,"
        "`key`      TEXT,"
        "`suite`    TEXT,"
        "`serial`   TEXT,"
        "`revoke`   TEXT,"
        "`issuer`   TEXT,"
        "`days_valid`   TEXT)");
    query->exec("CREATE TABLE cert ("
        "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`CN`       TEXT,"
        "`pem`      TEXT,"
        "`key`      TEXT,"
        "`suite`    TEXT,"
        "`serial`   TEXT,"
        "`revoke`   TEXT,"
        "`issuer`   TEXT,"
        "`days_valid`   TEXT)");
    query->exec("CREATE TABLE crl ("
        "`id`       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`index`	TEXT,"
        "`crl`      TEXT)");
}
BOOL_ERR DataBase::saveToDb(DbTable table, QSqlQuery *query) {
    if (table.table_name == "cert") {
        table.condition = "CN = '" + table.CN + "'";
        DbTable tmp_table = loadFromDb(table.table_name, table.condition, query);
        if (tmp_table.pem.isEmpty()) {
            if (!query->prepare("INSERT INTO cert (CN, pem, key, suite, serial, revoke, issuer, days_valid) "
                           "VALUES (:CN, :pem, :key, :suite, :serial, :revoke, :issuer, :days_valid)"))
            {
                setErrorString("query->prepare() fail with: " + query->lastError().text());
                return FAIL;
            }
            query->bindValue(":CN", table.CN);
            query->bindValue(":pem", table.pem);
            query->bindValue(":key", table.key);
            query->bindValue(":suite", table.suite);
            query->bindValue(":serial", table.serial);
            query->bindValue(":revoke", table.revoke);
            query->bindValue(":issuer", table.issuer);
            query->bindValue(":days_valid", table.days_valid);
        } else {
            if (!query->prepare("UPDATE cert SET "
                           "CN = '" + table.CN + "', "
                           "pem = '" + table.pem + "', "
                           "key = '" + table.key + "', "
                           "suite = '" + table.suite + "', "
                           "serial = '" + table.serial + "', "
                           "revoke = '" + table.revoke + "', "
                           "issuer = '" + table.issuer + "', "
                           "days_valid = '" + table.days_valid + "' "
                           "WHERE " + table.condition))
            {
                setErrorString("query->prepare() fail with: " + query->lastError().text());
                return FAIL;
            }
        }
    } else if (table.table_name == "crl") {
        if (!query->prepare("INSERT INTO crl (CN, pem, key, suite, serial) "
                   "VALUES (:CN, :pem, :key, :suite, :serial)"))
        {
            setErrorString("query->prepare() fail with: " + query->lastError().text());
            return FAIL;
        }
        query->bindValue(":CN", table.CN);
        query->bindValue(":pem", table.pem);
        query->bindValue(":key", table.key);
        query->bindValue(":suite", table.suite);
        query->bindValue(":serial", table.serial);
    } else if (table.table_name == "ca") {
        if (!query->prepare("INSERT INTO ca (CN, pem, key, suite, serial, revoke, issuer, days_valid) "
                       "VALUES (:CN, :pem, :key, :suite, :serial, :revoke, :issuer, :days_valid)"))
        {
            setErrorString("query->prepare() fail with: " + query->lastError().text());
            return FAIL;
        }
        query->bindValue(":CN", table.CN);
        query->bindValue(":pem", table.pem);
        query->bindValue(":key", table.key);
        query->bindValue(":suite", table.suite);
        query->bindValue(":serial", table.serial);
        query->bindValue(":revoke", table.revoke);
        query->bindValue(":issuer", table.issuer);
        query->bindValue(":days_valid", table.days_valid);
    }
    if (!query->exec())
    {
        setErrorString("query->exec() fail with: " + query->lastError().text());
        return FAIL;
    }
    return OK;
}

DbTable DataBase::loadFromDb(QString table_name, QString table_condition, QSqlQuery *query) {
    DbTable table;
    if (table_name == "cert") {
        if (!query->prepare("SELECT CN, pem, key, suite, serial, revoke, issuer, days_valid "
                            "FROM cert "
                            "WHERE " + table_condition)) {
            return table;
        }
    } else if (table_name == "crl") {
        if (!query->prepare("SELECT CN, pem, key, suite "
                            "FROM crl "
                            "WHERE " + table_condition)) {
            return table;
        }
    }
    query->exec();
    if (query->next()) table.CN = query->value(0).toString();
    table.pem = query->value(1).toString();
    table.key = query->value(2).toString();
    table.suite = query->value(3).toString();
    table.serial = query->value(4).toString();
    table.revoke = query->value(5).toString();
    table.issuer = query->value(6).toString();
    table.days_valid = query->value(7).toString();
    query->finish();
    return table;
}


QString Config::writeToConfigFile(QString file_name, QString data) {
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream out(&file_config);
    out << data;
    file_config.close();
    return "ok";
}
QString Config::readAllFromConfigFile(QString file_name) {
    QString config_data;
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream in(&file_config);
    if (!in.atEnd()) {
         config_data = in.readAll();
    }
    file_config.close();
    return config_data;
}
//------------------------------------------------------
//--------------------WorkDir---------------------------
//------------------------------------------------------

WorkDir::WorkDir () {}
WorkDir::WorkDir (QString work_path) {
    this->work_path = work_path;
    isOk = true;
    config.isOk = true;
    data_base.isOk = true;
    isOpenMod = true;
    data_base.table.table_name = "cert";
    //work_path = ui_dl->textEdit->toPlainText() + "/";
    data_base.name = work_path + "ca_db";
    config.name = work_path + "config";
    files.index = work_path + "index.txt";
    files.crlnumber = work_path + "crlnumber";
    files.openssl_config = work_path + "openssl_calite.cnf";
}

//------------------------------------------------------
//-----------ЗАГРУЗКА РАБОЧЕЙ ДИРЕКТОРИИ----------------
//------------------------------------------------------
//Создание новой рабочей директории
BOOL_ERR WorkDir::newWorkDir() {

    //creat crlnumber file
    QFile file_crlnumber(files.crlnumber);
    if (!file_crlnumber.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return FAIL;
    }
    QTextStream stream_crlnumber(&file_crlnumber);
    stream_crlnumber << "01";
    file_crlnumber.close();

    //creat serial file
    QFile file_serial(files.srl_ca_cert_file);
    if (!file_serial.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return FAIL;
    }
    QTextStream stream_serial(&file_serial);
    stream_serial << "00";
    file_serial.close();

    //creat index file
    QFile file_index(files.index);
    if (!file_index.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return FAIL;
    }
    file_index.close();

    //creat openssl.cnf file
    QFile::copy(QString(OPENSSL_CONFIG_PATH) + "openssl.cnf", files.openssl_config);
    QFile file_openssl_config(files.openssl_config);
    if (!file_openssl_config.open(QIODevice::ReadOnly | QIODevice::Text )) {
        return FAIL;
    }
    QString file_text = file_openssl_config.readAll();
    file_openssl_config.close();
    QRegExp regexp_dir(RegexpPatternWorkDir::OpensslConfCADir);
    int pos = 0;
    // меняем переменную dir в файле openssl.cnf на текущую work_path
    if ((pos = regexp_dir.indexIn(file_text, 0)) != -1) {
        if (!file_openssl_config.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return FAIL;
        }
        file_text.remove(pos + regexp_dir.cap(1).length(), regexp_dir.cap(2).length());
        file_text.insert(pos + regexp_dir.cap(1).length(), work_path);
        file_openssl_config.write(file_text.toLocal8Bit());
    } else {
        return FAIL;
    }
    file_openssl_config.close();

    //creat config file
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return FAIL;
    }
    QTextStream stream_config(&file_config);
    stream_config << "csptest = " << CRYPTOPRO_DIR_PATH << "csptest\n";
    stream_config << "openssl = " << OPENSSL_DIR_PATH << "openssl\n";
    stream_config << "CAcert = \n";
    stream_config << "CAkey = \n";
    file_config.close();
    isOk = true;
    return OK;
}
//Инициализация рабочей дериктории
BOOL_ERR WorkDir::initialiseWorkDir() {
    QDir dir;
    isOpenMod = false;
    isOk = false;
    config.isOk = false;
    data_base.isOk = false;
    isOpenMod = false;
    data_base.table.table_name = "cert";
    data_base.name = work_path + "ca_db";
    config.name = work_path + "config";
    files.index = work_path + "index.txt";
    files.crlnumber = work_path + "crlnumber";
    files.openssl_config = work_path + "openssl.cnf";
    files.srl_ca_cert_file = work_path + "ca_cert.srl";

    if (!dir.exists(work_path))
    {
        dir.mkpath(work_path);
    }

    if (!QFile::exists(config.name) || !QFile::exists(files.openssl_config) ||
        !QFile::exists(data_base.name) || !QFile::exists(files.crlnumber) ||
        !QFile::exists(files.index))
    {
        return FAIL;
    }
    else
    {
        isOk = true;
        isOpenMod = true;
    }

    return OK;
}
BOOL_ERR WorkDir::initialiseConfig() {
    config = loadConfig(config.name);
    if (!config.isOk)
    {
        return FAIL;
    }
    else
    {
        return OK;
    }
}
BOOL_ERR WorkDir::initialiseDatabase() {
    if (!data_base.load_db(data_base.name))
    {
        return FAIL;
    }
    else
    {
        data_base.isOk = true;
        data_base.query = new QSqlQuery(data_base.sql_db);
        if (!isOpenMod)
        {
            data_base.newTables(data_base.query);
        }
        if (data_base.isOk && config.isOk)
        {
            isOk = true;
        }
    }
    return OK;
}

//------------------------------------------------------
//------------------------------------------------------
//------------------------------------------------------
QStringList WorkDir::parsingContainers(QString info) {
    QStringList containers;
    int pos;
    QRegExp regexp_skip(RegexpPatternWorkDir::SkipCsptestOutputWords); //crypto pro output containers
    QRegExp regexp_cont_line(RegexpPatternWorkDir::ContainerLine);
    if ((pos = regexp_skip.indexIn(info, 0)) == -1) {
        //ui_mw->debugLogEdit->setPlainText(ui_mw->debugLogEdit->toPlainText() + prog.output
        //                                    + "\n В " + prog.file_out + " не удалось определить CN");
    } else {
        while (regexp_cont_line.indexIn(info, pos) != -1) {
            pos = regexp_cont_line.pos(1) + regexp_cont_line.cap(1).size();
            containers << "Name: " + regexp_cont_line.cap(2) +
                          " (len = " + regexp_cont_line.cap(1) + ")";
        }
    }
    return containers;
}

QStringList WorkDir::checkContainers() {
    Program program("openssl", "cert");
    program.program_path = "";
    program.program_name = config.csptest;
    program.args = QString("-enum -info -type PP_ENUMCONTAINERS").split(" ");
    program.run();
    return parsingContainers(program.output);
}


Config WorkDir::loadConfig(QString file_name) {
    config.name = file_name;
    config.isOk = true;
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setErrorString("Open failed: " + config.name);
        config.isOk = false;
        return config;
    }
    QTextStream stream_config(&file_config);
    QString line;
    while (!stream_config.atEnd()) {
        line = stream_config.readLine();
        QRegExp regexp(RegexpPatternWorkDir::ConfigLine);
        regexp.indexIn(line, 0);
        QString term = regexp.cap(1);
        QString define = regexp.cap(2);
        if (define.isEmpty()) {
            setErrorString("Пустое определение при параметре:" + term);
            config.isOk = false;
            break;
        }
        if (term == "openssl") {
            config.openssl = define;
            if (!QFile::exists(config.openssl)) {
                setErrorString("Не найден файл: " + config.openssl);
                config.isOk = false;
                break;
            }
        } else if (term == "csptest") {
            config.csptest = define;
            if (!QFile::exists(config.csptest)) {
                setErrorString("Не найден файл: " + config.csptest);
                config.isOk = false;
                break;
            }
        } else if (term == "CAkey") {
            config.CAkey = define;
            if (!QFile::exists(config.CAkey)) {
                bool isOk = false;
                QStringList container = checkContainers();
                QRegExp regexp_cont(RegexpPatternWorkDir::Container);
                foreach (QString cont, container) {
                    regexp_cont.indexIn(cont, 0);
                    if (config.CAkey == regexp_cont.cap(1)) {
                        isOk = true;
                        break;
                    }
                }
                if (!isOk) {
                    setErrorString("Ключ сертификата УЦ не найден: " + config.CAkey);
                    config.isOk = false;
                    break;
                }
            }
        } else if (term == "CAcert") {
            config.CAcert = define;
            if (!QFile::exists(config.CAcert)) {
                setErrorString("Не найден файл: " + config.CAcert);
                config.isOk = false;
                break;
            }
        }
    }
    file_config.close();
    if (config.openssl.isEmpty() || config.csptest.isEmpty() ||
        config.CAcert.isEmpty() || config.CAkey.isEmpty())
            config.isOk = false;
    return config;
}

void WorkDir::saveConfig(Config config) {
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text)) {
     return;
    }
    QTextStream stream_config(&file_config);
    stream_config << "csptest = " << config.csptest << '\n';
    stream_config << "openssl = " << config.openssl << '\n';
    stream_config << "CAcert = " << config.CAcert << '\n';
    stream_config << "CAkey = " << config.CAkey << '\n';
    file_config.close();
 }

CACert WorkDir::loadCaCert(Config config) {
    CACert ca_cert;
    ca_cert.file_name = config.CAcert;
    ca_cert.file_key = config.CAkey;
    ca_cert.key = config.CAkey;
    ca_cert.CN = DbTable::getCNFromCert(ca_cert.file_name);
    return ca_cert;
 }

DbTable WorkDir::exportCert(QString CN, QString file_name) {
    DbTable table_cert;
    QFile file_cert(file_name);
    if (!file_cert.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return table_cert;
    }
    QTextStream stream_cert(&file_cert);

    table_cert.table_name = "cert";
    table_cert.condition = "CN = '" + CN + "'";
    table_cert = data_base.loadFromDb(table_cert.table_name, table_cert.condition, data_base.query);
    stream_cert << table_cert.pem;
    file_cert.close();
    return table_cert;
}

DbTable WorkDir::importCert(QString file_name) {
    DbTable table_cert;
    table_cert.isOk = true;
    if (!QFile::exists(file_name)) {
        table_cert.isOk = false;
        table_cert.status = "не удалось открыть файл";
        return table_cert;
    }

    if ((table_cert.CN = table_cert.getCNFromCsr(file_name)) == "error") {
        if ((table_cert.CN = table_cert.getCNFromCert(file_name)) == "error") {
            table_cert.isOk = false;
            table_cert.status = "не удалось распознать CN";
            return table_cert;
        }
        // Работаем с подписанным сертификатом
        else {
            table_cert.serial = table_cert.getSerialFromCert(file_name);
            table_cert.issuer = table_cert.getIssuerFromCert(file_name);
            table_cert.revoke = "unknown";
        }
    }
    // Работем с CSR
    else {
        table_cert.serial = "no";
        table_cert.issuer = "no";
        table_cert.revoke = "no";
    }
    table_cert.pem = table_cert.getPemFromFile(file_name);
    table_cert.table_name = "cert";
    table_cert.days_valid = "no info";
    if (table_cert.serial == "error" || table_cert.issuer == "error" ||
        table_cert.pem == "error" || table_cert.revoke == "error") {
        table_cert.isOk = false;
        table_cert.status = "не удалось импортировать сертификат";
        return table_cert;
    }
    data_base.saveToDb(table_cert, data_base.query);
    return table_cert;
}
// Генерация шаблона сертификата в конфиг
BOOL_ERR WorkDir::genCertConfig(DbTable table) {
    QFile file(files.cert_config);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return FAIL;
    }
    QTextStream stream_file(&file);
    stream_file << "oid_section = OIDs" << endl;

    stream_file << "[ req ]" << endl;
    stream_file << "prompt = no" << endl;
    stream_file << "encrypt_key = no" << endl;
    stream_file << "distinguished_name = dn" << endl;
    stream_file << "req_extensions = req_ext" << endl;

    stream_file << "[ OIDs ]" << endl;
    stream_file << "MySensationalOID=1.2.3.45" << endl;
    stream_file << "MyOutstandingOID=2.3.4.56" << endl;

    stream_file << "[ dn ]" << endl;
    stream_file << "CN = " << table.CN << endl;
    stream_file << "emailAddress = " << table.email << endl;
    stream_file << "O = " << table.O << endl;
    stream_file << "C = " << table.C << endl;
    stream_file << "MySensationalOID = Support Department" << endl;

    stream_file << "[ req_ext ]" << endl;
    stream_file << "subjectAltName = " << table.sun << endl;
    file.close();
    return OK;
}
// Удаление шаблона сертификата (конфига)
BOOL_ERR WorkDir::delCertConfig() {
    Program::removeFile(files.cert_config);
}

