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

    DbTable table;
    QStringList conts_info;
    void initialise();
    void setParams();

signals:
    void readyToCheck(DbTable table);
private slots:
    void on_cancelBtn_clicked();
    void on_creatBtn_clicked();
    void on_nameCertEdit_textChanged();
    void on_containersBox_currentIndexChanged(const QString &arg1);
    void getData(QStringList conts_info);
    void closeWindow(QString rc);

private:
    Ui::CertParams *ui_cp;
};

#endif // CERT_CREAT_H
