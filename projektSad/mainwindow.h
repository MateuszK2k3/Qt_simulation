#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include "organism.h"
#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QRect>
#include <QPoint>
#include <QKeyEvent>
#include <QElapsedTimer>

namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void moveOrganisms();
    void spawnFood();

    void on_startButton_clicked();

    void on_infoButton_clicked();

    void on_exitButton_clicked();

    void on_pauseButton_clicked();

    void on_continueButton_clicked();

    void on_restartButton_clicked();

    void keyPressEvent(QKeyEvent *event);

    void on_menu_exitButton_clicked();

    void resizeEvent(QResizeEvent *event);

    void on_settingsButton_clicked();

    void on_menu_backButton_clicked();

    void on_organizm_speed_valueChanged(int value);

    void on_food_slider2_valueChanged(int value);

    void on_food_slider1_valueChanged(int value);

    void on_checkBox_stateChanged(int arg1);

    void on_default_settings_clicked();

    void incrementOrganismTime();

    void moveBadOrganisms();

    void hittingAnimation(Organism *organism);

    void endSymulation();

    void updateTimer();
    void startSimulation();
    void stopSimulation();

    void on_bad_organizm_speed_valueChanged(int value);

    void on_restartButton_2_clicked();

    void on_menu_exitButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTimer *timer_bad;
    QTimer *foodTimer;
    QList<QRect> foodList; // Lista przechowująca prostokąty reprezentujące jedzenie
    QList<Organism> organisms; // Lista przechowująca organizmy
    QList<Organism> bad_organisms; // Lista przechowująca złe organizmy
    QLabel *timerLabel;

    QPoint generateRandomDirection(); // Funkcja generująca losowy kierunek dla organizmu
    QElapsedTimer elapsedTimer; // Obiekt QElapsedTimer do mierzenia czasu

    QTimer *simulationTimer;
    int elapsedSeconds = 0;
};

#endif // MAINWINDOW_H
