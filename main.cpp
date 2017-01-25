#include "mainwindow.h"
#include <QApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
    using namespace std;
    QApplication::addLibraryPath(("plugins/"));
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Prywatyny pomiar czasu pracy ZMT");
    w.show();



   w.setVisible(false);

  // QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(close()));


    return a.exec();
}

