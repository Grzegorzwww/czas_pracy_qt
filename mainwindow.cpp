#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <stdio.h>

#include <QFileDialog>


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


    ui->checkBox_2->setChecked(true);

    let_to_alarm_enter = true;
    let_to_display_erp_alert = true;


    controlDelayTime();


    check_checkbox_state();


    //nazwapliku.append(FILE_NAME_PREFIX+getUserName()+FILE_NAME_SUFFIX);
    //qDebug() << QString::fromStdString(nazwapliku);

    refresh_gui_timer->start(timer_period_ms);
    zapiszPierwszeWlaczenie(QDateTime::currentDateTime() );
    ui->timeEdit->setTime(time_info->getCzasPrzebywaniaWpracy());

    connect( qApp, SIGNAL( aboutToQuit()),this,SLOT(saveData()));
    connect(ui->pushButton, SIGNAL(clicked(bool)),this,SLOT(on_manual_time_chaged(bool)));
    connect(refresh_gui_timer, SIGNAL(timeout()), this, SLOT(on_refreshGui()));
    connect(ui->checkBox_2, SIGNAL(clicked(bool)), this, SLOT(on_erp_reminder(bool )));

    connect(icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_activated(QSystemTrayIcon::ActivationReason)));

    this->setFixedSize(426,336);
    hide();
}

MainWindow::~MainWindow()
{
    delete icon;
    delete menu;
    delete restore;
    delete quitAction;
    delete ui;
}


bool MainWindow::controlAppRun(){
    bool app;


    QSettings settings;
    settings.beginGroup( GRUPA_USTAWIEN);
    app = settings.value(WLACZENIE_APLIKACJI, false).toBool();

    if(app == true){
         exit(EXIT_FAILURE);
    }else {
         ;
        settings.setValue(WLACZENIE_APLIKACJI, true);
    }
    settings.endGroup();
}



void MainWindow::setUserDefinedPath(){

    QString temp_qstr = QDir::homePath();
    default_path = temp_qstr.toStdString();

    qDebug() << QString::fromStdString(default_path);

    QSettings settings;
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
    ui->label_5->setText("Czas pracy komputera: "+ time_info->getCzasPracyKomputera().toString("hh:mm:ss")+" - [ "+
                         time_info->odejmij(CZAS_PRACY, time_info->getCzasPracyKomputera()).toString()+" ]"  );
    ui->label_2->setText("Oszacowany czas wejścia na CBR: "+ time_info->getCzasPrzebywaniaWpracy().toString("hh:mm:ss"));
    ui->label_3->setText("Oszacowany czas wejścia na ZMT: "+ time_info-> getCzasWejsciaNaZaklad().toString("hh:mm:ss"));
    ui->label_4->setText("Rekomendowana godzina wyjścia z pracy: "+ time_info-> getCzasWyjsciaZPRacy().toString("hh:mm:ss"));
    QString format = "dddd, d MMMM yyyy";


    QString icon_tool_tip_str = "Czas pierwszego włączenia komputera: "+time_info->getWlaczenieKomputera().toString("hh:mm:ss")+"\n";
    icon_tool_tip_str += "Czas pracy komputera: "+ time_info->getCzasPracyKomputera().toString("hh:mm:ss")+"\n";
    icon_tool_tip_str += "Oszacowany czas wejścia na CBR: "+ time_info->getCzasPrzebywaniaWpracy().toString("hh:mm:ss")+"\n";
    icon_tool_tip_str += "Oszacowany czas wejścia na ZMT: "+ time_info-> getCzasWejsciaNaZaklad().toString("hh:mm:ss")+"\n";
    icon_tool_tip_str += "Rekomendowana godzina wyjścia z pracy: "+ time_info-> getCzasWyjsciaZPRacy().toString("hh:mm:ss");
    icon->setToolTip(icon_tool_tip_str);


    ui->label_6->setText(QDate::currentDate().toString(format) +", "+ QTime::currentTime().toString("hh:mm:ss"));
    QTime temp_time_to_end = time_info->dodaj_czasy(time_info->getCzasPracyKomputera(), QTime(0,10,0));
    if(temp_time_to_end > QTime(ILOSC_GODZIN_PRACY,0,0) && let_to_alarm_enter){
         QSound::play(":/new/prefix1/syrena.wav");
        let_to_alarm_enter = false;
    }

    if(ui->checkBox_2->isChecked()){
        if(QDateTime::currentDateTime().date().dayOfWeek() == 5) {
            if(let_to_display_erp_alert && QTime::currentTime() > QTime(12,0,0)){
                QSound::play(":/sms-alert-2-daniel_simon.wav");
                QMessageBox::information( this, "Przypomnienie o ERPie", "Uzupełnij ERP'a !. \n"
                                                                         ""
                                                                         "", QMessageBox::Ok, 0 );
                let_to_display_erp_alert = false;
            }
        }
    }
}


