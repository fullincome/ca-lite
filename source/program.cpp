#include "program.h"

ErrorStr errorStr;

void setErrorString(QString str, int error_type)
{
    if (error_type == ERR_OPEN_FILE)
    {
        errorStr.setError("Fail open file: " + str);
    }
    else if (error_type == ERR_EXIST_FILE)
    {
        errorStr.setError("File not exist: " + str);
    }
    else if (error_type == ERR_STRING)
    {
        errorStr.setError(str);
    }
}

QString getErrorString()
{
    return errorStr.getLastError();
}

Program::Program () {}

Program::Program (QString prog_name, QString work_path,
                  int user, QString password)
{
    _work_path = work_path;
    if (prog_name == "openssl")
    {
        program_path = OPENSSL_DIR_PATH;
        program_name = OPENSSL_EXE;
    }
#if defined(Q_OS_WIN)
    else if(prog_name == "powershell")
    {
        program_path = "C:/Windows/system32/WindowsPowerShell/v1.0/";
        program_name = "powershell.exe";
    }
#endif
#if defined(Q_OS_UNIX)
    else if (prog_name == "wget")
    {
        program_path = "";
        program_name = "bash";
        args.push_back("-c");
        QString install_str;
        if (user == ROOT)
        {
            install_str += "echo '" + password + "' | sudo --user=root -S ";
        }
        install_str += "wget ";
        args.push_back(install_str);
    }
    else if (prog_name == "install" || prog_name == "dpkg")
    {
        program_path = "";
        program_name = "bash";
        args.push_back("-c");
        QString install_str;
        if (user == ROOT)
        {
            install_str += "echo '" + password + "' | sudo --user=root -S ";
        }
        install_str += "dpkg -i ";
        args.push_back(install_str);
    }
#endif
    else
    {
        program_path = "";
        program_name = prog_name;
    }
}

BOOL_ERR Program::run()
{
   isError = 0;
   QProcess *proc = new QProcess();
   proc->setEnvironment(QString("OPENSSL_CONF=" + _work_path + OPENSSL_CONFIG).split("\n"));
   proc->start(program_path + program_name, args);
   if (!proc->waitForStarted())
   {
       output += "\nProcess error:\n" + proc->errorString() + "\n";
       isError = true;
       return FAIL;
   }
   if (!proc->waitForFinished())
   {
       output += "\nProcess error:\n" + proc->errorString() + "\n";
       isError = true;
       return FAIL;
   }
   isError = proc->exitCode();
   output += QString(program_path + program_name + " " + args.join(" ") + ": " + "\n\n");
   output += QString("\nAllStandardOutput:\n") + QString(proc->readAllStandardOutput());
   output += QString("\nAllStandardError:\n") +QString(proc->readAllStandardError());
   if (isError)
   {
       output += "\nProcess error:\n" + proc->errorString() + "\n";
       return FAIL;
   }
   return OK;
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
