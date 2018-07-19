#ifndef DB_H
#define DB_H

#include "program.h"

// Регулярки
namespace RegexpPatternWorkDir {
    const QString CN = "Subject: CN = (\\w+)\n";
    const QString Issuer = "Issuer: CN = (\\w+)\n";
    const QString Serial = "Serial Number:[\n ]+([\\w: \\(\\)]+)\n";
    const QString OpensslConfCADir = "(\\[ CA_default \\]\\s+dir\\s+=\\s+)([^\\t\\n]+)";
    const QString ContainerLine = "\n +(\\d+) +(\\w+)\n";
    const QString SkipCsptestOutputWords = "___+\n";
    const QString ConfigLine = "(\\w+)\\s*=\\s*([^\\n\\s]+)";
    // Выбор из parsed контейнеров (by ContainerLine)
    const QString Container = "Name: (\\w+) \\(len = \\d+\\)";
}

class DbTable {
public:
    QString CN;
    QString O;
    QString C;
    QString email;
    QString sun;
    QString subj;
    QString days_valid;
    QString key;
    QString pem;
    QString table_name;
    QString suite;
    QString serial;
    QString revoke;
    QString issuer;
    QString condition;
    QString status;
    bool isOk;
    DbTable();
    QStringList checkErrorFields();
    static QString getPemFromFile(QString file_name);
    static QString getTextFromCsr(QString file_in);
    static BOOL_ERR getTextFromCert(QString file_in, QString &cert_info);
    static QString getTextFromAny(QString file_in, QString work_path);
    static QString getCNFromCsr(QString file);
    static QString getCNFromCert(QString file);
    static QString getIssuerFromCert(QString file);
    static QString creatSerialToFile(QString file_name);
    static QString getSerialFromCert(QString file_name);
    static QString getSerialFromFile(QString file_name);
};

class DataBase {
public:
    QString name;
    QSqlDatabase sql_db;
    QSqlQuery *query;
    DbTable table;
    bool isOk;

    bool load_db(QString name);
    void newTables(QSqlQuery *query);
    DbTable loadFromDb(QString table_name, QString table_condition, QSqlQuery *query);
    BOOL_ERR saveToDb(DbTable table, QSqlQuery *query);
};

class Config {
public:
    QString name;
    QString openssl;
    QString csptest;
    QString CAkey;
    QString CAcert;
    QString writeToConfigFile(QString file_name, QString data);
    QString readAllFromConfigFile(QString file_name);
    bool isOk;
};

class CACert {
public:
    QString file_name;
    QString file_key;
    QString key;
    QString CN;
};

class WorkDirFiles {
public:
    QString ca_cert_file = "ca_cert.cer";
    QString srl_ca_cert_file = "ca_cert.srl";
    QString key_ca_cert_file = "ca_cert.key";

    QString export_cert_file = "export_cert.cer";
    QString import_cert_file = "import_cert.cer";

    QString csr_file = "csr.csr";
    QString cert_file = "cert.cer";
    QString signing_file = "signing_cert.cer";

    QString key_csr_file = "csr.key";
    QString key_signing_file = "signing_cert.key";
    QString key_cert_file = "signing_cert.key";

    QString config_file = "config";
    QString openssl_config = "openssl_calite.cnf";
    QString cert_config = "openssl_cert.cnf";

    QString crlnumber = "crlnumber";
    QString index = "index.txt";
};

class WorkDir {
public:
    WorkDir();
    WorkDir(QString work_path);
    QString work_path;
    Config config;
    CACert ca_cert;
    WorkDirFiles files;
    DataBase data_base;
    bool isOpenMod;
    bool isOk;
    BOOL_ERR newWorkDir();
    BOOL_ERR initialiseWorkDir();
    BOOL_ERR initialiseConfig();
    BOOL_ERR initialiseDatabase();
    QStringList parsingContainers(QString info);
    QStringList checkContainers();
    Config loadConfig(QString file_name);
    void saveConfig(Config config);
    CACert loadCaCert(Config config);
    DbTable exportCert(QString CN, QString file_name);
    DbTable importCert(QString file_name);
    BOOL_ERR genCertConfig(DbTable table);
    BOOL_ERR delCertConfig();
};


#endif // DB_H
