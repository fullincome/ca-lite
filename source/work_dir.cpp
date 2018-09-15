#include "work_dir.h"

CertExt::CertExt()
{

}

DbTable::DbTable()
{
    CN = "CN_calite";
    L = "Moscow";
    O = "O_calite";
    C = "RU";
    email = "calite@calite.ru";
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

QStringList DbTable::checkErrorFields()
{
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

QString CertExt::readAllFromFile(QString file_name)
{
    QString config_data;
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "error";
    }
    QTextStream in(&file_config);
    if (!in.atEnd())
    {
         config_data = in.readAll();
    }
    file_config.close();
    return config_data;
}

BOOL_ERR CertExt::writeAllToFile(QString file_name, QString text)
{
    QString config_data;
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return FAIL;
    }
    QTextStream out(&file_config);
    out << text;
    file_config.close();
    return OK;
}

BOOL DbTable::existExtension(QString config)
{
    QRegExp regexp_dir(RegexpPatternWorkDir::OpensslExtension);
    if (regexp_dir.indexIn(config) == -1)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

QString DbTable::getPemFromFile(QString file_name)
{
    QString pem;
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "error";
    }
    QTextStream in(&file);
    if (!in.atEnd())
    {
        pem = in.readAll();
    }
    file.close();
    return pem;
}

QString DbTable::getTextFromCsr(QString file_in)
{
    Program prog = Program("openssl", "csr");
    prog.args = QString("req -in " + file_in + " -noout -text").split(" ");
    prog.run();
    return prog.output;
}

BOOL_ERR DbTable::getTextFromCert(QString file_in, QString &cert_info)
{
    Program prog = Program("openssl", "ca");
    prog.args = QString("x509 -in " + file_in + " -noout -text").split(" ");
    prog.run();
    if (prog.isError)
    {
        setErrorString(prog.output);
        return FAIL;
    }
    else
    {
        cert_info = prog.output;
        return OK;
    }
}

QString DbTable::getTextFromAny(QString file_in, QString work_path)
{
    Program prog = Program("openssl", "ca", work_path);
    prog.args = QString("x509 -in " + file_in + " -noout -text").split(" ");
    prog.run();
    if (prog.isError)
    {
        prog.args = QString("req -in " + file_in + " -noout -text").split(" ");
        prog.run();
    }
    return prog.output;
}

QString DbTable::getCNFromCsr(QString file_in)
{
    QString CN;
    QRegExp regexp(RegexpPatternWorkDir::CN);

    if (regexp.indexIn(getTextFromCsr(file_in), 0) == -1)
    {
        return "error";
    }
    else
    {
        CN = regexp.cap(1);
    }
    return CN;
}

BOOL_ERR DbTable::getCNFromCert(QString file_in, QString &CN)
{
    QString textCert;
    QRegExp regexp(RegexpPatternWorkDir::CN);

    getTextFromCert(file_in, textCert);
    if (regexp.indexIn(textCert, 0) == -1)
    {
        setErrorString("CN was not found in " + file_in);
        return FAIL;
    }
    else
    {
        CN = regexp.cap(1);
    }
    return OK;
}

QString DbTable::getIssuerFromCert(QString file_in)
{
    QString issuer;
    QString textCert;
    QRegExp regexp(RegexpPatternWorkDir::Issuer);

    getTextFromCert(file_in, textCert);
    if (regexp.indexIn(textCert, 0) == -1)
    {
        return "error";
    }
    else
    {
        issuer = regexp.cap(1);
    }
    return issuer;
}

QString DbTable::creatSerialToFile(QString file_name)
{
    QFile file_srl(file_name);
    if (!file_srl.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return "error";
    }
    QTextStream stream_srl(&file_srl);
    stream_srl << "AF";
    file_srl.close();
    return "ok";
}

QString DbTable::getSerialFromFile(QString file_name)
{
    QString serial;
    QFile file_srl(file_name);
    if (!file_srl.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "error";
    }
    QTextStream stream_srl(&file_srl);
    serial = stream_srl.readAll();
    file_srl.close();
    return serial;
}

QString DbTable::getSerialFromCert(QString file_name)
{
    QString serial;
    QString textCert;
    QRegExp regexp(RegexpPatternWorkDir::Serial);

    getTextFromCert(file_name, textCert);
    if (regexp.indexIn(textCert, 0) == -1)
    {
        return "error";
    }
    else
    {
        serial = regexp.cap(1);
    }
    return serial;
}




