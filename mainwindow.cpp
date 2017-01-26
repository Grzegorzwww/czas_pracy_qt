#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <stdio.h>

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    ui(new Ui::MainWindow)
{





    ui->setupUi(this);
    ui->checkBox->setChecked(true);
    refresh_gui_timer = new QTimer(this);
    time_info = new TimeWorkingInfo();

    createMinimalizeToTry();
    UstawZegar();



    //setUserDefinedPath();
    //QDir::setCurrent(QString::fromStdString(default_path));
//    nazwapliku.append(default_path);
//    nazwapliku.append("/");

    nazwapliku.append( FILE_NAME_PREFIX+getUserName()+FILE_NAME_SUFFIX);
    qDebug() << QString::fromStdString(nazwapliku);

    this->hide();
    restore->setEnabled(true);

    this->setFixedSize(426,336);
    this->setVisible(false);
    this->hide();

    refresh_gui_timer->start(timer_period_ms);

    zapiszPierwszeWlaczenie(QDateTime::currentDateTime());
    ui->timeEdit->setTime(time_info->getCzasPrzebywaniaWpracy());



    connect( qApp, SIGNAL( aboutToQuit()),this,SLOT(saveData()));
    connect(ui->pushButton, SIGNAL(clicked(bool)),this,SLOT(on_manual_time_chaged(bool)));
    connect(refresh_gui_timer, SIGNAL(timeout()), this, SLOT(on_refreshGui()));
}




MainWindow::~MainWindow()
{
    delete dateEdit;
    delete calendarWidget;
    delete icon;
    delete menu;
    delete restore;
    delete quitAction;
    delete ui;
}


void MainWindow::setUserDefinedPath(){

    QString temp_qstr = QDir::homePath();
            /*"/" + QStandardPaths::displayName( QStandardPaths::DesktopLocation );
    qDebug() << temp_qstr;*/

    default_path = temp_qstr.toStdString();





    qDebug() << QString::fromStdString(default_path);

    QString m_sSettingsFile = QApplication::applicationDirPath()/*.left(1)*/ + "/settings.ini";
    QSettings settings(m_sSettingsFile,QSettings::NativeFormat);    // qt linux format
    settings.beginGroup( GRUPA_USTAWIEN);
    bool flag = settings.value(FLAG_TO_LET_USE_PATH, false).toBool();

    if(flag == true){
        default_path = settings.value(PATH_TO_SAVE_FILE, false).toString().toStdString();

    }else {
        settings.setValue(FLAG_TO_LET_USE_PATH, false);
        settings.setValue(PATH_TO_SAVE_FILE, QString::fromStdString(default_path));
    }
    settings.endGroup();

}


void MainWindow::on_refreshGui(){

    ui->label->setText("Czas pierwszego włączenia komputera: "+time_info->getWlaczenieKomputera().toString("hh:mm:ss"));
    ui->label_5->setText("Czas pracy komputera: "+ time_info->getCzasPracyKomputera().toString("hh:mm:ss"));
    ui->label_2->setText("Oszacowany czas przybycia do pracy: "+ time_info->getCzasPrzebywaniaWpracy().toString("hh:mm:ss"));
    ui->label_3->setText("Oszacowany czas wejścia na zakład: "+ time_info-> getCzasWejsciaNaZaklad().toString("hh:mm:ss"));
    ui->label_4->setText("Rekomendowana godzina wyjścia z pracy: "+ time_info-> getCzasWyjsciaZPRacy().toString("hh:mm:ss"));
    QString format = "dddd, d MMMM yyyy";
    ui->label_6->setText(QDate::currentDate().toString(format) +", "+ QTime::currentTime().toString("hh:mm:ss"));
}


