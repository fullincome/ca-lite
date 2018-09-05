#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QInputDialog>
#include "program.h"
#include "dialog.h"
#include "cert_params.h"
#include "work_dir.h"


#define TERMINAL_MOD 0
#define WINDOW_MOD 1

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

    bool initialize();
    int getWMod();
    bool setWMod(int mod);

    Program program;
    WorkDir work_dir;
    void initWindow();
    void disableBtn();
    void enableBtn();
    bool prepareSaveToDb(Program prog);
    BOOL_ERR prepareSaveToDb(Program prog, DbTable table);
    void updateView(DataBase data_base);
    void setSelectedName(QString field, QString &select_name, QTableView *list_view);
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
    void sendData(QString data1, QString data2);
    void closeCertParam(QString rc);
private slots:
    BOOL_ERR generateCert(Program prog, DbTable table);
    void checkWorkDir(WorkDir work_dir);
    void checkCertParam(DbTable table);
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

    void on_installOpensslBtn_clicked();

private:
    Ui::MainWindow *ui_mw;
    int w_mod;
};

#endif // MAINWINDOW_H