void MainWindow::checkAplicationFirstRun(void)
{

    bool app_first_run = false;
    QSettings settings;

    settings.beginGroup( GRUPA_USTAWIEN);
    //settings.setValue(PIERWSZE_WLACZENIE_APLIKACJI);
        app_first_run = settings.value(PIERWSZE_WLACZENIE_APLIKACJI, true).toBool();

        if(app_first_run){
            QString sciezka = QFileDialog::getExistingDirectory(
                        this,
                        tr("Gdzie zapisać plik z logami ?"),
                        QDir::homePath());

            qDebug() << "sciezka: " << sciezka;

            nazwapliku.clear();
            nazwapliku.append(sciezka.toStdString());
            nazwapliku.append("/");
            nazwapliku.append(FILE_NAME_PREFIX+getUserName()+FILE_NAME_SUFFIX);

            settings.setValue(PATH_TO_SAVE_FILE, QString::fromStdString(nazwapliku));
            settings.setValue(PIERWSZE_WLACZENIE_APLIKACJI, false);

            qDebug() << QString::fromStdString(nazwapliku);
        }
        else {

            nazwapliku.clear();
            nazwapliku.append(settings.value(PATH_TO_SAVE_FILE, "log" ).toString().toStdString());
            qDebug() << QString::fromStdString(nazwapliku);

        }
    settings.endGroup();
  //  }
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
    enter_times_menu = new QMenu("Ustaw godziny", this);

    hide_window = new QAction("Minimalizuj",this);
    connect( hide_window,SIGNAL(triggered()),this,SLOT(hide()));

    quitAction = new QAction("Zamknij",this);
    connect(quitAction,SIGNAL(triggered()),this,SLOT(on_close()));

    restore = new QAction("Przywróć", this);
    connect (restore, SIGNAL(triggered()), this, SLOT(showNormal()));

     setPathToSaveFile  = new QAction("Ustaw nową scieżkę", this);
     connect (setPathToSaveFile, SIGNAL(triggered()), this, SLOT(on_setNewPathToSaveFile()));



    menu->addMenu(enter_times_menu);
    menu->addAction( setPathToSaveFile);
    menu->addAction(hide_window);
    menu->addAction(restore);
    menu->addAction(quitAction);


    enterCBRTimeAction = new QAction("Ustaw Czas CBR->KOMPUTER",this);
    connect (enterCBRTimeAction, SIGNAL(triggered()), this, SLOT(on_enterCBRTimeAction()));

    enterZMTTimeAction = new QAction("Ustaw Czas ZMT->KOMPUTER",this);
    connect (enterZMTTimeAction, SIGNAL(triggered()), this, SLOT(on_enterZMTTimeAction()));

    enter_times_menu->addAction(enterCBRTimeAction);
    enter_times_menu->addAction(enterZMTTimeAction);


    icon->setContextMenu(menu);


}
void MainWindow::on_activated(QSystemTrayIcon::ActivationReason reason){

    if(reason == QSystemTrayIcon::DoubleClick){
        if(this->isHidden())
        {
            show();
        }
        else
        {
            hide();
        }
    }
}

void MainWindow::on_close()
{
    QApplication::quit();
}

void MainWindow::on_enterCBRTimeAction(){
    bool ok;
    QString temp_time_str = QInputDialog::getText(this, "Podaj czas CBR->KOMPUTER ",
                                                  tr("Podaj czas (format: hh,mm,ss): "), QLineEdit::Normal,
                                                  "", &ok);
    if (ok && !temp_time_str.isEmpty()){
        QTime::fromString(temp_time_str, "hh,mm,ss").toString();
        time_info->setTimeCbrComp( QTime::fromString(temp_time_str , "hh,mm,ss"));
        if(time_info->getTimeCbrComp() > QTime(0,0,1)){
        QSettings settings;
        settings.beginGroup( GRUPA_USTAWIEN);
            settings.setValue(TIME_CBR_COMP_DELAY, time_info->getTimeCbrComp());
            settings.setValue(FLAG_CBR_COMP_DELAY, true);
        settings.endGroup();
        }
    }
}
void MainWindow::on_enterZMTTimeAction(){
    bool ok;
    QString temp_time_str = QInputDialog::getText(this, "Podaj czas ZMT->KOMPUTER ",
                                                  tr("Podaj czas (format: hh,mm,ss): "), QLineEdit::Normal,
                                                  "", &ok);
    if (ok && !temp_time_str.isEmpty()){
        QTime::fromString(temp_time_str, "hh,mm,ss").toString();
        time_info->setTimeZmtComp(QTime::fromString(temp_time_str , "hh,mm,ss"));
        if(time_info->getTimeZmtComp() > QTime(0,0,1)){
       // QString m_sSettingsFile = QApplication::applicationDirPath()/*.left(1)*/ + "/settings.ini";
        QSettings settings/*(m_sSettingsFile,QSettings::NativeFormat)*/;    // qt linux format
        settings.beginGroup( GRUPA_USTAWIEN);
            settings.setValue(TIME_ZMT_COMP_DELAY, time_info->getTimeZmtComp());
            settings.setValue(FLAG_ZMT_COMP_DELAY, true);
        settings.endGroup();
        }
    }
  
}

