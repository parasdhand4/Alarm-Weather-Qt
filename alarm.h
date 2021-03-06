#ifndef ALARM_H
#define ALARM_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QRect>
#include <QDesktopWidget>
#include <QDebug>
#include <QScreen>
#include "mthread.h"
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QSound>
#include "QtNetwork/QNetworkAccessManager"
#include "QtNetwork/QNetworkRequest"

QT_BEGIN_NAMESPACE
namespace Ui { class Alarm; }
QT_END_NAMESPACE

class Alarm : public QWidget
{
    Q_OBJECT

public:
    Alarm(QWidget *parent = nullptr);
    ~Alarm();

    // variables global

    QString text2 ;
    QString alarm_time  ;
    QString minutes ="00";
    QString hours ="00";
    bool flag = false;
    bool isSetFlag = false ;
    mThread *m_thread ;
    bool f_fired = false ;

private slots:
    void showTime();
    void updateAlarm();
    void setalarmtime();
    void setAlarmStatus();
    void on_btnStopAlarm_clicked();
    void soundAlarm();
    void on_queryButton_clicked();
    void displayWeatherData(QString jsonString);
    QString direction(int deg);
    void clearDisplay();
private:
    Ui::Alarm *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;

};
#endif // ALARM_H
