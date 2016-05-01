#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <QtGui>
#include <QtCore>

#define PRIEMER 5

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void getPath();
    void generuj();
    void derivuj();
    
private:
    Ui::MainWindow *ui;
};

class Bod
{
public:
    int cislo;
    double x;
    double y;
    double z;
    char kod_kompat[5];
    int stav; //0 - pod trojuholnikom, 1 - pouzitelny, 2 - sucastou trojuholnika
    double zx;
    double zy;
};

struct TTstrukt
{
    int A1;
    int A2;
    int A3;
    int M1;
    int M2;
    int M3;
    int n;
};

struct Bod_pomoc
{
    double x;
    double y;
};

struct Priamka
{
    double k;
    double q;
};

struct Vektor
{
    double x;
    double y;
    double dlzka;
};

struct Vektor3D
{
    double x;
    double y;
    double z;
};

struct Esigma
{
    int cislobodu;
    int prioritny; //1 - ano, 0 - nie
    Esigma *p_dalsi;
};

class TimerUser: public QObject
{
    Q_OBJECT
public:
    TimerUser(QObject *parent = 0):QObject(parent)
    {
        QTimer::singleShot(1000,this,SLOT(timerMethod()));
    }
public slots:
    void timerMethod()
    {
        // Do your timer work
    }
};

#endif // MAINWINDOW_H
