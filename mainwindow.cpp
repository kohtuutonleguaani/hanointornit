#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QGraphicsItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ajanotto(new QTimer)
{
    ui->setupUi(this);

    //Käsitellään pelin piirtoaluetta.
    maisema_ = new QGraphicsScene(this);
    ui->graphicsView->setGeometry(1,1,LEVEYS,KORKEUS);
    ui->graphicsView->setScene(maisema_);
    maisema_->setSceneRect(0,0,LEVEYS-2,KORKEUS-2);

    //Näin saa näppäimet sellaisiksi, että ne pysyvät pohjassa klikkauksella.
    ui->ApushButton->setCheckable(true);
    ui->BpushButton->setCheckable(true);
    ui->CpushButton->setCheckable(true);

    //Jotta ohjelmaa ei saisi kaatumaan ensimmäisellä napinpainalluksella,
    //otetaan nämä pois käytöstä siihen asti, että on painettu 'Aloita'.
    ui->ApushButton->setDisabled(true);
    ui->BpushButton->setDisabled(true);
    ui->CpushButton->setDisabled(true);
    ui->resetPushButton->setDisabled(true);

    //Ajanotto tulee tässä
    connect(ajanotto, SIGNAL(timeout()), this, SLOT(ajanotto_slot()));
    ui->lcdNumberSek->setStyleSheet("background-color:black");
    ui->lcdNumberMin->setStyleSheet("background-color:black");
    ui->lcdNumberSek->setPalette(Qt::red);
    ui->lcdNumberMin->setPalette(Qt::red);

}

MainWindow::~MainWindow()
{
    for (unsigned i = 0; i < lahto_.size(); ++i) {
        delete lahto_.at(i).laatikko;
    }
    for (unsigned i = 0; i < apu_.size(); ++i) {
        delete apu_.at(i).laatikko;
    }
    for (unsigned i = 0; i < maali_.size(); ++i) {
        delete maali_.at(i).laatikko;
    }
    for (unsigned i = 0; i < kalusteet_.size(); ++i) {
        delete kalusteet_.at(i);
    }
    delete ajanotto;
    delete ui;
}

void MainWindow::ajanotto_slot()
{
    ++sek_;
    ui->lcdNumberSek->display(sek_);

    if (sek_ == 60) {
        ++min_;
        sek_ = 0;
        ui->lcdNumberSek->display(sek_);
        ui->lcdNumberMin->display(min_);
    }
}

void MainWindow::piirra_alkutilanne(){
    QBrush punainen_suti(Qt::red);
    QBrush ruskea_suti(QColor(115,34,0));
    QBrush tpun_suti(QColor(127, 10, 10));
    QBrush tvihr_suti(QColor(10, 127, 10));
    QBrush tsin_suti(QColor(10, 10, 127));
    QPen musta_kyna(Qt::black);
    //Pinta tappien alle
    QGraphicsRectItem* poyta = maisema_->addRect((LEVEYS-POYTA)/2,
                               KORKEUS/2+TAPPI_Y/(2*skaalaus_),
                                POYTA, TAPPI_X, musta_kyna, ruskea_suti);
    //Tappien y-sijanti on kaikilla sama
    int y_sij = (KORKEUS-TAPPI_Y/skaalaus_)/2;
    //
    //keskimmäinen tappi
    QGraphicsRectItem* keskitappi = maisema_->addRect(APUTAPPI-TAPPI_X/2,
                       y_sij, TAPPI_X,
                       TAPPI_Y/skaalaus_, musta_kyna, tsin_suti);
    //vasen tappi
    QGraphicsRectItem* vasentappi = maisema_->addRect(LAHTOTAPPI-TAPPI_X/2, y_sij,
                      TAPPI_X, TAPPI_Y/skaalaus_, musta_kyna, tpun_suti);
    //oikea tappi
    QGraphicsRectItem* oikeatappi = maisema_->addRect(MAALITAPPI-TAPPI_X/2, y_sij,
                      TAPPI_X, TAPPI_Y/skaalaus_, musta_kyna, tvihr_suti);
    //Kuviot tallennetaan vektoriin, jotta ne on helppo myös tuhota,
    //jos peli pelataan uudelleen eri arvoilla.
    kalusteet_.push_back(poyta);
    kalusteet_.push_back(vasentappi);
    kalusteet_.push_back(keskitappi);
    kalusteet_.push_back(oikeatappi);
    //luodaan varsinaiset kiekot.
    for (int i = 0; i < lkm_; ++i){
        Kiekko kpl;
        kpl.arvo = lkm_-i;
        int x= (LEVEYS/2-(KIEKKO_X-KOKOERO*skaalaus_*i))/2-MARG;
        int y = KORKEUS/2+TAPPI_Y/(2*skaalaus_)-TAPPI_X-i*KIEKKO_Y;
        kpl.kiekon_leveys = KIEKKO_X - KOKOERO*skaalaus_*i;
        kpl.laatikko = maisema_->
                addRect(0, 0,
                kpl.kiekon_leveys, KIEKKO_Y, musta_kyna, punainen_suti);
        kpl.laatikko->setPos(x, y);
        kpl.laatikko->setZValue(1);
        lahto_.push_back(kpl);
    }
}

