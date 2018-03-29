#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "classes.h"
#include "dialog.h"
#include "ca_cert_params.h"
#include "csr_params.h"
#include "work_dir.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


#ifdef QT_DEBUG
    bool isDebug = true;
    bool isRelease = false;
#else
    bool isDebug = false;
    bool isRelease = true;
#endif

    Program program;
    WorkDir work_dir;
    void disableBtn();
    void enableBtn();
    bool prepareSaveToDb(Program prog);
    bool prepareSaveToDb(Program prog, DbTable table);
    void updateView(DataBase data_base);
    void setSelectedName(QString &select_name, QTableView *list_view);
    bool checkConfig(QString file_name);

    void messageDebug(QString message);
    void messageDebugUpdate(QString messageUpdate);
    void messageError(QWidget *window, QString message);
    void messageWarning(QWidget *window, QString message);
    void messageSuccess(QWidget *window, QString message);
signals:
    void sendData(QStringList data_list);
    void sendData(WorkDir work_dir);
    void sendData(QString data);
private slots:
    void generateCert(Program prog, DbTable table);
    void checkWorkDir(WorkDir work_dir);
    void saveConfig(QString data);
    void getData(QString data);

    //BUTTONS REACT
    void on_creatCaCertBtn_clicked();
    void on_editConfigBtn_clicked();
    void on_openWorkDirBtn_clicked();
    void on_creatCrlBtn_clicked();
    void on_creatCsrBtn_clicked();
    void on_revokeCertBtn_clicked();
    void on_signCsrBtn_clicked();
    void on_exportBtn_clicked();
    void on_importBtn_clicked();

    void on_debugCheckContainersBtn_clicked();

    void on_certTableView_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void on_certTableView_activated(const QModelIndex &index);



private:
    Ui::MainWindow *ui_mw;
};

#endif // MAINWINDOW_H
