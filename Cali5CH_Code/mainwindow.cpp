#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "hexfloat.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTimer>
#include <QTime>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_hexCmd(new DHexCmd(this))
{
    ui->setupUi(this);
    initCaliError(); //默认
    m_serialPort = new QSerialPort(this);
    createWidget();
    this->setWindowTitle(tr("5CH"));
    this->resize(800, 600);
    updateSerialPortInfo();
//    m_updatePortInfoTimeId = startTimer(5000);
}

MainWindow::~MainWindow()
{
//    killTimer(m_updatePortInfoTimeId);
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_updatePortInfoTimeId)
    {
        updateSerialPortInfo();
    }
}

void MainWindow::on_switchCH_currentIndexChanged(int index)
{
    emit switchCHChanged(index);

    if(0 == index && 0 == m_boardNum)
    {
        m_caliButton[HVR_BTN]->setText(tr("HVR1"));
        m_caliButton[LVR_BTN]->setText(tr("LVR1"));
        m_caliButton[HH_BTN]->setText(tr("HH1"));
        m_caliButton[HL_BTN]->setText(tr("HL1"));
        m_caliButton[LH_BTN]->setText(tr("LH1"));
        m_caliButton[LL_BTN]->setText(tr("LL1"));
    }
    else
    {
        m_caliButton[HVR_BTN]->setText(tr("HVR"));
        m_caliButton[LVR_BTN]->setText(tr("LVR"));
        m_caliButton[HH_BTN]->setText(tr("HH"));
        m_caliButton[HL_BTN]->setText(tr("HL"));
        m_caliButton[LH_BTN]->setText(tr("LH"));
        m_caliButton[LL_BTN]->setText(tr("LL"));
    }
}

void MainWindow::on_configButton_toggled(bool checked)
{
    if(checked)
    {
        configSerialPort();       
        if(!m_serialPort->open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(this, tr("Waring"), tr("Open Serial Port Failed!"));
            m_configButton->setChecked(false);
            return;
        }
        qDebug() << tr("Open Serial Port success");
        m_configButton->setText(tr("Close"));
//        connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readSerialPort()));
        m_statusLabel->setText(tr("Open"));
        m_msgLabel->setText(tr("Open"));
    }
    else
    {
        m_configButton->setText(tr("Open"));
        if(m_serialPort->isOpen())
        {
            m_serialPort->close();
        }
        disconnect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readSerialPort()));
        m_statusLabel->setText(tr("Close"));
    }
}

void MainWindow::on_clearButton_clicked()
{
    m_rTextEdit->clear();
    m_msgLabel->setText(tr("Clear Complete"));
}

void MainWindow::on_switchBoadr_currentIndexChanged(int index)
{
    m_boardNum = index;
    if(0 == m_switchCH->currentIndex())
    {
        if(0 == m_boardNum)
        {
            m_caliButton[HVR_BTN]->setText(tr("HVR1"));
            m_caliButton[LVR_BTN]->setText(tr("LVR1"));
            m_caliButton[HH_BTN]->setText(tr("HH1"));
            m_caliButton[HL_BTN]->setText(tr("HL1"));
            m_caliButton[LH_BTN]->setText(tr("LH1"));
            m_caliButton[LL_BTN]->setText(tr("LL1"));
        }
        else
        {
            m_caliButton[HVR_BTN]->setText(tr("HVR"));
            m_caliButton[LVR_BTN]->setText(tr("LVR"));
            m_caliButton[HH_BTN]->setText(tr("HH"));
            m_caliButton[HL_BTN]->setText(tr("HL"));
            m_caliButton[LH_BTN]->setText(tr("LH"));
            m_caliButton[LL_BTN]->setText(tr("LL"));
        }
    }

    if(1 == m_boardNum)
    {
        m_portComboBox[Baud]->setCurrentIndex(7);
    }
    else
    {
         m_portComboBox[Baud]->setCurrentIndex(3);
    }

}

void MainWindow::on_ASCIIButton_clicked()
{
    QByteArray buf = m_hexCmd->getSendDebugData(DHexCmd::CMD_SWITCH_ASICC);
    m_serialPort->write(buf);
    sleep(200);
    m_msgLabel->setText(tr("Cmd ASCII"));
}