void MainWindow::on_setNewPathToSaveFile(){

            QSettings settings;
            settings.beginGroup( GRUPA_USTAWIEN);

    QString sciezka = QFileDialog::getExistingDirectory(
                this,
                tr("Gdzie zapisać plik z logami ?"),
                QDir::homePath());

           qDebug() << "sciezka: " << sciezka;

            nazwapliku.clear();
            nazwapliku.append(sciezka.toStdString());
            nazwapliku.append("/");
            nazwapliku.append(FILE_NAME_PREFIX+getUserName()+FILE_NAME_SUFFIX);

            settings.setValue(PATH_TO_SAVE_FILE, QString::fromStdString(nazwapliku));
            settings.setValue(PIERWSZE_WLACZENIE_APLIKACJI, false);
            qDebug() << QString::fromStdString(nazwapliku);
             settings.endGroup();

}


void MainWindow::on_erp_reminder(bool x){

    QSettings settings;
    settings.beginGroup( GRUPA_USTAWIEN);
    if(x){
        settings.setValue(PRZYPOMNIENIE_O_ERP, true);
        qDebug() <<"on_erp_reminder = true";

    }else{
        settings.setValue(PRZYPOMNIENIE_O_ERP, false);
        qDebug() <<"on_erp_reminder = false";
    }
    settings.endGroup();
}

void MainWindow::check_checkbox_state(){

    QSettings settings;
    bool state;
    settings.beginGroup( GRUPA_USTAWIEN);

    state =  settings.value(PRZYPOMNIENIE_O_ERP, false).toBool();
    if(state)
        ui->checkBox_2->setChecked(true);
    else
        ui->checkBox_2->setChecked(false);
    settings.endGroup();
}

void MainWindow::controlDelayTime(){

    bool cbr_flag = false;
    bool zmt_flag = false;

    QSettings settings;
    settings.beginGroup( GRUPA_USTAWIEN);
    cbr_flag = settings.value(FLAG_CBR_COMP_DELAY, false).toBool();
    if(cbr_flag){
        time_info->setTimeCbrComp(settings.value(TIME_CBR_COMP_DELAY, CZAS_WEJSCIE_WLACZENIE_KOMP).toTime());
    }
    zmt_flag = settings.value(FLAG_ZMT_COMP_DELAY, false).toBool();
    if(zmt_flag){
        time_info->setTimeZmtComp( settings.value(TIME_ZMT_COMP_DELAY, CZAS_WEJSCIE_NA_ZAKLAD_WLACZENIE_KOMP).toTime());
    }
    settings.endGroup();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
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
    string line;
    ifstream in(filme_name.c_str());
    if( !in.is_open()){
        cout << "Input file failed to open\n";
        return 1;
    }

    // now open temp output file
    // string path

    QString temp_str_name = QString::fromStdString(nazwapliku);
    temp_str_name = temp_str_name.replace(QString::fromStdString(FILE_NAME_PREFIX+getUserName()+FILE_NAME_SUFFIX), "outfile.txt");
    ofstream out(temp_str_name.toStdString());

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
    rename(temp_str_name.toStdString().c_str(),filme_name.c_str());
    // all done!
    return 0;
}


void MainWindow::compessFreeTimes(QDateTime first, QDateTime second ){

    if(first < second){
        if((first.addDays(1) != second)){
            first.addDays(1);
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
               // qDebug() << first.toString();
                first = first.addDays(1);
            }
        }
    }
}