//------------------------------------------------------
//--------------------DataBase--------------------------
//------------------------------------------------------
bool DataBase::load_db(QString name)
{
    sql_db = QSqlDatabase::addDatabase("QSQLITE");
    sql_db.setDatabaseName(name);
    return sql_db.open() ? true : false;
}

BOOL_ERR DataBase::newTables(QSqlQuery *query)
{
    if (!query->exec("CREATE TABLE ca ("
        "`id`           INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`CN`           TEXT NOT NULL,"
        "`pem`          TEXT NOT NULL,"
        "`L`            TEXT,"
        "`C`            TEXT,"
        "`O`            TEXT,"
        "`key`          TEXT,"
        "`suite`        TEXT,"
        "`serial`       TEXT,"
        "`revoke`       TEXT,"
        "`issuer`       TEXT,"
        "`days_valid`   TEXT)"))
    {
        setErrorString("query->exec() fail with: " + query->lastError().text());
        return FAIL;
    }
    if (!query->exec("CREATE TABLE cert ("
        "`id`           INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`CN`           TEXT NOT NULL,"
        "`pem`          TEXT NOT NULL,"
        "`L`            TEXT,"
        "`C`            TEXT,"
        "`O`            TEXT,"
        "`key`          TEXT,"
        "`suite`        TEXT,"
        "`serial`       TEXT,"
        "`revoke`       TEXT,"
        "`issuer`       TEXT,"
        "`days_valid`   TEXT)"))
    {
        setErrorString("query->exec() fail with: " + query->lastError().text());
        return FAIL;
    }
    if (!query->exec("CREATE TABLE ca_extension ("
        "`id`     INTEGER PRIMARY KEY REFERENCES cert(id) ON DELETE CASCADE ON UPDATE CASCADE,"
        "`config` TEXT NOT NULL,"
        "`basicConstraints`    TEXT,"
        "`authorityKeyIdentifier`    TEXT,"
        "`subjectKeyIdentifier`    TEXT,"
        "`keyUsage`    TEXT,"
        "`nsCertType`    TEXT,"
        "`subjectAltName`    TEXT,"
        "`extendedKeyUsage`    TEXT)"))
    {
        setErrorString("query->exec() fail with: " + query->lastError().text());
        return FAIL;
    }
    if (!query->exec("CREATE TABLE cert_extension ("
        "`id`     INTEGER PRIMARY KEY REFERENCES cert(id) ON DELETE CASCADE ON UPDATE CASCADE,"
        "`config` TEXT NOT NULL,"
        "`basicConstraints`    TEXT,"
        "`authorityKeyIdentifier`    TEXT,"
        "`subjectKeyIdentifier`    TEXT,"
        "`keyUsage`    TEXT,"
        "`nsCertType`    TEXT,"
        "`subjectAltName`    TEXT,"
        "`extendedKeyUsage`    TEXT)"))
    {
        setErrorString("query->exec() fail with: " + query->lastError().text());
        return FAIL;
    }
    if (!query->exec("CREATE TABLE crl ("
        "`id`       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "`index`	TEXT NOT NULL,"
        "`crl`      TEXT NOT NULL)"))
    {
        setErrorString("query->exec() fail with: " + query->lastError().text());
        return FAIL;
    }
    return OK;
}

