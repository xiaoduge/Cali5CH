#ifndef DHEXCMD_H
#define DHEXCMD_H

#include <QObject>
#include <QString>

/*
 * 调试确认：
C0 03 06 00 00 00 88 55     //保存               SAVE
C0 03 01 00 00 00 FC 54     //调试确认            confirm
C0 03 08 00 00 00 60 57     //输出模式切换为ASCII方式
C0 03 07 00 00 00 74 54     //校准参数打印输出
C0 03 09 00 00 00 9C 56     //输出模式切换为MODBUS方式

高量程VR空载： C1 02 80 81 00 02 38 91   //C1 02 02 D5 EC B4 C2   HVR
低量程VR空载： C1 02 8A 8B 00 02 E2 B2   //C1 02 02 D6 44 FA C3   LVR
高量程高限100：C1 02 82 83 00 02 40 31   //C1 02 02 67 D1 C5 77   HH
高量程低限500：C1 02 84 85 00 02 C9 D1   //C1 02 02 B1 14 F6 E9   HL
低量程高限500：C1 02 86 87 00 02 B1 71   //C1 02 02 43 36 8F 2C   LH
低量程低限1K： C1 02 88 89 00 02 9A 12   //C1 02 02 68 78 4B B2   LL
温度标定：C1 02 D0 D1 00 02 29 80        //C1 02 02 26 99 A3 47   RT
写电极常数：C1 03 8E 8F 03 E8 AC 4E   //无返回                     CONST
写温度偏差：C1 03 8C 8D 00 00 6A EE   //无返回                     OFFSET
通道1读取数据：C1 02 00 03 00 06 13 19                             READ

 */

class DHexCmd : public QObject
{
    Q_OBJECT
public:
    DHexCmd(QObject* parent = Q_NULLPTR);
    ~DHexCmd();

public:
    typedef enum  CHCmd
    {
        CMD_HVR = 0,
        CMD_LVR,
        CMD_HH,
        CMD_HL,
        CMD_LH,
        CMD_LL,
        CMD_RT,
        CMD_CONST,    
        CMD_OFFSET,
        CMD_READ,
        CH_CMD_NUM,
    }CH_CMD;

    typedef enum DebugCmd
    {
        CMD_DEBUG,
        CMD_SAVE,
        CMD_SWITCH_ASICC,
        CMD_SWITCH_MODBUS,
        CMD_PRINT,
        DEBUG_CMD_NUM,
    }DEBUG_CMD;

    QByteArray getSendCHData(CH_CMD code);
    QByteArray getSendDebugData(DEBUG_CMD code);
    void setCH(int ch);
    int getCH();

protected:
    void setHexCmd();
    void setDebugHexCmd();
    char convertHexChar(char ch);
    void stringToHex(QString str, QByteArray &sendData);

protected slots:
    void on_Ch_Change(int ch);

private:
    void initCH1StrHex();
    void initCH2StrHex();
    void initCH3StrHex();
    void initCH4StrHex();
    void initCH5StrHex();
    void initDebugStrHex();
private:
    QString m_strCHCmd[CH_CMD_NUM];
    QString m_strDebugCmd[DEBUG_CMD_NUM];

    QByteArray m_sendCHCmd[CH_CMD_NUM];
    QByteArray m_sendDebugCmd[DEBUG_CMD_NUM];

    int m_ch; //CH
};

#endif // DHEXCMD_H
