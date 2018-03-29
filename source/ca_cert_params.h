#ifndef CERT_CREAT_H
#define CERT_CREAT_H

#include <QDialog>
#include "classes.h"
#include "work_dir.h"

namespace Ui {
class CertParams;
}

class CertParams : public QDialog
{
    Q_OBJECT

public:
    explicit CertParams(QWidget *parent = 0);
    ~CertParams();

    Program prog;
    WorkDir work_dir;
    void initialise();
    void setParams();

signals:
    void readyRunProgram(Program prog, DbTable table);

private slots:
    void on_cancelBtn_clicked();
    void on_creatBtn_clicked();
    void on_nameCertEdit_textChanged();
    void on_containersBox_currentIndexChanged(const QString &arg1);
    void getData(WorkDir work_dir);

private:
    Ui::CertParams *ui_cp;
};

#endif // CERT_CREAT_H