BOOL_ERR DataBase::saveToDb(DbTable table, QSqlQuery *query)
{
    BOOL_ERR rc = FAIL;
    if (table.table_name == "cert" || table.table_name == "csr")
    {
        DbTable tmp_table;
        table.condition = "CN = '" + table.CN + "'";
        rc = loadFromDb(table.table_name, table.condition, query, tmp_table);
        if (!rc)
        {
            rc = query->prepare("INSERT INTO cert (CN, pem, L, C, O, key, suite, serial, revoke, issuer, days_valid) "
                           "VALUES (:CN, :pem, :L, :C, :O, :key, :suite, :serial, :revoke, :issuer, :days_valid)");
            if (!rc)
            {
                setErrorString("query->prepare() fail with: " + query->lastError().text());
                return FAIL;
            }

            query->bindValue(":CN", table.CN);
            query->bindValue(":pem", table.pem);
            query->bindValue(":key", table.key);
            query->bindValue(":L", table.L);
            query->bindValue(":C", table.C);
            query->bindValue(":O", table.O);
            query->bindValue(":suite", table.suite);
            query->bindValue(":serial", table.serial);
            query->bindValue(":revoke", table.revoke);
            query->bindValue(":issuer", table.issuer);
            query->bindValue(":days_valid", table.days_valid);

            rc = query->exec();
            if (!rc)
            {
                setErrorString("query->exec() fail with: " + query->lastError().text());
                return FAIL;
            }
        }
        else
        {
            rc = query->prepare("UPDATE cert SET "
                           "CN = '" + table.CN + "', "
                           "pem = '" + table.pem + "', "
                           "key = '" + table.key + "', "
                           "L = '" + table.L + "', "
                           "C = '" + table.C + "', "
                           "O = '" + table.O + "', "
                           "suite = '" + table.suite + "', "
                           "serial = '" + table.serial + "', "
                           "revoke = '" + table.revoke + "', "
                           "issuer = '" + table.issuer + "', "
                           "days_valid = '" + table.days_valid + "' "
                           "WHERE " + table.condition);
            if (!rc)
            {
                setErrorString("query->prepare() fail with: " + query->lastError().text());
                return FAIL;
            }

            rc = query->exec();
            if (!rc)
            {
                setErrorString("query->exec() fail with: " + query->lastError().text());
                return FAIL;
            }
        }
        //"CREATE TABLE cert_extension ("
          //      "`id`     INTEGER PRIMARY KEY REFERENCES cert(id) ON DELETE CASCADE ON UPDATE CASCADE,"
            //    "`config` TEXT NOT NULL,"
              //  "`sun`    TEXT,"
               // "`eku`    TEXT)"

        // Загружаем заново, чтобы узнать id
        rc = loadFromDb(table.table_name, table.condition, query, tmp_table);
        if (!rc)
        {
            setErrorString("loadFromDb fail with: " + query->lastError().text());
            return FAIL;
        }
        table.condition = "id = '" + tmp_table.id + "'";
        rc = loadFromDb("cert_extension", table.condition, query, tmp_table);
        if (!rc)
        {
            rc = query->prepare("INSERT INTO cert_extension ("
                                    "id, "
                                    "config, "
                                    "basicConstraints, "
                                    "authorityKeyIdentifier, "
                                    "subjectKeyIdentifier, "
                                    "keyUsage, "
                                    "nsCertType, "
                                    "subjectAltName, "
                                    "extendedKeyUsage"
                                ") "
                                "VALUES ("
                                    ":id, "
                                    ":config, "
                                    ":basicConstraints, "
                                    ":authorityKeyIdentifier, "
                                    ":subjectKeyIdentifier, "
                                    ":keyUsage, "
                                    ":nsCertType, "
                                    ":subjectAltName, "
                                    ":extendedKeyUsage"
                                ")"
                                );
            if (!rc)
            {
                setErrorString("query->prepare() fail with: " + query->lastError().text());
                return FAIL;
            }

            query->bindValue(":id", tmp_table.id);
            query->bindValue(":config", table.cert_extension.config);
            query->bindValue(":basicConstraints", table.cert_extension.basicConstraints);
            query->bindValue(":authorityKeyIdentifier", table.cert_extension.authorityKeyIdentifier);
            query->bindValue(":subjectKeyIdentifier", table.cert_extension.subjectKeyIdentifier);
            query->bindValue(":keyUsage", table.cert_extension.keyUsage);
            query->bindValue(":nsCertType", table.cert_extension.nsCertType);
            query->bindValue(":subjectAltName", table.cert_extension.subjectAltName);
            query->bindValue(":extendedKeyUsage", table.cert_extension.extendedKeyUsage);

            rc = query->exec();
            if (!rc)
            {
                setErrorString("query->exec() fail with: " + query->lastError().text());
                return FAIL;
            }
        }
        else
        {
            rc = query->prepare("UPDATE cert_extension SET "
                                "id = '" + tmp_table.id + "', "
                                "config = '" + table.cert_extension.config + "', "
                                "basicConstraints = '" + table.cert_extension.basicConstraints + "', "
                                "authorityKeyIdentifier = '" + table.cert_extension.authorityKeyIdentifier + "', "
                                "subjectKeyIdentifier = '" + table.cert_extension.subjectKeyIdentifier + "', "
                                "keyUsage = '" + table.cert_extension.keyUsage + "', "
                                "nsCertType = '" + table.cert_extension.nsCertType + "', "
                                "subjectAltName = '" + table.cert_extension.subjectAltName + "', "
                                "extendedKeyUsage = '" + table.cert_extension.extendedKeyUsage + "' "
                                "WHERE " + table.condition);
            if (!rc)
            {
                setErrorString("query->prepare() fail with: " + query->lastError().text());
                return FAIL;
            }

            rc = query->exec();
            if (!rc)
            {
                setErrorString("query->exec() fail with: " + query->lastError().text());
                return FAIL;
            }
        }
    }
    else if (table.table_name == "crl")
    {
        rc = query->prepare("INSERT INTO crl (CN, pem, key, suite, serial) "
                   "VALUES (:CN, :pem, :key, :suite, :serial)");
        if (!rc)
        {
            setErrorString("query->prepare() fail with: " + query->lastError().text());
            return FAIL;
        }

        query->bindValue(":CN", table.CN);
        query->bindValue(":pem", table.pem);
        query->bindValue(":key", table.key);
        query->bindValue(":suite", table.suite);
        query->bindValue(":serial", table.serial);

        rc = query->exec();
        if (!rc)
        {
            setErrorString("query->exec() fail with: " + query->lastError().text());
            return FAIL;
        }
    }
    else if (table.table_name == "ca")
    {
        rc = query->prepare("INSERT INTO ca (CN, pem, L, C, O, key, suite, serial, revoke, issuer, days_valid) "
                       "VALUES (:CN, :pem, :L, :C, :O, :key, :suite, :serial, :revoke, :issuer, :days_valid)");
        if (!rc)
        {
            setErrorString("query->prepare() fail with: " + query->lastError().text());
            return FAIL;
        }

        query->bindValue(":CN", table.CN);
        query->bindValue(":pem", table.pem);
        query->bindValue(":L", table.L);
        query->bindValue(":C", table.C);
        query->bindValue(":O", table.O);
        query->bindValue(":key", table.key);
        query->bindValue(":suite", table.suite);
        query->bindValue(":serial", table.serial);
        query->bindValue(":revoke", table.revoke);
        query->bindValue(":issuer", table.issuer);
        query->bindValue(":days_valid", table.days_valid);

        rc = query->exec();
        if (!rc)
        {
            setErrorString("query->exec() fail with: " + query->lastError().text());
            return FAIL;
        }

        rc = query->prepare("INSERT INTO ca_extension ("
                                "id, "
                                "config, "
                                "basicConstraints, "
                                "authorityKeyIdentifier, "
                                "subjectKeyIdentifier, "
                                "keyUsage, "
                                "nsCertType, "
                                "subjectAltName, "
                                "extendedKeyUsage"
                            ") "
                            "VALUES ("
                                ":id, "
                                ":config, "
                                ":basicConstraints, "
                                ":authorityKeyIdentifier, "
                                ":subjectKeyIdentifier, "
                                ":keyUsage, "
                                ":nsCertType, "
                                ":subjectAltName, "
                                ":extendedKeyUsage"
                            ")"
                            );
        if (!rc)
        {
            setErrorString("query->prepare() fail with: " + query->lastError().text());
            return FAIL;
        }

        query->bindValue(":id", "1");
        query->bindValue(":config", table.cert_extension.config);
        query->bindValue(":basicConstraints", table.cert_extension.basicConstraints);
        query->bindValue(":authorityKeyIdentifier", table.cert_extension.authorityKeyIdentifier);
        query->bindValue(":subjectKeyIdentifier", table.cert_extension.subjectKeyIdentifier);
        query->bindValue(":keyUsage", table.cert_extension.keyUsage);
        query->bindValue(":nsCertType", table.cert_extension.nsCertType);
        query->bindValue(":subjectAltName", table.cert_extension.subjectAltName);
        query->bindValue(":extendedKeyUsage", table.cert_extension.extendedKeyUsage);

        rc = query->exec();
        if (!rc)
        {
            setErrorString("query->exec() fail with: " + query->lastError().text());
            return FAIL;
        }
    }

    return OK;
}

