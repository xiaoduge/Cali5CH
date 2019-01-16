#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dhexcmd.h"

namespace Ui {
class MainWindow;
}

class QSerialPort;
//class DHexCmd;
class QLabel;
class QComboBox;
class QPushButton;
class QTextEdit;
class QGroupBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    typedef enum  Cali_Btn
    {
        HVR_BTN = 0,
        LVR_BTN,
        HH_BTN,
        HL_BTN,
        LH_BTN,
        LL_BTN,
        RT_BTN,
        CONST_BTN, 
        OFFSET_BTN,
        READ_BTN,
        CALI_BTN_NUM,
    }CALI_BTN;

    typedef enum Port_Cfg
    {
        PortName = 0,
        Baud,
        DataBits,
        Parity,
        StopBits,
        FlowControl,
        Cfg_Num,
    }PORT_CFG;

    typedef enum Cali_Error
    {
        HVR_ERROR = 0,
        LVR_ERROR,
        HH_ERROR,
        HL_ERROR,
        LH_ERROR,
        LL_ERROR,
        RT_ERROR,
        CALI_ERROR_NUM,
    }CALI_ERROR;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *event);

protected slots:
    void on_switchCH_currentIndexChanged(int index);
    void on_configButton_toggled(bool checked);
    void on_clearButton_clicked();
    void on_switchBoadr_currentIndexChanged(int index);
    void on_ASCIIButton_clicked();
    void on_modbusButton_clicked();
    void on_printButton_clicked();
    void on_HVRButton_clicked();
    void on_LVRButton_clicked();
    void on_HHButton_clicked();
    void on_HLButton_clicked();
    void on_LHButton_clicked();
    void on_LLButton_clicked();
    void on_CONSTButton_clicked();
    void on_RTButton_clicked();
    void on_OFFSETButton_clicked();
    void on_READButton_clicked();
    void on_Savebutton_Clicked();

    void readSerialPort();

signals:
    void switchCHChanged(int index);

private:
    void createWidget(); // 创建子控件
    void initWidget();   // 创建控件文字描述
    void initLayout();   // 创建布局
    void updateSerialPortInfo();
    void initCaliError();

    void configSerialPort();
    void setPortName(const QString& portName);
    void setBaudRate(const QString& rate);
    void setParity(int parity);
    void setDataBits(int databits);
    void setStopBits(int stopbits);
    void setFlowControl(int flow);

    void initConnect();
    void sleep(unsigned int msec);
    void writeBuf(DHexCmd::CH_CMD code);
    void writeDebugBuf();
    void analysisChannel(QByteArray& bytes, DHexCmd::CH_CMD code);
    void analysisData(int ch, QByteArray& bytes, DHexCmd::CH_CMD code);
    void analysisCaliData(int ch, QByteArray& bytes, DHexCmd::CH_CMD code);
    void analysisReadData(int ch, QByteArray& bytes, DHexCmd::CH_CMD code);

    void updateTextEdit();
    void checkupData(int ch, int target, DHexCmd::CH_CMD code);

private:
    Ui::MainWindow *ui;

    QSerialPort* m_serialPort;
    DHexCmd* m_hexCmd;
    QByteArray m_receiveData;
    QString m_msgTextEdit;

//    bool m_isOpen;
    int m_updatePortInfoTimeId;

    bool m_caliError[CALI_ERROR_NUM];
//定义桌面控件
private:
    QLabel *m_portLabel[Cfg_Num];
    QComboBox *m_portComboBox[Cfg_Num];

    QLabel *m_pTypeLabel;
    QComboBox *m_pswitchBoard;
    int m_boardNum;

    QPushButton *m_configButton;
    QPushButton *m_clearButton;

    QPushButton *m_ASCIIButton;
    QPushButton *m_modbusButton;
    QPushButton *m_printButton;
    QPushButton *m_saveButton;

    QComboBox *m_switchCH;
    QLabel *m_chLabel;

    QPushButton *m_caliButton[CALI_BTN_NUM];

    QTextEdit *m_rTextEdit;
    QGroupBox *m_configGroup;
    QGroupBox *m_printGroup;
    QGroupBox *m_caliGroup;

    //状态栏信息
    QLabel *m_statusLabel;
    QLabel *m_msgLabel;
};

#endif // MAINWINDOW_H
