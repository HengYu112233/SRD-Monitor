#include "MainWindow.h"
#include <QPen>
#include <QtMath>
#include <QRandomGenerator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    setWindowTitle("深蓝动力 SRD-Monitor v1.0 - 工业专业数据库版");
    resize(1200, 700);

    // 设置全局 QSS 样式表
    this->setStyleSheet(
        "QMainWindow { background-color: #1e1e2e; }"
        "QLabel { color: #cdd6f4; font-size: 14px; }"
        "QLabel#ctrlTitle { color: #89b4fa; font-size: 22px; font-weight: bold; margin-bottom: 10px; }"
        "QLabel#dbLabel { color: #a6e3a1; font-size: 12px; font-style: italic; }"
        "QPushButton { background-color: #313244; color: #cdd6f4; border: 1px solid #45475a; border-radius: 6px; padding: 10px; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45475a; }"
        "QPushButton#btnStart { background-color: #a6e3a1; color: #11111b; border: none; }"
        "QPushButton#btnStart:hover { background-color: #94e2d5; }"
        "QPushButton#btnStop { background-color: #f38ba8; color: #11111b; border: none; }"
        "QPushButton#btnStop:hover { background-color: #eba0ac; }"
        "QComboBox { background-color: #313244; color: #cdd6f4; border: 1px solid #45475a; border-radius: 4px; padding: 5px; font-size: 14px; }"
        "QSlider::groove:horizontal { border: 1px solid #45475a; height: 8px; background: #313244; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: #89b4fa; width: 16px; margin: -4px 0; border-radius: 8px; }"
    );

    // UI 构建完成后，立刻调用数据库初始化
    initDatabase();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QVBoxLayout *leftPanel = new QVBoxLayout();
    leftPanel->setSpacing(20);

    QLabel *ctrlTitle = new QLabel("电机控制面板");
    ctrlTitle->setObjectName("ctrlTitle"); 
    
    portSelector = new QComboBox();
    portSelector->addItem("COM1 (虚拟通信端)");
    btnConnect = new QPushButton("连接电机控制器");
    btnConnect->setFixedHeight(45);

    btnStart = new QPushButton("启动系统");
    btnStart->setObjectName("btnStart"); 
    btnStop = new QPushButton("紧急停止");
    btnStop->setObjectName("btnStop"); 
    btnStart->setFixedHeight(55);
    btnStop->setFixedHeight(55);

    speedLabel = new QLabel("目标转速: 0 RPM");
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(0, 5000);

    // 新增数据库状态展示标签
    dbStatusLabel = new QLabel("数据库: 未初始化");
    dbStatusLabel->setObjectName("dbLabel");

    leftPanel->addWidget(ctrlTitle);
    leftPanel->addWidget(new QLabel("通信端口选择:"));
    leftPanel->addWidget(portSelector);
    leftPanel->addWidget(btnConnect);
    leftPanel->addSpacing(30);
    leftPanel->addWidget(btnStart);
    leftPanel->addWidget(btnStop);
    leftPanel->addSpacing(20);
    leftPanel->addWidget(speedLabel);
    leftPanel->addWidget(speedSlider);
    leftPanel->addSpacing(20);
    leftPanel->addWidget(dbStatusLabel); // 塞入左侧布局底部
    leftPanel->addStretch();

    createCharts();

    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addWidget(chartView, 4);

    dataTimer = new QTimer(this);
    timeCounter = 0.0;
    currentTargetRpm = 0.0;

    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    connect(dataTimer, &QTimer::timeout, this, &MainWindow::updateChartData);
    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::onSpeedSliderChanged);
}

void MainWindow::createCharts() {
    series = new QLineSeries();
    series->setName("A相实时电流");

    QPen pen(QColor(137, 180, 250));
    pen.setWidth(3);
    series->setPen(pen);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("实时动力学数据监控");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);

    axisX = new QValueAxis();
    axisX->setTitleText("时间轴 (ms)");
    axisX->setRange(0, 100);
    
    axisY = new QValueAxis();
    axisY->setTitleText("电流数值 (A)");
    axisY->setRange(0, 50);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

// 建立或打开本地本地文件型数据库
void MainWindow::initDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("srd_data.db"); // 该文件会自动生成在你的 build 目录下

    if (!db.open()) {
        dbStatusLabel->setText("数据库状态: 连接失败");
        qDebug() << "Database Error: " << db.lastError().text();
        return;
    }

    QSqlQuery query;
    // 使用标准的 SQL 语句建立高频运行时数据表
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS motor_log ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "time_ms REAL, "
        "target_rpm INTEGER, "
        "current_a REAL)"
    );

    if (success) {
        dbStatusLabel->setText("存储引擎: SQLite 3 本地数据库已就绪");
    } else {
        dbStatusLabel->setText("数据表建立失败");
    }
}

void MainWindow::onSpeedSliderChanged(int value) {
    currentTargetRpm = value;
    speedLabel->setText(QString("目标转速: %1 RPM").arg(value));
}

void MainWindow::onStartClicked() {
    dataTimer->start(50);
    dbStatusLabel->setText("存储引擎: 正在高频同步写入数据...");
}

void MainWindow::onStopClicked() {
    dataTimer->stop();

    // 当停止运行时，通过 SQL 的聚集函数 SELECT COUNT 统计当前数据总行数
    QSqlQuery query;
    if (query.exec("SELECT COUNT(*) FROM motor_log")) {
        if (query.next()) {
            int totalRows = query.value(0).toInt();
            dbStatusLabel->setText(QString("存储引擎: 已冻结。库内累计存入 %1 条记录").arg(totalRows));
        }
    }
}

void MainWindow::updateChartData() {
    double baseCurrent = 5.0 + (currentTargetRpm / 5000.0) * 30.0; 
    double amplitude = 2.0 + (currentTargetRpm / 5000.0) * 10.0;
    
    double noise = QRandomGenerator::global()->generateDouble() * 2.0 - 1.0; 
    double current = baseCurrent + amplitude * qSin(timeCounter * 0.1) + noise;

    series->append(timeCounter, current);

    if (timeCounter > 100.0) {
        axisX->setRange(timeCounter - 100.0, timeCounter);
    }

    // 核心修改点：将实时仿真出的高频数据通过标准化 SQL 参数绑定的方式写入 SQLite
    QSqlQuery query;
    query.prepare("INSERT INTO motor_log (time_ms, target_rpm, current_a) VALUES (:time, :rpm, :current)");
    query.bindValue(":time", timeCounter);
    query.bindValue(":rpm", currentTargetRpm);
    query.bindValue(":current", current);
    query.exec();

    timeCounter += 1.0;
}