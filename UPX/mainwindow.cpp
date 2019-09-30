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
    baseAddr = quint32(ui->baseDec_spinBox->value());
    length = quint32(ui->length_spinBox->value());
    tableInit();

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
    QByteArray buf;
    makeUPXHeader(MEM_READ_TYPE, baseAddr, length, 0, buf);
    sendMsg(buf);
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

int MainWindow::StrIntValueCheck(QString &&str, const int base, const int min, const int max, const int def, int &state, QString label)
{
    state = OK;
    int buf = 0;
    bool flag = false;

    buf = str.toInt(&flag, base);

    if (!flag)
    {
        state = MISTAKE;
        qDebug() << "Input value '" << label << "' incorrected";
        buf = def;
        str = QString::number(def);
    }
    if (buf < min)
    {
        state = LESS;
        qDebug() << "Input value '" << label << "' less than the lower limit";
        buf = min;
        str = QString::number(min);
    }
    if (buf > max)
    {
        state = GREATER;
        qDebug() << "Input value '" << label << "' is greater than the upper limit";
        buf = max;
        str = QString::number(max);
    }
    return buf;
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
    currentData.readData = msg;
    currentData.addr = baseAddr;
    writeDataUpdate();
    tableUpdate();
}

void MainWindow::tableInit()
{
    tableResize();
}

void MainWindow::tableClear()
{
    ui->tableWidget->clear();
}

void MainWindow::tableResize()
{
    writeDataUpdate();

    tableClear();
    QStringList HorizontalHeaderLabels;

    if (ui->decAddres_checkBox->isChecked())
        HorizontalHeaderLabels << STR_DEC_ADRR;
    if (ui->hexAddres_checkBox->isChecked())
        HorizontalHeaderLabels << STR_HEX_ADRR;
    HorizontalHeaderLabels << STR_READ_DATA << STR_WRITE_DATA;

    const int NumRow = int(length);
    const int NumColumn = HorizontalHeaderLabels.length();

    ui->tableWidget->setColumnCount(NumColumn);
    ui->tableWidget->setRowCount(NumRow);
    //ui->tableWidget->verticalHeader()->setDefaultSectionSize(10);
    ui->tableWidget->verticalHeader()->setVisible(false);
    //verticalHeader()->setStretchLastSection(true);
    //ui->tableWidget->setVerticalHeaderLabels(VerticalHeaderLabels);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);

    int posColumn = 0;
    QTableWidgetItem *item;

    // Dec Address
    if (ui->decAddres_checkBox->isChecked()){
        for (int i = 0; i < NumRow; i++)
        {
            item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString("%0").arg(baseAddr + quint32(i)));
            ui->tableWidget->setItem(i, posColumn, item);
        }
        posColumn++;
    }

    // Hex Address
    if (ui->hexAddres_checkBox->isChecked()){
        for (int i = 0; i < NumRow; i++)
        {
            item = new QTableWidgetItem();
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(QString("%0").arg(baseAddr + quint32(i), 0, 16));
            ui->tableWidget->setItem(i, posColumn, item);
        }
        posColumn++;
    }

    // Read data
    for (int i = 0; i < NumRow; i++)
    {
        item = new QTableWidgetItem();
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText("0");
        ui->tableWidget->setItem(i, posColumn, item);
    }
    posColumn++;

    // Write data
    for (int i = 0; i < NumRow; i++)
    {
        item = new QTableWidgetItem();
        item->setTextAlignment(Qt::AlignCenter);
        item->setText("0");
        ui->tableWidget->setItem(i, posColumn, item);
    }

    tableUpdate();
}

void MainWindow::tableUpdate()
{



    const int rowCount = ui->tableWidget->rowCount();
    const int lastColumn = ui->tableWidget->columnCount() - 1;
    qint32 offset = qint32(currentData.addr - baseAddr);
    qint32 absOffset = qAbs(offset);

    // buf init
    QByteArray bufR, bufW;
    bufR.resize(rowCount);
    bufW.resize(rowCount);
    for (auto it = bufR.begin(); it < bufR.end(); ++it){
        *it = 0;
    }
    for (auto it = bufW.begin(); it < bufW.end(); ++it){
        *it = 0;
    }


    if ((rowCount - absOffset) > 0){
        int j = 0;
        if (offset < 0){

            for (int i = absOffset; (j < rowCount) && (i < currentData.readData.length()); ++i, ++j) {
                bufR[j] =  currentData.readData.at(i);
            }
            j = 0;
            for (int i = absOffset; (j < rowCount) && (i < currentData.writeData.length()); ++i, ++j) {
                bufW[j] =  currentData.writeData.at(i);
            }


        } else {

            j = 0;
            for (int i = absOffset; (i < rowCount) && (j < currentData.readData.length()); ++i, ++j) {
                bufR[i] =  currentData.readData.at(j);
            }
            j = 0;
            for (int i = absOffset; (i < rowCount) && (j < currentData.writeData.length()); ++i, ++j) {
                bufW[i] =  currentData.writeData.at(j);
            }


        }
    }

    currentData.addr = baseAddr;
    currentData.readData = bufR;
    currentData.writeData = bufW;

    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        ui->tableWidget->item(i, lastColumn)->setText(QString("%0").arg(quint8(bufW.at(i)), 2, 16));
        ui->tableWidget->item(i, lastColumn - 1)->setText(QString("%0").arg(quint8(bufR.at(i)), 2, 16));
    }
}

