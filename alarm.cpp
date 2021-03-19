
#include "alarm.h"
#include "ui_alarm.h"
#include  <QDate>
#include <QMessageBox>
#include "QtNetwork/QNetworkReply"
#include "QUrl"
#include "QUrlQuery"
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkRequest"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "QDateTime"
Alarm::Alarm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Alarm)
{

    ui->setupUi(this);
    QDate date=QDate::currentDate();   // show  current date
    QString datetext=date.toString();
    ui->date->setText(datetext);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width()-this->width()) / 2;
    int y = (screenGeometry.height()-this->height()) / 2;
    this->move(x,y);

    QTimer *timer = new QTimer(this);



    // set  up signals and slots

    connect(timer, &QTimer::timeout, this, &Alarm::showTime);
    connect(ui->btnSetAlarm, &QPushButton::clicked, this, &Alarm::setalarmtime);
    connect(ui->btnEnableAlarm,&QPushButton::clicked,this,&Alarm::setAlarmStatus);
    connect(ui->btnStopAlarm,&QPushButton::clicked,this,&Alarm::on_btnStopAlarm_clicked);


    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");

    ui->timeLCD->display(text);

    QTime alarmtime = QTime::currentTime();
    text2 = alarmtime.toString("hh:mm");

    ui->alarmLCD->display(text2);
    m_thread = new mThread(this);

    // connect the thread
    connect(m_thread, &mThread::AlarmFired,this,&Alarm::soundAlarm) ;


    timer->start(1000);


    manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                qDebug() << reply->errorString();
                clearDisplay();
                ui->location->setText("Not Found");
                return;
            }

            QString jsonString = reply->readAll();

            qDebug() << jsonString;

            displayWeatherData(jsonString);
        }
    );


}

Alarm::~Alarm()
{
    delete ui;
}

void Alarm::  showTime(){

    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    if ((time.second() % 2) == 0)
    {
        text[2] = ' ';
        if(flag==true){
         alarm_time[2]  = ' ';

        }
    }
    else
    {
       alarm_time[2]  = ':';

    }


     if(flag==true){
     alarm_time = hours + alarm_time[2] + minutes ;

     ui->alarmLCD->display(alarm_time);
     updateAlarm();

     }


    ui->timeLCD->display(text);



}
void Alarm:: updateAlarm()
{
 QTime time = QTime::currentTime();
 QString _now = time.toString("hh:mm");

 if((_now == alarm_time)&&(flag== true))
 {




    if(f_fired==false){
    m_thread->start() ;
    f_fired= true ;
    }


 }


}

void Alarm::setalarmtime(){

// get the int value from the spingbox


int ihours = ui->spinBoxHrs->value() ;
int iminutes =ui->spinBoxMinutes->value() ;

 if (ihours<10)
 {
   hours = "0"+ QString::number(ihours);
 }

 else
 {
    hours=QString::number(ihours);

 }

 if(iminutes <10 )
 {
   minutes ="0"+ QString::number(iminutes);

 }
 else
 {
    minutes = QString::number(iminutes);
 }


    alarm_time = hours + ':' + minutes ;




ui->alarmLCD->display(alarm_time);
isSetFlag = true ;

}

void Alarm::setAlarmStatus()
{
   if(isSetFlag){

    QString labeltext = ui->lblalarmstatus->text();

    if(labeltext=="Alarm Switched Off")
    {

    ui->lblalarmstatus->setText("Alarm Switched On");
    QPalette *p = new QPalette;  // change lcd color

    p->setColor(QPalette::WindowText, QColor(255, 0, 0));
    ui->lblalarmstatus->setPalette(*p);
    flag = true ;
    }


    if(labeltext=="Alarm Switched On")
    {

    ui->lblalarmstatus->setText("Alarm Switched Off");
    QPalette *p = new QPalette;  // change lcd color

    p->setColor(QPalette::WindowText, QColor(0, 0, 255));
    ui->lblalarmstatus->setPalette(*p);
    alarm_time = hours + ':' + minutes ;
    ui->alarmLCD->display(alarm_time);
    flag = false ;
    }

 }
   else
   {


       QMessageBox *msgBox = new QMessageBox(this);
       msgBox->setText("You havent set an alarm time ");
       msgBox->exec();



   }


}