void MainWindow::siirtaja(std::vector<Kiekko> &alkuvektori,
                          std::vector<Kiekko> &loppuvektori, int loppu)
{
    int x = loppu-alkuvektori.at(alkuvektori.size()-1).kiekon_leveys/2;
    int y = KORKEUS/2+TAPPI_Y/(2*skaalaus_)-
            TAPPI_X - KIEKKO_Y*loppuvektori.size();
    alkuvektori.at(alkuvektori.size()-1).laatikko->setPos(x, y);
    loppuvektori.push_back(alkuvektori.at(alkuvektori.size()-1));
    if (loppu == MAALITAPPI) {
        QBrush suti(Qt::green);
        loppuvektori.at(loppuvektori.size()-1).laatikko->setBrush(suti);
    }
    if (loppu == APUTAPPI) {
        QBrush suti(Qt::blue);
        loppuvektori.at(loppuvektori.size()-1).laatikko->setBrush(suti);
    }
    if (loppu == LAHTOTAPPI) {
        QBrush suti(Qt::red);
        loppuvektori.at(loppuvektori.size()-1).laatikko->setBrush(suti);
    }
    alkuvektori.pop_back();
    ui->ApushButton->setEnabled(true);
    ui->BpushButton->setEnabled(true);
    ui->CpushButton->setEnabled(true);
}

void MainWindow::klikkaus_abc()
{
    //Ei yhtään nappia pohjassa, eli nappi vapautetaan.
    if (b_pohjassa_ == 0 and c_pohjassa_ == 0 and a_pohjassa_ == 0){
        ui->ApushButton->setEnabled(true);
        ui->BpushButton->setEnabled(true);
        ui->CpushButton->setEnabled(true);
    }
    //A pohjassa
    if (b_pohjassa_ == 0 and c_pohjassa_ == 0 and a_pohjassa_ == 1){
        if (lahto_.size()==0){
            a_pohjassa_ = 0;
            ui->ApushButton->setChecked(false);
            return;
        }
        QBrush keltainen(Qt::yellow);
        lahto_.at(lahto_.size()-1).laatikko->setBrush(keltainen);
        //Voiko siirtää B:hen?
        if (apu_.size() > 0) {
            if (apu_.back().arvo < lahto_.back().arvo) {
                ui->BpushButton->setDisabled(true);
            }
        }
        else {
            ui->BpushButton->setDisabled(false);
        }
        //Voiko siirtää C:hen?
        if (maali_.size() > 0) {
            if (maali_.back().arvo < lahto_.back().arvo) {
                ui->CpushButton->setDisabled(true);
            }
        }
        else {
            ui->CpushButton->setDisabled(false);
        }
    }
    //B pohjassa
    if (a_pohjassa_ == 0 and c_pohjassa_ == 0 and b_pohjassa_ == 1){
        if (apu_.size()==0){
            b_pohjassa_ = 0;
            ui->BpushButton->setChecked(false);
            return;
        }
        QBrush keltainen(Qt::yellow);
        apu_.at(apu_.size()-1).laatikko->setBrush(keltainen);
        //Voiko siirtää A:han?
        if (lahto_.size() > 0) {
            if (lahto_.back().arvo < apu_.back().arvo) {
                ui->ApushButton->setDisabled(true);
            }
        }
        else {
            ui->ApushButton->setDisabled(false);
        }
        //Voiko siirtää C:hen?
        if (maali_.size() > 0) {
            if (maali_.back().arvo < apu_.back().arvo) {
                ui->CpushButton->setDisabled(true);
            }
        }
        else {
            ui->CpushButton->setDisabled(false);
        }
    }
    //C pohjassa
    if (a_pohjassa_ == 0 and b_pohjassa_ == 0 and c_pohjassa_ == 1){
        if (maali_.size()==0){
            c_pohjassa_ = 0;
            ui->CpushButton->setChecked(false);
            return;
        }
        QBrush keltainen(Qt::yellow);
        maali_.at(maali_.size()-1).laatikko->setBrush(keltainen);
        //Voiko siirtää A:han?
        if (lahto_.size() > 0) {
            if (lahto_.back().arvo < maali_.back().arvo) {
                ui->ApushButton->setDisabled(true);
            }
        }
        else {
            ui->ApushButton->setDisabled(false);
        }
        //Voiko siirtää B:hen?
        if (apu_.size() > 0) {
            if (apu_.back().arvo < maali_.back().arvo) {
                ui->BpushButton->setDisabled(true);
            }
        }
        else {
            ui->BpushButton->setDisabled(false);
        }
    }

}

