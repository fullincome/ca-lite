#ifndef CLASSES_H
#define CLASSES_H

#include <QString>
#include <QStringList>
#include <QStack>
#include <QDir>
#include <QFile>
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


extern class ErrorStr {
    QStack<QString> stack;
public:
    void setError(QString str) {
        stack.push_back(str);
    }
    QString getLastError() {
        return stack.pop();
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
    void run();
    void clearResult(QStringList file_to_delete);
    void clearResult();
    void removeFile(QString file);
};

#endif // CLASSES_H
