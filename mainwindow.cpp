#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::fetchInstruments() {
    QNetworkRequest request(QUrl("https://www.okx.com/api/v5/market/tickers?instType=FUTURES"));
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonArray arr = doc.object()["data"].toArray();

        instrumentModel->clear();
        instrumentModel->setHorizontalHeaderLabels({"Instrument", "Best BID", "Best BID Qty", "Market Price", "Best ASK", "Best ASK Qty"});

        for (const QJsonValue &val : arr) {
            QJsonObject obj = val.toObject();
            if (!obj["instId"].toString().startsWith("BTC-USDT")) continue;

            QList<QStandardItem *> row;
            row << new QStandardItem(obj["instId"].toString());
            row << new QStandardItem(obj["bidPx"].toString());
            row << new QStandardItem(obj["bidSz"].toString());
            row << new QStandardItem(obj["last"].toString());
            row << new QStandardItem(obj["askPx"].toString());
            row << new QStandardItem(obj["askSz"].toString());
            instrumentModel->appendRow(row);
        }
    });
}


MainWindow::~MainWindow()
{
    instrumentModel = new QStandardItemModel(this);
    orderBookModel = new QStandardItemModel(this);

    ui->instrumentTableView->setModel(instrumentModel);
    ui->orderBookTableView->setModel(orderBookModel);

    networkManager = new QNetworkAccessManager(this);
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::fetchInstruments);
    refreshTimer->start(5000); // every 5 seconds


    delete ui;
}

