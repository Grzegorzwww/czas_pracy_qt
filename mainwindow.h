#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QInputDialog>
#include <QSound>
#include <QDir>
#include <QCloseEvent>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/stat.h>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QElapsedTimer>
#include <QPainter>
#include <QGuiApplication>
#include <QString>




const QString WLACZENIE_APLIKACJI = "uruchomiona_aplikacja";

const QString GRUPA_USTAWIEN = "settings_first_fun_group";
const QString PIERWSZE_WLACZENIE_KOMPUTERA = "first_tun_date_and_time";
const QString DZIEN_OSTATNIEJ_MODYFIKACJI = "last_moydifed_date_and_time";
const QString OSTATNIE_WYLACZENIE_KOMPUTERA = "last_turnoff_computer_time";

const QString FLAG_TO_LET_USE_PATH = "flag_to_let_use_path";
const QString PATH_TO_SAVE_FILE = "path_to_save_log";

const QString TIME_CBR_COMP_DELAY = "time_cbr_comp_delay";
const QString FLAG_CBR_COMP_DELAY = "flag_cbr_comp_delay";
const QString TIME_ZMT_COMP_DELAY = "time_zmt_comp_delay";
const QString FLAG_ZMT_COMP_DELAY = "flag_zmt_comp_delay";




const std::string FILE_NAME_PREFIX = "CzasPracy_";
const std::string FILE_NAME_SUFFIX = ".txt";

const int ILOSC_GODZIN_PRACY = 8;

const QTime CZAS_WEJSCIE_WLACZENIE_KOMP =  QTime(0,2,0);
const QTime CZAS_WEJSCIE_NA_ZAKLAD_WLACZENIE_KOMP =  QTime(0,5,40);
const QTime CZAS_PRACY = QTime(ILOSC_GODZIN_PRACY,0,0);




class TimeWorkingInfo
{
private:
    QTime system_uptime;
    QTime czas_wlaczenie_komputera;
    QTime czas_pracy_komputera;
    QElapsedTimer czas_pracy;
    QTime czas_przyjscia_do_pracy;
    QTime czas_wejsca_na_zaklad;
    QTime czas_opuszczenia_zakladu;
    QString time_format;

    QTime time_cbr_computer_delay;
    QTime time_zmt_computer_delay;




public:
    TimeWorkingInfo();
    ~TimeWorkingInfo() {}

    void setWlaczenieKomputera(const QTime & x) {czas_wlaczenie_komputera = x;}
    void setToCzasPracyKomputera(const QTime & x) {czas_pracy_komputera = x;}

    void setTimeCbrComp(QTime  t) {time_cbr_computer_delay = t;}
    void setTimeZmtComp(QTime  t) {time_zmt_computer_delay = t;}

    QTime getTimeCbrComp() {return  time_cbr_computer_delay ;}
    QTime getTimeZmtComp() {return  time_zmt_computer_delay ;}


    QTime getWlaczenieKomputera() {return czas_wlaczenie_komputera;}
    void setTimeFormat(const QString str) {time_format = str;}
    QTime getCzasPracyKomputera();
    QTime getCzasPracyKomputera(bool){ return czas_pracy_komputera;}
    QTime getCzasPrzebywaniaWpracy();
    QTime getCzasWejsciaNaZaklad();
    QTime getCzasWyjsciaZPRacy();

    QTime odejmij_czasy(QTime x, QTime y);
    QTime dodaj_czasy(QTime x, QTime y);

    QTime odejmij(QTime x, QTime y);
    QTime readLinuxSystemTime();


};




//#define NAZWA_PLIKU "log_file.txt"
#define SEPARATOR "|=========================================================================================|"
#define PRZESTRZEN"|                                                                                         |"
#define NAZWA     "|                                PLIK Z GODZINAMI                                         |"

#define MENU_FILE "|          DATA         |    GODZ WLACZENIA  |   GODZ WYLACZENIA   |  PRZEPRACOWANY CZAS  |"






#define PON 1
#define WT 2
#define SR 3
#define CZW 4
#define PT 5
#define SOBOTA 6
#define NIEDZIELA 7

#define JAN 1
#define FEB 2
#define MARC 3
#define APR 4
#define MAY 5
#define JUN 6
#define JUL 7
#define AUG 8
#define SEP 9
#define OCT 10
#define NOW 11
#define DEC 12


const int timer_period_ms = 1000;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ofstream fout;
    ifstream fin;

    ofstream fout_warehouse;
    ifstream fin_warehouse;
    streampos pos;
    string nazwapliku;
    string default_path;

    TimeWorkingInfo *time_info;


    QString data;
    QString godz;
    double suma_godz;
    int day_of_week;
    int month;

    QString dateString_first;
    QString timeString_first;
    QString dateString_second;
    QString timeString_second;
    QDate date;
    QTime time;
    QTime time_sec;
    QTimer *refresh_gui_timer;



    QSound *alarm;
    bool let_to_alarm_enter;

    QDateTime *last_turnoff_computer_time;
    QTime *last_working_time;
    bool was_modyfied;

    QDateTime *last_date_time;
    QSystemTrayIcon *icon;
    QMenu *menu;
    QMenu *enter_times_menu;
    QAction *hide_window;
    QAction *restore;
    QAction *quitAction;

     QAction *enterCBRTimeAction;
     QAction *enterZMTTimeAction;


    QDateEdit *dateEdit = new QDateEdit;
    QCalendarWidget *calendarWidget = new QCalendarWidget();


    int wiersze_pliku;



    bool Openfile(char *NAZWAPLIKU);
    int SprawdzIloscWierszy(FILE *PLIK);
    void UstawNaKoniec(FILE *PLIK);
    int ZapiszWiersz(FILE *PLIK, char *tab);
    void UstawZegar(void);
    void uworzPlik(const char *nazwapliku);
    void otworzPlik(const char *nazwapliku);
    bool isOpenfirst(FILE *plik);
    void createMinimalizeToTry(void);
    bool engineApp(bool sel);
    bool fileExists(const char *str);
    string getUserName() const;
    void setUserDefinedPath();
    bool controlAppRun();
    void restoreDefault();


    void zapiszPierwszeWlaczenie(QDateTime date_time);
    bool odczytajPierwszeWlaczenie(QDateTime date_time);
    string compensText(string text, int total_space, int ofset);
    void compessFreeTimes(QDateTime first, QDateTime second );

    void controlDelayTime();

    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

    bool modyfiLastRecordInFile(string filme_name, string str_to_modify);



private slots:
    void on_activated(QSystemTrayIcon::ActivationReason reason);
    void on_close();
    void on_enterCBRTimeAction();
    void on_enterZMTTimeAction();

public slots:



    void saveData(void);
    void on_manual_time_chaged(bool x);
    void on_refreshGui();




};

#endif // MAINWINDOW_H
