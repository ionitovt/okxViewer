#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    instrumentModel = new QStandardItemModel(this);
    orderBookModel = new QStandardItemModel(this);

    ui->instrumentTableView->setModel(instrumentModel);
    ui->orderBookTableView->setModel(orderBookModel);

    networkManager = new QNetworkAccessManager(this);
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::fetchInstruments);
    refreshTimer->start(5000); // every 5 seconds

    connect(ui->instrumentTableView, &QTableView::clicked, this, [=](const QModelIndex &index) {
        QString instrumentId = instrumentModel->item(index.row(), 0)->text();
        qDebug() << "Clicked on:" << instrumentId;
        fetchOrderBook(instrumentId);
    });
}

void MainWindow::fetchInstruments() {
    qDebug() << "[fetchInstruments] sending request...";

    QNetworkRequest request(QUrl("https://www.okx.com/api/v5/market/tickers?instType=FUTURES"));
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonArray arr = doc.object()["data"].toArray();
        qDebug() << "[fetchInstruments] response:" << response;

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
            qDebug() << "[fetchInstruments] parsed rows:" << arr.size();

        }
    });
}

void MainWindow::fetchOrderBook(const QString &instrumentId) {
    QString url = QString("https://www.okx.com/api/v5/market/books?instId=%1&sz=50").arg(instrumentId);
    QUrl qurl(url);
    QNetworkRequest request(qurl);

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray response = reply->readAll();
        reply->deleteLater();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object()["data"].toArray()[0].toObject();

        QJsonArray bids = obj["bids"].toArray();
        QJsonArray asks = obj["asks"].toArray();

        orderBookModel->clear();
        orderBookModel->setHorizontalHeaderLabels({"BID Qty", "BID Price", "ASK Price", "ASK Qty"});

        int count = std::max(bids.size(), asks.size());
        for (int i = 0; i < count; ++i) {
            QList<QStandardItem *> row;
            if (i < bids.size()) {
                QJsonArray bid = bids[i].toArray();
                row << new QStandardItem(bid[1].toString());
                row << new QStandardItem(bid[0].toString());
            } else {
                row << new QStandardItem("");
                row << new QStandardItem("");
            }

            if (i < asks.size()) {
                QJsonArray ask = asks[i].toArray();
                row << new QStandardItem(ask[0].toString());
                row << new QStandardItem(ask[1].toString());
            } else {
                row << new QStandardItem("");
                row << new QStandardItem("");
            }

            orderBookModel->appendRow(row);
        }
    });
}



MainWindow::~MainWindow()
{
    delete ui;
}

