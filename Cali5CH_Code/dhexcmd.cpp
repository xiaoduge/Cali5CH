#include "dhexcmd.h"
#include <QDebug>

DHexCmd::DHexCmd(QObject* parent):QObject(parent)
{
    initCH1StrHex();
    setHexCmd();

    initDebugStrHex();
    setDebugHexCmd();
}

DHexCmd::~DHexCmd()
{
    qDebug() << "~DHexCmd()";
}

QByteArray DHexCmd::getSendCHData(DHexCmd::CH_CMD code)
{
    return m_sendCHCmd[code];
}

QByteArray DHexCmd::getSendDebugData(DHexCmd::DEBUG_CMD code)
{
    return m_sendDebugCmd[code];
}

void DHexCmd::setHexCmd()
{
    for(int i = 0; i < CH_CMD_NUM; i++)
    {
        stringToHex(m_strCHCmd[i], m_sendCHCmd[i]);
    }

}

void DHexCmd::setDebugHexCmd()
{
    for(int i = 0; i < DEBUG_CMD_NUM; i++)
    {
        stringToHex(m_strDebugCmd[i], m_sendDebugCmd[i]);
    }
}


void DHexCmd::setCH(int ch)
{
    m_ch = ch;
}

int DHexCmd::getCH()
{
    return m_ch;
}

char DHexCmd::convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
    {
        return ch - 0x30;
    }
    else if((ch >= 'a') && (ch <= 'f'))
    {
        return ch - 'a' + 10;
    }
    else if((ch >= 'A') && (ch <= 'F'))
    {
        return ch - 'A' + 10;
    }
    else
    {
        return ch - ch;
    }
}

void DHexCmd::stringToHex(QString str, QByteArray &sendData)
{
    char lstr, hstr;
    int hHexData, lHexData, hexData;
    int dataLen = 0;
    int len = str.length();
    sendData.resize(len/2);
    for(int i = 0; i < len;)
    {
        hstr = str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
        {
            break;
        }
        lstr = str[i].toLatin1();

        hHexData = convertHexChar(hstr);
        lHexData = convertHexChar(lstr);
        if(lstr == 16 || hHexData == 16)
        {
            break;
        }
        else
        {
            hexData = hHexData*16 + lHexData;
        }
        i++;
        sendData[dataLen] = (char)hexData;
        dataLen++;
    }
    sendData.resize(dataLen);
}

void DHexCmd::on_Ch_Change(int ch)
{
    qDebug() <<tr("Changed: %1").arg(ch);
    setCH(ch);
    switch (ch)
    {
    case 0:
        initCH1StrHex();
        break;
    case 1:
        initCH2StrHex();
        break;
    case 2:
        initCH3StrHex();
        break;
    case 3:
        initCH4StrHex();
        break;
    case 4:
        initCH5StrHex();
        break;
    default:
        break;
    }
    setHexCmd();
}

/*
 * 1通道：
高量程VR空载： C1 02 80 81 00 02 38 91   //C1 02 02 D5 EC B4 C2
低量程VR空载： C1 02 8A 8B 00 02 E2 B2   //C1 02 02 D6 44 FA C3
高量程高限100：C1 02 82 83 00 02 40 31   //C1 02 02 67 D1 C5 77
高量程低限500：C1 02 84 85 00 02 C9 D1   //C1 02 02 B1 14 F6 E9
低量程高限500：C1 02 86 87 00 02 B1 71   //C1 02 02 43 36 8F 2C
低量程低限1K： C1 02 88 89 00 02 9A 12   //C1 02 02 68 78 4B B2
温度标定：C1 02 D0 D1 00 02 29 80        //C1 02 02 26 99 A3 47
写电极常数：C1 03 8E 8F 03 E8 AC 4E   //无返回
写温度偏差：C1 03 8C 8D 00 00 6A EE   //无返回
通道1读取数据：C1 02 00 03 00 06 13 19
*/
void DHexCmd::initCH1StrHex()
{
    m_strCHCmd[CMD_HVR] = "C1 02 80 81 00 02 38 91"; //1CH 高量程VR空载
    m_strCHCmd[CMD_LVR] = "C1 02 8A 8B 00 02 E2 B2"; //1CH 低量程VR空载
    m_strCHCmd[CMD_HH] = "C1 02 82 83 00 02 40 31"; //1CH 高量程高限100
    m_strCHCmd[CMD_HL] = "C1 02 84 85 00 02 C9 D1"; //1CH 高量程低限500
    m_strCHCmd[CMD_LH] = "C1 02 86 87 00 02 B1 71"; //1CH 低量程高限500
    m_strCHCmd[CMD_LL] = "C1 02 88 89 00 02 9A 12"; //1CH 低量程低限1K
    m_strCHCmd[CMD_CONST] = "C1 03 8E 8F 03 E8 AC 4E"; //1CH 写电极常数
    m_strCHCmd[CMD_RT] = "C1 02 D0 D1 00 02 29 80"; //1CH 温度标定
    m_strCHCmd[CMD_OFFSET] = "C1 03 8C 8D 00 00 6A EE"; //1CH 写温度偏差
    m_strCHCmd[CMD_READ] = "C1 02 00 03 00 06 13 19"; //1CH 读数据
}

