#ifndef PROGRAM_H
#define PROGRAM_H

#include <QString>
#include <QStringList>
#include <QStack>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QRegExp>
#include <QProcess>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <QSqlError>
#include <QListView>
#include <QTableView>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGlobal>


typedef long long BOOL_ERR;
typedef BOOL_ERR BOOL;
#define TRUE 1
#define FALSE 0
#define OK 1
#define FAIL 0

#define CRT_GOST 0
#define CRT_RSA 1

#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
#define OPENSSL_DIR_PATH QString("/opt/cprocsp/cp-openssl-1.1.0/bin/amd64/")
#define OPENSSL_CONFIG_PATH QString("/var/opt/cprocsp/cp-openssl-1.1.0/")
#define OPENSSL_CONFIG QString("openssl.cfg")
#define OPENSSL_EXE QString("openssl")
#define CRYPTOPRO_DIR_PATH QString("/opt/cprocsp/bin/amd64/")

#define OPENSSL_URL QString("https://update.cryptopro.ru/support/nginx-gost/bin/180423/")
#define OPENSSL_X64_DEB QString("cprocsp-cpopenssl-110-64_5.0.11216-5_amd64.deb")
#define OPENSSL_BASE_DEB QString("cprocsp-cpopenssl-110-base_5.0.11216-5_all.deb")
#define OPENSSL_DEVEL_DEB QString("cprocsp-cpopenssl-110-devel_5.0.11216-5_all.deb")
#define OPENSSL_GOST_DEB QString("cprocsp-cpopenssl-110-gost-64_5.0.11216-5_amd64.deb")

#elif defined(Q_OS_WIN)
#define OPENSSL_DIR_PATH QString("C:\\OpenSSL-Win64\\gost\\")
#define OPENSSL_CONFIG_PATH QString("C:\\OpenSSL-Win64\\gost\\")
#define CRYPTOPRO_DIR_PATH QString("C:\\Program Files\\Crypto Pro\\CSP\\")

#define OPENSSL_URL QString("http://37.230.116.109/openssl-1.1.0i-gost-0.17/bin64/")
#define OPENSSL_X64_LIBCRYPTO QString("libcrypto-1_1-x64.dll")
#define OPENSSL_X64_LIBSSL QString("libssl-1_1-x64.dll")
#define OPENSSL_X64_LIBGOSTENGY QString("gostengy.dll")
#define OPENSSL_X64_EXE QString("openssl.exe")
#define OPENSSL_CONFIG "openssl.cfg"
#define OPENSSL_EXE OPENSSL_X64_EXE
#endif

extern class ErrorStr {
    QStack<QString> stack;
public:
    bool empty()
    {
        return stack.empty();
    }
    void setError(QString str)
    {
        stack.push_back(str);
    }
    QString getLastError()
    {
        if (empty())
        {
            return "getLastErrorString() is empty";
        }
        else
        {
            QString str;
            while (!empty())
            {
                str += stack.pop();
            }
            return str;
        }
    }
} errorStr;

extern void setErrorString(QString str);
extern QString getLastErrorString();

//extern void setErrorStringasdf(QString str) {
//    errorStr.setError(str);
//}


class Program {
public:
    Program(QString prog_name, QString mod, QString work_path);
    Program(QString prog_name, QString mod);
    Program(QString prog_name);
    Program();
    QStringList files_to_delete;
    QString program_path;
    QString program_name;
    QString work_path;
    QStringList args;
    QString mod;
    QString suite;
    int isError;
    QString output;
    BOOL_ERR run();
    void clearResult(QStringList files);
    void clearResult();
    static void removeFile(QString file);
};

#endif // PROGRAM_H
