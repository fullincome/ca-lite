#ifndef NEWDB_H
#define NEWDB_H

#include <QDialog>
#include "program.h"
#include "work_dir.h"

namespace Ui {
class DbDialog;
}

class DbDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbDialog(QWidget *parent = 0);
    ~DbDialog();

    QString dialog_mod;
    QString work_path;
    void setDialogMod(QString dialog_mod, QString data);

signals:
    void sendData(QString data);
private slots:
    //BUTTONS REACT
    void on_creatBtn_clicked();
    void on_cancelBtn_clicked();
    void on_textEdit_textChanged();
    void on_fileDialogBtn_clicked();
    void getData(QString data);

private:

    Ui::DbDialog *ui_dl;
};

#endif // NEWDB_H