void MainWindow::on_modbusButton_clicked()
{
    QByteArray buf = m_hexCmd->getSendDebugData(DHexCmd::CMD_SWITCH_MODBUS);
    m_serialPort->write(buf);
    sleep(200);
    m_msgLabel->setText(tr("Cmd Modbus"));
}

void MainWindow::on_printButton_clicked()
{
    QByteArray buf = m_hexCmd->getSendDebugData(DHexCmd::CMD_PRINT);
    m_serialPort->write(buf);
    sleep(200);
    m_msgLabel->setText(tr("Finish printing"));
}

void MainWindow::on_HVRButton_clicked()
{
    writeBuf(DHexCmd::CMD_HVR);

    if(m_caliError[HVR_ERROR])
    {
         m_msgLabel->setText(tr("HVR Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali HVR"));
    }

}

void MainWindow::on_LVRButton_clicked()
{
    writeBuf(DHexCmd::CMD_LVR);
    if(m_caliError[LVR_ERROR])
    {
        m_msgLabel->setText(tr("LVR Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali LVR"));
    }
}

void MainWindow::on_HHButton_clicked()
{
    writeBuf(DHexCmd::CMD_HH);

    if(m_caliError[HH_ERROR])
    {
        m_msgLabel->setText(tr("HH Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali HH"));
    }

}

void MainWindow::on_HLButton_clicked()
{
    writeBuf(DHexCmd::CMD_HL);
    if(m_caliError[HL_ERROR])
    {
        m_msgLabel->setText(tr("HL Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali HL"));
    }

}

void MainWindow::on_LHButton_clicked()
{
    writeBuf(DHexCmd::CMD_LH);

    if(m_caliError[LH_ERROR])
    {
        m_msgLabel->setText(tr("LH Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali LH"));
    }

}

void MainWindow::on_LLButton_clicked()
{
    writeBuf(DHexCmd::CMD_LL);

    if(m_caliError[LL_ERROR])
    {
        m_msgLabel->setText(tr("LL Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali LL"));
    }

}

void MainWindow::on_CONSTButton_clicked()
{
    writeBuf(DHexCmd::CMD_CONST);
    m_msgLabel->setText(tr("Cali CONST"));
}

void MainWindow::on_RTButton_clicked()
{
    writeBuf(DHexCmd::CMD_RT);

    if(m_caliError[RT_ERROR])
    {
        m_msgLabel->setText(tr("RT Cali error"));
    }
    else
    {
        m_msgLabel->setText(tr("Cali RT"));
    }

}

void MainWindow::on_OFFSETButton_clicked()
{
    writeBuf(DHexCmd::CMD_OFFSET);
    m_msgLabel->setText(tr("Cali OFFSET"));
}

void MainWindow::on_READButton_clicked()
{
    QByteArray buf = m_hexCmd->getSendCHData(DHexCmd::CMD_READ);
    m_serialPort->write(buf);
    sleep(200);

    QByteArray tmp = m_serialPort->readAll();
    analysisChannel(tmp, DHexCmd::CMD_READ);
    m_msgLabel->setText(tr("Read Finished"));
}

void MainWindow::on_Savebutton_Clicked()
{
    qDebug() << tr("Save");
    QByteArray buf = m_hexCmd->getSendDebugData(DHexCmd::CMD_DEBUG);
    m_serialPort->write(buf);
    sleep(50);
    buf.clear();
    buf = m_hexCmd->getSendDebugData(DHexCmd::CMD_SAVE);
    m_serialPort->write(buf);
    sleep(200);
    m_msgLabel->setText(tr("Save Finished"));
}

void MainWindow::readSerialPort()
{
    QByteArray buf = m_serialPort->readAll();

    QDataStream out(&buf, QIODevice::ReadWrite);
    while(!out.atEnd())
    {
        qint8 outChar = 0;
        out >> outChar;
        m_receiveData.append(outChar);

//        QString strReceive = QString("%1").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));
//        m_receiveData += strReceive.toUpper();
    }
    qDebug() << "m_receiveData: " << m_receiveData;
}