BOOL_ERR DataBase::loadFromDb(QString table_name, QString table_condition, QSqlQuery *query, DbTable &table)
{
    if (table_name == "cert" || table_name == "csr")
    {
        if (!query->prepare("SELECT id, CN, pem, L, C, O, key, suite, serial, revoke, issuer, days_valid "
                            "FROM cert "
                            "WHERE " + table_condition))
        {
            return FAIL;
        }

        query->exec();
        if (!query->next())
        {
            return FAIL;
        }
        else
        {
            table.id = query->value(0).toString();
            table.CN = query->value(1).toString();
            table.pem = query->value(2).toString();
            table.L = query->value(3).toString();
            table.C = query->value(4).toString();
            table.O = query->value(5).toString();
            table.key = query->value(6).toString();
            table.suite = query->value(7).toString();
            table.serial = query->value(8).toString();
            table.revoke = query->value(9).toString();
            table.issuer = query->value(10).toString();
            table.days_valid = query->value(11).toString();
            query->finish();
            return OK;
        }
    }
    else if (table_name == "cert_extension")
    {
        if (!query->prepare("SELECT "
                            "id, "
                            "config, "
                            "basicConstraints, "
                            "authorityKeyIdentifier, "
                            "subjectKeyIdentifier, "
                            "keyUsage, "
                            "nsCertType, "
                            "subjectAltName, "
                            "extendedKeyUsage "
                            "FROM cert_extension "
                            "WHERE " + table_condition))
        {
            return FAIL;
        }

        query->exec();
        if (!query->next())
        {
            return FAIL;
        }
        else
        {
            table.id = query->value(0).toString();
            table.cert_extension.config = query->value(1).toString();
            table.cert_extension.basicConstraints = query->value(2).toString();
            table.cert_extension.authorityKeyIdentifier = query->value(3).toString();
            table.cert_extension.subjectKeyIdentifier = query->value(4).toString();
            table.cert_extension.keyUsage = query->value(5).toString();
            table.cert_extension.nsCertType = query->value(6).toString();
            table.cert_extension.subjectAltName = query->value(7).toString();
            table.cert_extension.extendedKeyUsage = query->value(8).toString();
            return OK;
        }
    }
    else if (table_name == "crl")
    {
        if (!query->prepare("SELECT id, CN, pem, key, suite "
                            "FROM crl "
                            "WHERE " + table_condition))
        {
            return FAIL;
        }
    }
}


