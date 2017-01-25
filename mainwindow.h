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

const QString SETTINGS_GRUPA_USTAWIEN = "settings_first_fun_group";
const QString SETTINGS_PIERWSZE_WLACZENIE_KOMPUTERA = "first_tun_date_and_time";
const QString SETTINGS_DZIEN_OSTATNIEJ_MODYFIKACJI = "last_moydifed_date_and_time";

const QString SETTINGS_OSTATNIE_WYLACZENIE_KOMPUTERA = "last_turnoff_computer_time";

const QString SETTINGS_FLAGA_MODYFIKACJI = "was_modyfication";

const QString SETTINGS_OSTATNIA_POZYCJA_W_PLIKU = "last_fpos_location";




class TimeWorkingInfo
{
private:

    QTime czas_wlaczenie_komputera;
    QTime czas_pracy_komputera;
    QElapsedTimer czas_pracy;
    QTime czas_przyjscia_do_pracy;
    QTime czas_wejsca_na_zaklad;
    QTime czas_opuszczenia_zakladu;
    QString time_format;




public:
    TimeWorkingInfo();
    ~TimeWorkingInfo() {}

    void setWlaczenieKomputera(const QTime & x) {czas_wlaczenie_komputera = x;}
    void setToCzasPracyKomputera(const QTime & x) {czas_pracy_komputera = x;}

    QTime getWlaczenieKomputera() {return czas_wlaczenie_komputera;}
    void setTimeFormat(const QString str) {time_format = str;}
    QTime getCzasPracyKomputera();
    QTime getCzasPracyKomputera(bool){ return czas_pracy_komputera;}
    QTime getCzasPrzebywaniaWpracy();
    QTime getCzasWejsciaNaZaklad();
    QTime getCzasWyjsciaZPRacy();

    QTime odejmij_czasy(QTime x, QTime y);
    QTime dodaj_czasy(QTime x, QTime y);





};






#define NAZWA_PLIKU "log_file.txt"
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

    QDateTime *last_turnoff_computer_time;
    QTime *last_working_time;
    bool was_modyfied;

    QDateTime *last_date_time;
    QSystemTrayIcon *icon;
    QMenu *menu;
    QAction *restore;
    QAction *quitAction;


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


    void zapiszPierwszeWlaczenie(QDateTime date_time);
    bool odczytajPierwszeWlaczenie(QDateTime date_time);
    string compensText(string text, int total_space, int ofset);
    void compessFreeTimes(QDateTime first, QDateTime second );


    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

    bool modyfiLastRecordInFile(string filme_name, string str_to_modify);





public slots:



    void saveData(void);
//    void on_pushButton_clicked(void);
//    void on_pushButton_2_clicked(void);
    void on_refreshGui();



};

#endif // MAINWINDOW_H
