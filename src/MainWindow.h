#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QComboBox>
#include <QTimer>

// 引入 Qt 数据库操作核心类
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void createCharts();
    void initDatabase(); // 新增：初始化 SQLite 数据库的函数

    QWidget *centralWidget;
    QChartView *chartView;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QComboBox *portSelector;
    QPushButton *btnConnect;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QSlider *speedSlider;
    QLabel *speedLabel;
    QLabel *dbStatusLabel; // 新增：用于在面板上实时显示数据库状态的标签

    QTimer *dataTimer;
    double timeCounter;
    double currentTargetRpm;

private slots:
    void onStartClicked();
    void onStopClicked();
    void updateChartData();
    void onSpeedSliderChanged(int value);
};

#endif