void MainWindow::createWidget()
{
    for(int i = 0; i < Cfg_Num; i++)
    {
        m_portLabel[i] = new QLabel;
        m_portComboBox[i] = new QComboBox;
    }
    m_configButton = new QPushButton;
    m_configButton->setCheckable(true);
    m_clearButton = new QPushButton;
    m_pTypeLabel = new QLabel;
    m_pswitchBoard = new QComboBox;
    m_ASCIIButton = new QPushButton;
    m_modbusButton = new QPushButton;
    m_printButton = new QPushButton;
    m_saveButton = new QPushButton;


    m_switchCH = new QComboBox;
    m_switchCH->setMaximumWidth(200);
    m_switchCH->setMinimumWidth(150);
    m_chLabel = new QLabel;
    m_chLabel->setMaximumWidth(200);
    m_chLabel->setMinimumWidth(100);

    m_rTextEdit = new QTextEdit;
    m_rTextEdit->setReadOnly(true);
    m_configGroup = new QGroupBox;
    m_printGroup = new QGroupBox;
    m_caliGroup = new QGroupBox;

    for(int i = 0; i < CALI_BTN_NUM; i++)
    {
        m_caliButton[i] = new QPushButton;
    }

    m_msgLabel = new QLabel;
    m_statusLabel = new QLabel;

    initWidget();
    initLayout(); //启用布局
    initConnect(); //信号与槽
}

void MainWindow::initWidget()
{
    m_portLabel[PortName]->setText(tr("PortName"));
    m_portLabel[Baud]->setText(tr("BaudRate"));
    m_portLabel[DataBits]->setText(tr("DataBits"));
    m_portLabel[Parity]->setText(tr("Parity"));
    m_portLabel[StopBits]->setText(tr("StopBits"));
    m_portLabel[FlowControl]->setText(tr("FlowControl"));

    QStringList baudList;
    baudList << tr("1200") << tr("2400") << tr("4800") << tr("9600")
             << tr("19200") << tr("38400") << tr("57600") << tr("115200");
    m_portComboBox[Baud]->addItems(baudList);
    m_portComboBox[Baud]->setCurrentIndex(3);

    QStringList dataList;
    dataList << tr("5") << tr("6") << tr("7") << tr("8");
    m_portComboBox[DataBits]->addItems(dataList);
    m_portComboBox[DataBits]->setCurrentIndex(3);


    QStringList parityList;
    parityList << tr("None") << tr("Even") << tr("Odd") << tr("Space") << tr("Mark");
    m_portComboBox[Parity]->addItems(parityList);
    m_portComboBox[Parity]->setCurrentIndex(0);

    QStringList stopList;
    stopList << tr("1") << tr("1.5") << tr("2");
    m_portComboBox[StopBits]->addItems(stopList);
    m_portComboBox[StopBits]->setCurrentIndex(0);

    QStringList flowList;
    flowList << tr("None") << tr("RTS/CTS") << tr("XON/XOFF");
    m_portComboBox[FlowControl]->addItems(flowList);
    m_portComboBox[FlowControl]->setCurrentIndex(0);

    m_configButton->setText(tr("Open"));
    m_clearButton->setText(tr("Clear"));

    m_pTypeLabel->setText(tr("Type"));
    QStringList boardList;
    boardList << tr("5_CH") << tr("PURIST A+");
    m_pswitchBoard->addItems(boardList);
    m_pswitchBoard->setCurrentIndex(0);
    m_boardNum = 0;

    m_ASCIIButton->setText(tr("ASCII"));
    m_modbusButton->setText(tr("Modbus"));
    m_printButton->setText(tr("Print"));
    m_saveButton->setText(tr("Save"));

    m_chLabel->setText(tr("Channel Selection:"));

    QStringList chList;
    chList << tr("1") << tr("2") << tr("3") << tr("4") << tr("5");
    m_switchCH->addItems(chList);
    m_switchCH->setCurrentIndex(0);

    m_caliButton[HVR_BTN]->setText(tr("HVR1"));
    m_caliButton[LVR_BTN]->setText(tr("LVR1"));
    m_caliButton[HH_BTN]->setText(tr("HH1"));
    m_caliButton[HL_BTN]->setText(tr("HL1"));
    m_caliButton[LH_BTN]->setText(tr("LH1"));
    m_caliButton[LL_BTN]->setText(tr("LL1"));
    m_caliButton[CONST_BTN]->setText(tr("CONST"));
    m_caliButton[RT_BTN]->setText(tr("RT"));
    m_caliButton[OFFSET_BTN]->setText(tr("OFFSET"));
    m_caliButton[READ_BTN]->setText(tr("READ"));

    m_configGroup->setTitle(tr("SerialPort Config"));
    m_printGroup->setTitle(tr("Print"));
    m_caliGroup->setTitle(tr("Calibration"));

    m_statusLabel->setText(tr("Calibration Tool"));
    m_msgLabel->setText(tr("Close"));
}

