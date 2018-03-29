#include "work_dir.h"

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
    Program prog = Program("openssl", "ca_cert");
    prog.args = QString("req -in " + file_in + " -noout -text").split(" ");
    prog.run();
    return prog.output;
}

QString DbTable::getTextFromCert(QString file_in) {
    Program prog = Program("openssl", "ca_cert");
    prog.args = QString("x509 -in " + file_in + " -noout -text").split(" ");
    prog.run();
    return prog.output;
}

QString DbTable::getTextFromAny(QString file_in) {
    Program prog = Program("openssl", "ca_cert");
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
    QRegExp regexp(RegexpPatternWorkDir::CN);

    if (regexp.indexIn(getTextFromCert(file_in), 0) == -1) {
        return "error";
    } else {
        CN = regexp.cap(1);
    }
    return CN;
}
QString DbTable::getIssuerFromCert(QString file_in) {
    QString issuer;
    QRegExp regexp(RegexpPatternWorkDir::Issuer);

    if (regexp.indexIn(getTextFromCert(file_in), 0) == -1) {
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
    QRegExp regexp(RegexpPatternWorkDir::Serial);

    if (regexp.indexIn(getTextFromCert(file_name), 0) == -1) {
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
        "`issuer`    TEXT)");
    query->exec("CREATE TABLE cert ("
        "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`CN`       TEXT,"
        "`pem`      TEXT,"
        "`key`      TEXT,"
        "`suite`    TEXT,"
        "`serial`   TEXT,"
        "`revoke`   TEXT,"
        "`issuer`    TEXT)");
    query->exec("CREATE TABLE crl ("
        "`id`       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`index`	TEXT,"
        "`crl`      TEXT)");
}
void DataBase::saveToDb(DbTable table, QSqlQuery *query) {
    if (table.table_name == "cert") {
        table.condition = "CN = '" + table.CN + "'";
        DbTable tmp_table = loadFromDb(table.table_name, table.condition, query);
        if (tmp_table.pem.isEmpty()) {
            query->prepare("INSERT INTO cert (CN, pem, key, suite, serial, revoke, issuer) "
                           "VALUES (:CN, :pem, :key, :suite, :serial, :revoke, :issuer)");
            query->bindValue(":CN", table.CN);
            query->bindValue(":pem", table.pem);
            query->bindValue(":key", table.key);
            query->bindValue(":suite", table.suite);
            query->bindValue(":serial", table.serial);
            query->bindValue(":revoke", table.revoke);
            query->bindValue(":issuer", table.issuer);
        } else {
            query->prepare("UPDATE cert SET "
                           "CN = '" + table.CN + "', "
                           "pem = '" + table.pem + "', "
                           "key = '" + table.key + "', "
                           "suite = '" + table.suite + "', "
                           "serial = '" + table.serial + "', "
                           "revoke = '" + table.revoke + "', "
                           "issuer = '" + table.issuer + "' "
                           "WHERE " + table.condition);
        }
    } else if (table.table_name == "crl") {
        query->prepare("INSERT INTO crl (CN, pem, key, suite, serial) "
                   "VALUES (:CN, :pem, :key, :suite, :serial)");
        query->bindValue(":CN", table.CN);
        query->bindValue(":pem", table.pem);
        query->bindValue(":key", table.key);
        query->bindValue(":suite", table.suite);
        query->bindValue(":serial", table.serial);
    } else if (table.table_name == "ca") {
        query->prepare("INSERT INTO ca (CN, pem, key, suite, serial, revoke, issuer) "
                       "VALUES (:CN, :pem, :key, :suite, :serial, :revoke, :issuer)");
        query->bindValue(":CN", table.CN);
        query->bindValue(":pem", table.pem);
        query->bindValue(":key", table.key);
        query->bindValue(":suite", table.suite);
        query->bindValue(":serial", table.serial);
        query->bindValue(":revoke", table.revoke);
        query->bindValue(":issuer", table.issuer);
    }
    query->exec();
    query->finish();
}

DbTable DataBase::loadFromDb(QString table_name, QString table_condition, QSqlQuery *query) {
    DbTable table;
    if (table_name == "cert") {
        if (!query->prepare("SELECT CN, pem, key, suite, serial, revoke, issuer "
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
    query->finish();
    return table;
}


QString CAConfig::writeToConfigFile(QString file_name, QString data) {
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream out(&file_config);
    out << data;
    file_config.close();
    return "ok";
}
QString CAConfig::readAllFromConfigFile(QString file_name) {
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
    index = work_path + "index.txt";
    crlnumber = work_path + "crlnumber";
    openssl_config = work_path + "openssl.cnf";
}

//------------------------------------------------------
//-----------ЗАГРУЗКА РАБОЧЕЙ ДИРЕКТОРИИ----------------
//------------------------------------------------------
//Создание новой рабочей директории
QString WorkDir::newWorkDir() {

    //creat crlnumber file
    QFile file_crlnumber(crlnumber);
    if (!file_crlnumber.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream stream_crlnumber(&file_crlnumber);
    stream_crlnumber << "01";
    file_crlnumber.close();

    //creat serial file
    QFile file_serial(files.srl_ca_cert_file);
    if (!file_serial.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream stream_serial(&file_serial);
    stream_serial << "00";
    file_serial.close();

    //creat index file
    QFile file_index(index);
    if (!file_index.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    file_index.close();

    //creat openssl.cnf file
    QFile::copy("/var/opt/cprocsp/cp-openssl-1.1.0/openssl.cnf", openssl_config);
    QFile file_openssl_config(openssl_config);
    if (!file_openssl_config.open(QIODevice::ReadOnly | QIODevice::Text )) {
        return "error";
    }
    QString file_text = file_openssl_config.readAll();
    file_openssl_config.close();
    QRegExp regexp_dir(RegexpPatternWorkDir::OpensslConfCADir);
    int pos = 0;
    if ((pos = regexp_dir.indexIn(file_text, 0)) != -1) {
        if (!file_openssl_config.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            return "error";
        }
        file_text.remove(pos + regexp_dir.cap(1).length(), regexp_dir.cap(2).length());
        file_text.insert(pos + regexp_dir.cap(1).length(), work_path);
        file_openssl_config.write(file_text.toLocal8Bit());
    } else {
        return "error";
    }
    file_openssl_config.close();

    //creat config file
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return "error";
    }
    QTextStream stream_config(&file_config);
    stream_config << "csptest = /opt/cprocsp/bin/amd64/csptest\n";
    stream_config << "openssl = /opt/cprocsp/cp-openssl-1.1.0/bin/amd64/openssl\n";
    stream_config << "CAcert = \n";
    stream_config << "CAkey = \n";
    file_config.close();
    isOk = true;
    return "ok";
}
//Инициализация рабочей дериктории
QString WorkDir::initialiseWorkDir() {
    isOpenMod = false;
    isOk = false;
    config.isOk = false;
    data_base.isOk = false;
    isOpenMod = false;
    data_base.table.table_name = "cert";
    //work_path = ui_dl->textEdit->toPlainText() + "/";
    data_base.name = work_path + "ca_db";
    config.name = work_path + "config";
    index = work_path + "index.txt";
    crlnumber = work_path + "crlnumber";
    openssl_config = work_path + "openssl.cnf";
    QDir dir;
    if (!dir.exists(work_path)) dir.mkpath(work_path);
    if (!QFile::exists(config.name) || !QFile::exists(openssl_config) ||
        !QFile::exists(data_base.name) || !QFile::exists(crlnumber) || !QFile::exists(index)) {
        return "error";
    } else {
        isOk = true;
        isOpenMod = true;
    }
    return "ok";
}
QString WorkDir::initialiseConfig() {
    config = loadConfig(config.name);
    if (!config.isOk) return "error";
    return "ok";
}
QString WorkDir::initialiseDatabase() {
    if (!data_base.load_db(data_base.name)) {
        return "error";
    } else {
        data_base.isOk = true;
        data_base.query = new QSqlQuery(data_base.sql_db);
        if (!isOpenMod) {
            data_base.newTables(data_base.query);
        }
        if (data_base.isOk && config.isOk) {
            isOk = true;
        }
    }
    return "ok";
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


CAConfig WorkDir::loadConfig(QString file_name) {
    config.name = file_name;
    config.isOk = true;
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
            config.isOk = false;
            break;
        }
        if (term == "openssl") {
            config.openssl = define;
            if (!QFile::exists(config.openssl)) {
                config.isOk = false;
                break;
            }
        } else if (term == "csptest") {
            config.csptest = define;
            if (!QFile::exists(config.csptest)) {
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
                    config.isOk = false;
                    break;
                }
            }
        } else if (term == "CAcert") {
            config.CAcert = define;
            if (!QFile::exists(config.CAcert)) {
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

void WorkDir::saveConfig(CAConfig config) {
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

CAcert WorkDir::loadCaCert(CAConfig config) {
    CAcert ca_cert;
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
    if (table_cert.serial == "error" || table_cert.issuer == "error" ||
        table_cert.pem == "error" || table_cert.revoke == "error") {
        table_cert.isOk = false;
        table_cert.status = "не удалось импортировать сертификат";
        return table_cert;
    }
    data_base.saveToDb(table_cert, data_base.query);
    return table_cert;
}


