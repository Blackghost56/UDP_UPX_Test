#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ip.setAddress(ui->ip_lineEdit->text());
    dst_port = quint16(ui->dst_port_spinBox->value());
    src_port = quint16(ui->src_port_spinBox->value());

    socket = new QUdpSocket(this);
    connect(socket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            [=](QAbstractSocket::SocketError socketError){ qDebug() << socketError; qDebug() << "Error string:" << socket->errorString();});
    socket->bind(QHostAddress::Any, src_port);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readPendingDatagrams()
{
    QByteArray buf;
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        if (datagram.data().length() > 4)
            buf = datagram.data().mid(4, datagram.data().length());
        //RAWdataOut(buf);
        processData(buf);
    }

    /*qDebug() << "< recive";
    int i = 0;
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        qDebug() << "pack: " << i++;
        RAWdataOut(datagram.data());
    }*/
}


void MainWindow::on_set_pushButton_clicked()
{
    ip.setAddress(ui->ip_lineEdit->text());
    dst_port = quint16(ui->dst_port_spinBox->value());
    quint16 buf_src_port = quint16(ui->src_port_spinBox->value());
    if (buf_src_port != src_port){
        socket->close();
        src_port = buf_src_port;
        socket->bind(QHostAddress::Any, src_port);
    }
}

void MainWindow::on_read_pushButton_clicked()
{
    quint32 baseAddr = quint32(ui->baseDec_spinBox->value());
    quint32 length = quint32(ui->length_spinBox->value());

    QByteArray buf;
    makeUPXHeader(MEM_READ_TYPE, baseAddr, length, 0, buf);
    sendMsg(buf);

    /*qDebug() << "> send begin";
    QByteArray buf;
    makeUPXHeader(MEM_READ_TYPE, 0, 6, 0, buf);
    sendMsg(buf);

    delay(1000);

    buf.clear();
    makeUPXHeader(MEM_READ_TYPE, 0, 20, 0, buf);
    sendMsg(buf);
    qDebug() << "> send end";*/
}

QString MainWindow::checkIP(const QString &str)
{
    QString out_str;
    QStringList list;
    QVector<int> vec;
    int buf = 0;
    bool flag = false;

    list = str.split(".");
    for (auto i : list)
    {
        buf = i.toInt(&flag);

        if (!(flag && (buf <= 255) && (buf >= 0)))         // IP check
        {
           buf = 0;
        }
        vec.append(buf);
    }
    out_str = QString("%1.%2.%3.%4").arg(vec.at(0)).arg(vec.at(1)).arg(vec.at(2)).arg(vec.at(3));

    return out_str;
}

void MainWindow::RAWdataOut(const QByteArray &data, const int str_length)
{
    QString str;
    for (int i = 0; i < data.length(); i++){
        if ((i % str_length == 0) && (i != 0))
        {
            qDebug() << str;
            str.clear();
        }
        str.push_back(QString("%1 ").arg(quint8(data.at(i)), 2, 16, QChar('0')));
    }
    if (!str.isEmpty())
        qDebug() << str;
}

void MainWindow::sendMsg(const QByteArray &msg)
{
    socket->writeDatagram(msg, ip, dst_port) ;
}

void MainWindow::makeUPXHeader(const quint8 cmd, const quint32 startAddr, const quint32 size, const quint32 id, QByteArray &msg)
{
    // Уникальный идентификатор
     msg.prepend(qint8(id));
     msg.prepend(qint8(id >>  8));
     msg.prepend(qint8(id >> 16));
     msg.prepend(qint8(id >> 24));

    // Тип сообщения
    quint32 tmp32 = quint32(cmd << 30);

    // Начальный адрес
    tmp32 |= ((startAddr & 0x3FFFF) << 12);

    // Размер
    tmp32 |= ((size - 1) & 0xFFF);

    // Добавляем заголовок в соообщение
    msg.prepend(qint8(tmp32));
    msg.prepend(qint8(tmp32 >>  8));
    msg.prepend(qint8(tmp32 >> 16));
    msg.prepend(qint8(tmp32 >> 24));
}

void MainWindow::delay(const int time_ms)
{
    QTime t1 = QTime::currentTime();
    while (t1.msecsTo(QTime::currentTime()) > time_ms){
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

void MainWindow::processData(const QByteArray &msg)
{
    QString str;
    QString delimiter = ui->delimiter_lineEdit->text();
    ui->data_plainTextEdit->clear();

    // Header
    if (ui->decAddres_checkBox->isChecked())
        str.push_back("Dec Address");             // 13
    if (ui->hexAddres_checkBox->isChecked())
        str.push_back(" | Hex Address");            // 13
    if (ui->decData_checkBox->isChecked())
        str.push_back(" | Dec Data");               // 10
    if (ui->hexData_checkBox->isChecked())
        str.push_back(" | Hex Data");              // 10
    ui->data_plainTextEdit->appendPlainText(str);

    // Body
    for (int i = 0; i < msg.length(); ++i){
        str.clear();
        if (ui->decAddres_checkBox->isChecked())
            str.push_back(QString("%0").arg(i + ui->baseDec_spinBox->value(), 0));
        if (ui->hexAddres_checkBox->isChecked())
            str.push_back(delimiter + QString("0x%0").arg(i + ui->baseDec_spinBox->value(), 0, 16, QChar('0')));
        if (ui->decData_checkBox->isChecked())
            str.push_back(delimiter + QString("%0").arg(quint8(msg.at(i)), 0));
        if (ui->hexData_checkBox->isChecked())
            str.push_back(delimiter + QString("0x%0").arg(quint8(msg.at(i)), 2, 16, QChar('0')));
        ui->data_plainTextEdit->appendPlainText(str);
    }

}

void MainWindow::on_ip_lineEdit_editingFinished()
{
    ui->ip_lineEdit->setText(checkIP(ui->ip_lineEdit->text()));
}

void MainWindow::on_baseDec_spinBox_valueChanged(int arg1)
{
    ui->baseHex_spinBox->setValue(arg1);
}

void MainWindow::on_baseHex_spinBox_valueChanged(int arg1)
{
    ui->baseDec_spinBox->setValue(arg1);
}
