#include "program.h"

ErrorStr errorStr;

void setErrorString(QString str)
{
    return errorStr.setError(str);
}

QString getLastErrorString()
{
    return errorStr.getLastError();
}

Program::Program () {}
Program::Program (QString prog_name, QString mod, QString work_path)
{
    if (prog_name == "openssl")
    {
        program_path = OPENSSL_DIR_PATH;
        program_name = "openssl";
        this->work_path = work_path;
        this->mod = "no";
        args << "version";
        if (mod == "csr")
        {
            this->mod = "csr";
        }
        else if (mod == "ca")
        {
            this->mod = "ca";
        }
        else if (mod == "signing_cert")
        {
            this->mod = "signing_cert";
        }
        else if (mod == "revoke_cert")
        {
        }
    }
    else if (prog_name == "curl")
    {
#if defined(Q_OS_UNIX)
        program_path = "";
        program_name = "wget";
#endif
        this->work_path = work_path;
    }
}

Program::Program (QString prog_name, QString mod)
{
    if (prog_name == "openssl")
    {
        program_path = "/opt/cprocsp/cp-openssl-1.1.0/bin/amd64/";
        program_name = "openssl";
        work_path = QCoreApplication::applicationDirPath() + "/";
        this->mod = "no";
        args << "version";
        if (mod == "cert") {};
        if (mod == "csr")
        {
            this->mod = "csr";
        }
        else if (mod == "ca")
        {
            this->mod = "ca";
        }
        else if (mod == "signing_cert")
        {
            this->mod = "signing_cert";
        }
        else if (mod == "revoke_cert")
        {
        }
    }
    else if (prog_name == "curl")
    {
#if defined(Q_OS_UNIX)
        program_path = "";
        program_name = "wget";
#endif
        this->work_path = work_path;
    }
    else if (prog_name == "install")
    {
#if defined(Q_OS_UNIX)
        program_path = "";
        program_name = "bash";
        args.push_back("-c");
        args.push_back("echo '" + mod + "' | sudo --user=root -S dpkg -i ");
#endif
    }
}

Program::Program (QString prog_name)
{
    if (prog_name == "curl")
    {
#if defined(Q_OS_UNIX)
        program_path = "";
        program_name = "wget";
#endif
    }
}

BOOL_ERR Program::run()
{
   isError = 0;
   QProcess *proc = new QProcess();
   proc->setEnvironment(QString("OPENSSL_CONF=" + work_path + "openssl.cnf").split(" "));
   proc->start(program_path + program_name, args);
   if (!proc->waitForStarted())
   {
       //return FAIL;
   }
   if (!proc->waitForFinished())
   {
       //return FAIL;
   }
   isError = proc->exitCode();
   output = QString(program_path + program_name + " " + args.join(" ") + ": " + "\n\n");
   //output += QString(proc->readAll());
   output += QString("\nAllStandardOutput:\n") + QString(proc->readAllStandardOutput());
   output += QString("\nAllStandardError:\n") +QString(proc->readAllStandardError());
   if (isError)
   {
       output += "\nProcess error:\n" + proc->errorString() + "\n";
   }
}

void Program::removeFile(QString file_name)
{
    QFile file(file_name);
    file.remove();
}

void Program::clearResult(QStringList files)
{
    foreach (QString file, files_to_delete)
    {
        if (QFile::exists(file)) removeFile(file);
    }

    foreach (QString file, files)
    {
        if (QFile::exists(file)) removeFile(file);
    }
}

void Program::clearResult()
{
    foreach (QString file, files_to_delete)
    {
        if (QFile::exists(file)) removeFile(file);
    }
}
