#ifndef DB_H
#define DB_H

#include "program.h"

// Регулярки
namespace RegexpPatternWorkDir {
    const QString CN = "Subject: CN = (\\w+)";
    const QString Issuer = "Issuer: CN = (\\w+)";
    const QString Serial = "Serial Number:[\\r\\n ]+([\\w: \\(\\)]+)\\r?\\n";
    const QString OpensslConfCADir = "(\\[ CA_default \\]\\s+dir\\s+=\\s+)([^\\t\\r\\n]+)";
    const QString ContainerLine = "\\r*\\n +(\\d+) +(\\w+)\\r*\\n";
    const QString SkipCsptestOutputWords = "___+\\r*\\n";
    const QString ConfigLine = "(\\w+)\\s*=\\s*([^\\s][^\\r\\n]+)\\s*";
    const QString OpensslExtension = "(\\[ req_ext \\])";
    // Выбор из parsed контейнеров (by ContainerLine)
    const QString Container = "Name: (\\w+) \\(len = \\d+\\)";
}

#define SIGN_CSR 0
#define GEN_CSR 1
#define SELF_SIGNED 2

class CertExt {
public:
    QString config;
    QString basicConstraints;
    QString authorityKeyIdentifier;
    QString subjectKeyIdentifier;
    QString keyUsage;
    QString nsCertType;
    QString subjectAltName;
    QString extendedKeyUsage;
    CertExt();
    QString readAllFromFile(QString file_name);
    BOOL_ERR writeAllToFile(QString file_name, QString text);
};

class DbTable {
public:
    QString id;
    QString CN;
    QString L;
    QString O;
    QString C;
    QString email;
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
    CertExt cert_extension;
    bool isOk;
    DbTable();
    QStringList checkErrorFields();
    static BOOL existExtension(QString config);
    static QString getPemFromFile(QString file_name);
    static QString getTextFromCsr(QString file_in);
    static BOOL_ERR getTextFromCert(QString file_in, QString &cert_info);
    static QString getTextFromAny(QString file_in, QString work_path);
    static QString getCNFromCsr(QString file);
    static BOOL_ERR getCNFromCert(QString file, QString &CN);
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
    BOOL_ERR newTables(QSqlQuery *query);
    BOOL_ERR loadFromDb(QString table_name, QString table_condition, QSqlQuery *query, DbTable &table);
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
    QString ca_cert_file;
    QString srl_ca_cert_file;
    QString key_ca_cert_file;

    QString export_cert_file;
    QString import_cert_file;

    QString csr_file;
    QString cert_file;
    QString signing_file;

    QString key_csr_file;
    QString key_signing_file;
    QString key_cert_file;

    QString config_file;
    QString openssl_config;
    QString cert_config;

    QString crl;
    QString crlnumber;
    QString index;
    WorkDirFiles();
    WorkDirFiles(QString path);
    QStringList getVariableFiles();
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
    BOOL_ERR loadCaCert(Config config, CACert &ca_cert);
    BOOL_ERR exportCert(QString CN, QString file_name);
    BOOL_ERR importCert(QString file_name, DbTable &table);
    BOOL_ERR genCertConfig(DbTable &table, BOOL mod);
    BOOL_ERR saveCertConfigToFile(DbTable table);
    BOOL_ERR delCertConfigFile();
    BOOL_ERR checkOpenssl();
};


#endif // DB_H