void MainWindow::initLayout()
{
    QFormLayout* cfgLayout = new QFormLayout;
    cfgLayout->addRow(m_pTypeLabel, m_pswitchBoard);
    for(int i = 0; i < Cfg_Num; i++)
    {
        cfgLayout->addRow(m_portLabel[i], m_portComboBox[i]);
    }
    cfgLayout->addRow(m_configButton);
    cfgLayout->addRow(m_clearButton);

    m_configGroup->setLayout(cfgLayout);

    QVBoxLayout* printGroupLayout = new QVBoxLayout;
    printGroupLayout->addWidget(m_ASCIIButton);
    printGroupLayout->addWidget(m_modbusButton);
    printGroupLayout->addWidget(m_printButton);
    m_printGroup->setLayout(printGroupLayout);

    QSpacerItem* spacerItem = new QSpacerItem(2, 10);
    QVBoxLayout* leftVLayout = new QVBoxLayout;
    leftVLayout->addWidget(m_configGroup);
    leftVLayout->addSpacerItem(spacerItem);
    leftVLayout->addWidget(m_printGroup);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addLayout(leftVLayout);
    topLayout->addSpacing(20);
    topLayout->addWidget(m_rTextEdit);

    QHBoxLayout* h1Layout = new QHBoxLayout;
//    h1Layout->addStretch(1);
    h1Layout->addWidget(m_chLabel);
    h1Layout->addWidget(m_switchCH);
    h1Layout->addWidget(m_saveButton);
    h1Layout->addStretch(1);

    QHBoxLayout* h2Layout = new QHBoxLayout;
    for(int i = 0; i < 5; i++)
    {
        h2Layout->addWidget(m_caliButton[i]);
    }

    QHBoxLayout* h3Layout = new QHBoxLayout;
    for(int i = 5; i < CALI_BTN_NUM; i++)
    {
        h3Layout->addWidget(m_caliButton[i]);
    }

    QVBoxLayout* bottomLayout = new QVBoxLayout;
    bottomLayout->addLayout(h1Layout);
    bottomLayout->addLayout(h2Layout);
    bottomLayout->addLayout(h3Layout);
    m_caliGroup->setLayout(bottomLayout);

    QHBoxLayout* labelLayout = new QHBoxLayout;
    labelLayout->addStretch(1);
    labelLayout->addWidget(m_msgLabel);
    labelLayout->addStretch(1);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addLayout(labelLayout);
    layout->addWidget(m_caliGroup);

    ui->centralWidget->setLayout(layout);
    ui->statusBar->addPermanentWidget(m_statusLabel);
}

void MainWindow::updateSerialPortInfo()
{
    m_portComboBox[PortName]->clear();
    foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serialPort;
        serialPort.setPort(info);
        if(serialPort.open(QIODevice::ReadWrite))
        {
            m_portComboBox[PortName]->addItem(serialPort.portName());
            serialPort.close();
        }
    }
}

void MainWindow::initCaliError()
{
    for(int i = 0; i < CALI_ERROR_NUM; i++)
    {
        m_caliError[i] = false;
    }
}

void MainWindow::configSerialPort()
{
    setPortName(m_portComboBox[PortName]->currentText());
    setBaudRate(m_portComboBox[Baud]->currentText());
    setDataBits(m_portComboBox[DataBits]->currentIndex());
    setParity(m_portComboBox[Parity]->currentIndex());
    setStopBits(m_portComboBox[StopBits]->currentIndex());
    setFlowControl(m_portComboBox[FlowControl]->currentIndex());
}

