#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "organism.h"
#include "badorganism.h"
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QPushButton>
#include <QMessageBox>
#include <cstdlib>
#include <QVBoxLayout>
#include <QThread>

// zmienne globalne
bool show_widget = true; // zmienna która operuje przycisk Esc(zamyka i otwiera widget_2)
bool game_on = false; // zmienna która umożliwia wysłanie sygnału Esc tylko gdy symulacja jest aktywnada

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    foodTimer(new QTimer(this))

{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    timer->start(1000);

    QTimer *timer_l = new QTimer(this);
    connect(timer_l, SIGNAL(timeout()), this, SLOT(incrementOrganismTime()));
    timer_l->start(1000); // Uruchamiamy timer co sekundę (1000 ms)

    resize(1400, 1000);

    //ustawienia domyślne
    ui->organizm_speed->setValue(25); // Prędkość organizmu
    ui->food_slider1->setValue(5); // Ilość potrzebna do rozmnarzania
    ui->food_slider2->setValue(20);// Ilość jedzenia na mapie
    ui->food_show_time->hide(); // ukrycie opcji ustawienia czasu pojawiania się jedzenia
    ui->s_label->hide();
    ui->organism_time->setValue(1.00); // czas życia organizmu
    ui->bad_organisms_number->setValue(3); // ilośc złych organizmów
    ui->bad_organizm_speed->setValue(50);  // Prędkość złego organizmu
    ui->bad_organism_damage->setValue(2); // damage złego organizmu
    ui->infoButton->hide();

    // ukrywanie obiektów w menu
    ui->widget->hide();
    ui->widget_2->hide();
    ui->widget_3->hide();
    ui->widget_settings->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::startSimulation()
{
    // Rozpocznij odliczanie czasu
    elapsedTimer.start();
    elapsedSeconds = 0;

    // Rozpocznij odliczanie czasu w timerze
    timer->start(1000); // Aktualizuj co 1000 ms (1 sekunda)

}

void MainWindow::stopSimulation()
{
    // Zatrzymaj odliczanie czasu w timerze
    timer->stop();
}
void MainWindow::incrementOrganismTime()
{
    for (int i = 0; i < organisms.size(); i++) {
        Organism &organism = organisms[i];
        organism.time++;
    }
}

void MainWindow::updateTimer()
{
    int elapsedSeconds = elapsedTimer.elapsed() / 1000;

    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    QString timeText = QString("%1:%2").arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));

    ui->timerLabel->setText(timeText);
}


void MainWindow::on_startButton_clicked()
{
    game_on = true;

    ui->widget->setStyleSheet("background-color: rgba(255, 255, 255, 0.5);");

    // wyswietlanie interfejsu
    ui->widget->show();

    // ukrywanie przyciskow
    ui->widget_menu->hide();

    // Inicjalizacja kulki na środku mapy
    Organism organism;
    organism.position = QPoint(width() / 2, height() / 2);
    organisms.append(organism);

    // Inicjalizacja złego organizmu
    srand(time(NULL));
    for(int i=0; i<ui->bad_organisms_number->value(); i++){
        BadOrganism bad_organism;
        bad_organism.generatePosition(width(), height());
        bad_organisms.append(bad_organism);
    }

    // Utworzenie timera i połączenie sygnału timeout() z metodą moveOrganisms()
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::moveOrganisms);
    timer->start(ui->organizm_speed->value()); // Uruchomienie timera dla organizmu

    // Utworzenie timera dla złego organizmu i połączenie sygnału z moveBadOrganisms()
    timer_bad = new QTimer(this);
    connect(timer_bad, &QTimer::timeout, this, &MainWindow::moveBadOrganisms);
    timer_bad->start(100 - ui->bad_organizm_speed->value() + 10);

    // Utworzenie timera do generowania jedzenia
    connect(foodTimer, &QTimer::timeout, this, &MainWindow::spawnFood);

    // Uruchomienie timera dla jedzenia
    if(ui->checkBox->isChecked())
        foodTimer->start(ui->food_show_time->value() * 1000);
    else
        foodTimer->start(1);

    timer->start();
    foodTimer->start();
    // Zresetuj elapsedTimer
    elapsedTimer.restart();

    // Rozpocznij odliczanie czasu
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    timer->start(1000);

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // Rysowanie organizmów
    for (Organism &organism : organisms) {
        organism.paintOrganism(painter);
    }

    painter.setBrush(Qt::red);
    for (const Organism &organism : bad_organisms) {
        painter.drawEllipse(QRect(organism.position.x(), organism.position.y(), organism.size, organism.size));
    }

    // Rysowanie jedzenia
    painter.setBrush(Qt::green);
    for (const QRect &foodRect : foodList) {
        painter.drawEllipse(foodRect);
    }
}


