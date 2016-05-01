#include "mainwindow.h"
#include "ui_mainwindow.h"

QString adresa;
Bod *bodovepole[10000];
TTstrukt *TT[10000];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getPath()
{
    adresa = QFileDialog::getOpenFileName(this, "Vložte .txt súbor s PDBP", QString::null, QString::null);
    ui->lineEdit->setText(adresa);
    ui->spinBox->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->spinBox_2->setEnabled(true);
    ui->spinBox_3->setEnabled(true);
}

void MainWindow::generuj()
{
    ui->graphicsView->setScene(new QGraphicsScene());

    FILE *fr, *fw, *fw2;
    int trojuh = 1, pocet_bodov = 0, iteruj = 0, k = 1, zalozena = 0;
    char adresa[100], riadok[60], retazec[31];
    double slim = ui->spinBox->value();
    Bod *I, *J, *L;
    Esigma *esigma_prvy, *esigma_akt, *esigma_pred, *esigma_pr_prvy, *esig_pr_akt, *esigma_pr_pred;
    TimerUser t;

    strcpy_s(adresa, ::adresa.toAscii().data()); // KONVERZIA QString na char()

    ui->label_2->setText(adresa);

    for (int i = 0; i < 10000; i++){
        TT[i] = NULL;
        bodovepole[i] = NULL;
    }

    if ((fr = fopen(adresa, "r")) == NULL)
        ui->label_2->setText("Súbor sa nepodarilo otvori.");
    else {
        fw = fopen("Bodove_pole.txt", "w");
        while (fgets(riadok, 60, fr) != NULL){
            bodovepole[pocet_bodov] = new Bod();
            sscanf(riadok, "%s", retazec);
            sscanf(riadok, "%d;%lf;%lf;%lf;%s", &bodovepole[pocet_bodov]->cislo,
                   &bodovepole[pocet_bodov]->x, &bodovepole[pocet_bodov]->y, &bodovepole[pocet_bodov]->z, bodovepole[pocet_bodov]->kod_kompat);
            //fprintf(fw, "%d %f %f %f %s\n", bodovepole[pocet_bodov]->cislo, bodovepole[pocet_bodov]->x, bodovepole[pocet_bodov]->y, bodovepole[pocet_bodov]->z, bodovepole[pocet_bodov]->kod_kompat);
            bodovepole[pocet_bodov]->stav = 1;
            pocet_bodov++;
        }

        for (int i = 0; i < pocet_bodov; i++){
            if(bodovepole[i]->kod_kompat[0] == 'r'){
                ui->graphicsView->scene()->addEllipse(bodovepole[i]->x, -bodovepole[i]->y, PRIEMER, PRIEMER, QPen(Qt::blue), QBrush(Qt::blue));
            }
            else if (bodovepole[i]->kod_kompat[0] == 'a'){
                ui->graphicsView->scene()->addEllipse(bodovepole[i]->x, -bodovepole[i]->y, PRIEMER, PRIEMER, QPen(Qt::black), QBrush(Qt::black));
            }
        }

        //Tvorba prveho trojuholnika-------------------------------------------------------------------------------        
        Bod_pomoc Ls, Is, Js;
        Vektor LsI, LsJ, dvasig;
        Priamka p, a, b, r1, r2;
        double koef, sum_pam;
        int vhodny = -1, priortest = 0;

        esigma_prvy = NULL;

        I = bodovepole[ui->spinBox_2->value()-1];
        J = bodovepole[ui->spinBox_3->value()-1];

        fprintf(fw, "Bod I ma cislo %d. Bod J ma cislo %d.\n", I->cislo, J->cislo);

        Ls.x = (I->x + J->x) / 2;
        Ls.y = (I->y + J->y) / 2; //Urcenie L'

        fprintf(fw, "Bod L' ma suradnice [x, y] = [%f, %f].\n", Ls.x, Ls.y);

        LsI.x = I->x - Ls.x;
        LsI.y = I->y - Ls.y; //Vektor L'I

        LsJ.x = J->x - Ls.x;
        LsJ.y = J->y - Ls.y; //Vektor L'J

        fprintf(fw, "Vektor L'I ma suradnice [x, y] = [%f, %f].\n", LsI.x, LsI.y);
        fprintf(fw, "Vektor L'J ma suradnice [x, y] = [%f, %f].\n", LsJ.x, LsJ.y);

        if ((J->x - I->x) == 0){
            p.k = 0.0000001;
            p.q = 0.0000001;
        }
        else {
            p.k = (J->y - I->y) / (J->x - I->x);
            p.q = I->y - p.k * I->x; //Priamka p vedena bodmi I a J
        }

        fprintf(fw, "Smernica priamky p ma hodnotu %f. Priamka p pretina os y v bode %f.\n", p.k, p.q);

        LsI.dlzka = sqrt(LsI.x * LsI.x + LsI.y * LsI.y); //Dlzka vektora L'I

        koef = (slim / LsI.dlzka); //Koeficient nasobenia
        if (koef < 1)
            koef = 1;

        fprintf(fw, "Dlzka vektora L'I je %f. Koeficient nasobenia je %f.\n", LsI.dlzka, koef);

        Is.x = Ls.x + LsI.x * koef;
        Is.y = Ls.y + LsI.y * koef; //Bod I'

        fprintf(fw, "Bod I' ma suradnice [x, y] = [%f, %f].\n", Is.x, Is.y);

        Js.x = Ls.x + LsJ.x * koef;
        Js.y = Ls.y + LsJ.y * koef; //Bod J'

        fprintf(fw, "Bod J' ma suradnice [x, y] = [%f, %f].\n", Js.x, Js.y);

        if (p.k == 0.0000001){
            a.k = 0;
            a.q = Is.y - a.k * Is.x;
        }
        else if (p.k == 0){
            a.k = 0.0000001;
            a.q = 0.0000001;
        }
        else {
            a.k = -1 / p.k;
            a.q = Is.y - a.k * Is.x; //Urcenie priamky a vedenej bodom I' a kolmej na priamku p
        }

        fprintf(fw, "Smernica priamky a ma hodnotu %f. Priamka a pretina os y v bode %f.\n", a.k, a.q);

        if (p.k == 0.0000001){
            b.k = 0;
            b.q = Js.y - b.k * Js.x;
        }
        else if (p.k == 0){
            b.k = 0.0000001;
            b.q = 0.0000001;
        }
        else {
            b.k = -1 / p.k;
            b.q = Js.y - b.k * Js.x; //Urcenie priamky b vedenej bodom J' a kolmej na priamku p
        }

        fprintf(fw, "Smernica priamky b ma hodnotu %f. Priamka b pretina os y v bode %f.\n", b.k, b.q);            

        for (int i = 0; i < pocet_bodov; i++){ //Urcenie Esigma IJ                      
            if (bodovepole[i] != I && bodovepole[i] != J){
                //fprintf(fw, "Ide %d\n", i);
                double da, db;
                int sgnda, sgndb;

                if (a.k == 0.0000001)
                    da = bodovepole[i]->x - Is.x;
                else
                    da = a.k * bodovepole[i]->x + a.q - bodovepole[i]->y;

                //fprintf (fw, "da = %f\n", da);

                if (da > 0)
                    sgnda = 1;
                else if (da == 0)
                    sgnda = 0;
                else if (da < 0)
                    sgnda = -1; //Urcenie sgnda

                //fprintf(fw, "sgnda = %d\n", sgnda);

                if (b.k == 0.0000001)
                    db = bodovepole[i]->x - Js.x;
                else
                    db = b.k * bodovepole[i]->x + b.q - bodovepole[i]->y;

                //fprintf (fw, "db = %f\n", db);

                if (db > 0)
                    sgndb = 1;
                else if (db == 0)
                    sgndb = 0;
                else if (db < 0)
                    sgndb = -1; //Urcenie sgndb

                //fprintf(fw, "sgndb = %d\n", sgndb);

                if (sgnda != sgndb){
                    if (zalozena == 0){
                        esigma_prvy = new Esigma();
                        esigma_akt = esigma_prvy;
                        zalozena = 1;
                    }
                    else {
                        esigma_akt->p_dalsi = new Esigma();
                        esigma_akt = esigma_akt->p_dalsi;
                    }

                    esigma_akt->cislobodu = i;
                    if (bodovepole[i]->stav == 2 || bodovepole[i]->kod_kompat[0] != 'r')
                        esigma_akt->prioritny = 1;
                    else
                        esigma_akt->prioritny = 0;
                    esigma_akt->p_dalsi = NULL;
                    //fprintf(fw, "IDE\n");

                }
            }
        }

        esigma_akt = esigma_prvy; //Redukcia mnoziny Esigma o body leziace na p a body s dp vacsou ako slim
        esigma_pred = esigma_prvy;
        while (esigma_akt != NULL){
            double dp;
            if (p.k == 0.0000001)
                dp = bodovepole[esigma_akt->cislobodu]->x - Ls.x;
            else
                dp = (p.k * bodovepole[esigma_akt->cislobodu]->x + p.q - bodovepole[esigma_akt->cislobodu]->y) / sqrt(p.k * p.k + 1);

            if (fabs(dp) == 0 || fabs(dp) > slim){
                if (esigma_akt == esigma_prvy){
                    esigma_prvy = esigma_prvy->p_dalsi;
                    delete esigma_akt;
                    esigma_akt = esigma_prvy;
                    esigma_pred = esigma_prvy;
                }
                else {
                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                    delete esigma_akt;
                    esigma_akt = esigma_pred->p_dalsi;
                }
            }
            else {
                esigma_pred = esigma_akt;
                esigma_akt = esigma_akt->p_dalsi;
            }
        }        

        esigma_akt = esigma_prvy; //Redukcia Esigma o body, ktorych vzdialenost od I alebo J je > slim
        esigma_pred = esigma_prvy;
        while (esigma_akt != NULL){
            double dlzkaI, dlzkaJ;
            dlzkaI = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - I->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - I->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - I->z, 2));
            dlzkaJ = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - J->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - J->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - J->z, 2));
            if (dlzkaI > slim || dlzkaJ > slim){
                if (esigma_akt == esigma_prvy){
                    esigma_prvy = esigma_prvy->p_dalsi;
                    delete esigma_akt;
                    esigma_akt = esigma_prvy;
                    esigma_pred = esigma_prvy;
                }
                else {
                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                    delete esigma_akt;
                    esigma_akt = esigma_pred->p_dalsi;
                }
            }
            else {
                esigma_pred = esigma_akt;
                esigma_akt = esigma_akt->p_dalsi;
            }
        }

        esigma_akt = esigma_prvy;
        while (esigma_akt != NULL){
            if (esigma_akt->prioritny){
                priortest = 1;
            }
            esigma_akt = esigma_akt->p_dalsi;
        }

        fprintf(fw, "V ESigma sa nachadzaju body cislo: ");
        esigma_akt = esigma_prvy;
        while (esigma_akt != NULL){
            fprintf(fw, "%d ", esigma_akt->cislobodu + 1);
            esigma_akt = esigma_akt->p_dalsi;
        }
        fprintf(fw, "\n");

        esigma_akt = esigma_prvy; //Vyber vhodneho bodu z Esigma na tvorbu trojuholnika
        if (priortest){
            int i = 1;
            while (esigma_akt != NULL){
                if (esigma_akt->prioritny){
                    double dlzkaI, dlzkaJ, suma;                    
                    dlzkaI = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - I->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - I->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - I->z, 2));
                    dlzkaJ = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - J->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - J->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - J->z, 2));
                    suma = dlzkaI + dlzkaJ;
                    if (i == 1)
                        sum_pam = dlzkaI + dlzkaJ;
                    if (suma <= sum_pam){
                        sum_pam = suma;
                        vhodny = esigma_akt->cislobodu;
                    }
                    i++;
                }                
                esigma_akt = esigma_akt->p_dalsi;
            }
        }
        else {
            int i = 1;
            while (esigma_akt != NULL){
                double dlzkaI, dlzkaJ, suma;
                dlzkaI = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - I->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - I->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - I->z, 2));
                dlzkaJ = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - J->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - J->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - J->z, 2));
                suma = dlzkaI + dlzkaJ;
                if (i == 1)
                    sum_pam = dlzkaI + dlzkaJ;
                if (suma >= sum_pam){
                    sum_pam = suma;
                    vhodny = esigma_akt->cislobodu;
                }
                esigma_akt = esigma_akt->p_dalsi;
                i++;
            }
        }

        if (vhodny != -1){ //Samotne vykreslenie trojuholnika, zapisanie informacie do TT a testovanie bodov "pod"
            iteruj = 1; //Premenna iteruj zabezpeci kontrolu stran v TT strukture, co bude dalej v kode
            L = bodovepole[vhodny];
            TT[trojuh - 1] = new TTstrukt();
            TT[trojuh - 1]->A1 = I->cislo;
            TT[trojuh - 1]->A2 = J->cislo;
            TT[trojuh - 1]->A3 = L->cislo;
            TT[trojuh - 1]->M1 = -1;
            TT[trojuh - 1]->M2 = -1;
            TT[trojuh - 1]->M3 = -1;
            TT[trojuh - 1]->n = trojuh;

            int pom;
            if (TT[trojuh - 1]->A2 < TT[trojuh - 1]->A1){
                pom = TT[trojuh - 1]->A1;
                TT[trojuh - 1]->A1 = TT[trojuh - 1]->A2;
                TT[trojuh - 1]->A2 = pom;
            }
            if (TT[trojuh - 1]->A3 < TT[trojuh - 1]->A1){
                pom = TT[trojuh - 1]->A1;
                TT[trojuh - 1]->A1 = TT[trojuh - 1]->A3;
                TT[trojuh - 1]->A3 = pom;
            }
            if (TT[trojuh - 1]->A3 < TT[trojuh - 1]->A2){
                pom = TT[trojuh - 1]->A2;
                TT[trojuh - 1]->A2 = TT[trojuh - 1]->A3;
                TT[trojuh - 1]->A3 = pom;
            } //Zapis informacie o trojuholniku do TT struktury a usporiadanie bodov od najmensieho po najvacsi

            fprintf(fw, "A1 = %d, A2 = %d, A3 = %d\n", bodovepole[TT[trojuh-1]->A1-1]->cislo, bodovepole[TT[trojuh-1]->A2-1]->cislo, bodovepole[TT[trojuh-1]->A3-1]->cislo);

            ui->graphicsView->scene()->addLine(bodovepole[TT[trojuh - 1]->A1 - 1]->x+2.5,-bodovepole[TT[trojuh - 1]->A1 - 1]->y+2.5,bodovepole[TT[trojuh - 1]->A2 - 1]->x+2.5,-bodovepole[TT[trojuh - 1]->A2 - 1]->y+2.5,QPen(Qt::black));
            ui->graphicsView->scene()->addLine(bodovepole[TT[trojuh - 1]->A2 - 1]->x+2.5,-bodovepole[TT[trojuh - 1]->A2 - 1]->y+2.5,bodovepole[TT[trojuh - 1]->A3 - 1]->x+2.5,-bodovepole[TT[trojuh - 1]->A3 - 1]->y+2.5,QPen(Qt::black));
            ui->graphicsView->scene()->addLine(bodovepole[TT[trojuh - 1]->A3 - 1]->x+2.5,-bodovepole[TT[trojuh - 1]->A3 - 1]->y+2.5,bodovepole[TT[trojuh - 1]->A1 - 1]->x+2.5,-bodovepole[TT[trojuh - 1]->A1 - 1]->y+2.5,QPen(Qt::black));           

            bodovepole[TT[trojuh - 1]->A1-1]->stav = 2;
            bodovepole[TT[trojuh - 1]->A2-1]->stav = 2;
            bodovepole[TT[trojuh - 1]->A3-1]->stav = 2;

            esigma_akt = esigma_prvy; //Test bodov z Esigma, ci sa nachadzaju pod trojuholnikom a ich vylucovanie
            while (esigma_akt != NULL){
                Priamka i, j, l;
                double diM, djM, dlM, diI, djJ, dlL;
                int sgndiM, sgndjM, sgndlM, sgndiI, sgndjJ, sgndlL;

                if ((bodovepole[TT[trojuh-1]->A2-1]->x - bodovepole[TT[trojuh-1]->A1-1]->x) == 0){ //Urcenie priamky l
                    l.k = 0.0000001;
                    l.q = 0.0000001;
                }
                else {
                    l.k = (bodovepole[TT[trojuh-1]->A2-1]->y - bodovepole[TT[trojuh-1]->A1-1]->y) / (bodovepole[TT[trojuh-1]->A2-1]->x - bodovepole[TT[trojuh-1]->A1-1]->x);
                    l.q = bodovepole[TT[trojuh-1]->A1-1]->y - l.k * bodovepole[TT[trojuh-1]->A1-1]->x;
                }

                if (l.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov L a M od priamky
                    dlL = bodovepole[TT[trojuh-1]->A3-1]->x - bodovepole[TT[trojuh-1]->A1-1]->x;
                    dlM = bodovepole[esigma_akt->cislobodu]->x - bodovepole[TT[trojuh-1]->A1-1]->x;
                }
                else {
                    dlL = l.k * bodovepole[TT[trojuh-1]->A3-1]->x + l.q - bodovepole[TT[trojuh-1]->A3-1]->y;
                    dlM = l.k * bodovepole[esigma_akt->cislobodu]->x + l.q - bodovepole[esigma_akt->cislobodu]->y;
                }

                if (dlL > 0)
                    sgndlL = 1;
                else if (dlL == 0)
                    sgndlL = 0;
                else if (dlL < 0)
                    sgndlL = -1;

                if (dlM > 0)
                    sgndlM = 1;
                else if (dlM == 0)
                    sgndlM = 0;
                else if (dlM < 0)
                    sgndlM = -1;

                if (sgndlM == 0){
                    bodovepole[esigma_akt->cislobodu]->stav = 0;
                    esigma_akt = esigma_akt->p_dalsi;
                    continue;
                }

                if ((bodovepole[TT[trojuh-1]->A3-1]->x - bodovepole[TT[trojuh-1]->A2-1]->x) == 0){ //Urcenie priamky i
                    i.k = 0.0000001;
                    i.q = 0.0000001;
                }
                else {
                    i.k = (bodovepole[TT[trojuh-1]->A3-1]->y - bodovepole[TT[trojuh-1]->A2-1]->y) / (bodovepole[TT[trojuh-1]->A3-1]->x - bodovepole[TT[trojuh-1]->A2-1]->x);
                    i.q = bodovepole[TT[trojuh-1]->A2-1]->y - i.k * bodovepole[TT[trojuh-1]->A2-1]->x;
                }

                if (i.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov I a M od priamky
                    diI = bodovepole[TT[trojuh-1]->A1-1]->x - bodovepole[TT[trojuh-1]->A2-1]->x;
                    diM = bodovepole[esigma_akt->cislobodu]->x - bodovepole[TT[trojuh-1]->A2-1]->x;
                }
                else {
                    diI = i.k * bodovepole[TT[trojuh-1]->A1-1]->x + i.q - bodovepole[TT[trojuh-1]->A1-1]->y;
                    diM = i.k * bodovepole[esigma_akt->cislobodu]->x + i.q - bodovepole[esigma_akt->cislobodu]->y;
                }

                if (diI > 0)
                    sgndiI = 1;
                else if (diI == 0)
                    sgndiI = 0;
                else if (diI < 0)
                    sgndiI = -1;

                if (diM > 0)
                    sgndiM = 1;
                else if (diM == 0)
                    sgndiM = 0;
                else if (diM < 0)
                    sgndiM = -1;

                if (sgndiM == 0){
                    bodovepole[esigma_akt->cislobodu]->stav = 0;
                    esigma_akt = esigma_akt->p_dalsi;
                    continue;
                }

                if ((bodovepole[TT[trojuh-1]->A1-1]->x - bodovepole[TT[trojuh-1]->A3-1]->x) == 0){ //Urcenie priamky j
                    j.k = 0.0000001;
                    j.q = 0.0000001;
                }
                else {
                    j.k = (bodovepole[TT[trojuh-1]->A1-1]->y - bodovepole[TT[trojuh-1]->A3-1]->y) / (bodovepole[TT[trojuh-1]->A1-1]->x - bodovepole[TT[trojuh-1]->A3-1]->x);
                    j.q = bodovepole[TT[trojuh-1]->A3-1]->y - j.k * bodovepole[TT[trojuh-1]->A3-1]->x;
                }

                if (j.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov L a M od priamky
                    djJ = bodovepole[TT[trojuh-1]->A2-1]->x - bodovepole[TT[trojuh-1]->A3-1]->x;
                    djM = bodovepole[esigma_akt->cislobodu]->x - bodovepole[TT[trojuh-1]->A3-1]->x;
                }
                else {
                    djJ = j.k * bodovepole[TT[trojuh-1]->A2-1]->x + j.q - bodovepole[TT[trojuh-1]->A2-1]->y;
                    djM = j.k * bodovepole[esigma_akt->cislobodu]->x + j.q - bodovepole[esigma_akt->cislobodu]->y;
                }

                if (djJ > 0)
                    sgndjJ = 1;
                else if (djJ == 0)
                    sgndjJ = 0;
                else if (djJ < 0)
                    sgndjJ = -1;

                if (djM > 0)
                    sgndjM = 1;
                else if (djM == 0)
                    sgndjM = 0;
                else if (djM < 0)
                    sgndjM = -1;

                if (sgndjM == 0){
                    bodovepole[esigma_akt->cislobodu]->stav = 0;
                    esigma_akt = esigma_akt->p_dalsi;
                    continue;
                }

                if (sgndlM == sgndlL && sgndiM == sgndiI && sgndjM == sgndjJ){
                    bodovepole[esigma_akt->cislobodu]->stav = 0;
                    esigma_akt = esigma_akt->p_dalsi;
                }
                else {
                    esigma_akt = esigma_akt->p_dalsi;
                    fprintf(fw, "Nespravne vyhodnocuje.\n");
                }
            }
        }
        else
            ui->label_2->setText("Nenašiel sa žiadny vhodný bod na vytvorenie prvého trojuholníka. Skúste zmeni slim na vaäèšiu hodnotu.");

        esigma_akt = esigma_prvy; //Zaverecne uvolnenie pamate
        while (esigma_akt != NULL){
            esigma_akt = esigma_akt->p_dalsi;
            delete esigma_prvy;
            esigma_prvy = esigma_akt;
        }

        //Koniec tvorby prveho trojuholnika------------------------------------------------------------------------

        while (iteruj == 1){                        
            int *p_strana = &TT[trojuh-1]->M3, clen1, clen2;
            TTstrukt *troj_sigmaI[50], *troj_sigmaJ[50], *troj_sigma[50];

            for (int strana = 1; strana <= 3; strana++){
                if (*p_strana == -1){
                                fprintf(fw, "\n%d. TOJUHOLNIK\n", k+1);
                                //fprintf(fw, "Stav bodu 183 je %d\n", bodovepole[182]->stav);
                    if (strana == 1){
                        clen1 = TT[trojuh-1]->A1-1;
                        clen2 = TT[trojuh-1]->A2-1;
                    }
                    else if (strana == 2){
                        clen1 = TT[trojuh-1]->A1-1;
                        clen2 = TT[trojuh-1]->A3-1;
                    }
                    else if (strana == 3){
                        clen1 = TT[trojuh-1]->A2-1;
                        clen2 = TT[trojuh-1]->A3-1;
                    }

                    //Proces, ako pri tvorbe prveho trojuholnika
                    Bod_pomoc Ls, Is, Js;
                    Vektor LsI, LsJ;
                    Priamka p, a, b;
                    double koef, sum_pam;
                    int vhodny = -1, priortest = 0, zalozena;

                    esigma_prvy = NULL;

                    I = bodovepole[clen1];
                    J = bodovepole[clen2];

                    fprintf(fw, "Bod I ma cislo %d, bod J ma cislo %d\n", I->cislo, J->cislo);

                    Ls.x = (I->x + J->x) / 2;
                    Ls.y = (I->y + J->y) / 2; //Urcenie L'

                    //fprintf(fw, "Bod L' ma suradnice [x, y] = [%f, %f].\n", Ls.x, Ls.y);

                    LsI.x = I->x - Ls.x;
                    LsI.y = I->y - Ls.y; //Vektor L'I

                    LsJ.x = J->x - Ls.x;
                    LsJ.y = J->y - Ls.y; //Vektor L'J

                    //fprintf(fw, "Vektor L'I ma suradnice [x, y] = [%f, %f].\n", LsI.x, LsI.y);
                    //fprintf(fw, "Vektor L'J ma suradnice [x, y] = [%f, %f].\n", LsJ.x, LsJ.y);

                    if ((J->x - I->x) == 0){
                        p.k = 0.0000001;
                        p.q = 0.0000001;
                    }
                    else {
                        p.k = (J->y - I->y) / (J->x - I->x);
                        p.q = I->y - p.k * I->x; //Priamka p vedena bodmi I a J
                    }

                    //fprintf(fw, "Smernica priamky p ma hodnotu %f. Priamka p pretina os y v bode %f.\n", p.k, p.q);

                    LsI.dlzka = sqrt(LsI.x * LsI.x + LsI.y * LsI.y); //Dlzka vektora L'I

                    koef = (slim / LsI.dlzka); //Koeficient nasobenia
                    if (koef < 1)
                        koef = 1;

                    //fprintf(fw, "Dlzka vektora L'I je %f. Koeficient nasobenia je %f.\n", LsI.dlzka, koef);

                    Is.x = Ls.x + LsI.x * koef;
                    Is.y = Ls.y + LsI.y * koef; //Bod I'

                    //fprintf(fw, "Bod I' ma suradnice [x, y] = [%f, %f].\n", Is.x, Is.y);

                    Js.x = Ls.x + LsJ.x * koef;
                    Js.y = Ls.y + LsJ.y * koef; //Bod J'

                    dvasig.x = Js.x - Is.x;
                    dvasig.y = Js.y - Is.y;
                    dvasig.dlzka = sqrt(dvasig.x * dvasig.x + dvasig.y * dvasig.y);
                    fprintf(fw, "Dlzka vektora I'J' = %f\n", dvasig.dlzka);

                    //fprintf(fw, "Bod J' ma suradnice [x, y] = [%f, %f].\n", Js.x, Js.y);

                    if (p.k == 0.0000001){
                        a.k = 0;
                        a.q = Is.y - a.k * Is.x;
                    }
                    else if (p.k == 0){
                        a.k = 0.0000001;
                        a.q = 0.0000001;
                    }
                    else {
                        a.k = -1 / p.k;
                        a.q = Is.y - a.k * Is.x; //Urcenie priamky a vedenej bodom I' a kolmej na priamku p
                    }

                    //fprintf(fw, "Smernica priamky a ma hodnotu %f. Priamka a pretina os y v bode %f.\n", a.k, a.q);

                    if (p.k == 0.0000001){
                        b.k = 0;
                        b.q = Js.y - b.k * Js.x;
                    }
                    else if (p.k == 0){
                        b.k = 0.0000001;
                        b.q = 0.0000001;
                    }
                    else {
                        b.k = -1 / p.k;
                        b.q = Js.y - b.k * Js.x; //Urcenie priamky b vedenej bodom J' a kolmej na priamku p
                    }

                    //fprintf(fw, "Smernica priamky b ma hodnotu %f. Priamka b pretina os y v bode %f.\n", b.k, b.q);

                    esigma_prvy = NULL;
                    zalozena = 0;
                    for (int i = 0; i < pocet_bodov; i++){ //Urcenie Esigma IJ
                        if (bodovepole[i] != I && bodovepole[i] != J){
                            //fprintf(fw, "Ide %d\n", i);
                            double da, db;
                            int sgnda, sgndb;

                            if (a.k == 0.0000001)
                                da = bodovepole[i]->x - Is.x;
                            else
                                da = a.k * bodovepole[i]->x + a.q - bodovepole[i]->y;

                            //fprintf (fw, "da = %f\n", da);

                            if (da > 0)
                                sgnda = 1;
                            else if (da == 0)
                                sgnda = 0;
                            else if (da < 0)
                                sgnda = -1; //Urcenie sgnda

                            //fprintf(fw, "sgnda = %d\n", sgnda);

                            if (b.k == 0.0000001)
                                db = bodovepole[i]->x - Js.x;
                            else
                                db = b.k * bodovepole[i]->x + b.q - bodovepole[i]->y;

                            //fprintf (fw, "db = %f\n", db);

                            if (db > 0)
                                sgndb = 1;
                            else if (db == 0)
                                sgndb = 0;
                            else if (db < 0)
                                sgndb = -1; //Urcenie sgndb

                            //fprintf(fw, "sgndb = %d\n", sgndb);

                            if (sgnda != sgndb){
                                if (zalozena == 0){
                                    esigma_prvy = new Esigma();
                                    esigma_akt = esigma_prvy;
                                    zalozena = 1;
                                }
                                else {
                                    esigma_akt->p_dalsi = new Esigma();
                                    esigma_akt = esigma_akt->p_dalsi;
                                }

                                esigma_akt->cislobodu = i;
                                if (bodovepole[i]->stav == 2 || bodovepole[i]->kod_kompat[0] != 'r')
                                    esigma_akt->prioritny = 1;
                                else
                                    esigma_akt->prioritny = 0;
                                esigma_akt->p_dalsi = NULL;
                                //fprintf(fw, "IDE\n");

                            }
                        }
                    }                    

                    int j = 0; //Zistenie trojuholnikov, ktorym su body I a J spolocne okrem vybraneho trojuholnika
                    for (int i = 0; i < k; i++){
                        if (i != trojuh-1){
                            if (TT[i]->A1 == I->cislo){
                                troj_sigmaI[j] = TT[i];
                                j++;
                            }
                            else if (TT[i]->A2 == I->cislo){
                                troj_sigmaI[j] = TT[i];
                                j++;
                            }
                            else if (TT[i]->A3 == I->cislo){
                                troj_sigmaI[j] = TT[i];
                                j++;
                            }
                        }
                    }
                    for ( ; j < 50; j++)
                        troj_sigmaI[j] = NULL;

                    for (int i = 0; troj_sigmaI[i] != NULL; i++)
                        fprintf(fw, "Trojuholnik cislo %d je susedny\n", troj_sigmaI[i]->n);

                    j = 0;
                    for (int i = 0; i < k; i++){
                        if (i != trojuh-1){
                            if (TT[i]->A1 == J->cislo){
                                troj_sigmaJ[j] = TT[i];
                                j++;
                            }
                            else if (TT[i]->A2 == J->cislo){
                                troj_sigmaJ[j] = TT[i];
                                j++;
                            }
                            else if (TT[i]->A3 == J->cislo){
                                troj_sigmaJ[j] = TT[i];
                                j++;
                            }
                        }
                    }
                    for ( ; j < 50; j++)
                        troj_sigmaJ[j] = NULL;


                    for (int i = 0; troj_sigmaJ[i] != NULL; i++)
                        fprintf(fw, "Trojuholnik cislo %d je susedny\n", troj_sigmaJ[i]->n);                    

                    esigma_akt = esigma_prvy; //Redukcia mnoziny Esigma o body leziace na p a v polrovine tretieho bodu trojuholnika a body s dp vacsou ako slim
                    esigma_pred = esigma_prvy;
                    while (esigma_akt != NULL){
                        double dp, dpL;
                        int sgndp, sgndpL;
                        if (p.k == 0.0000001){
                            dp = bodovepole[esigma_akt->cislobodu]->x - Ls.x;
                            if (strana == 1)
                                dpL = bodovepole[TT[trojuh-1]->A3-1]->x - Ls.x;
                            else if (strana == 2)
                                dpL = bodovepole[TT[trojuh-1]->A2-1]->x - Ls.x;
                            else if (strana == 3)
                                dpL = bodovepole[TT[trojuh-1]->A1-1]->x - Ls.x;
                        }
                        else {
                            dp = (p.k * bodovepole[esigma_akt->cislobodu]->x + p.q - bodovepole[esigma_akt->cislobodu]->y) / sqrt(p.k * p.k + 1);
                            if (strana == 1)
                                dpL = (p.k * bodovepole[TT[trojuh-1]->A3-1]->x + p.q - bodovepole[TT[trojuh-1]->A3-1]->y) / sqrt(p.k * p.k + 1);
                            else if (strana == 2)
                                dpL = (p.k * bodovepole[TT[trojuh-1]->A2-1]->x + p.q - bodovepole[TT[trojuh-1]->A2-1]->y) / sqrt(p.k * p.k + 1);
                            else if (strana == 3)
                                dpL = (p.k * bodovepole[TT[trojuh-1]->A1-1]->x + p.q - bodovepole[TT[trojuh-1]->A1-1]->y) / sqrt(p.k * p.k + 1);
                        }

                        if (bodovepole[esigma_akt->cislobodu] == I || bodovepole[esigma_akt->cislobodu] == J)
                            dp = 0;

                        if (dp > 0)
                            sgndp = 1;
                        else if (dp == 0)
                            sgndp = 0;
                        else if (dp < 0)
                            sgndp = -1;

                        if (dpL > 0)
                            sgndpL = 1;
                        else if (dpL == 0)
                            sgndpL = 0;
                        else if (dpL < 0)
                            sgndpL = -1;

                        //if (k = 64){
                        if(I == bodovepole[esigma_akt->cislobodu])
                            fprintf(fw, "Vzdialenost bodu I druheho trojuholnika od priamky p je %f\n", dp);
                        if(J == bodovepole[esigma_akt->cislobodu])
                            fprintf(fw, "Vzdialenost bodu J druheho trojuholnika od priamky p je %f\n", dp);
                        //}

                        if (fabs(dp) == 0 || fabs(dp) > slim || sgndp == sgndpL){
                            if (esigma_akt == esigma_prvy){
                                esigma_prvy = esigma_prvy->p_dalsi;
                                delete esigma_akt;
                                esigma_akt = esigma_prvy;
                                esigma_pred = esigma_prvy;
                            }
                            else {
                                esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                delete esigma_akt;
                                esigma_akt = esigma_pred->p_dalsi;
                            }
                        }
                        else {
                            esigma_pred = esigma_akt;
                            esigma_akt = esigma_akt->p_dalsi;
                        }
                    }

                    esigma_akt = esigma_prvy; //Redukcia Esigma o body, ktorych stav je 0
                    esigma_pred = esigma_prvy;
                    while (esigma_akt != NULL){
                        if (bodovepole[esigma_akt->cislobodu]->stav == 0){
                            if (esigma_akt == esigma_prvy){
                                esigma_prvy = esigma_prvy->p_dalsi;
                                delete esigma_akt;
                                esigma_akt = esigma_prvy;
                                esigma_pred = esigma_prvy;
                            }
                            else {
                                esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                delete esigma_akt;
                                esigma_akt = esigma_pred->p_dalsi;
                            }
                        }
                        else {
                            esigma_pred = esigma_akt;
                            esigma_akt = esigma_akt->p_dalsi;
                        }
                    }

                    esigma_akt = esigma_prvy;//Zistenie, ktore body z Esigma patria uz nejakemu trojuholniku
                    esigma_pred = esigma_prvy;
                    esigma_pr_prvy = new Esigma();
                    esigma_pr_prvy->cislobodu = -1;
                    zalozena = 0;
                    fprintf(fw, "Cisla bodov z esigma, ktore patria trojuholniku: ");
                    while (esigma_akt != NULL){
                        if (bodovepole[esigma_akt->cislobodu]->stav == 2){
                            //fprintf(fw, "STAV2 a zalozena = %d\n", zalozena);
                            if (zalozena == 0){
                                esig_pr_akt = esigma_pr_prvy;
                                zalozena = 1;
                            }
                            else{
                                esig_pr_akt->p_dalsi = new Esigma();
                                esig_pr_akt = esig_pr_akt->p_dalsi;
                            }
                            esig_pr_akt->cislobodu = bodovepole[esigma_akt->cislobodu]->cislo;
                            fprintf(fw, "%d ", esig_pr_akt->cislobodu);
                            esig_pr_akt->p_dalsi = NULL;
                        }
                        esigma_akt = esigma_akt->p_dalsi;
                    }
                    fprintf(fw, "\n");

                    esigma_akt = esigma_prvy; //Redukcia Esigma o body, ktorych vzdialenost od I alebo J je > slim
                    esigma_pred = esigma_prvy;
                    while (esigma_akt != NULL){
                        double dlzkaI, dlzkaJ;
                        dlzkaI = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - I->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - I->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - I->z, 2));
                        dlzkaJ = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - J->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - J->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - J->z, 2));
                        if (dlzkaI > slim || dlzkaJ > slim){
                            if (esigma_akt == esigma_prvy){
                                esigma_prvy = esigma_prvy->p_dalsi;
                                delete esigma_akt;
                                esigma_akt = esigma_prvy;
                                esigma_pred = esigma_prvy;
                            }
                            else {
                                esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                delete esigma_akt;
                                esigma_akt = esigma_pred->p_dalsi;
                            }
                        }
                        else {
                            esigma_pred = esigma_akt;
                            esigma_akt = esigma_akt->p_dalsi;
                        }
                    }

                    esigma_akt = esigma_prvy; //Redukcia Esigma o prioritne body, s ktorymi nebude mozne vytvorit spojenie pre I
                    esigma_pred = esigma_prvy;
                    while (esigma_akt != NULL){
                        int vyluc = 0;
                        if (esigma_akt->prioritny){
                            for (int i = 0; troj_sigmaI[i] != NULL; i++){
                                if (bodovepole[esigma_akt->cislobodu]->cislo == troj_sigmaI[i]->A1)
                                    vyluc++;
                                else if (bodovepole[esigma_akt->cislobodu]->cislo == troj_sigmaI[i]->A2)
                                    vyluc++;
                                else if (bodovepole[esigma_akt->cislobodu]->cislo == troj_sigmaI[i]->A3)
                                    vyluc++;
                            }
                            if (vyluc >= 2){
                                if (esigma_akt == esigma_prvy){
                                    esigma_prvy = esigma_prvy->p_dalsi;
                                    delete esigma_akt;
                                    esigma_akt = esigma_prvy;
                                    esigma_pred = esigma_prvy;
                                }
                                else {
                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                    delete esigma_akt;
                                    esigma_akt = esigma_pred->p_dalsi;
                                }
                            }
                            else {
                                esigma_pred = esigma_akt;
                                esigma_akt = esigma_akt->p_dalsi;
                            }
                        }
                        else {
                            esigma_akt = esigma_akt->p_dalsi;
                        }
                    }

                    esigma_akt = esigma_prvy; //Redukcia Esigma o prioritne body, s ktorymi nebude mozne vytvorit spojenie pre J
                    esigma_pred = esigma_prvy;
                    while (esigma_akt != NULL){
                        int vyluc = 0;
                        if (esigma_akt->prioritny){
                            for (int i = 0; troj_sigmaJ[i] != NULL; i++){
                                if (bodovepole[esigma_akt->cislobodu]->cislo == troj_sigmaJ[i]->A1)
                                    vyluc++;
                                else if (bodovepole[esigma_akt->cislobodu]->cislo == troj_sigmaJ[i]->A2)
                                    vyluc++;
                                else if (bodovepole[esigma_akt->cislobodu]->cislo == troj_sigmaJ[i]->A3)
                                    vyluc++;
                            }
                            if (vyluc >= 2){
                                if (esigma_akt == esigma_prvy){
                                    esigma_prvy = esigma_prvy->p_dalsi;
                                    delete esigma_akt;
                                    esigma_akt = esigma_prvy;
                                    esigma_pred = esigma_prvy;
                                }
                                else {
                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                    delete esigma_akt;
                                    esigma_akt = esigma_pred->p_dalsi;
                                }
                            }
                            else {
                                esigma_pred = esigma_akt;
                                esigma_akt = esigma_akt->p_dalsi;
                            }
                        }
                        else {
                            esigma_akt = esigma_akt->p_dalsi;
                        }
                    }

                    esigma_akt = esigma_prvy;
                    while (esigma_akt != NULL){                                                    
                        if (esigma_akt->prioritny){
                            priortest = 1;
                        }
                        esigma_akt = esigma_akt->p_dalsi;
                    }

                    fprintf(fw, "priortest = %d\n", priortest);

                    /*j = 0;//Hladanie a vylucenie vsetkych potencialnych bodov leziacich za uz jestvujucou stranou trojuholnika
                    if (priortest == 0 && esigma_pr_prvy->cislobodu != -1){
                        int troj_pole[15];
                        bool mozno = 1;
                        for (int i = 0; i < 15; i++)
                            troj_pole[i] = -1;
                        esig_pr_akt = esigma_pr_prvy;
                        while (esig_pr_akt != NULL){
                            for (int i = 0; i < k; i++){
                                if (esig_pr_akt->cislobodu == TT[i]->A1 || esig_pr_akt->cislobodu == TT[i]->A2 || esig_pr_akt->cislobodu == TT[i]->A3){
                                    for (int q = 0; troj_pole[q] != -1; q++){
                                        if (troj_pole[q] == TT[i]->n){
                                            mozno = 0;
                                            break;
                                        }
                                        else
                                            mozno = 1;
                                    }
                                    if (mozno){
                                        troj_sigma[j] = TT[i];
                                        troj_pole[j] = TT[i]->n;
                                        j++;
                                    }
                                }
                            }
                            esig_pr_akt = esig_pr_akt->p_dalsi;
                        }

                        for ( ; j < 50; j++){
                            troj_sigma[j] = NULL;
                        }
                        fprintf(fw, "Priortest = 0 && esigma_pr_prvy != -1\n");
                        for (int i = 0; troj_sigma[i] != NULL; i++)
                            fprintf(fw, "troj %d ", troj_sigma[i]->n);
                        fprintf(fw, "\n");

                        double drQ, drI, drJ;
                        int sgndrQ, sgndrI, sgndrJ;
                        esig_pr_akt = esigma_pr_prvy;
                        while (esig_pr_akt != NULL){
                            fprintf(fw, "IDE pr_akt\n");
                            j = 0;
                            while (troj_sigma[j] != NULL){
                                fprintf(fw, "IDE troj_sigma j = %d\n", j);
                                if (esig_pr_akt->cislobodu == troj_sigma[j]->A1){
                                    fprintf(fw, "IDE A1\n");
                                    if ((bodovepole[troj_sigma[j]->A2 - 1]->x - bodovepole[troj_sigma[j]->A1 - 1]->x) == 0){
                                        r1.k = 0.0000001;
                                        r1.q = 0.0000001;
                                    }
                                    else {
                                        r1.k = (bodovepole[troj_sigma[j]->A2 - 1]->y - bodovepole[troj_sigma[j]->A1 - 1]->y) / (bodovepole[troj_sigma[j]->A2 - 1]->x - bodovepole[troj_sigma[j]->A1 - 1]->x);
                                        r1.q = bodovepole[troj_sigma[j]->A1 - 1]->y - r1.k * bodovepole[troj_sigma[j]->A1 - 1]->x; //Priamka r1 vedena bodmi A1 a A2
                                    }
                                    if (r1.k == 0.0000001){
                                        drI = I->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                        drJ = J->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                    }
                                    else{
                                        drI = r1.k * I->x + r1.q - I->y;
                                        drJ = r1.k * J->x + r1.q - J->y;
                                    }
                                    if (drI > 0)
                                        sgndrI = 1;
                                    else if (drI == 0)
                                        sgndrI = 0;
                                    else if (drI < 0)
                                        sgndrI = -1;

                                    if (drJ > 0)
                                        sgndrJ = 1;
                                    else if (drJ == 0)
                                        sgndrJ = 0;
                                    else if (drJ < 0)
                                        sgndrJ = -1;

                                    if (sgndrI == sgndrJ){
                                        esigma_akt = esigma_prvy;
                                        esigma_pred = esigma_prvy;
                                        while (esigma_akt != NULL){
                                            if (r1.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov z Esigma od priamky r1
                                                drQ = bodovepole[esigma_akt->cislobodu]->x - bodovepole[troj_sigma[j]->A1-1]->x;
                                            }
                                            else {
                                                drQ = r1.k * bodovepole[esigma_akt->cislobodu]->x + r1.q - bodovepole[esigma_akt->cislobodu]->y;
                                            }

                                            if (drQ > 0)
                                                sgndrQ = 1;
                                            else if (drQ == 0)
                                                sgndrQ = 0;
                                            else if (drQ < 0)
                                                sgndrQ = -1;

                                            if (sgndrQ != sgndrI){
                                                if (esigma_akt == esigma_prvy){
                                                    esigma_prvy = esigma_prvy->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_prvy;
                                                    esigma_pred = esigma_prvy;
                                                }
                                                else {
                                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_pred->p_dalsi;
                                                }
                                            }
                                            else {
                                                esigma_pred = esigma_akt;
                                                esigma_akt = esigma_akt->p_dalsi;
                                            }
                                        }
                                    }
                                    if ((bodovepole[troj_sigma[j]->A3 - 1]->x - bodovepole[troj_sigma[j]->A1 - 1]->x) == 0){
                                        r2.k = 0.0000001;
                                        r2.q = 0.0000001;
                                    }
                                    else {
                                        r2.k = (bodovepole[troj_sigma[j]->A3 - 1]->y - bodovepole[troj_sigma[j]->A1 - 1]->y) / (bodovepole[troj_sigma[j]->A3 - 1]->x - bodovepole[troj_sigma[j]->A1 - 1]->x);
                                        r2.q = bodovepole[troj_sigma[j]->A1 - 1]->y - r1.k * bodovepole[troj_sigma[j]->A1 - 1]->x; //Priamka r1 vedena bodmi A1 a A3
                                    }
                                    if (r2.k == 0.0000001){
                                        drI = I->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                        drJ = J->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                    }
                                    else{
                                        drI = r2.k * I->x + r2.q - I->y;
                                        drJ = r2.k * J->x + r2.q - J->y;
                                    }
                                    if (drI > 0)
                                        sgndrI = 1;
                                    else if (drI == 0)
                                        sgndrI = 0;
                                    else if (drI < 0)
                                        sgndrI = -1;

                                    if (drJ > 0)
                                        sgndrJ = 1;
                                    else if (drJ == 0)
                                        sgndrJ = 0;
                                    else if (drJ < 0)
                                        sgndrJ = -1;

                                    if (sgndrI == sgndrJ){
                                        esigma_akt = esigma_prvy;
                                        esigma_pred = esigma_prvy;
                                        while (esigma_akt != NULL){
                                            if (r2.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov z Esigma od priamky r2
                                                drQ = bodovepole[esigma_akt->cislobodu]->x - bodovepole[troj_sigma[j]->A1-1]->x;
                                            }
                                            else {
                                                drQ = r2.k * bodovepole[esigma_akt->cislobodu]->x + r2.q - bodovepole[esigma_akt->cislobodu]->y;
                                            }

                                            if (drQ > 0)
                                                sgndrQ = 1;
                                            else if (drQ == 0)
                                                sgndrQ = 0;
                                            else if (drQ < 0)
                                                sgndrQ = -1;

                                            if (sgndrQ != sgndrI){
                                                if (esigma_akt == esigma_prvy){
                                                    esigma_prvy = esigma_prvy->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_prvy;
                                                    esigma_pred = esigma_prvy;
                                                }
                                                else {
                                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_pred->p_dalsi;
                                                }
                                            }
                                            else {
                                                esigma_pred = esigma_akt;
                                                esigma_akt = esigma_akt->p_dalsi;
                                            }
                                        }
                                    }
                                }

                                if (esig_pr_akt->cislobodu == troj_sigma[j]->A2){
                                    fprintf(fw, "IDE A2\n");
                                    if ((bodovepole[troj_sigma[j]->A1 - 1]->x - bodovepole[troj_sigma[j]->A2 - 1]->x) == 0){
                                        r1.k = 0.0000001;
                                        r1.q = 0.0000001;
                                    }
                                    else {
                                        r1.k = (bodovepole[troj_sigma[j]->A1 - 1]->y - bodovepole[troj_sigma[j]->A2 - 1]->y) / (bodovepole[troj_sigma[j]->A1 - 1]->x - bodovepole[troj_sigma[j]->A2 - 1]->x);
                                        r1.q = bodovepole[troj_sigma[j]->A2 - 1]->y - r1.k * bodovepole[troj_sigma[j]->A2 - 1]->x; //Priamka r1 vedena bodmi A2 a A1
                                    }
                                    if (r1.k == 0.0000001){
                                        drI = I->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                        drJ = J->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                    }
                                    else{
                                        drI = r1.k * I->x + r1.q - I->y;
                                        drJ = r1.k * J->x + r1.q - J->y;
                                    }
                                    if (drI > 0)
                                        sgndrI = 1;
                                    else if (drI == 0)
                                        sgndrI = 0;
                                    else if (drI < 0)
                                        sgndrI = -1;

                                    if (drJ > 0)
                                        sgndrJ = 1;
                                    else if (drJ == 0)
                                        sgndrJ = 0;
                                    else if (drJ < 0)
                                        sgndrJ = -1;

                                    if (sgndrI == sgndrJ){
                                        esigma_akt = esigma_prvy;
                                        esigma_pred = esigma_prvy;
                                        while (esigma_akt != NULL){
                                            if (r1.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov z Esigma od priamky r1
                                                drQ = bodovepole[esigma_akt->cislobodu]->x - bodovepole[troj_sigma[j]->A2-1]->x;
                                            }
                                            else {
                                                drQ = r1.k * bodovepole[esigma_akt->cislobodu]->x + r1.q - bodovepole[esigma_akt->cislobodu]->y;
                                            }

                                            if (drQ > 0)
                                                sgndrQ = 1;
                                            else if (drQ == 0)
                                                sgndrQ = 0;
                                            else if (drQ < 0)
                                                sgndrQ = -1;

                                            if (sgndrQ != sgndrI){
                                                if (esigma_akt == esigma_prvy){
                                                    esigma_prvy = esigma_prvy->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_prvy;
                                                    esigma_pred = esigma_prvy;
                                                }
                                                else {
                                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_pred->p_dalsi;
                                                }
                                            }
                                            else {
                                                esigma_pred = esigma_akt;
                                                esigma_akt = esigma_akt->p_dalsi;
                                            }
                                        }
                                    }
                                    if ((bodovepole[troj_sigma[j]->A3 - 1]->x - bodovepole[troj_sigma[j]->A2 - 1]->x) == 0){
                                        r2.k = 0.0000001;
                                        r2.q = 0.0000001;
                                    }
                                    else {
                                        r2.k = (bodovepole[troj_sigma[j]->A3 - 1]->y - bodovepole[troj_sigma[j]->A2 - 1]->y) / (bodovepole[troj_sigma[j]->A3 - 1]->x - bodovepole[troj_sigma[j]->A2 - 1]->x);
                                        r2.q = bodovepole[troj_sigma[j]->A2 - 1]->y - r1.k * bodovepole[troj_sigma[j]->A2 - 1]->x; //Priamka r1 vedena bodmi A2 a A3
                                    }
                                    if (r2.k == 0.0000001){
                                        drI = I->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                        drJ = J->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                    }
                                    else{
                                        drI = r2.k * I->x + r2.q - I->y;
                                        drJ = r2.k * J->x + r2.q - J->y;
                                    }
                                    if (drI > 0)
                                        sgndrI = 1;
                                    else if (drI == 0)
                                        sgndrI = 0;
                                    else if (drI < 0)
                                        sgndrI = -1;

                                    if (drJ > 0)
                                        sgndrJ = 1;
                                    else if (drJ == 0)
                                        sgndrJ = 0;
                                    else if (drJ < 0)
                                        sgndrJ = -1;

                                    if (sgndrI == sgndrJ){
                                        esigma_akt = esigma_prvy;
                                        esigma_pred = esigma_prvy;
                                        while (esigma_akt != NULL){
                                            if (r2.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov z Esigma od priamky r2
                                                drQ = bodovepole[esigma_akt->cislobodu]->x - bodovepole[troj_sigma[j]->A2-1]->x;
                                            }
                                            else {
                                                drQ = r2.k * bodovepole[esigma_akt->cislobodu]->x + r2.q - bodovepole[esigma_akt->cislobodu]->y;
                                            }

                                            if (drQ > 0)
                                                sgndrQ = 1;
                                            else if (drQ == 0)
                                                sgndrQ = 0;
                                            else if (drQ < 0)
                                                sgndrQ = -1;

                                            if (sgndrQ != sgndrI){
                                                if (esigma_akt == esigma_prvy){
                                                    esigma_prvy = esigma_prvy->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_prvy;
                                                    esigma_pred = esigma_prvy;
                                                }
                                                else {
                                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_pred->p_dalsi;
                                                }
                                            }
                                            else {
                                                esigma_pred = esigma_akt;
                                                esigma_akt = esigma_akt->p_dalsi;
                                            }
                                        }
                                    }
                                }

                                if (esig_pr_akt->cislobodu == troj_sigma[j]->A3){
                                    fprintf(fw, "IDE A3\n");
                                    if ((bodovepole[troj_sigma[j]->A1 - 1]->x - bodovepole[troj_sigma[j]->A3 - 1]->x) == 0){
                                        r1.k = 0.0000001;
                                        r1.q = 0.0000001;
                                    }
                                    else {
                                        r1.k = (bodovepole[troj_sigma[j]->A1 - 1]->y - bodovepole[troj_sigma[j]->A3 - 1]->y) / (bodovepole[troj_sigma[j]->A1 - 1]->x - bodovepole[troj_sigma[j]->A3 - 1]->x);
                                        r1.q = bodovepole[troj_sigma[j]->A3 - 1]->y - r1.k * bodovepole[troj_sigma[j]->A3 - 1]->x; //Priamka r1 vedena bodmi A3 a A1
                                    }
                                    if (r1.k == 0.0000001){
                                        drI = I->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                        drJ = J->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                    }
                                    else{
                                        drI = r1.k * I->x + r1.q - I->y;
                                        drJ = r1.k * J->x + r1.q - J->y;
                                    }
                                    if (drI > 0)
                                        sgndrI = 1;
                                    else if (drI == 0)
                                        sgndrI = 0;
                                    else if (drI < 0)
                                        sgndrI = -1;

                                    if (drJ > 0)
                                        sgndrJ = 1;
                                    else if (drJ == 0)
                                        sgndrJ = 0;
                                    else if (drJ < 0)
                                        sgndrJ = -1;
                                    if (sgndrI == sgndrJ){
                                        esigma_akt = esigma_prvy;
                                        esigma_pred = esigma_prvy;
                                        while (esigma_akt != NULL){
                                            if (r1.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov z Esigma od priamky r1
                                                drQ = bodovepole[esigma_akt->cislobodu]->x - bodovepole[troj_sigma[j]->A3-1]->x;
                                            }
                                            else {
                                                drQ = r1.k * bodovepole[esigma_akt->cislobodu]->x + r1.q - bodovepole[esigma_akt->cislobodu]->y;
                                            }

                                            if (drQ > 0)
                                                sgndrQ = 1;
                                            else if (drQ == 0)
                                                sgndrQ = 0;
                                            else if (drQ < 0)
                                                sgndrQ = -1;

                                            if (sgndrQ != sgndrI){
                                                if (esigma_akt == esigma_prvy){
                                                    esigma_prvy = esigma_prvy->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_prvy;
                                                    esigma_pred = esigma_prvy;
                                                }
                                                else {
                                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_pred->p_dalsi;
                                                }
                                            }
                                            else {
                                                esigma_pred = esigma_akt;
                                                esigma_akt = esigma_akt->p_dalsi;
                                            }
                                        }
                                    }
                                    if ((bodovepole[troj_sigma[j]->A2 - 1]->x - bodovepole[troj_sigma[j]->A3 - 1]->x) == 0){
                                        r2.k = 0.0000001;
                                        r2.q = 0.0000001;
                                    }
                                    else {
                                        r2.k = (bodovepole[troj_sigma[j]->A2 - 1]->y - bodovepole[troj_sigma[j]->A3 - 1]->y) / (bodovepole[troj_sigma[j]->A2 - 1]->x - bodovepole[troj_sigma[j]->A3 - 1]->x);
                                        r2.q = bodovepole[troj_sigma[j]->A2 - 1]->y - r1.k * bodovepole[troj_sigma[j]->A3 - 1]->x; //Priamka r2 vedena bodmi A3 a A2
                                    }
                                    if (r2.k == 0.0000001){
                                        drI = I->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                        drJ = J->x - bodovepole[esig_pr_akt->cislobodu]->x;
                                    }
                                    else{
                                        drI = r2.k * I->x + r2.q - I->y;
                                        drJ = r2.k * J->x + r2.q - J->y;
                                    }
                                    if (drI > 0)
                                        sgndrI = 1;
                                    else if (drI == 0)
                                        sgndrI = 0;
                                    else if (drI < 0)
                                        sgndrI = -1;

                                    if (drJ > 0)
                                        sgndrJ = 1;
                                    else if (drJ == 0)
                                        sgndrJ = 0;
                                    else if (drJ < 0)
                                        sgndrJ = -1;

                                    if (sgndrI == sgndrJ){
                                        esigma_akt = esigma_prvy;
                                        esigma_pred = esigma_prvy;
                                        while (esigma_akt != NULL){
                                            if (r2.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov z Esigma od priamky r2
                                                fprintf(fw, "IDEEEEEEEEEEEE if\n");
                                                drQ = bodovepole[esigma_akt->cislobodu]->x - bodovepole[troj_sigma[j]->A3-1]->x;
                                            }
                                            else {
                                                fprintf(fw, "IDEEEEEEEEEEEE else\n");
                                                drQ = r2.k * bodovepole[esigma_akt->cislobodu]->x + r2.q - bodovepole[esigma_akt->cislobodu]->y;
                                            }

                                            if (drQ > 0)
                                                sgndrQ = 1;
                                            else if (drQ == 0)
                                                sgndrQ = 0;
                                            else if (drQ < 0)
                                                sgndrQ = -1;

                                            if (sgndrQ != sgndrI){
                                                if (esigma_akt == esigma_prvy){
                                                    esigma_prvy = esigma_prvy->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_prvy;
                                                    esigma_pred = esigma_prvy;
                                                }
                                                else {
                                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                                    delete esigma_akt;
                                                    esigma_akt = esigma_pred->p_dalsi;
                                                }
                                            }
                                            else {
                                                esigma_pred = esigma_akt;
                                                esigma_akt = esigma_akt->p_dalsi;
                                            }
                                        }
                                    }
                                }
                                j++;
                            }
                            esig_pr_akt = esig_pr_akt->p_dalsi;
                        }
                    }*/

                    esigma_akt = esigma_prvy; //Vyber vhodneho bodu z Esigma na tvorbu trojuholnika
                    if (priortest){
                        int i = 1;
                        while (esigma_akt != NULL){
                            if (esigma_akt->prioritny){
                                double dlzkaI, dlzkaJ, suma;
                                dlzkaI = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - I->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - I->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - I->z, 2));
                                dlzkaJ = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - J->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - J->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - J->z, 2));
                                suma = dlzkaI + dlzkaJ;
                                if (i == 1)
                                    sum_pam = dlzkaI + dlzkaJ;
                                if (suma <= sum_pam){
                                    sum_pam = suma;
                                    vhodny = esigma_akt->cislobodu;
                                }
                                //fprintf(fw, "Vhodny = %d a suma = %f a sum_pam = %f\n", vhodny+1, suma, sum_pam);
                                i++;
                            }                            
                            esigma_akt = esigma_akt->p_dalsi;
                        }
                    }
                    else {
                        int i = 1;
                        while (esigma_akt != NULL){
                            double dlzkaI, dlzkaJ, suma;
                            dlzkaI = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - I->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - I->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - I->z, 2));
                            dlzkaJ = sqrt(pow(bodovepole[esigma_akt->cislobodu]->x - J->x, 2) + pow(bodovepole[esigma_akt->cislobodu]->y - J->y, 2) + pow(bodovepole[esigma_akt->cislobodu]->z - J->z, 2));
                            suma = dlzkaI + dlzkaJ;
                            if (i == 1)
                                sum_pam = dlzkaI + dlzkaJ;
                            if (suma >= sum_pam){
                                sum_pam = suma;
                                vhodny = esigma_akt->cislobodu;
                            }
                            esigma_akt = esigma_akt->p_dalsi;
                            fprintf(fw, "Vhodny = %d a suma = %f a sum_pam = %f\n", vhodny+1, suma, sum_pam);
                            i++;
                        }
                    }                    

                    if (vhodny != -1){ //Samotne vykreslenie trojuholnika, zapisanie informacie do TT a testovanie bodov "pod"
                        //iteruj = 1; //Premenna iteruj zabezpeci kontrolu stran v TT strukture, co bude dalej v kode
                        L = bodovepole[vhodny];
                        TT[k] = new TTstrukt(); //Premennu strana vyuzivame v hranatych zatvorkach len kvoli hodnote v nej. De facto so stranou trojuholnika nema nic spolocne
                        TT[k]->A1 = I->cislo;
                        TT[k]->A2 = J->cislo;
                        TT[k]->A3 = L->cislo;
                        TT[k]->M1 = -1;
                        TT[k]->M2 = -1;
                        TT[k]->M3 = -1;
                        TT[k]->n = k + 1;                     

                        int pom;
                        if (TT[k]->A2 < TT[k]->A1){
                            pom = TT[k]->A1;
                            TT[k]->A1 = TT[k]->A2;
                            TT[k]->A2 = pom;
                        }
                        if (TT[k]->A3 < TT[k]->A1){
                            pom = TT[k]->A1;
                            TT[k]->A1 = TT[k]->A3;
                            TT[k]->A3 = pom;
                        }
                        if (TT[k]->A3 < TT[k]->A2){
                            pom = TT[k]->A2;
                            TT[k]->A2 = TT[k]->A3;
                            TT[k]->A3 = pom;
                        } //Zapis informacie o trojuholniku do TT struktury a usporiadanie bodov od najmensieho po najvacsi

                        /*if (L->cislo > I->cislo && L->cislo > J->cislo)
                            TT[k]->M3 = trojuh;
                        else if ((L->cislo > I->cislo && L->cislo < J->cislo) || (L->cislo < I->cislo && L->cislo > J->cislo))
                            TT[k]->M2 = trojuh;
                        else
                            TT[k]->M1 = trojuh;*/

                        for (int iter_TT = 0; iter_TT < k; iter_TT++){
                            if (TT[k]->A1 == TT[iter_TT]->A1){
                                if (TT[k]->A2 == TT[iter_TT]->A2){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A3){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A2){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A3){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A1 == TT[iter_TT]->A2){
                                if (TT[k]->A2 == TT[iter_TT]->A1){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A3){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;                                   
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A1){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A3){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A1 == TT[iter_TT]->A3){
                                if (TT[k]->A2 == TT[iter_TT]->A1){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A2){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A1){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A2){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A2 == TT[iter_TT]->A1){
                                if (TT[k]->A1 == TT[iter_TT]->A2){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A1 == TT[iter_TT]->A3){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A2){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A3){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A2 == TT[iter_TT]->A2){
                                if (TT[k]->A1 == TT[iter_TT]->A1){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A1 == TT[iter_TT]->A3){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A1){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A3){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A2 == TT[iter_TT]->A3){
                                if (TT[k]->A1 == TT[iter_TT]->A1){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A1 == TT[iter_TT]->A2){
                                    TT[k]->M3 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A1){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A3 == TT[iter_TT]->A2){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A3 == TT[iter_TT]->A1){
                                if (TT[k]->A1 == TT[iter_TT]->A2){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A1 == TT[iter_TT]->A3){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A2){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A3){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A3 == TT[iter_TT]->A2){
                                if (TT[k]->A1 == TT[iter_TT]->A1){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A1 == TT[iter_TT]->A3){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A1){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M3 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A3){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                            }
                            else if (TT[k]->A3 == TT[iter_TT]->A3){
                                if (TT[k]->A1 == TT[iter_TT]->A1){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A1 == TT[iter_TT]->A2){
                                    TT[k]->M2 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A1){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M2 = TT[k]->n;
                                }
                                else if (TT[k]->A2 == TT[iter_TT]->A2){
                                    TT[k]->M1 = TT[iter_TT]->n;
                                    TT[iter_TT]->M1 = TT[k]->n;
                                }
                            }
                        }                        

                        fprintf(fw, "A1 = %d, A2 = %d, A3 = %d, M1 = %d, M2 = %d, M3 = %d, n = %d\n", bodovepole[TT[k]->A1-1]->cislo, bodovepole[TT[k]->A2-1]->cislo, bodovepole[TT[k]->A3-1]->cislo, TT[k]->M1, TT[k]->M2, TT[k]->M3, TT[k]->n);
                        //fprintf(fw, "Stav bodu A1 je %d, stav bodu A2 je %d, stav bodu A3 je %d\n", bodovepole[TT[k]->A1]->stav, bodovepole[TT[k]->A2]->stav, bodovepole[TT[k]->A3]->stav);

                        ui->graphicsView->scene()->addLine(bodovepole[TT[k]->A1 - 1]->x+2.5,-bodovepole[TT[k]->A1 - 1]->y+2.5,bodovepole[TT[k]->A2 - 1]->x+2.5,-bodovepole[TT[k]->A2 - 1]->y+2.5,QPen(Qt::red));
                        ui->graphicsView->scene()->addLine(bodovepole[TT[k]->A2 - 1]->x+2.5,-bodovepole[TT[k]->A2 - 1]->y+2.5,bodovepole[TT[k]->A3 - 1]->x+2.5,-bodovepole[TT[k]->A3 - 1]->y+2.5,QPen(Qt::red));
                        ui->graphicsView->scene()->addLine(bodovepole[TT[k]->A3 - 1]->x+2.5,-bodovepole[TT[k]->A3 - 1]->y+2.5,bodovepole[TT[k]->A1 - 1]->x+2.5,-bodovepole[TT[k]->A1 - 1]->y+2.5,QPen(Qt::red));

                        bodovepole[TT[k]->A1-1]->stav = 2;
                        bodovepole[TT[k]->A2-1]->stav = 2;
                        bodovepole[TT[k]->A3-1]->stav = 2;

                        fprintf(fw, "Stav bodu 92 je %d\n", bodovepole[91]->stav);

                        fprintf(fw, "Stav bodu A1 je %d, stav bodu A2 je %d, stav bodu A3 je %d\n", bodovepole[TT[k]->A1-1]->stav, bodovepole[TT[k]->A2-1]->stav, bodovepole[TT[k]->A3-1]->stav);

                        esigma_akt = esigma_prvy; //Vylucenie bodov A1, A2, A3 z Esigma
                        esigma_pred = esigma_prvy;
                        while (esigma_akt != NULL){
                            if (bodovepole[esigma_akt->cislobodu]->stav == 2){
                                if (esigma_akt == esigma_prvy){
                                    esigma_prvy = esigma_prvy->p_dalsi;
                                    delete esigma_akt;
                                    esigma_akt = esigma_prvy;
                                    esigma_pred = esigma_prvy;
                                }
                                else {
                                    esigma_pred->p_dalsi = esigma_akt->p_dalsi;
                                    delete esigma_akt;
                                    esigma_akt = esigma_pred->p_dalsi;
                                }
                            }
                            else {
                                esigma_pred = esigma_akt;
                                esigma_akt = esigma_akt->p_dalsi;
                            }
                        }                       

                        esigma_akt = esigma_prvy; //Test bodov z Esigma, ci sa nachadzaju pod trojuholnikom a ich vylucovanie
                        while (esigma_akt != NULL){
                            Priamka i, j, l;
                            double diM, djM, dlM, diI, djJ, dlL;
                            int sgndiM, sgndjM, sgndlM, sgndiI, sgndjJ, sgndlL;

                            if ((bodovepole[TT[k]->A2-1]->x - bodovepole[TT[k]->A1-1]->x) == 0){ //Urcenie priamky l
                                l.k = 0.0000001;
                                l.q = 0.0000001;
                            }
                            else {
                                l.k = (bodovepole[TT[k]->A2-1]->y - bodovepole[TT[k]->A1-1]->y) / (bodovepole[TT[k]->A2-1]->x - bodovepole[TT[k]->A1-1]->x);
                                l.q = bodovepole[TT[k]->A1-1]->y - l.k * bodovepole[TT[k]->A1-1]->x;
                            }

                            if (l.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov L a M od priamky
                                dlL = bodovepole[TT[k]->A3-1]->x - bodovepole[TT[k]->A1-1]->x;
                                dlM = bodovepole[esigma_akt->cislobodu]->x - bodovepole[TT[k]->A1-1]->x;
                            }
                            else {
                                dlL = l.k * bodovepole[TT[k]->A3-1]->x + l.q - bodovepole[TT[k]->A3-1]->y;
                                dlM = l.k * bodovepole[esigma_akt->cislobodu]->x + l.q - bodovepole[esigma_akt->cislobodu]->y;
                            }

                            if (dlL > 0)
                                sgndlL = 1;
                            else if (dlL == 0)
                                sgndlL = 0;
                            else if (dlL < 0)
                                sgndlL = -1;

                            if (dlM > 0)
                                sgndlM = 1;
                            else if (dlM == 0)
                                sgndlM = 0;
                            else if (dlM < 0)
                                sgndlM = -1;

                            /*if (sgndlM == 0){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "sgndlM Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;
                                continue;
                            }*/

                            if ((bodovepole[TT[k]->A3-1]->x - bodovepole[TT[k]->A2-1]->x) == 0){ //Urcenie priamky i
                                i.k = 0.0000001;
                                i.q = 0.0000001;
                            }
                            else {
                                i.k = (bodovepole[TT[k]->A3-1]->y - bodovepole[TT[k]->A2-1]->y) / (bodovepole[TT[k]->A3-1]->x - bodovepole[TT[k]->A2-1]->x);
                                i.q = bodovepole[TT[k]->A2-1]->y - i.k * bodovepole[TT[k]->A2-1]->x;
                            }

                            if (i.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov I a M od priamky
                                diI = bodovepole[TT[k]->A1-1]->x - bodovepole[TT[k]->A2-1]->x;
                                diM = bodovepole[esigma_akt->cislobodu]->x - bodovepole[TT[k]->A2-1]->x;
                            }
                            else {
                                diI = i.k * bodovepole[TT[k]->A1-1]->x + i.q - bodovepole[TT[k]->A1-1]->y;
                                diM = i.k * bodovepole[esigma_akt->cislobodu]->x + i.q - bodovepole[esigma_akt->cislobodu]->y;
                            }

                            if (diI > 0)
                                sgndiI = 1;
                            else if (diI == 0)
                                sgndiI = 0;
                            else if (diI < 0)
                                sgndiI = -1;

                            if (diM > 0)
                                sgndiM = 1;
                            else if (diM == 0)
                                sgndiM = 0;
                            else if (diM < 0)
                                sgndiM = -1;

                            /*if (sgndiM == 0){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "sgndiM Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;
                                continue;
                            }*/

                            if ((bodovepole[TT[k]->A1-1]->x - bodovepole[TT[k]->A3-1]->x) == 0){ //Urcenie priamky j
                                j.k = 0.0000001;
                                j.q = 0.0000001;
                            }
                            else {
                                j.k = (bodovepole[TT[k]->A1-1]->y - bodovepole[TT[k]->A3-1]->y) / (bodovepole[TT[k]->A1-1]->x - bodovepole[TT[k]->A3-1]->x);
                                j.q = bodovepole[TT[k]->A3-1]->y - j.k * bodovepole[TT[k]->A3-1]->x;
                            }

                            if (j.k == 0.0000001){ //Urcenie citaleta vzdialenosti bodov L a M od priamky
                                djJ = bodovepole[TT[k]->A2-1]->x - bodovepole[TT[k]->A3-1]->x;
                                djM = bodovepole[esigma_akt->cislobodu]->x - bodovepole[TT[k]->A3-1]->x;
                            }
                            else {
                                djJ = j.k * bodovepole[TT[k]->A2-1]->x + j.q - bodovepole[TT[k]->A2-1]->y;
                                djM = j.k * bodovepole[esigma_akt->cislobodu]->x + j.q - bodovepole[esigma_akt->cislobodu]->y;
                            }

                            if (djJ > 0)
                                sgndjJ = 1;
                            else if (djJ == 0)
                                sgndjJ = 0;
                            else if (djJ < 0)
                                sgndjJ = -1;

                            if (djM > 0)
                                sgndjM = 1;
                            else if (djM == 0)
                                sgndjM = 0;
                            else if (djM < 0)
                                sgndjM = -1;

                            /*if (sgndjM == 0){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "sgndjM Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;
                                continue;
                            }*/

                            if (sgndlM == sgndlL && sgndiM == sgndiI && sgndjM == sgndjJ){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;                                
                            }
                            else if (sgndlM == 0 && sgndiM == sgndiI && sgndjM == sgndjJ){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;
                            }
                            else if (sgndiM == 0 && sgndlM == sgndlL && sgndjM == sgndjJ){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;
                            }
                            else if (sgndjM == 0 && sgndlM == sgndlL && sgndiM == sgndiI){
                                bodovepole[esigma_akt->cislobodu]->stav = 0;
                                fprintf(fw, "Stav bodu %d je %d\n", bodovepole[esigma_akt->cislobodu]->cislo,bodovepole[esigma_akt->cislobodu]->stav);
                                esigma_akt = esigma_akt->p_dalsi;
                            }
                            else {
                                esigma_akt = esigma_akt->p_dalsi;
                                fprintf(fw, "Nespravne vyhodnocuje.\n");
                            }
                        }

                        fprintf(fw, "Po testovani pod trojuholnikom stav bodu A1 je %d, stav bodu A2 je %d, stav bodu A3 je %d\n", bodovepole[TT[k]->A1-1]->stav, bodovepole[TT[k]->A2-1]->stav, bodovepole[TT[k]->A3-1]->stav);
                        fprintf(fw, "Kod bodu 183 je %c\n", bodovepole[182]->kod_kompat[0]);
                        ui->graphicsView->scene()->addLine(bodovepole[TT[k-1]->A1 - 1]->x+2.5,-bodovepole[TT[k-1]->A1 - 1]->y+2.5,bodovepole[TT[k-1]->A2 - 1]->x+2.5,-bodovepole[TT[k-1]->A2 - 1]->y+2.5,QPen(Qt::black));
                        ui->graphicsView->scene()->addLine(bodovepole[TT[k-1]->A2 - 1]->x+2.5,-bodovepole[TT[k-1]->A2 - 1]->y+2.5,bodovepole[TT[k-1]->A3 - 1]->x+2.5,-bodovepole[TT[k-1]->A3 - 1]->y+2.5,QPen(Qt::black));
                        ui->graphicsView->scene()->addLine(bodovepole[TT[k-1]->A3 - 1]->x+2.5,-bodovepole[TT[k-1]->A3 - 1]->y+2.5,bodovepole[TT[k-1]->A1 - 1]->x+2.5,-bodovepole[TT[k-1]->A1 - 1]->y+2.5,QPen(Qt::black));

                        //t.timerMethod();

                        /*if (k >= 590 && k <= 600){
                            char string[30];
                            sprintf(string, "Bol vygenerovaný %d. trojuholník", k+1);
                            ui->label_2->setText(QString::fromStdString(string));
                            QMessageBox msgBox;
                            msgBox.setText("Continue");
                            msgBox.exec();
                        }*/

                        k++;
                    }
                    else {
                        //ui->label_2->setText("Nenašiel sa žiadny vhodný bod na vytvorenie druhého trojuholníka. Skúste zmeni slim na vaäèšiu hodnotu.");
                        if (strana == 1)
                            TT[trojuh-1]->M3 = 0;
                        else if (strana == 2)
                            TT[trojuh-1]->M2 = 0;
                        else if (strana == 3)
                            TT[trojuh-1]->M1 = 0;
                    }

                    fprintf(fw, "A1 = %d, A2 = %d, A3 = %d, M1 = %d, M2 = %d, M3 = %d, n = %d\n", bodovepole[TT[trojuh-1]->A1-1]->cislo, bodovepole[TT[trojuh-1]->A2-1]->cislo, bodovepole[TT[trojuh-1]->A3-1]->cislo, TT[trojuh-1]->M1, TT[trojuh-1]->M2, TT[trojuh-1]->M3, TT[trojuh-1]->n);

                    esigma_akt = esigma_prvy; //Zaverecne uvolnenie pamate
                    while (esigma_akt != NULL){
                        esigma_akt = esigma_akt->p_dalsi;
                        delete esigma_prvy;
                        esigma_prvy = esigma_akt;
                    }
                }
                if (strana == 1)
                    p_strana = &TT[trojuh-1]->M2;
                else if (strana == 2)
                    p_strana = &TT[trojuh-1]->M1;
            }
            trojuh++;
            /*if (trojuh > 600)
                iteruj = 0;
            else
                iteruj = 1;*/
            //iteruj = 0;
            if (k+1 <= trojuh)
                iteruj = 0;
            else
                iteruj = 1;
            /*if (trojuh <= k){
                trojuh++;
                iteruj = 1;
            }*/

        }

        fprintf(fw, "V Esigma sa nachadzaju body cislo:\n");
        esigma_akt = esigma_prvy;
        while (esigma_akt != NULL){
            ui->graphicsView->scene()->addEllipse(bodovepole[esigma_akt->cislobodu]->x, -bodovepole[esigma_akt->cislobodu]->y, PRIEMER, PRIEMER, QPen(Qt::red), QBrush(Qt::red));
            fprintf(fw, "%d", esigma_akt->cislobodu + 1);
            if (esigma_akt->prioritny == 1)
                fprintf(fw, " PRIORITNY\n");
            else
                fprintf(fw, " NEPRIORITNY\n");
            esigma_akt = esigma_akt->p_dalsi;
        }
        //ui->graphicsView->scene()->addEllipse(bodovepole[vhodny]->x, -bodovepole[vhodny]->y, PRIEMER, PRIEMER, QPen(Qt::green), QBrush(Qt::green));

        fw2 = fopen("TT_struktura.txt", "w");
        int i = 0;
        while (TT[i] != NULL){
            fprintf (fw2, "%d; %d; %d; %d; %d; %d; %d\n", TT[i]->A1, TT[i]->A2, TT[i]->A3, TT[i]->M1, TT[i]->M2, TT[i]->M3, TT[i]->n);
            i++;
        }

        fclose(fr);
        fclose(fw);
        if (fclose(fw2) != EOF)
            ui->pushButton_3->setEnabled(true);
    }
}

void MainWindow::derivuj()
{
    FILE *fw;
    Vektor3D Vij, Vil;
    Priamka a12;
    double nxI = 0, nyI = 0, nzI = 0, da12A3;
    int orientacia, sgnda12A3; //1 - v smere hodinovych ruciciek, -1 - v protismere hodinovych ruciciek

    fw = fopen ("Parcialne_derivacie.txt", "w");

    for (int i = 0; bodovepole[i] != NULL; i++){
        if (bodovepole[i]->stav == 2){
            for (int j = 0; TT[j] != NULL; j++){
                if (bodovepole[TT[j]->A2-1]->x - bodovepole[TT[j]->A1-1]->x == 0){
                    a12.k = 0.0000001;
                    a12.q = 0.0000001;
                }
                else {
                    a12.k = (bodovepole[TT[j]->A2-1]->y - bodovepole[TT[j]->A1-1]->y) / (bodovepole[TT[j]->A2-1]->x - bodovepole[TT[j]->A1-1]->x);
                    a12.q = bodovepole[TT[j]->A1-1]->y - a12.k * bodovepole[TT[j]->A1-1]->x;
                }

                if (a12.k != 0.0000001){
                    da12A3 = a12.k * bodovepole[TT[j]->A3-1]->x + a12.q - bodovepole[TT[j]->A3-1]->y;
                    if (da12A3 > 0)
                        sgnda12A3 = 1;
                    else if (da12A3 == 0)
                        sgnda12A3 = 0;
                    else if (da12A3 < 0)
                        sgnda12A3 = -1;

                    if ((sgnda12A3 == 1) && (bodovepole[TT[j]->A1-1]->x < bodovepole[TT[j]->A2-1]->x))
                        orientacia = 1;
                    else if ((sgnda12A3 == 1) && (bodovepole[TT[j]->A1-1]->x > bodovepole[TT[j]->A2-1]->x))
                        orientacia = -1;
                    else if ((sgnda12A3 == -1) && (bodovepole[TT[j]->A1-1]->x < bodovepole[TT[j]->A2-1]->x))
                        orientacia = -1;
                    else if ((sgnda12A3 == -1) && (bodovepole[TT[j]->A1-1]->x > bodovepole[TT[j]->A2-1]->x))
                        orientacia = 1;
                }
                else {
                    da12A3 = bodovepole[TT[j]->A3-1]->x - bodovepole[TT[j]->A1-1]->x;
                    if (da12A3 > 0)
                        sgnda12A3 = 1;
                    else if (da12A3 == 0)
                        sgnda12A3 = 0;
                    else if (da12A3 < 0)
                        sgnda12A3 = -1;

                    if ((sgnda12A3 == 1) && (bodovepole[TT[j]->A1-1]->y < bodovepole[TT[j]->A2-1]->y))
                        orientacia = 1;
                    else if ((sgnda12A3 == 1) && (bodovepole[TT[j]->A1-1]->y > bodovepole[TT[j]->A2-1]->y))
                        orientacia = -1;
                    else if ((sgnda12A3 == -1) && (bodovepole[TT[j]->A1-1]->y < bodovepole[TT[j]->A2-1]->y))
                        orientacia = -1;
                    else if ((sgnda12A3 == -1) && (bodovepole[TT[j]->A1-1]->y > bodovepole[TT[j]->A2-1]->y))
                        orientacia = 1;
                }

                if (TT[j]->A1 == bodovepole[i]->cislo && orientacia == 1){
                    Vij.x = bodovepole[TT[j]->A2-1]->x - bodovepole[TT[j]->A1-1]->x;
                    Vij.y = bodovepole[TT[j]->A2-1]->y - bodovepole[TT[j]->A1-1]->y;
                    Vij.z = bodovepole[TT[j]->A2-1]->z - bodovepole[TT[j]->A1-1]->z;
                    Vil.x = bodovepole[TT[j]->A3-1]->x - bodovepole[TT[j]->A1-1]->x;
                    Vil.y = bodovepole[TT[j]->A3-1]->y - bodovepole[TT[j]->A1-1]->y;
                    Vil.z = bodovepole[TT[j]->A3-1]->z - bodovepole[TT[j]->A1-1]->z;
                    nxI += (Vij.y * Vil.z - Vil.y * Vij.z);
                    nyI += (Vij.z * Vil.x - Vil.z * Vij.x);
                    nzI += (Vij.x * Vil.y - Vil.x * Vij.y);
                }
                else if (TT[j]->A1 == bodovepole[i]->cislo && orientacia == -1){
                    Vij.x = bodovepole[TT[j]->A3-1]->x - bodovepole[TT[j]->A1-1]->x;
                    Vij.y = bodovepole[TT[j]->A3-1]->y - bodovepole[TT[j]->A1-1]->y;
                    Vij.z = bodovepole[TT[j]->A3-1]->z - bodovepole[TT[j]->A1-1]->z;
                    Vil.x = bodovepole[TT[j]->A2-1]->x - bodovepole[TT[j]->A1-1]->x;
                    Vil.y = bodovepole[TT[j]->A2-1]->y - bodovepole[TT[j]->A1-1]->y;
                    Vil.z = bodovepole[TT[j]->A2-1]->z - bodovepole[TT[j]->A1-1]->z;
                    nxI += (Vij.y * Vil.z - Vil.y * Vij.z);
                    nyI += (Vij.z * Vil.x - Vil.z * Vij.x);
                    nzI += (Vij.x * Vil.y - Vil.x * Vij.y);
                }
                else if (TT[j]->A2 == bodovepole[i]->cislo && orientacia == 1){
                    Vij.x = bodovepole[TT[j]->A3-1]->x - bodovepole[TT[j]->A2-1]->x;
                    Vij.y = bodovepole[TT[j]->A3-1]->y - bodovepole[TT[j]->A2-1]->y;
                    Vij.z = bodovepole[TT[j]->A3-1]->z - bodovepole[TT[j]->A2-1]->z;
                    Vil.x = bodovepole[TT[j]->A1-1]->x - bodovepole[TT[j]->A2-1]->x;
                    Vil.y = bodovepole[TT[j]->A1-1]->y - bodovepole[TT[j]->A2-1]->y;
                    Vil.z = bodovepole[TT[j]->A1-1]->z - bodovepole[TT[j]->A2-1]->z;
                    nxI += (Vij.y * Vil.z - Vil.y * Vij.z);
                    nyI += (Vij.z * Vil.x - Vil.z * Vij.x);
                    nzI += (Vij.x * Vil.y - Vil.x * Vij.y);
                }
                else if (TT[j]->A2 == bodovepole[i]->cislo && orientacia == -1){
                    Vij.x = bodovepole[TT[j]->A1-1]->x - bodovepole[TT[j]->A2-1]->x;
                    Vij.y = bodovepole[TT[j]->A1-1]->y - bodovepole[TT[j]->A2-1]->y;
                    Vij.z = bodovepole[TT[j]->A1-1]->z - bodovepole[TT[j]->A2-1]->z;
                    Vil.x = bodovepole[TT[j]->A3-1]->x - bodovepole[TT[j]->A2-1]->x;
                    Vil.y = bodovepole[TT[j]->A3-1]->y - bodovepole[TT[j]->A2-1]->y;
                    Vil.z = bodovepole[TT[j]->A3-1]->z - bodovepole[TT[j]->A2-1]->z;
                    nxI += (Vij.y * Vil.z - Vil.y * Vij.z);
                    nyI += (Vij.z * Vil.x - Vil.z * Vij.x);
                    nzI += (Vij.x * Vil.y - Vil.x * Vij.y);
                }
                else if (TT[j]->A3 == bodovepole[i]->cislo && orientacia == 1){
                    Vij.x = bodovepole[TT[j]->A1-1]->x - bodovepole[TT[j]->A3-1]->x;
                    Vij.y = bodovepole[TT[j]->A1-1]->y - bodovepole[TT[j]->A3-1]->y;
                    Vij.z = bodovepole[TT[j]->A1-1]->z - bodovepole[TT[j]->A3-1]->z;
                    Vil.x = bodovepole[TT[j]->A2-1]->x - bodovepole[TT[j]->A3-1]->x;
                    Vil.y = bodovepole[TT[j]->A2-1]->y - bodovepole[TT[j]->A3-1]->y;
                    Vil.z = bodovepole[TT[j]->A2-1]->z - bodovepole[TT[j]->A3-1]->z;
                    nxI += (Vij.y * Vil.z - Vil.y * Vij.z);
                    nyI += (Vij.z * Vil.x - Vil.z * Vij.x);
                    nzI += (Vij.x * Vil.y - Vil.x * Vij.y);
                }
                else if (TT[j]->A3 == bodovepole[i]->cislo && orientacia == -1){
                    Vij.x = bodovepole[TT[j]->A2-1]->x - bodovepole[TT[j]->A3-1]->x;
                    Vij.y = bodovepole[TT[j]->A2-1]->y - bodovepole[TT[j]->A3-1]->y;
                    Vij.z = bodovepole[TT[j]->A2-1]->z - bodovepole[TT[j]->A3-1]->z;
                    Vil.x = bodovepole[TT[j]->A1-1]->x - bodovepole[TT[j]->A3-1]->x;
                    Vil.y = bodovepole[TT[j]->A1-1]->y - bodovepole[TT[j]->A3-1]->y;
                    Vil.z = bodovepole[TT[j]->A1-1]->z - bodovepole[TT[j]->A3-1]->z;
                    nxI += (Vij.y * Vil.z - Vil.y * Vij.z);
                    nyI += (Vij.z * Vil.x - Vil.z * Vij.x);
                    nzI += (Vij.x * Vil.y - Vil.x * Vij.y);
                }
            }
            bodovepole[i]->zx = -nxI / nzI;
            bodovepole[i]->zy = -nyI / nzI;
            fprintf (fw, "cislo = %d; x = %f; y = %f; z = %f; zx = %f; zy = %f\n", bodovepole[i]->cislo, bodovepole[i]->x, bodovepole[i]->y, bodovepole[i]->z, bodovepole[i]->zx, bodovepole[i]->zy);
            nxI = 0;
            nyI = 0;
            nzI = 0;
        }
    }

    fclose(fw);
}
