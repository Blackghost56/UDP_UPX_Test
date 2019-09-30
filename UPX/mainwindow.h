#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDebug>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>

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

    void on_baseDec_spinBox_valueChanged(int arg1);

    void on_baseHex_spinBox_valueChanged(int arg1);

    void on_write_pushButton_clicked();

    void on_save_pushButton_clicked();

    void on_open_pushButton_clicked();

    void on_length_spinBox_valueChanged(int arg1);

    void on_copy_pushButton_clicked();

    void on_decAddres_checkBox_clicked();

    void on_hexAddres_checkBox_clicked();

    void on_tableWidget_cellChanged(int row, int column);

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QHostAddress ip;
    quint16 src_port;
    quint16 dst_port;
    quint32 baseAddr;
    quint32 length;

    struct currentData {
        QByteArray readData;
        QByteArray writeData;
        quint32 addr = 0;
    } currentData;

    enum CHECK_STATE {OK = 0, MISTAKE, LESS, GREATER};

    // Типы сообщений
    static constexpr quint8 MEM_READ_TYPE = 0x0;
    static constexpr quint8 MEM_WRITE_TYPE = 0x1;

    // Названия колонок
    const QString STR_DEC_ADRR = tr("Dec addr");
    const QString STR_HEX_ADRR = tr("Hex addr");
    const QString STR_READ_DATA = tr("Read data (Hex)");
    const QString STR_WRITE_DATA = tr("Write data (Hex)");

    // Проверка IP адреса
    QString checkIP(const QString &str);
    // Проверка м чтение значения
    int StrIntValueCheck(QString &&str, const int base, const int min, const int max, const int def, int &state, QString label = "");

    // Вывод данных QByteArray
    static void RAWdataOut(const QByteArray &data, const int str_length = 50);

    void sendMsg(const QByteArray &msg);
    void makeUPXHeader(const quint8 cmd, const quint32 startAddr, const quint32 size, const quint32 id, QByteArray &msg);
    void delay(const int time_ms);

    // Обработка данных
    void processData(const QByteArray &msg);
    // Инициализация таблицы
    void tableInit();
    // Очистка таблицы
    void tableClear();
    // Изменение размера таблицы
    void tableResize();
    // Обновление таблицы
    void tableUpdate();
    // Копирование одного стобца в другой
    void tableCopyColumn(const int from, const int to);
    // Обновление модели данных предназначеных для записи
    void writeDataUpdate();


};

#endif // MAINWINDOW_H