void MainWindow::moveOrganisms()
{
    for (int i = 0; i < organisms.size(); i++) {
        Organism &organism = organisms[i];

        // Usunięcie organizmu któremu skończył się czas
        if(organism.time >= ui->organism_time->value() * 60){
            organisms.removeAt(i);
            int count = ui->organizmCounter->text().toInt();
            ui->organizmCounter->setText(QString::number(count - 1));
        }

        organism.directOrganism(width(), height());

        for (int j = 0; j < foodList.size(); j++) {
            const QRect &foodRect = foodList[j];
            if (foodRect.intersects(QRect(organism.position, QSize(organism.size, organism.size)))) {
                // Usunięcie zjedzonego jedzenia
                foodList.removeAt(j);

                // Zwiększenie licznika zjedzonego jedzenia dla organizmu
                organism.foodCounter++;

                // Zwiększenie wielkości organizmu
                organism.size += 2;

                // Sprawdzenie, czy organizm zjadł wystarczającą ilość jedzenia do rozmnożenia
                if (organism.foodCounter >= ui->food_slider1->value()) {
                    // Skopiowanie cech rodzica dla nowego organizmu
                    Organism newOrganism;
                    newOrganism.position = organism.position;
                    organism.foodCounter = 0;
                    organism.size = 50;

                    // Dodanie nowego organizmu do listy
                    organisms.append(newOrganism);

                    //wyswietlenie licznika organizmów
                    int count = ui->organizmCounter->text().toInt();
                    ui->organizmCounter->setText(QString::number(count + 1));
                }

                // Przerwanie pętli, ponieważ zderzenie z jedzeniem wystąpiło tylko raz dla danego organizmu
                break;
            }
        }
        // Zakończenie symulacji
        if(ui->organizmCounter->text().toInt() <= 0)
            endSymulation();

    }
    //ustawnienie predkosci organizmu
    timer->start(30 - ui->organizm_speed->value());

    // Odświeżenie widoku
    update();
}

void MainWindow::moveBadOrganisms(){
    for(int i=0; i<bad_organisms.size();i++){
        Organism &bad_organism = bad_organisms[i];

        // Poruszanie się złego organizmu
        bad_organism.directOrganism(width(), height());

        // Kolizje z jedzeniem
        for (int j = 0; j < foodList.size(); j++) {
            const QRect &foodRect = foodList[j];
            if (foodRect.intersects(QRect(bad_organism.position, QSize(bad_organism.size, bad_organism.size)))) {
                foodList.removeAt(j);
            }
        }

        // Kolizja z głównym organizmem
        for (int j = 0; j < organisms.size(); j++) {
            Organism &organism = organisms[j];
            if (QRect(organism.position, QSize(organism.size, organism.size)).intersects(QRect(bad_organism.position, QSize(bad_organism.size, bad_organism.size)))) {

                // Animacja zderzenia
                hittingAnimation(&organism);

                // "Odbicie sie" od złego organizmu
                organism.direction.setX(-organism.direction.x());
                organism.direction.setY(-organism.direction.y());

                // Zmniejszenie licznika jedzienia
                organism.foodCounter -= ui->bad_organism_damage->value();

                // Zmniejszenie wielkości organizmu
                organism.size -= ui->bad_organism_damage->value();

                if(organism.foodCounter <= 0){
                    // Usunięcie organizmu
                    organisms.removeAt(j);
                    // Licznik organizmów -1
                    int count = ui->organizmCounter->text().toInt();
                    ui->organizmCounter->setText(QString::number(count - 1));
                }
            }
            // Zakończenie symulacji
            if(ui->organizmCounter->text().toInt() <= 0)
                endSymulation();
        }
    }

    update();
}

void MainWindow::hittingAnimation(Organism *organism)
{
    organism->color = Qt::red;
    update();

    QTimer::singleShot(100, this, [organism, this]() {
        organism->color = Qt::black;
        update();
    });
}

void MainWindow::spawnFood()
{
    // Generowanie losowej pozycji jedzenia
    int foodX = rand() % (width() - 25);
    int foodY = rand() % (height() - 25);

    // Tworzenie prostokąta reprezentującego jedzenie i dodawanie go do listy
    QRect foodRect(foodX, foodY, 25, 25);
    foodList.append(foodRect);

    // Ograniczenie liczby elementów w liście jedzenia
    if (foodList.size() > ui->food_slider2->value()) {
        foodList.removeLast();
    }

    // Odświeżenie widoku
    update();
}