/*
 * 2通道：
高量程VR空载： C2 02 90 91 00 02 0E 94
低量程VR空载： C2 02 9A 9B 00 02 D4 B7
高量程高限10K：C2 02 92 93 00 02 76 34
高量程低限50K：C2 02 94 95 00 02 FF D4
低量程高限50K：C2 02 96 97 00 02 87 74
低量程低限100K：C2 02 98 99 00 02 AC 17
温度标定：C2 02 D2 D3 00 02 62 20
写电极常数：C2 03 9E 9F 03 E8 9A 4B
写温度偏差：C2 02 9C 9D 00 02 5D 57
通道2读取指令：C2 02 00 0E 00 06 E3 88
*/
void DHexCmd::initCH2StrHex()
{
    m_strCHCmd[CMD_HVR] = "C2 02 90 91 00 02 0E 94";
    m_strCHCmd[CMD_LVR] = "C2 02 9A 9B 00 02 D4 B7";
    m_strCHCmd[CMD_HH] = "C2 02 92 93 00 02 76 34";
    m_strCHCmd[CMD_HL] = "C2 02 94 95 00 02 FF D4";
    m_strCHCmd[CMD_LH] = "C2 02 96 97 00 02 87 74";
    m_strCHCmd[CMD_LL] = "C2 02 98 99 00 02 AC 17";
    m_strCHCmd[CMD_CONST] = "C2 03 9E 9F 03 E8 9A 4B";
    m_strCHCmd[CMD_RT] = "C2 02 D2 D3 00 02 62 20";
    m_strCHCmd[CMD_OFFSET] = "C2 02 9C 9D 00 02 5D 57";
    m_strCHCmd[CMD_READ] = "C2 02 00 0E 00 06 E3 88"; //2CH 读数据
}

/*
 * 3通道：
高量程VR空载： C3 02 A0 A1 00 02 D0 9A
低量程VR空载： C3 02 AA AB 00 02 0A B9
高量程高限10K：C3 02 A2 A3 00 02 A8 3A
高量程低限50K：C3 02 A4 A5 00 02 21 DA
低量程高限50K：C3 02 A6 A7 00 02 59 7A
低量程低限100K：C3 02 A8 A9 00 02 72 19
温度标定：C3 02 D4 D5 00 02 3A C1
写电极常数：C3 03 AE AF 03 E8 44 45
写温度偏差：C3 03 AC AD 00 00 82 E5
通道3读取指令：C3 02 00 19 00 06 36 39
*/
void DHexCmd::initCH3StrHex()
{
    m_strCHCmd[CMD_HVR] = "C3 02 A0 A1 00 02 D0 9A";
    m_strCHCmd[CMD_LVR] = "C3 02 AA AB 00 02 0A B9";
    m_strCHCmd[CMD_HH] = "C3 02 A2 A3 00 02 A8 3A";
    m_strCHCmd[CMD_HL] = "C3 02 A4 A5 00 02 21 DA";
    m_strCHCmd[CMD_LH] = "C3 02 A6 A7 00 02 59 7A";
    m_strCHCmd[CMD_LL] = "C3 02 A8 A9 00 02 72 19";
    m_strCHCmd[CMD_CONST] = "C3 03 AE AF 03 E8 44 45";
    m_strCHCmd[CMD_RT] = "C3 02 D4 D5 00 02 3A C1";
    m_strCHCmd[CMD_OFFSET] = "C3 03 AC AD 00 00 82 E5";
    m_strCHCmd[CMD_READ] = "C3 02 00 19 00 06 36 39"; //3CH 读数据
}