void MainWindow::UstawZegar(void)
{

}
void MainWindow::uworzPlik(const char *nazwapliku)
{
    fout.open(nazwapliku, ios_base::out);
    fout << SEPARATOR <<endl;
    fout << PRZESTRZEN << endl;
    fout << NAZWA << endl;
    fout << PRZESTRZEN << endl;
    fout << SEPARATOR << endl;
    fout << MENU_FILE << endl;
    fout.close();

}
void MainWindow::otworzPlik(const char *nazwapliku)
{
    fout.open(nazwapliku,  ios_base::app |  ios_base::ate);
}

void MainWindow::saveData(void)
{
    if(ui->checkBox->isChecked()){
        qDebug() << "save data..." << endl;
        if(fileExists(nazwapliku.c_str())){
            qDebug() << "ISTNIEJE" << endl;
            otworzPlik(nazwapliku.c_str());
            engineApp(false);
        }
        else {
            qDebug() << "NIE ISTNIEJE" << endl;
            uworzPlik(nazwapliku.c_str());
            otworzPlik(nazwapliku.c_str());
            engineApp(false);
        }
    }
}

bool MainWindow::fileExists(const char *str)
{

    QFileInfo check_file(str);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}



void MainWindow::createMinimalizeToTry(void)
{
    icon= new QSystemTrayIcon(this);
    QPixmap pixmap(25,25);
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(pixmap.rect(),Qt::AlignCenter,"RCP");
    icon->setIcon(pixmap);
    icon->setToolTip("licznik czasu pracy, kliknij aby rozwinąć");
    icon->setVisible(true);
    icon->show();

    menu = new QMenu(this);
    quitAction = new QAction("Zamknij",this);
    connect(quitAction,SIGNAL(triggered()),this,SLOT(close()));

    menu->addAction(quitAction);
    icon->setContextMenu(menu);
    restore = new QAction("Przywróć", this);
    restore->setEnabled(false);
    connect (restore, SIGNAL(triggered()), this, SLOT(showNormal()));
    menu->addAction(restore);

}
void MainWindow::closeEvent(QCloseEvent *event)
{
    //saveData();
    QApplication::quit();
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    case QEvent::WindowStateChange:
    {
        if(isMinimized())
        {
            hide();
            restore->setEnabled(true);
        }
        else
        {
            restore->setEnabled(false);
        }
    }
        break;
    default:
        break;
    }
}

bool MainWindow::modyfiLastRecordInFile(string filme_name, string str_to_modify){

    int i = 0;
    fin.open(filme_name.c_str());
    std::string wiersz, wiersz_2;
    while( std::getline( fin, wiersz ) ){
        wiersz_2 = wiersz;
        i++;
    }
    //qDebug() << "stary wiersz = "<< QString::fromStdString(wiersz_2);
    string line;
    ifstream in(filme_name.c_str());
    if( !in.is_open()){
        cout << "Input file failed to open\n";
        return 1;
    }
    // now open temp output file
    ofstream out("outfile.txt");
    // loop to read/write the file.  Note that you need to add code here to check
    // if you want to write the line
    while( getline(in,line) ){
        if(line.compare(wiersz_2)){
            out << line << "\n";
        }
        else {
            out << str_to_modify;
        }
    }
    in.close();
    out.close();
    // delete the original file
    remove(filme_name.c_str());
    // rename old to new
    rename("outfile.txt",filme_name.c_str());
    // all done!
    return 0;
}


void MainWindow::compessFreeTimes(QDateTime first, QDateTime second ){

    if(first < second){
        //        qDebug() << "first < second - ok";
        if((first.addDays(1) != second)){
            //            qDebug() << "first.addDays(1) != second";
            first.addDays(1);
           //                  qDebug() << "first.toString()"  << first.toString();
           //                   qDebug() << "second.toString()" << second.toString();
            second = second.addDays(-1);
            first = first.addDays(1);
            second = second.addDays(1);
            while(first < second)
            {
                fout <<"|"<<compensText(first.date().toString().trimmed().toStdString(), 23, 1);
                fout <<"|"<<compensText("-", 20, 11);
                fout <<"|"<<compensText("-", 21, 12);
                fout <<"|"<<compensText("WOLNE", 22, 9);
                fout <<"|";
                fout <<"\n";
                qDebug() << first.toString();
                first = first.addDays(1);
            }
        }
    }
}




