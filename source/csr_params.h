#ifndef CSR_PARAMS_H
#define CSR_PARAMS_H

#include <QDialog>
#include "classes.h"
#include "work_dir.h"

namespace Ui {
class CsrParams;
}

class CsrParams : public QDialog
{
    Q_OBJECT

public:
    explicit CsrParams(QWidget *parent = 0);
    ~CsrParams();

    Program prog;
    WorkDir work_dir;
    void initialise();
    void setParams();
signals:
    void readyRunProgram(Program prog, DbTable table);

private slots:
    void on_cancelBtn_clicked();
    void on_creatBtn_clicked();
    void on_nameCsrEdit_textChanged();
    void on_containersBox_currentIndexChanged(const QString &arg1);
    void getData(WorkDir work_dir);

private:
    Ui::CsrParams *ui_cp;
};

#endif // CSR_PARAMS_H
