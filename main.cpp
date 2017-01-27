#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QProcess>

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>

using namespace std;

pid_t proc_find(const char* name)
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}


#ifdef __linux__
bool isRunning(const QString &process) {
    QProcess tasklist;
    QStringList args;
    args << process;
    tasklist.start("pgrep -c ", args);
    tasklist.waitForFinished();
   // tasklist.readpgrep
    QString output = tasklist.readAllStandardOutput();
    qDebug() << output;
    return output.length();
}
#else
bool isRunning(const QString &process) {
    QProcess tasklist;
    tasklist.start(
                "tasklist",
                QStringList() << "/NH"
                << "/FO" << "CSV"
                << "/FI" << QString("IMAGENAME eq %1").arg(process));
    tasklist.waitForFinished();
    return output.length();
}
#endif


int main(int argc, char *argv[])
{

    //qDebug() << QString(argv[0]).split('/').last();
    //qDebug() << QString(argv[0]);
//qDebug() << "my dupa: " << proc_find(argv[0]);
    isRunning(QString(argv[0]).split('/').last());
//    {
//        return 0;
//    }


    QApplication::addLibraryPath(("plugins/"));
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Prywatyny pomiar czasu pracy ZMT");
    w.show();
  //  w.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowTitleHint);







    // QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(close()));


    return a.exec();
}