QString Config::writeToConfigFile(QString file_name, QString data)
{
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return "error";
    }
    QTextStream out(&file_config);
    out << data;
    file_config.close();
    return "ok";
}

QString Config::readAllFromConfigFile(QString file_name)
{
    QString config_data;
    QFile file_config(file_name);
    if (!file_config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "error";
    }
    QTextStream in(&file_config);
    if (!in.atEnd())
    {
         config_data = in.readAll();
    }
    file_config.close();
    return config_data;
}

QStringList WorkDirFiles::getVariableFiles()
{
    return QStringList() << csr_file << cert_file
                         << signing_file << key_csr_file
                         << key_signing_file << key_cert_file
                         << cert_config;
}
//------------------------------------------------------
//--------------------WorkDir---------------------------
//------------------------------------------------------

WorkDir::WorkDir () {}

WorkDir::WorkDir (QString work_path)
{
    this->work_path = work_path;
    isOk = true;
    config.isOk = true;
    data_base.isOk = true;
    isOpenMod = true;
    data_base.table.table_name = "cert";
    //work_path = ui_dl->textEdit->toPlainText() + "/";
    data_base.name = work_path + "ca_db";
    config.name = work_path + "config";
}

//------------------------------------------------------
//-----------ЗАГРУЗКА РАБОЧЕЙ ДИРЕКТОРИИ----------------
//------------------------------------------------------
//Создание новой рабочей директории
BOOL_ERR WorkDir::newWorkDir()
{
    //creat crlnumber file
    if (!QFile::exists(files.crlnumber))
    {
        QFile file_crlnumber(files.crlnumber);
        if (!file_crlnumber.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            setErrorString(files.crlnumber, ERR_OPEN_FILE);
            return FAIL;
        }
        QTextStream stream_crlnumber(&file_crlnumber);
        stream_crlnumber << "01";
        file_crlnumber.close();
    }

    //creat serial file
    if (!QFile::exists(files.srl_ca_cert_file))
    {
        QFile file_serial(files.srl_ca_cert_file);
        if (!file_serial.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            setErrorString(files.srl_ca_cert_file, ERR_OPEN_FILE);
            return FAIL;
        }
        QTextStream stream_serial(&file_serial);
        stream_serial << "00";
        file_serial.close();
    }

    //creat index file
    if (!QFile::exists(files.index))
    {
        QFile file_index(files.index);
        if (!file_index.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            setErrorString(files.index, ERR_OPEN_FILE);
            return FAIL;
        }
        file_index.close();
    }

    //creat config file
    if (!QFile::exists(config.name))
    {
        QFile file_config(config.name);
        if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            setErrorString(config.name, ERR_OPEN_FILE);
            return FAIL;
        }
        QTextStream stream_config(&file_config);
        stream_config << "csptest = " << CRYPTOPRO_DIR_PATH << CSPTEST_EXE;
        stream_config << "openssl = " << OPENSSL_DIR_PATH << OPENSSL_EXE;
        stream_config << "CAcert = \n";
        stream_config << "CAkey = \n";
        file_config.close();
    }

    //creat openssl.cnf file
    if (checkOpenssl())
    {
        QFile::copy(OPENSSL_CONFIG_PATH + OPENSSL_CONFIG, files.openssl_config);
        QFile file_openssl_config(files.openssl_config);
        if (!file_openssl_config.open(QIODevice::ReadOnly | QIODevice::Text ))
        {
            setErrorString(files.openssl_config, ERR_OPEN_FILE);
            return FAIL;
        }
        QString file_text = file_openssl_config.readAll();
        file_openssl_config.close();
        QRegExp regexp_dir(RegexpPatternWorkDir::OpensslConfCADir);
        int pos = 0;
        // меняем переменную dir в файле openssl.cnf на текущую work_path
        if ((pos = regexp_dir.indexIn(file_text, 0)) != -1)
        {
            if (!file_openssl_config.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            {
                setErrorString(files.openssl_config, ERR_OPEN_FILE);
                return FAIL;
            }
            file_text.remove(pos + regexp_dir.cap(1).length(), regexp_dir.cap(2).length());
            file_text.insert(pos + regexp_dir.cap(1).length(), work_path);
            file_openssl_config.write(file_text.toLocal8Bit());
        }
        else
        {
            setErrorString("patch openssl config " + files.openssl_config + " fail");
            return FAIL;
        }
        file_openssl_config.close();
    }
    else
    {
        setErrorString("openssl was not found");
        return FAIL;
    }

    isOk = true;
    return OK;
}