void Alarm::on_btnStopAlarm_clicked()
{
 alarm_time ="00:00" ;
 ui->lblalarmstatus->setText("Alarm Switched Off");
 QPalette *p = new QPalette;  // change lcd color

 p->setColor(QPalette::WindowText, QColor(0, 0, 255));
 ui->lblalarmstatus->setPalette(*p);
 ui->alarmLCD->display(alarm_time);
 flag =false ;
 isSetFlag =false ;
 m_thread->stop= true ;
 f_fired= false ;

}

void Alarm::soundAlarm(){

    QSound::play(":/bells.wav");
}
void Alarm::on_queryButton_clicked()
{
    QString query = ui->query->text();
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery attributes;
    attributes.addQueryItem("q", query);
    attributes.addQueryItem("appid", "3130fe9c95bcd429a94c61a79c4f7c32");
    attributes.addQueryItem("units", "metric");
    url.setQuery(attributes);
    request.setUrl(url);
    manager->get(request);
}

void Alarm::displayWeatherData(QString jsonString){
    QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject data = jsonResponse.object();
    QJsonObject dataMain = data["main"].toObject();
    QJsonObject dataWeather = data["weather"].toArray().at(0).toObject();
    QJsonObject dataWind = data["wind"].toObject();
    QJsonObject dataSys = data["sys"].toObject();

    QString city = data["name"].toString();
    QString countryCode = dataSys["country"].toString();
    QString location = city.append(", ").append(countryCode);

    QString weatherCondition = dataWeather["main"].toString();

    int temp = dataMain["temp"].toDouble();
    int feelsLike = dataMain["feels_like"].toDouble();
    int tempMax = dataMain["temp_max"].toDouble();
    int tempMin = dataMain["temp_min"].toDouble();

    int pressure = dataMain["pressure"].toDouble();
    int humidity = dataMain["humidity"].toDouble();

    int visibility = data["visibility"].toDouble();

    int windSpeed = dataWind["speed"].toDouble();
    int windDirDegree = dataWind["deg"].toDouble();

    int sunriseTimestamp = dataSys["sunrise"].toInt();
    int sunsetTimestamp = dataSys["sunset"].toInt();

    QDateTime sunriseDateTime;
    sunriseDateTime.setTime_t(sunriseTimestamp);
    QDateTime sunsetDateTime;
    sunsetDateTime.setTime_t(sunsetTimestamp);

    QDate date = sunriseDateTime.date();
    QTime sunriseTime = sunriseDateTime.time();
    QTime sunsetTime = sunsetDateTime.time();

    ui->query->setText(city);

    ui->location->setText(location);
    ui->date->setText(date.toString());
    ui->weatherlabel->setText(weatherCondition);
    ui->temperature->setText(QString::number(temp));
    ui->feelsLikeTempValue->setText(QString::number(feelsLike));
    ui->maxTempValue->setText(QString::number(tempMax));
    ui->minTempValue->setText(QString::number(tempMin));
    ui->pressurevalue->setText(QString::number(pressure).append(" hPa"));
    ui->humidityValue->setText(QString::number(humidity).append("%"));
    ui->visibilityValue->setText(QString::number(visibility).append(" m"));
    ui->windSpeedValue->setText(QString::number(windSpeed).append(" m/s"));
    ui->windDirValue->setText(direction(windDirDegree));
    ui->sunriseValue->setText(sunriseTime.toString().remove(5, 3));
    ui->sunsetValue->setText(sunsetTime.toString().remove(5, 3));
}

QString Alarm::direction(int deg){
    QString direction;
    deg += 23;
    deg %= 360;
    if(deg <= 45){
        direction = "North";
    }else if(deg < 90){
        direction = "North-East";
    }else if(deg <= 135){
        direction = "West";
    }else if(deg < 180){
        direction = "South-West";
    }else if(deg <= 225){
        direction = "South";
    }else if(deg < 270){
        direction = "South-West";
    }else if(deg < 315){
        direction = "West";
    }else{
        direction = "North-West";
    }
    return direction;
}

void Alarm::clearDisplay(){
    ui->location->setText("WeatherApp");
    ui->date->setText("");
    ui->weatherlabel->setText("Partly sunny");
    ui->temperature->setText(QString::number(0));
    ui->feelsLikeTempValue->setText(QString::number(0));
    ui->maxTempValue->setText(QString::number(0));
    ui->minTempValue->setText(QString::number(0));
    ui->pressurevalue->setText("");
    ui->humidityValue->setText("");
    ui->visibilityValue->setText("");
    ui->windSpeedValue->setText(QString::number(0).append(" m/s"));
    ui->windDirValue->setText(direction(0));
    ui->sunriseValue->setText("00:00");
    ui->sunsetValue->setText("00:00");
}

