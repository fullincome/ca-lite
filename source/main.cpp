#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QtGlobal>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
#endif

    QApplication a(argc, argv);

    MainWindow w;
    w.initialize();
    if (w.getWMod() == WINDOW_MOD)
        w.show();

    return a.exec();
}