//Инициализация рабочей дериктории
BOOL_ERR WorkDir::initialiseWorkDir()
{
    QDir dir;
    isOpenMod = false;
    isOk = false;
    config.isOk = false;
    data_base.isOk = false;
    data_base.table.table_name = "cert";
    data_base.name = work_path + "ca_db";

    if (!dir.exists(work_path))
    {
        dir.mkpath(work_path);
    }

    if (!QFile::exists(config.name))
    {
        setErrorString("QFile::exists fail(" + config.name + ")");
        return FAIL;
    }

    if (!QFile::exists(files.openssl_config))
    {
        setErrorString("QFile::exists fail(" + files.openssl_config + ")");
        return FAIL;
    }

    if (!QFile::exists(data_base.name))
    {
        setErrorString("QFile::exists fail(" + data_base.name + ")");
        return FAIL;
    }

    if (!QFile::exists(files.srl_ca_cert_file))
    {
        setErrorString("QFile::exists fail(" + files.srl_ca_cert_file + ")");
        return FAIL;
    }

    if (!QFile::exists(files.crlnumber))
    {
        setErrorString("QFile::exists fail(" + files.crlnumber + ")");
        return FAIL;
    }

    if (!QFile::exists(files.index))
    {
        setErrorString("QFile::exists fail(" + files.index + ")");
        return FAIL;
    }


    isOk = true;
    isOpenMod = true;

    return OK;
}