bool MainWindow::engineApp(bool sel)
{

    if( QFileInfo("settings.ini").exists()){
        QString temp_str;
        compessFreeTimes(*last_date_time, QDateTime::currentDateTime());

        QString m_sSettingsFile = QApplication::applicationDirPath()/*.left(1)*/ + "/settings.ini";
        QSettings settings(m_sSettingsFile,QSettings::NativeFormat);    // qt linux format
        settings.beginGroup( GRUPA_USTAWIEN);
        QDateTime temp = settings.value(PIERWSZE_WLACZENIE_KOMPUTERA, 0).toDateTime();
        settings.setValue(DZIEN_OSTATNIEJ_MODYFIKACJI, QDateTime::currentDateTime());
        settings.setValue(OSTATNIE_WYLACZENIE_KOMPUTERA, QDateTime::currentDateTime());
        settings.endGroup();

        if(was_modyfied){
            qDebug() <<"kolejne zapisanie";
            string temp_str;
            temp_str =  temp_str + "|" + compensText(temp.date().toString().trimmed().toStdString(), 23, 2);
            temp_str =  temp_str + "|"+compensText(temp.time().toString().trimmed().toStdString(), 20, 7);
            temp_str =  temp_str +  "|"+compensText(QDateTime::currentDateTime().time().toString().trimmed().toStdString(), 21, 7);
            QTime temp_time = time_info->odejmij_czasy(temp.time(), QDateTime::currentDateTime().time());
            temp_str =  temp_str +  "|"+compensText(temp_time.toString().trimmed().toStdString(), 22, 7);
            temp_str =  temp_str +  "|";
            temp_str =  temp_str +  "\n";
            //qDebug() <<"nowy wiersz = "<< QString::fromStdString(temp_str);

            modyfiLastRecordInFile(nazwapliku, temp_str);
        }else
        {
            qDebug() <<"pierwsze zapisanie";
            fout <<"|"<<compensText(temp.date().toString().trimmed().toStdString(), 23, 2);
            fout <<"|"<<compensText(temp.time().toString().trimmed().toStdString(), 20, 7);
            fout <<"|"<<compensText(QDateTime::currentDateTime().time().toString().trimmed().toStdString(), 21, 7);
            QTime temp_time = time_info->odejmij_czasy(temp.time(), QDateTime::currentDateTime().time());
            fout <<"|"<<compensText(temp_time.toString().trimmed().toStdString(), 22, 7);
            fout <<"|";
            fout <<"\n";
        }
    }
}

void MainWindow::zapiszPierwszeWlaczenie(QDateTime   date_time){

    QString m_sSettingsFile = QApplication::applicationDirPath()/*.left(1)*/ + "/settings.ini";
    qDebug() << m_sSettingsFile;
    QSettings settings(m_sSettingsFile,QSettings::NativeFormat);    // qt linux format
    settings.beginGroup(GRUPA_USTAWIEN);

    if(odczytajPierwszeWlaczenie(QDateTime::currentDateTime()) || !QFileInfo("settings.ini").exists()){    // pierwsze wlaczenie
        qDebug() << "wlaczenie - nowy dzien ";
        settings.setValue(PIERWSZE_WLACZENIE_KOMPUTERA, date_time);
        time_info->setWlaczenieKomputera(QTime::currentTime());
        was_modyfied = false;

    }
    else {  //kolejne wlaczenia
        qDebug() << "kolejne wlaczenie ";
        was_modyfied = true;
        time_info->setWlaczenieKomputera(settings.value(PIERWSZE_WLACZENIE_KOMPUTERA, QTime::currentTime()).toTime());

    }
    settings.endGroup();
}