//funkcja do wyśrodkowywanmia widgetów w oknie
void MainWindow::resizeEvent(QResizeEvent *event)
{
    // widget_2
    ui->widget_2->setGeometry((width()/2) -200, (height()/2) -175, 400, 350);

    // widget_menu
    ui->widget_menu->setGeometry((width()/2) -200, (height()/2) -200, 400, 400);

    // widget_3
    ui->widget_3->setGeometry((width()/2) -200, (height()/2) -175, 400, 350);

    // widget_settings
    ui->widget_settings->setGeometry((width()/2) -400, (height()/2) -250, 800, 500);
}

void MainWindow::endSymulation()
{
    ui->widget_3->show();

    timer->stop();
    timer_bad->stop();
    foodTimer->stop();
}

void MainWindow::on_settingsButton_clicked()
{
    ui->widget_settings->show();

}

void MainWindow::on_menu_backButton_clicked()
{
    ui->widget_settings->hide();
}

void MainWindow::on_infoButton_clicked(){}

void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(game_on){
        if (event->key() == Qt::Key_Escape && show_widget){
            on_pauseButton_clicked();
        }
        else{
            on_continueButton_clicked();
        }
    }
}

void MainWindow::on_pauseButton_clicked()
{
    ui->widget_2->show(); // pokazanie opcji pause

    timer->stop();
    timer_bad->stop();
    foodTimer->stop();

    show_widget = false;
}


void MainWindow::on_continueButton_clicked()
{
    ui->widget_2->hide(); // ukrycie opcji pause

    // wznowienie symulacji
    timer->start();
    timer_bad->start();
    foodTimer->start();

    show_widget = true;
}


void MainWindow::on_restartButton_clicked()
{
    ui->widget_2->hide(); // ukrycie opcji pause
    ui->widget_3->hide();

    // Zatrzymanie timera symulacji, i timeru jedzenia
    timer->stop();
    timer_bad->stop();
    foodTimer->stop();

    // Usunięcie organizmów i jedzenia
    organisms.clear();
    foodList.clear();
    bad_organisms.clear();

    // Reset licznika organizmów
    ui->organizmCounter->setText("1");

    // Powrót do fukncji startButton
    on_startButton_clicked();
}


void MainWindow::on_menu_exitButton_clicked()
{
    //wyłączenie symulacji
    game_on = false;

    // czyszczenie ekranu
    organisms.clear();
    foodList.clear();
    bad_organisms.clear();

    //odświerzenie ekranu
    update();

    ui->organizmCounter->setText("1");

   // schowanie niepotrzebnych widgetów i wyświetlenie menu
    ui->widget->hide();
    ui->widget_2->hide();
    ui->widget_3->hide();
    ui->widget_menu->show();

    // zatrzymanie timera
    timer->stop();
    timer_bad->stop();
    foodTimer->stop();

}

// Obsługa sliderów w ustawieniach
void MainWindow::on_organizm_speed_valueChanged(int value)
{
    ui->org_speed->setText(QString::number(value));
}


void MainWindow::on_food_slider2_valueChanged(int value)
{
    ui->f_slider2->setText(QString::number(value));
}


void MainWindow::on_food_slider1_valueChanged(int value)
{
    ui->f_slider1->setText(QString::number(value));
}

void MainWindow::on_bad_organizm_speed_valueChanged(int value)
{
    ui->bad_org_speed->setText(QString::number(value/10));
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    // obsługa przycisku
    if(!arg1){
        ui->food_show_time->hide();// Niepokazuje obiekty z wyborem czasu gdy jest wciśniety przycisk
        ui->s_label->hide();
    }
    else{
        ui->food_show_time->show();// Pokazuje obiekty z wyborem czasu gdy jest wciśniety przycisk
        ui->s_label->show();
    }
}

void MainWindow::on_default_settings_clicked()
{
    //potwierdzenie o przywróceniu ustawień domyślnych
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Przywracanie ustawień domyślnych", "Czy na pewno chcesz przywrócić ustawienia domyślne?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Ustawienia domyślne
        ui->organizm_speed->setValue(25);  // Prędkość organizmu
        ui->food_slider1->setValue(5);     // Ilość potrzebna do rozmnażania
        ui->food_slider2->setValue(20);    // Ilość jedzenia na mapie
        ui->food_show_time->setValue(0.00);
        ui->checkBox->setChecked(false);
        ui->organism_time->setValue(1.00);
        ui->bad_organisms_number->setValue(3); // ilośc złych organizmów
        ui->bad_organizm_speed->setValue(50);  // Prędkość złego organizmu
        ui->bad_organism_damage->setValue(2); // damage złego organizmu
    }

}

void MainWindow::on_restartButton_2_clicked()
{
    on_restartButton_clicked();
}


void MainWindow::on_menu_exitButton_2_clicked()
{
    on_menu_exitButton_clicked();
}