void MainWindow::tableCopyColumn(const int from, const int to)
{
    if ((ui->tableWidget->columnCount() <= from) || (ui->tableWidget->columnCount() <= to)){
        qDebug() << "Error. Copy table column. Size: " << ui->tableWidget->columnCount() << "\tfrom: " << from << "\tto: " << to;
        return;
    }
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        ui->tableWidget->item(i, to)->setText(ui->tableWidget->item(i, from)->text());
    }
}

void MainWindow::writeDataUpdate()
{
    currentData.writeData.clear();
    int state;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        currentData.writeData.push_back(char(StrIntValueCheck(ui->tableWidget->item(row, ui->tableWidget->columnCount() - 1)->text(), 16, 0, 255, 0, state)));
    }
}

void MainWindow::on_ip_lineEdit_editingFinished()
{
    ui->ip_lineEdit->setText(checkIP(ui->ip_lineEdit->text()));
}

void MainWindow::on_baseDec_spinBox_valueChanged(int arg1)
{
    ui->baseHex_spinBox->setValue(arg1);
    baseAddr = quint32(arg1);
    tableResize();
}

void MainWindow::on_baseHex_spinBox_valueChanged(int arg1)
{
    ui->baseDec_spinBox->setValue(arg1);
}

void MainWindow::on_length_spinBox_valueChanged(int arg1)
{
    length = quint32(arg1);
    tableResize();
}

void MainWindow::on_write_pushButton_clicked()
{
    int lastColumn = ui->tableWidget->columnCount() - 1;
    QByteArray buf;
    int state;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i){
        buf.push_back(char(StrIntValueCheck(ui->tableWidget->item(i, lastColumn)->text(), 16, 0, 255, 0, state)));
        if (state != OK){
            qDebug() << "Error. Writed data incorrected";
            return;
        }
    }
    RAWdataOut(buf);
    makeUPXHeader(MEM_WRITE_TYPE, baseAddr, length, 0, buf);
    sendMsg(buf);
}

void MainWindow::on_save_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "reg_data"
                                                    + QString("_0x%0__L%1").arg(baseAddr, 0, 16).arg(length), tr("CSV files (*.csv)"));

    if (fileName == "") return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "Error opening file";
        return;
    }

    QTextStream out(&file);

    int lastColumn = ui->tableWidget->columnCount() - 1;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        out << ui->tableWidget->item(row, lastColumn)->text() << endl;
    }

    file.close();
}

void MainWindow::on_open_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "reg_data" , tr("Text files (*.csv)"));

    if (fileName == "") return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Error opening file";
        return;
    }


    QByteArray buf;
    int state;
    while (!file.atEnd()) {
        buf.push_back(char(StrIntValueCheck(file.readLine(), 16, 0, 255, 0, state) & 0xff));
    }
    file.close();

    int rowCount = ui->tableWidget->rowCount();
    if (buf.length() != rowCount){
        QMessageBox msgBox;
        msgBox.setText(tr("File contains invalid data"));
        msgBox.exec();
        return;
    }
    currentData.writeData = buf;
    tableUpdate();
}

void MainWindow::on_copy_pushButton_clicked()
{
    int lastColum = ui->tableWidget->columnCount() - 1;
    tableCopyColumn(lastColum - 1, lastColum);
}

void MainWindow::on_decAddres_checkBox_clicked()
{
    tableResize();
}

void MainWindow::on_hexAddres_checkBox_clicked()
{
    tableResize();
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    int lastColumn = ui->tableWidget->columnCount() - 1;
    if (lastColumn == column){
        int state;
        int res = StrIntValueCheck(ui->tableWidget->item(row, column)->text(), 16, 0, 255, 0, state);
        if (state != OK){
            ui->tableWidget->item(row, column)->setText(QString("%0").arg(res, 0, 16));
        }
    }
}