BOOL_ERR WorkDir::initialiseConfig()
{
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

BOOL_ERR WorkDir::initialiseDatabase()
{
    if (!data_base.load_db(data_base.name))
    {
        setErrorString("Load data base fail: " + data_base.name);
        return FAIL;
    }
    else
    {
        data_base.isOk = true;
        data_base.query = new QSqlQuery(data_base.sql_db);
        if (!isOpenMod)
        {
            if (!data_base.newTables(data_base.query))
            {
                setErrorString("data_base.newTables fail");
                return FAIL;
            }
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
QStringList WorkDir::parsingContainers(QString info)
{
    QStringList containers;
    int pos;
    QRegExp regexp_skip(RegexpPatternWorkDir::SkipCsptestOutputWords); //crypto pro output containers
    QRegExp regexp_cont_line(RegexpPatternWorkDir::ContainerLine);
    if ((pos = regexp_skip.indexIn(info, 0)) == -1)
    {
        //ui_mw->debugLogEdit->setPlainText(ui_mw->debugLogEdit->toPlainText() + prog.output
        //                                    + "\n В " + prog.file_out + " не удалось определить CN");
    }
    else
    {
        while (regexp_cont_line.indexIn(info, pos) != -1)
        {
            pos = regexp_cont_line.pos(1) + regexp_cont_line.cap(1).size();
            containers << "Name: " + regexp_cont_line.cap(2) +
                          " (len = " + regexp_cont_line.cap(1) + ")";
        }
    }
    return containers;
}

QStringList WorkDir::checkContainers()
{
    Program program("openssl", "cert");
    program.program_path = "";
    program.program_name = config.csptest;
    program.args = QString("-enum -info -type PP_ENUMCONTAINERS").split(" ");
    program.run();
    return parsingContainers(program.output);
}

Config WorkDir::loadConfig(QString file_name)
{
    config.name = file_name;
    config.isOk = true;
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setErrorString("Open config fail: " + config.name);
        config.isOk = false;
        return config;
    }
    QTextStream stream_config(&file_config);
    QString line;
    while (!stream_config.atEnd())
    {
        line = stream_config.readLine();
        QRegExp regexp(RegexpPatternWorkDir::ConfigLine);
        regexp.indexIn(line, 0);
        QString term = regexp.cap(1);
        QString define = regexp.cap(2);
        if (define.isEmpty())
        {
            setErrorString("Пустое определение при параметре:" + term);
            config.isOk = false;
            break;
        }
        if (term == "openssl")
        {
            config.openssl = define;
            if (!checkOpenssl())
            {
                setErrorString("Не найден файл: " + config.openssl);
                config.isOk = false;
                config.openssl = "no";
                break;
            }
        }
        else if (term == "csptest")
        {
            config.csptest = define;
            if (!QFile::exists(config.csptest))
            {
                setErrorString("Не найден файл: " + config.csptest);
                config.isOk = false;
                break;
            }
        }
        else if (term == "CAkey")
        {
            config.CAkey = define;
            if (!QFile::exists(config.CAkey))
            {
                bool isOk = false;
                QStringList container = checkContainers();
                QRegExp regexp_cont(RegexpPatternWorkDir::Container);
                foreach (QString cont, container)
                {
                    regexp_cont.indexIn(cont, 0);
                    if (config.CAkey == regexp_cont.cap(1))
                    {
                        isOk = true;
                        break;
                    }
                }
                if (!isOk)
                {
                    setErrorString("Ключ сертификата УЦ не найден: " + config.CAkey);
                    config.isOk = false;
                    break;
                }
            }
        }
        else if (term == "CAcert")
        {
            config.CAcert = define;
            if (!QFile::exists(config.CAcert))
            {
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
    {
        return config;
    }
}

void WorkDir::saveConfig(Config config)
{
    QFile file_config(config.name);
    if (!file_config.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream stream_config(&file_config);
    stream_config << "csptest = " << config.csptest << '\n';
    stream_config << "openssl = " << config.openssl << '\n';
    stream_config << "CAcert = " << config.CAcert << '\n';
    stream_config << "CAkey = " << config.CAkey << '\n';
    file_config.close();
 }

BOOL_ERR WorkDir::loadCaCert(Config config, CACert &ca_cert)
{
    BOOL_ERR rc = FAIL;
    rc = DbTable::getCNFromCert(ca_cert.file_name, ca_cert.CN);
    if (!rc)
    {
        setErrorString("getCNFrom");
        return FAIL;
    }
    ca_cert.file_name = config.CAcert;
    ca_cert.file_key = config.CAkey;
    ca_cert.key = config.CAkey;
    return OK;
 }

BOOL_ERR WorkDir::exportCert(QString CN, QString file_name)
{
    BOOL_ERR rc = FAIL;
    DbTable table_cert;
    QFile file_cert(file_name);
    if (!file_cert.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        setErrorString("Error open");
        return FAIL;
    }
    QTextStream stream_cert(&file_cert);

    table_cert.table_name = "cert";
    table_cert.condition = "CN = '" + CN + "'";
    rc = data_base.loadFromDb(table_cert.table_name, table_cert.condition, data_base.query, table_cert);
    if (!rc)
    {
        setErrorString("Error loadFromDb");
        return FAIL;
    }
    stream_cert << table_cert.pem;
    file_cert.close();
}

DbTable WorkDir::importCert(QString file_name)
{
    DbTable table_cert;
    table_cert.isOk = true;
    if (!QFile::exists(file_name))
    {
        table_cert.isOk = false;
        table_cert.status = "не удалось открыть файл";
        return table_cert;
    }

    if ((table_cert.CN = table_cert.getCNFromCsr(file_name)) == "error")
    {
        if (table_cert.getCNFromCert(file_name, table_cert.CN) == FAIL)
        {
            table_cert.isOk = false;
            table_cert.status = "не удалось распознать CN";
            return table_cert;
        }
        // Работаем с подписанным сертификатом
        else
        {
            table_cert.serial = table_cert.getSerialFromCert(file_name);
            table_cert.issuer = table_cert.getIssuerFromCert(file_name);
            table_cert.revoke = "unknown";
        }
    }
    // Работем с CSR
    else
    {
        table_cert.serial = "no";
        table_cert.issuer = "no";
        table_cert.revoke = "no";
    }
    table_cert.pem = table_cert.getPemFromFile(file_name);
    table_cert.table_name = "cert";
    table_cert.days_valid = "no info";
    if (table_cert.serial == "error" || table_cert.issuer == "error" ||
        table_cert.pem == "error" || table_cert.revoke == "error")
    {
        table_cert.isOk = false;
        table_cert.status = "не удалось импортировать сертификат";
        return table_cert;
    }
    data_base.saveToDb(table_cert, data_base.query);
    return table_cert;
}

// Генерация шаблона сертификата в конфиг
BOOL_ERR WorkDir::genCertConfig(DbTable &table, BOOL mod)
{
    // Сначала проверим и заполним req_ext, а сам конфиг заполним в конце,
    // основываясь на состоянии req_ext
    QString req_ext = QString("[ req_ext ]") + "\n";
    BOOL req_ext_exist = 0;
    if (!table.cert_extension.basicConstraints.isEmpty())
    {
        req_ext +=
            QString("basicConstraints = " + table.cert_extension.basicConstraints) + "\n";
        req_ext_exist = 1;
    }

    if (!table.cert_extension.authorityKeyIdentifier.isEmpty())
    {
        req_ext +=
            QString("authorityKeyIdentifier = " + table.cert_extension.authorityKeyIdentifier) + "\n";
        req_ext_exist = 1;
    }

    if (!table.cert_extension.subjectKeyIdentifier.isEmpty())
    {
        req_ext +=
            QString("subjectKeyIdentifier = " + table.cert_extension.subjectKeyIdentifier) + "\n";
        req_ext_exist = 1;
    }

    if (!table.cert_extension.keyUsage.isEmpty())
    {
        req_ext +=
            QString("keyUsage = " + table.cert_extension.keyUsage) + "\n";
        req_ext_exist = 1;
    }

    if (!table.cert_extension.nsCertType.isEmpty())
    {
        req_ext +=
            QString("nsCertType = " + table.cert_extension.nsCertType) + "\n";
        req_ext_exist = 1;
    }

    if (!table.cert_extension.subjectAltName.isEmpty())
    {
        req_ext +=
            QString("subjectAltName = " + table.cert_extension.subjectAltName) + "\n";
        req_ext_exist = 1;
    }

    if (!table.cert_extension.extendedKeyUsage.isEmpty())
    {
        req_ext +=
            QString("extendedKeyUsage = " + table.cert_extension.extendedKeyUsage) + "\n";
        req_ext_exist = 1;
    }

    table.cert_extension.config = QString("oid_section = OIDs") + "\n"

    + QString("[ req ]") + "\n"
    + QString("prompt = no") + "\n"
    + QString("encrypt_key = no") + "\n"
    + QString("distinguished_name = dn") + "\n";
    if (req_ext_exist)
    {
        if (mod == GEN_CSR)
        {
            table.cert_extension.config += QString("req_extensions = req_ext") + "\n";
        }
        else if (mod == SIGN_CSR || mod == SELF_SIGNED)
        {
            table.cert_extension.config += QString("x509_extensions = req_ext") + "\n";
        }
    }

    table.cert_extension.config += QString("[ OIDs ]") + "\n"
    + QString("# MySensationalOID=1.2.3.45") + "\n"
    + QString("# MyOutstandingOID=2.3.4.56") + "\n"

    + QString("[ dn ]") + "\n"
    + QString("CN = " + table.CN) + "\n"
    + QString("emailAddress = " + table.email) + "\n"
    + QString("L = " + table.L) + "\n"
    + QString("O = " + table.O) + "\n"
    + QString("C = " + table.C) + "\n"
    + QString("# MySensationalOID = Support Department") + "\n";

    if (req_ext_exist)
    {
        table.cert_extension.config += req_ext;
    }

    return OK;
}

// Сохранине конфига в table
BOOL_ERR WorkDir::saveCertConfigToFile(DbTable table)
{
    return table.cert_extension.writeAllToFile(files.cert_config, table.cert_extension.config);
}

// Удаление шаблона сертификата (конфига)
BOOL_ERR WorkDir::delCertConfigFile()
{
    Program::removeFile(files.cert_config);
    return OK;
}

BOOL_ERR WorkDir::checkOpenssl()
{
    if (QFile::exists(OPENSSL_DIR_PATH + OPENSSL_EXE) )
    {
        return OK;
    }
    else
    {
        return FAIL;
    }
}