void MainWindow::setPortName(const QString &portName)
{
    m_serialPort->setPortName(portName);
}

void MainWindow::setBaudRate(const QString &rate)
{
    m_serialPort->setBaudRate(rate.toInt());
}

void MainWindow::setParity(int parity)
{
    switch (parity)
    {
    case 0:
        m_serialPort->setParity(QSerialPort::NoParity);
        break;
    case 1:
        m_serialPort->setParity(QSerialPort::EvenParity);
        break;
    case 2:
        m_serialPort->setParity(QSerialPort::OddParity);
        break;
    case 3:
        m_serialPort->setParity(QSerialPort::SpaceParity);
        break;
    case 4:
        m_serialPort->setParity(QSerialPort::MarkParity);
        break;
    default:
        break;
    }
}

void MainWindow::setDataBits(int databits)
{
    switch(databits)
    {
    case 0:
        m_serialPort->setDataBits(QSerialPort::Data5);
        break;
    case 1:
        m_serialPort->setDataBits(QSerialPort::Data6);
        break;
    case 2:
        m_serialPort->setDataBits(QSerialPort::Data7);
        break;
    case 3:
        m_serialPort->setDataBits(QSerialPort::Data8);
        break;
    default:
        break;
    }
}

void MainWindow::setStopBits(int stopbits)
{
    switch(stopbits)
    {
    case 0:
        m_serialPort->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        m_serialPort->setStopBits(QSerialPort::OneAndHalfStop);
        break;
    case 2:
        m_serialPort->setStopBits(QSerialPort::TwoStop);
        break;
    default:
        break;
    }
}

void MainWindow::setFlowControl(int flow)
{
    switch(flow)
    {
    case 0:
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        break;
    case 1:
        m_serialPort->setFlowControl(QSerialPort::HardwareControl);
        break;
    case 2:
        m_serialPort->setFlowControl(QSerialPort::SoftwareControl);
        break;
    default:
        break;
    }
}

void MainWindow::initConnect()
{
    connect(m_configButton, SIGNAL(toggled(bool)), this, SLOT(on_configButton_toggled(bool)));
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::on_clearButton_clicked);
    connect(m_ASCIIButton, &QPushButton::clicked, this, &MainWindow::on_ASCIIButton_clicked);
    connect(m_modbusButton, &QPushButton::clicked, this, &MainWindow::on_modbusButton_clicked);
    connect(m_printButton, &QPushButton::clicked, this, &MainWindow::on_printButton_clicked);

    connect(m_caliButton[HVR_BTN], &QPushButton::clicked, this, &MainWindow::on_HVRButton_clicked);
    connect(m_caliButton[LVR_BTN], &QPushButton::clicked, this, &MainWindow::on_LVRButton_clicked);
    connect(m_caliButton[HH_BTN], &QPushButton::clicked, this, &MainWindow::on_HHButton_clicked);
    connect(m_caliButton[HL_BTN], &QPushButton::clicked, this, &MainWindow::on_HLButton_clicked);
    connect(m_caliButton[LH_BTN], &QPushButton::clicked, this, &MainWindow::on_LHButton_clicked);
    connect(m_caliButton[LL_BTN], &QPushButton::clicked, this, &MainWindow::on_LLButton_clicked);
    connect(m_caliButton[CONST_BTN], &QPushButton::clicked, this, &MainWindow::on_CONSTButton_clicked);
    connect(m_caliButton[RT_BTN], &QPushButton::clicked, this, &MainWindow::on_RTButton_clicked);
    connect(m_caliButton[OFFSET_BTN], &QPushButton::clicked, this, &MainWindow::on_OFFSETButton_clicked);
    connect(m_caliButton[READ_BTN], &QPushButton::clicked, this, &MainWindow::on_READButton_clicked);

    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::on_Savebutton_Clicked);

    connect(m_pswitchBoard, SIGNAL(currentIndexChanged(int)), this, SLOT(on_switchBoadr_currentIndexChanged(int)));
    connect(m_switchCH, SIGNAL(currentIndexChanged(int)), this, SLOT(on_switchCH_currentIndexChanged(int)));
    connect(this, SIGNAL(switchCHChanged(int)), m_hexCmd, SLOT(on_Ch_Change(int)));
}

