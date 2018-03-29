#include "classes.h"

Program::Program () {}
Program::Program (QString prog_name, QString mod, QString work_path) {
    if (prog_name == "openssl") {
        program_path = "/opt/cprocsp/cp-openssl-1.1.0/bin/amd64/";
        program_name = "openssl";
        this->work_path = work_path;
        file_in = "no";
        file_out = "no";
        key_in = "no";
        key_out = "no";
        this->mod = "no";
        args << "version";
        if (mod == "csr") {
            file_out = work_path + csr_filename;
            key_out = work_path + key_csr_filename;
            this->mod = "csr";
        } else if (mod == "ca_cert") {
            file_out = work_path + ca_cert_filename;
            key_out = work_path + key_ca_cert_filename;
            this->mod = "ca_cert";
        } else if (mod == "signing_cert") {
            file_in = work_path + csr_filename;
            file_out = work_path + signing_filename;
            key_in = work_path + key_ca_cert_filename;
            key_out = work_path + key_signing_filename;
            this->mod = "signing_cert";
        } else if (mod == "revoke_cert") {
        }
    }
}

Program::Program (QString prog_name, QString mod) {
    if (prog_name == "openssl") {
        program_path = "/opt/cprocsp/cp-openssl-1.1.0/bin/amd64/";
        program_name = "openssl";
        work_path = "no";
        file_in = "no";
        file_out = "no";
        key_in = "no";
        key_out = "no";
        this->mod = "no";
        args << "version";
        if (mod == "cert") {};
        if (mod == "csr") {
            file_out = work_path + csr_filename;
            key_out = work_path + key_csr_filename;
            this->mod = "csr";
        } else if (mod == "ca_cert") {
            file_out = work_path + ca_cert_filename;
            key_out = work_path + key_ca_cert_filename;
            this->mod = "ca_cert";
        } else if (mod == "signing_cert") {
            file_in = work_path + csr_filename;
            file_out = work_path + signing_filename;
            key_in = work_path + key_ca_cert_filename;
            key_out = work_path + key_signing_filename;
            this->mod = "signing_cert";
        } else if (mod == "revoke_cert") {
        }
    }
}

void Program::run() {
   QProcess *proc = new QProcess();
   proc->setEnvironment(QString("OPENSSL_CONF=" + work_path + "openssl.cnf").split(" "));
   proc->start(program_path + program_name, args);
   proc->waitForReadyRead();
   isError = proc->exitCode();
   output = QString(program_path + program_name + " " + args.join(" ") + ": " + "\n\n");
   //output += QString(proc->readAll());
   output += QString(proc->readAllStandardOutput()) + QString(proc->readAllStandardError());
}

void Program::removeFile(QString file_name) {
    QFile file(file_name);
    file.remove();
}

void Program::clearResult(QStringList file_to_delete) {
    foreach (QString file, file_to_delete) {
        if (QFile::exists(file)) removeFile(file);
    }
    if (QFile::exists(work_path + csr_filename)) removeFile(work_path + csr_filename);
    if (QFile::exists(work_path + cert_filename)) removeFile(work_path + csr_filename);
    if (QFile::exists(work_path + signing_filename)) removeFile(work_path + signing_filename);
    if (QFile::exists(work_path + key_ca_cert_filename)) removeFile(work_path + key_ca_cert_filename);
    if (QFile::exists(work_path + key_csr_filename)) removeFile(work_path + key_csr_filename);
    if (QFile::exists(work_path + key_signing_filename)) removeFile(work_path + key_signing_filename);
}

void Program::clearResult() {
    if (QFile::exists(work_path + csr_filename)) removeFile(work_path + csr_filename);
    if (QFile::exists(work_path + cert_filename)) removeFile(work_path + csr_filename);
    if (QFile::exists(work_path + signing_filename)) removeFile(work_path + signing_filename);
    if (QFile::exists(work_path + key_ca_cert_filename)) removeFile(work_path + key_ca_cert_filename);
    if (QFile::exists(work_path + key_csr_filename)) removeFile(work_path + key_csr_filename);
    if (QFile::exists(work_path + key_signing_filename)) removeFile(work_path + key_signing_filename);
}
