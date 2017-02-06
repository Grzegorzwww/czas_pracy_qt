#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QProcess>

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include "singleapplication.h"


using namespace std;



int main(int argc, char *argv[])
{
    QApplication::setDesktopSettingsAware(false);

    QCoreApplication::setOrganizationName("grzegorz_warchol_rcp");
    QCoreApplication::setApplicationName("mrcp");

    SingleApplication a(argc, argv);
    MainWindow w;

    QObject::connect(&a, &SingleApplication::instanceStarted, &w, &MainWindow::raise);

    w.setWindowTitle("Prywatyny pomiar czasu pracy ZMT");
    w.show();

    return a.exec();
}


