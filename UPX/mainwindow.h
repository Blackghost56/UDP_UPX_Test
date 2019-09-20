#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDebug>
#include <QTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void readPendingDatagrams();

    void on_set_pushButton_clicked();

    void on_read_pushButton_clicked();

    void on_ip_lineEdit_editingFinished();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QHostAddress ip;
    quint16 src_port;
    quint16 dst_port;

    // Типы сообщений
    static constexpr quint8 MEM_READ_TYPE = 0x0;
    static constexpr quint8 MEM_WRITE_TYPE = 0x1;


    QString checkIP(const QString &str);

    // Вывод данных QByteArray
    static void RAWdataOut(const QByteArray &data, const int str_length = 50);

    void sendMsg(const QByteArray &msg);
    void makeUPXHeader(const quint8 cmd, const quint32 startAddr, const quint32 size, const quint32 id, QByteArray &msg);
    void delay(const int time_ms);

};

#endif // MAINWINDOW_H
