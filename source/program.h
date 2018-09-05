﻿#ifndef PROGRAM_H
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
#define OPENSSL_DIR_PATH "/opt/cprocsp/cp-openssl-1.1.0/bin/amd64/"
#define OPENSSL_CONFIG_PATH "/var/opt/cprocsp/cp-openssl-1.1.0/"
#define CRYPTOPRO_DIR_PATH "/opt/cprocsp/bin/amd64/"

#define OPENSSL_URL "https://update.cryptopro.ru/support/nginx-gost/bin/180423/"
#define OPENSSL_X64_DEB "cprocsp-cpopenssl-110-64_5.0.11216-5_amd64.deb"
#define OPENSSL_BASE_DEB "cprocsp-cpopenssl-110-base_5.0.11216-5_all.deb"
#define OPENSSL_DEVEL_DEB "cprocsp-cpopenssl-110-devel_5.0.11216-5_all.deb"
#define OPENSSL_GOST_DEB "cprocsp-cpopenssl-110-gost-64_5.0.11216-5_amd64.deb"

#else
#ifdef Q_OS_WIN32
#define OPENSSL_DIR_PATH "c:\OpenSSL-Win32\bin\openssl.exe???"
#define OPENSSL_CONFIG_PATH "c:/openssl.cnf???"
#define CRYPTOPRO_DIR_PATH "c:/opt/cprocsp/bin/amd64/csptest???"
#endif
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
    QString ca_cert_filename = "ca_cert.cer";
    QString csr_filename = "csr.csr";
    QString signing_filename = "signing_cert.cer";
    QString cert_filename = "cert.cer";
    QString srl_ca_cert_filename = "ca_cert.srl";
    QString key_ca_cert_filename = "ca_cert.key";
    QString key_csr_filename = "csr.key";
    QString key_signing_filename = "signing_cert.key";
    QString program_path;
    QString program_name;
    QString work_path;
    QString file_in;
    QString file_out;
    QString key_in;
    QString key_out;
    QStringList args;
    QString mod;
    QString suite;
    int isError;
    QString output;
    BOOL_ERR run();
    void clearResult(QStringList file_to_delete);
    void clearResult();
    static void removeFile(QString file);
};

#endif // PROGRAM_H