bool MainWindow::odczytajPierwszeWlaczenie(QDateTime date_time){

    QString m_sSettingsFile = QApplication::applicationDirPath()/*.left(1)*/ + "/settings.ini";
    QSettings settings(m_sSettingsFile,QSettings::NativeFormat);    // qt linux format

    settings.beginGroup( GRUPA_USTAWIEN);
    QDateTime temp = settings.value(PIERWSZE_WLACZENIE_KOMPUTERA, QDateTime::currentDateTime()).toDateTime();
    last_date_time = new QDateTime(settings.value(DZIEN_OSTATNIEJ_MODYFIKACJI, QDateTime::currentDateTime()).toDateTime());
    last_turnoff_computer_time = new QDateTime(settings.value(OSTATNIE_WYLACZENIE_KOMPUTERA, QDateTime::currentDateTime()).toDateTime());
    settings.endGroup();

    if(temp.date() == date_time.currentDateTime().date()){
        return false;
    }
    else {
        return true;
    }
}

string MainWindow::compensText(string text, int total_space, int ofset){
    int i;
    string temp_str = " ";
    if(ofset + text.length() < total_space){
        for(i = 0; i <= total_space; i++){
            if(i == ofset -1){
                temp_str += text;
                i = text.length()+ ofset;
            }
            else temp_str += ' ';
        }
    }
    return temp_str;
}

void MainWindow::on_manual_time_chaged(bool x){
    qDebug() << "on_manual_time_chaged";
    if (QMessageBox::question(this, "Czy zmienić godzinę?",
                      "Napewno chcesz wprowadzić datę manualnie ?") == QMessageBox::Yes){
       time_info->setWlaczenieKomputera(ui->timeEdit->time());
    }

}

string MainWindow::getUserName() const{

    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString QStr = homePath.first().split(QDir::separator()).last();
    return string(QStr.toStdString());

}




TimeWorkingInfo::TimeWorkingInfo() {

    czas_wlaczenie_komputera  = QTime(0, 0,0);
    czas_pracy_komputera = QTime(0, 0,0);
    czas_przyjscia_do_pracy = QTime(0, 0,0);
    czas_wejsca_na_zaklad = QTime(0, 0,0);
    czas_opuszczenia_zakladu = QTime(0, 0,0);
}




QTime TimeWorkingInfo::getCzasPracyKomputera(){
    czas_pracy_komputera =  QTime(odejmij_czasy(czas_wlaczenie_komputera, QTime::currentTime()));
    return czas_pracy_komputera;
}

QTime TimeWorkingInfo::getCzasPrzebywaniaWpracy() {
    czas_przyjscia_do_pracy = QTime(dodaj_czasy(czas_wlaczenie_komputera, QTime(0,1,25))); //dodac dobry czas;
    return czas_przyjscia_do_pracy;
}
QTime TimeWorkingInfo::getCzasWejsciaNaZaklad(){
    czas_wejsca_na_zaklad = QTime(dodaj_czasy(czas_wlaczenie_komputera, QTime(0,5,10))); //dodac dobry czas
    return czas_wejsca_na_zaklad;
}
QTime TimeWorkingInfo::getCzasWyjsciaZPRacy(){
    czas_opuszczenia_zakladu = QTime(dodaj_czasy(czas_przyjscia_do_pracy, QTime(8,0,0)));
    return czas_opuszczenia_zakladu;
}


QTime TimeWorkingInfo::odejmij_czasy(QTime x, QTime y){

    long x_total_in_sek = x.second() + (x.minute() * 60)+ (x.hour() * 3600 );
    // long y_total_in_sek = y.second() + (y.minute() * 60)+ (y.hour() * 3600 );
    x_total_in_sek *= -1;
    QTime time_to_return(y.addSecs((int)x_total_in_sek));
    return time_to_return;

}

QTime TimeWorkingInfo::dodaj_czasy(QTime x, QTime y){

    long y_total_in_sek = y.second() + (y.minute() * 60)+ (y.hour() * 3600 );
    QTime time_to_return(x.addSecs((int)y_total_in_sek));
    return time_to_return;


}

