#ifndef DB_H
#define DB_H

#include "classes.h"

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
    QString subj;
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
    QStringList checkErrorFields();
    static QString getPemFromFile(QString file_name);
    static QString getTextFromCsr(QString file_in);
    static QString getTextFromCert(QString file_in);
    static QString getTextFromAny(QString file_in);
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
    void saveToDb(DbTable table, QSqlQuery *query);
};

class CAConfig {
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

class CAcert {
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
    QString openssl_config_file = "openssl.cnf";
};

class WorkDir {
public:
    WorkDir();
    WorkDir(QString work_path);
    QString name;
    QString work_path;
    QString crlnumber;
    QString index;
    CAConfig config;
    CAcert ca_cert;
    WorkDirFiles files;
    DataBase data_base;
    QString openssl_config;
    bool isOpenMod;
    bool isOk;
    QString newWorkDir();
    QString initialiseWorkDir();
    QString initialiseConfig();
    QString initialiseDatabase();
    QStringList parsingContainers(QString info);
    QStringList checkContainers();
    CAConfig loadConfig(QString file_name);
    void saveConfig(CAConfig config);
    CAcert loadCaCert(CAConfig config);
    DbTable exportCert(QString CN, QString file_name);
    DbTable importCert(QString file_name);
};


#endif // DB_H