void MainWindow::sleep(unsigned int msec)
{
    QTime delayTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < delayTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void MainWindow::writeBuf(DHexCmd::CH_CMD code)
{
    QByteArray buf;
    if(0 == m_boardNum)
    {
        buf = m_hexCmd->getSendDebugData(DHexCmd::CMD_DEBUG);
        m_serialPort->write(buf);
        sleep(300);
        buf.clear();
    }

    buf = m_hexCmd->getSendCHData(code);
    m_serialPort->write(buf);
    sleep(500);

    QByteArray tmp = m_serialPort->readAll();
    analysisChannel(tmp, code);

}

void MainWindow::writeDebugBuf()
{

}

void MainWindow::analysisChannel(QByteArray &bytes, DHexCmd::CH_CMD code)
{
    int ch = (bytes[0]&0xFF)*256 + (bytes[1]&0xFF);
    switch (ch)
    {
    case 0xc102:
        analysisData(0, bytes, code);
        break;
    case 0xc202:
        analysisData(1, bytes, code);
        break;
    case 0xc302:
        analysisData(2, bytes, code);
        break;
    case 0xc402:
        analysisData(3, bytes, code);
        break;
    case 0xc502:
        analysisData(4, bytes, code);
        break;
    default:
        break;
    }
    updateTextEdit();
}

void MainWindow::analysisData(int ch, QByteArray &bytes, DHexCmd::CH_CMD code)
{
    int dataBits = bytes[2]&0xFF;
    switch (dataBits)
    {
    case 2:
        analysisCaliData(ch, bytes, code);
        break;
    case 6:
        analysisReadData(ch, bytes, code);
        break;
    default:
        break;
    }

}

void MainWindow::analysisCaliData(int ch, QByteArray &bytes, DHexCmd::CH_CMD code)
{
//    Q_UNUSED(ch);
    m_msgTextEdit.clear();
    int caliR = (bytes[3]&0xFF)*256 + (bytes[4]&0xFF);
    checkupData(ch, caliR, code);
    switch (code)
    {
    case 0:
        m_msgTextEdit = tr("HVR: %1").arg(caliR);
        break;
    case 1:
        m_msgTextEdit = tr("LVR: %1").arg(caliR);
        break;
    case 2:
        m_msgTextEdit = tr("HH: %1").arg(caliR);
        break;
    case 3:
        m_msgTextEdit = tr("HL: %1").arg(caliR);
        break;
    case 4:
        m_msgTextEdit = tr("LH: %1").arg(caliR);
        break;
    case 5:
        m_msgTextEdit = tr("LL: %1").arg(caliR);
        break;
    case 6:
        m_msgTextEdit = tr("RT: %1").arg(caliR);
        break;
    default:
        break;
    }
}

void MainWindow::analysisReadData(int ch, QByteArray &bytes, DHexCmd::CH_CMD code)
{
    Q_UNUSED(code);

    QByteArray byteArray;
    byteArray = bytes;
    QByteArray rBuf = byteArray.mid(3, 4);
    QString strR;
    QDataStream out1(&rBuf, QIODevice::ReadWrite);
    while(!out1.atEnd())
    {
        qint8 outChar = 0;
        out1 >> outChar;

        QString strReceive = QString("%1").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));
        strR += strReceive;
    }
    qDebug() << "R: " << strR;
    QByteArray tBuf = byteArray.mid(7, 2);
    QString strT;
    QDataStream out2(&tBuf, QIODevice::ReadWrite);
    while(!out2.atEnd())
    {
        qint8 outChar = 0;
        out2 >> outChar;

        QString strReceive = QString("%1").arg(outChar&0xFF, 2, 16, QLatin1Char('0'));
        strT += strReceive;
    }

    qDebug() << "T: " << strT;
    DHexFloat hexFloat;
    bool ok;
    hexFloat.src = strR.toLong(&ok, 16)&0xFFFFFFFF;
    if(!ok)
    {
        qDebug() << "hexFloatError";
        return;
    }
    switch (ch) {
    case 0:
        m_msgTextEdit = tr("1CH: R: %1    T: %2").arg(hexFloat.dest).arg(strT.toInt(&ok, 16));
        break;
    case 1:
        m_msgTextEdit = tr("2CH: R: %1    T: %2").arg(hexFloat.dest).arg(strT.toInt(&ok, 16));
        break;
    case 2:
        m_msgTextEdit = tr("3CH: R: %1    T: %2").arg(hexFloat.dest).arg(strT.toInt(&ok, 16));
        break;
    case 3:
        m_msgTextEdit = tr("4CH: R: %1    T: %2").arg(hexFloat.dest).arg(strT.toInt(&ok, 16));
        break;
    case 4:
        m_msgTextEdit = tr("5CH: R: %1    T: %2").arg(hexFloat.dest).arg(strT.toInt(&ok, 16));
        break;
    default:
        break;
    }
}