/*
 * 4通道：
高量程VR空载： C4 02 B0 B1 00 02 62 9E
低量程VR空载： C4 02 BA BB 00 02 B8 BD
高量程高限10K：C4 02 B2 B3 00 02 1A 3E
高量程低限50K：C4 02 B4 B5 00 02 93 DE
低量程高限50K：C4 02 B6 B7 00 02 EB 7E
低量程低限100K：C4 02 B8 B9 00 02 C0 1D
温度标定：C4 02 D6 D7 00 02 F5 60
写电极常数：C4 03 BE BF 03 E8 F6 41
写温度偏差：C4 03 BC BD 00 00 30 E1
通道4读取指令：C4 02 00 21 00 06 4C B9
*/
void DHexCmd::initCH4StrHex()
{
    m_strCHCmd[CMD_HVR] = "C4 02 B0 B1 00 02 62 9E";
    m_strCHCmd[CMD_LVR] = "C4 02 BA BB 00 02 B8 BD";
    m_strCHCmd[CMD_HH] = "C4 02 B2 B3 00 02 1A 3E";
    m_strCHCmd[CMD_HL] = "C4 02 B4 B5 00 02 93 DE";
    m_strCHCmd[CMD_LH] = "C4 02 B6 B7 00 02 EB 7E";
    m_strCHCmd[CMD_LL] = "C4 02 B8 B9 00 02 C0 1D";
    m_strCHCmd[CMD_CONST] = "C4 03 BE BF 03 E8 F6 41";
    m_strCHCmd[CMD_RT] = "C4 02 D6 D7 00 02 F5 60";
    m_strCHCmd[CMD_OFFSET] = "C4 03 BC BD 00 00 30 E1";
    m_strCHCmd[CMD_READ] = "C4 02 00 21 00 06 4C B9";
}

/*
 * 5通道：
高量程VR空载：C5 02 C0 C1 00 02 A8 84
低量程VR空载：C5 02 CA CB 00 02 72 A7
高量程高限10K：C5 02 C2 C3 00 02 D0 24
高量程低限50K：C5 02 C4 C5 00 02 59 C4
低量程高限50K：C5 02 C6 C7 00 02 21 64
低量程低限100K： C5 02 C8 C9 00 02 0A 07
温度标定：C5 02 D8 D9 00 02 0F 02
写电极常数：C5 03 CE CF 03 E8 3C 5B
写温度偏差：C5 03 CC CD 00 00 FA FB
通道5读取指令：C5 02 00 2C 00 06 5E 29
*/
void DHexCmd::initCH5StrHex()
{
    m_strCHCmd[CMD_HVR] = "C5 02 C0 C1 00 02 A8 84";
    m_strCHCmd[CMD_LVR] = "C5 02 CA CB 00 02 72 A7";
    m_strCHCmd[CMD_HH] = "C5 02 C2 C3 00 02 D0 24";
    m_strCHCmd[CMD_HL] = "C5 02 C4 C5 00 02 59 C4";
    m_strCHCmd[CMD_LH] = "C5 02 C6 C7 00 02 21 64";
    m_strCHCmd[CMD_LL] = "C5 02 C8 C9 00 02 0A 07";
    m_strCHCmd[CMD_CONST] = "C5 03 CE CF 03 E8 3C 5B";
    m_strCHCmd[CMD_RT] = "C5 02 D8 D9 00 02 0F 02";
    m_strCHCmd[CMD_OFFSET] = "C5 03 CC CD 00 00 FA FB";
    m_strCHCmd[CMD_READ] = "C5 02 00 2C 00 06 5E 29";
}

/*
 * 调试确认：
C0 03 06 00 00 00 88 55     //保存
C0 03 01 00 00 00 FC 54     //调试确认
C0 03 08 00 00 00 60 57     //输出模式切换为ASCII方式
C0 03 07 00 00 00 74 54     //校准参数打印输出
C0 03 09 00 00 00 9C 56     //输出模式切换为MODBUS方式

*/
void DHexCmd::initDebugStrHex()
{
    m_strDebugCmd[CMD_DEBUG] = "C0 03 01 00 00 00 FC 54"; //调试确认
    m_strDebugCmd[CMD_SAVE] = "C0 03 06 00 00 00 88 55";  //保存
    m_strDebugCmd[CMD_SWITCH_ASICC] = "C0 03 08 00 00 00 60 57"; //输出模式切换为ASCII方式
    m_strDebugCmd[CMD_SWITCH_MODBUS] = "C0 03 09 00 00 00 9C 56"; //输出模式切换为MODBUS方式
    m_strDebugCmd[CMD_PRINT] = "C0 03 07 00 00 00 74 54"; //校准参数打印输出
}