//Funktion täytyy selvästikin tuhota kaikki kiekot,
//pyyhkiä taulu, ja vielä printata viesti käyttäjälle.
void MainWindow::voitontarkistus()
{
    int maali = static_cast<int>(maali_.size());
    int apu = static_cast<int>(apu_.size());
    if (lahto_.size()==0) {
        if (maali == lkm_ or apu == lkm_) {
            QBrush violetti(QColor(140,0,180));
            ui->label_2->setText("HIENOA DUUNIA!!!! Voitit pelin!");
            ui->ApushButton->setDisabled(true);
            ui->BpushButton->setDisabled(true);
            ui->CpushButton->setDisabled(true);
            if (maali == lkm_) {
                for (int i = 0; i < lkm_; ++i){
                    maali_.at(i).laatikko->setBrush(violetti);
                }
                
            }
            if (apu == lkm_) {
                for (int i = 0; i < lkm_; ++i){
                    apu_.at(i).laatikko->setBrush(violetti);
                }
                
            }
            ajanotto->stop();
        }
    }
}

void MainWindow::on_lopetaPushButton_clicked()
{
    ajanotto->stop();
    close();
}

void MainWindow::on_kiekkoaPushButton_clicked()
{
    QString syote = ui->kiekkoaLineEdit->text();
    if (syote != "") {
        std::string lukustring = syote.toUtf8().constData();
        bool kelpaako = (lukustring.find_first_not_of( "0123456789" )
                         == std::string::npos);
        if (kelpaako == 1) {
            int lkm = stoi(lukustring);
            if (lkm > 20 or lkm < 2) {
                ui->label_2->setText("...järkevä määrä kiekkoja on 2-20.");
            }
            else {
                lkm_ = lkm;
                ui->label_2->
                        setText("Se on hyvä määrä kiekkoja. Paina Aloita.");
            }
        }
        else {
            ui->label_2->setText("Ei kelpaa, syötä luku!");
        }
    }
    else{
        ui->label_2->setText("Pelataan siis kahdella kiekolla.");
    }
}

void MainWindow::on_aloitaPushButton_clicked()
{
    //Asetetaan sekuntikello nollaan, ja käynnistetään se.
    sek_ = 0;
    min_ = 0;
    ajanotto->start(1000);
    //Asetetaan tappien koon ja kiekon leveyden skaalaus
    //kiekkopinon määrän mukaan.
    if (lkm_ > 10) {
        skaalaus_ = 1;
    }
    else {
        skaalaus_ = 2;
    }
    //Jos kysessä on toinen pelikerta, täytyy pyyhkiä pelialueelta
    //myös tapit, koska niiden koko voi olla muuttunut.
    for (unsigned i = 0; i < kalusteet_.size(); ++i) {
        delete kalusteet_.at(i);
    }
    kalusteet_.clear();
    //Piirretään pelilauta ja luodaan kiekot.
    piirra_alkutilanne();
    //Sallitaan A-napin painaminen, sillä lähtötapissa on nyt kiekkoja.
    ui->ApushButton->setEnabled(true);
    //Otetaan samalla Ok ja aloita pois käytöstä.
    ui->aloitaPushButton->setEnabled(false);
    ui->kiekkoaPushButton->setEnabled(false);
    //Ja annetaan Reset käyttöön
    ui->resetPushButton->setEnabled(true);

}

