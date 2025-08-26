#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QUrl>



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void fetchInstruments();
    void fetchOrderBook(const QString &instrumentId);

private:
    Ui::MainWindow *ui;
    QStandardItemModel *instrumentModel;
    QStandardItemModel *orderBookModel;
    QNetworkAccessManager *networkManager;
    QTimer *refreshTimer;
};
#endif // MAINWINDOW_H
