#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>

#include <QGraphicsScene>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//Piirtoalueen mitat
const int LEVEYS = 439;
const int KORKEUS = 248;
//tappien mitat
const int TAPPI_X = 7;
const int TAPPI_Y = 200;
const int POYTA = 390;
//Yleismarginaali
const int MARG = 20;
//Kiekon mitat
const int KIEKKO_X = 120;
const int KIEKKO_Y = 10;
const int KOKOERO = 5;
//Tappien keskipisteiden sijainnit.
const int LAHTOTAPPI = LEVEYS/4-MARG;
const int APUTAPPI = LEVEYS/2;
const int MAALITAPPI = LEVEYS*3/4+MARG;

//Kiekolla on
//arvo, joka kertoo sen koosta
//pelialueelle piirrettävä laatikko
//leveys omassa muuttujassaan.
struct Kiekko {
    int arvo;
    int kiekon_leveys;
    QGraphicsRectItem* laatikko;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //Sulkee ohjelman, tuhoaa asiat
    void on_lopetaPushButton_clicked();

    //Luetaan käyttäjän tekstikenttään syöttämä kiekkomäärä. Jos kiekkojen
    //määrä ei ole järkevä, se kerrotaan käyttäjälle.
    //Jos käyttäjä ei anna lukumäärää, lkm_ on kuitenkin määritetty 2:ksi.
    void on_kiekkoaPushButton_clicked();

    //Kutsuu pelilaudan luovaa funktiota ja laittaa kellon käyntiin.
    void on_aloitaPushButton_clicked();

    //Funktio piirtää pelialueelle kolme tappia sekä lattian
    //ja luo halutun kokoisen kiekkopinon lähtötappiin.
    void piirra_alkutilanne();

    //Kun reset-nappia painetaan, peli keskeytetään, kiekot tuhotaan
    //pelialueelta (tapit jäävät), ja peli palaa alkutilaan, jossa
    //käyttäjä voi jälleen asettaa uuden kiekkomäärän ja aloittaa uuden pelin.
    void on_resetPushButton_clicked();

    //Kiekkojen siirtely tapahtuu nappeja A, B ja C painelemalla.
    //Napinpainallukset kutsuvat tarvittavia napinnostaja-/siirtäjäfunktioita
    void on_ApushButton_clicked(bool checked);

    void on_BpushButton_clicked(bool checked);

    void on_CpushButton_clicked(bool checked);

    //Funktio, jota kutsutaan ABC-napinpainalluksessa, jos tappi on
    //liikutettavan kiekon kohde. Funktio siirtää kiekon oikeaan
    //vektoriin, ja muuttaa pelikuvaa asianmukaisesti.
    void siirtaja(std::vector<Kiekko> &alkuvektori,
                  std::vector<Kiekko> &loppuvektori, int loppu);

    //Funktio, jota kutsutaan ABC-napinpainalluksessa, ja joka
    //käsittelee tapaukset, että klikkauksen jälkeen pohjassa on
    //nolla tai yksi nappia. Funktio myös torjuu tapaukset, että
    //tyhjästä tapista yritetään siirtää jotain pois.
    void klikkaus_abc();

    //Funktio, joka tarkistaa, onko kaikki kiekot saatu siirrettyä.
    //Funktio ei tässä ainakaan tässä vaiheessa erottele, ovatko kiekot
    //apu- vai maalitapissa, yksi pino jommassa kummassa käy voitosta.
    void voitontarkistus();

    //Pelissä on myös sekuntikello
    void ajanotto_slot();

private:
    Ui::MainWindow *ui;

    //Ajanotto
    QTimer* ajanotto;

    //Montako kiekkoa pelissä
    int lkm_ = 2;
    //pelimaisema
    QGraphicsScene* maisema_;

    //Tappivektorit, jotka pitävät kirjaa tapissa kulloinkin olevasta
    //kiekkopinosta.
    std::vector<Kiekko> lahto_; //eli tappi A
    std::vector<Kiekko>  apu_;//eli tappi B
    std::vector<Kiekko> maali_; //eli tappi C
    //Pöydän ja tappien kuviot sisältävä vektori
    std::vector<QGraphicsItem*> kalusteet_;

    //Näillä muuttujilla pidetään kirjaa siitä, mikä napeista on
    //pohjassa, jotta tiedetään, mikä kiekko liikkuu ja mihin.
    bool a_pohjassa_ = 0;
    bool b_pohjassa_ = 0;
    bool c_pohjassa_ = 0;

    //Vakio, joka kertoo pelialueella olevien
    //kuvioiden skaalauksesta.
    int skaalaus_ = 2;

    //Ajanoton minuutit ja sekunnit
    int sek_ = 0;
    int min_ = 0;

};
#endif // MAINWINDOW_HH