void MainWindow::on_resetPushButton_clicked()
{
    for (unsigned i = 0; i < lahto_.size(); ++i) {
        delete lahto_.at(i).laatikko;
    }
    for (unsigned i = 0; i < apu_.size(); ++i) {
        delete apu_.at(i).laatikko;
    }
    for (unsigned i = 0; i < maali_.size(); ++i) {
        delete maali_.at(i).laatikko;
    }
    lahto_.clear();
    apu_.clear();
    maali_.clear();
    ui->label_2->setText("Uusi peli alkaa, jos painat Aloita.");
    //A, B ja C pois käytöstä.
    ui->ApushButton->setDisabled(true);
    ui->BpushButton->setDisabled(true);
    ui->CpushButton->setDisabled(true);
    //Aloita ja Ok takaisin
    ui->aloitaPushButton->setEnabled(true);
    ui->kiekkoaPushButton->setEnabled(true);
    //Kellon pysäytys ja nollaus
    ajanotto->stop();
    sek_ = 0;
    min_ = 0;
    ui->lcdNumberSek->display(sek_);
    ui->lcdNumberMin->display(min_);
}

void MainWindow::on_ApushButton_clicked(bool checked)
{
    a_pohjassa_ = checked;
    klikkaus_abc();

    if (checked == 0) {
        QBrush punainen(Qt::red);
        lahto_.at(lahto_.size()-1).laatikko->setBrush(punainen);
    }

    //Ensin siirrytään B:stä A:han.
    if (b_pohjassa_ == 1 and c_pohjassa_ == 0) {
        siirtaja(apu_, lahto_, LAHTOTAPPI);
        ui->ApushButton->setChecked(false);
        ui->BpushButton->setChecked(false);
        a_pohjassa_ = 0;
        b_pohjassa_ = 0;
    }
    //Sitten tapaus C:stä A:han.
    if (b_pohjassa_ == 0 and c_pohjassa_ == 1) {
        siirtaja(maali_, lahto_, LAHTOTAPPI);
        ui->ApushButton->setChecked(false);
        ui->CpushButton->setChecked(false);
        a_pohjassa_ = 0;
        c_pohjassa_ = 0;
    }
    voitontarkistus();
}

void MainWindow::on_BpushButton_clicked(bool checked)
{
    b_pohjassa_ = checked;
    klikkaus_abc();

    if (checked == 0) {
        QBrush sininen(Qt::blue);
        apu_.at(apu_.size()-1).laatikko->setBrush(sininen);
    }

    //Tapaukset, että B on se, johon liikutaan.
    //Ensin siirrytään A:sta B:hen.
    if (a_pohjassa_==1 and c_pohjassa_ == 0){
        siirtaja(lahto_, apu_, APUTAPPI);
        ui->ApushButton->setChecked(false);
        ui->BpushButton->setChecked(false);
        a_pohjassa_ = 0;
        b_pohjassa_ = 0;
    }

    //Sitten tapaus C:stä B:hen.
    if (a_pohjassa_ == 0 and c_pohjassa_ == 1) {
        siirtaja(maali_, apu_, APUTAPPI);
        ui->BpushButton->setChecked(false);
        ui->CpushButton->setChecked(false);
        b_pohjassa_ = 0;
        c_pohjassa_ = 0;
    }
    voitontarkistus();
}

void MainWindow::on_CpushButton_clicked(bool checked)
{
    c_pohjassa_ = checked;
    klikkaus_abc();

    if (checked == 0) {
        QBrush vihrea(Qt::green);
        maali_.at(maali_.size()-1).laatikko->setBrush(vihrea);
    }

    //A:sta C:hen
    if (a_pohjassa_==1 and b_pohjassa_ == 0){
        siirtaja(lahto_, maali_, MAALITAPPI);
        ui->ApushButton->setChecked(false);
        ui->CpushButton->setChecked(false);
        a_pohjassa_ = 0;
        c_pohjassa_ = 0;
    }
    //B:stä C:hen
    if (a_pohjassa_==0 and b_pohjassa_ == 1){
        siirtaja(apu_, maali_, MAALITAPPI);
        ui->BpushButton->setChecked(false);
        ui->CpushButton->setChecked(false);
        b_pohjassa_ = 0;
        c_pohjassa_ = 0;
    }
    voitontarkistus();
}



