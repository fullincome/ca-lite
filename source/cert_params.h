#ifndef CERT_PARAMS_H
#define CERT_PARAMS_H

#include <QDialog>
#include "program.h"
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

    void initialise(QString table_name);
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

    void on_authorityKeyIdentifierContBox_activated(const QString &arg1);

    void on_extendedKeyUsageContBox_activated(const QString &arg1);

    void on_basicConstraintsContBox_activated(const QString &arg1);

    void on_subjectKeyIdentifierContBox_activated(const QString &arg1);

    void on_keyUsageContBox_activated(const QString &arg1);

    void on_nsCertTypeContBox_activated(const QString &arg1);

    void on_subjectAltNameContBox_activated(const QString &arg1);

    void on_keycontCertEdit_textChanged();

private:
    Ui::CertParams *ui_cp;
};

#endif // CERT_PARAMS_H