void MainWindow::updateTextEdit()
{
    m_rTextEdit->append(m_msgTextEdit);
    m_msgTextEdit.clear();
}

void MainWindow::checkupData(int ch, int target, DHexCmd::CH_CMD code)
{
    switch (code)
    {
    case 0:
    case 1:
    {
        if(0 == ch && 0 == m_boardNum)
        {
            if(target < 50000 || target > 60000)
            {   
                m_caliError[HVR_ERROR] = true;
                m_caliError[LVR_ERROR] = true;
            }
            else
            {
                m_caliError[HVR_ERROR] = false;
                m_caliError[LVR_ERROR] = false;
            }
        }
        else
        {
            if(target < 40000 || target > 50000)
            {
                m_caliError[HVR_ERROR] = true;
                m_caliError[LVR_ERROR] = true;
            }
            else
            {
                m_caliError[HVR_ERROR] = false;
                m_caliError[LVR_ERROR] = false;
            }
        }

        break;
    }

    case 2:
    {
        if(0 == ch && 0 == m_boardNum)
        {
            if(target < 25000 || target > 30000)
            {
                m_caliError[HH_ERROR] = true;
            }
            else
            {
                m_caliError[HH_ERROR] = false;
            }
        }
        else
        {
            if(target < 20000 || target > 25000)
            {
                m_caliError[HH_ERROR] = true;
            }
            else
            {
                m_caliError[HH_ERROR] = false;
            }
        }

        break;
    }
    case 3:
    {
        if(0 == ch && 0 == m_boardNum)
        {
            if(target < 40000 || target > 50000)
            {   
                m_caliError[HL_ERROR] = true;
            }
            else
            {
                m_caliError[HL_ERROR] = false;
            }
        }
        else
        {
            if(target < 32000 || target > 40000)
            {
                m_caliError[HL_ERROR] = true;
            }
            else
            {
                m_caliError[HL_ERROR] = false;
            }
        }

        break;
    }
    case 4:
    {
        if(0 == ch && 0 == m_boardNum)
        {
            if(target < 14000 || target > 20000)
            {
                m_caliError[LH_ERROR] = true;
            }
            else
            {
                m_caliError[LH_ERROR] = false;
            }
        }
        else
        {
            if(target < 12000 || target > 16000)
            {
                m_caliError[LH_ERROR] = true;
            }
            else
            {
                m_caliError[LH_ERROR] = false;
            }
        }

        break;
    }
    case 5:
    {
        if(0 == ch && 0 == m_boardNum)
        {
            if(target < 25000 || target > 30000)
            {
                m_caliError[LL_ERROR] = true;
            }
            else
            {
                m_caliError[LL_ERROR] = false;
            }
        }
        else
        {
            if(target < 20000 || target > 23000)
            {
                m_caliError[LL_ERROR] = true;
            }
            else
            {
                m_caliError[LL_ERROR] = false;
            }
        }

        break;
    }
    case 6:
    {
        if(target < 9000 || target > 11000)
        {
            m_caliError[RT_ERROR] = true;
        }
        else
        {
            m_caliError[RT_ERROR] = false;
        }
        break;
    }
    default:
        break;
    }
}