bool MainWindow::engineApp(bool sel)
{

    QSettings settings;


    if(QFile::exists(settings.fileName())){

        QString temp_str;
        compessFreeTimes(*last_date_time, QDateTime::currentDateTime());

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


    QSettings settings;
    settings.beginGroup(GRUPA_USTAWIEN);




    if(odczytajPierwszeWlaczenie(QDateTime::currentDateTime()) || (!QFile::exists(settings.fileName()))){    // pierwsze wlaczenie
        qDebug() << "wlaczenie - nowy dzien ";
        checkAplicationFirstRun();
        date_time.setTime(time_info->odejmij(QTime::currentTime(), time_info->readLinuxSystemTime()));
        settings.setValue(PIERWSZE_WLACZENIE_KOMPUTERA, date_time);
        time_info->setWlaczenieKomputera(time_info->odejmij( QTime::currentTime(), time_info->readLinuxSystemTime()));
        was_modyfied = false;

    }
    else {  //kolejne wlaczenia
        qDebug() << "kolejne wlaczenie ";
        was_modyfied = true;
        time_info->setWlaczenieKomputera(settings.value(PIERWSZE_WLACZENIE_KOMPUTERA, QTime::currentTime()).toTime());
        nazwapliku.clear();
        nazwapliku.append(settings.value(PATH_TO_SAVE_FILE, "log" ).toString().toStdString());

    }
    settings.endGroup();
}

bool MainWindow::odczytajPierwszeWlaczenie(QDateTime date_time){



    QSettings settings/*(m_sSettingsFile,QSettings::NativeFormat)*/;    // qt linux format
    qDebug() << settings.fileName();

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
   // qDebug() << "on_manual_time_chaged";
    if (QMessageBox::question(this, "Czy zmienić godzinę?",
                      "Napewno chcesz wprowadzić datę manualnie ?") == QMessageBox::Yes){
       time_info->setWlaczenieKomputera(time_info->dodaj_czasy(ui->timeEdit->time(), CZAS_WEJSCIE_WLACZENIE_KOMP));
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


    time_cbr_computer_delay =  QTime(CZAS_WEJSCIE_WLACZENIE_KOMP);
    time_zmt_computer_delay =  QTime(CZAS_WEJSCIE_NA_ZAKLAD_WLACZENIE_KOMP);
}

QTime TimeWorkingInfo::getCzasPracyKomputera(){
    czas_pracy_komputera =  QTime(odejmij_czasy(czas_wlaczenie_komputera, QTime::currentTime()));
    return czas_pracy_komputera;
}

QTime TimeWorkingInfo::getCzasPrzebywaniaWpracy() {
   // czas_przyjscia_do_pracy = QTime(dodaj_czasy(czas_wlaczenie_komputera, QTime(0,2,30))); //dodac dobry czas;
     czas_przyjscia_do_pracy = QTime(odejmij(czas_wlaczenie_komputera, time_cbr_computer_delay)); //dodac dobry czas;
    return czas_przyjscia_do_pracy;
}
QTime TimeWorkingInfo::getCzasWejsciaNaZaklad(){
    //czas_wejsca_na_zaklad = QTime(dodaj_czasy(czas_wlaczenie_komputera, QTime(0,4,0))); //dodac dobry czas
    czas_wejsca_na_zaklad = QTime(odejmij(czas_wlaczenie_komputera, time_zmt_computer_delay)); //dodac dobry czas
    return czas_wejsca_na_zaklad;
}
QTime TimeWorkingInfo::getCzasWyjsciaZPRacy(){
    czas_opuszczenia_zakladu = QTime(dodaj_czasy(czas_przyjscia_do_pracy, CZAS_PRACY));
    if(czas_opuszczenia_zakladu < QTime(14,45,0)){
        czas_opuszczenia_zakladu = QTime(14,45,0);
    }
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

QTime TimeWorkingInfo::odejmij(QTime x, QTime y){

    long x_total_in_sek = x.second() + (x.minute() * 60)+ (x.hour() * 3600 );
    long y_total_in_sek = y.second() + (y.minute() * 60)+ (y.hour() * 3600 );
    int xtemp = 0;
    xtemp = x_total_in_sek - y_total_in_sek;
    x = QTime(0,0,0);

    return QTime(x.addSecs((int)xtemp));
}

QTime TimeWorkingInfo::readLinuxSystemTime() {
    QString file_name = "/proc/uptime";
    double real_uptime = 0;

    QFile uptime(file_name);
    if (uptime.exists()) {
        if(uptime.open(QIODevice::ReadOnly)){
            QTextStream in(&uptime);
            QString line = in.readLine();
            if(!line.isEmpty()){
               // qDebug() <<"line = "<< line;

                real_uptime = line.section(" ", 0, 0).trimmed().toDouble();
            }
            int int_real_uptime = (int)real_uptime;
            system_uptime = QTime(int_real_uptime / 3600 % 24, int_real_uptime / 60 % 60, int_real_uptime % 60);
            return system_uptime;
        }
        else
            qDebug() << " Nie mozna otworzyc pliku: "<<file_name;
    }
    else
           qDebug() << "Plik nie istnieje"<<file_name;
}


