#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //默认选择COM3
    ui->combox->setCurrentIndex(3);
    //按钮响应
    PushButtonConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::PushButtonConnect()
{
    connect(ui->pb_start,&QPushButton::clicked,this,[this](){
        ui->pb_start->text()=="开始连接"?pb_start() : pb_stop();
        ui->pb_start->text()=="开始连接"?ui->pb_start->setText("断开连接") : ui->pb_start->setText("开始连接");
        });

    connect(ui->pb_send,&QPushButton::clicked,this,&MainWindow::pb_send);
    connect(ui->pb_clear,&QPushButton::clicked,this,&MainWindow::pb_clear);
    connect(ui->theme,&QPushButton::clicked,this,&MainWindow::theme);
    connect(ui->hexsend,&QPushButton::clicked,this,&MainWindow::hexsend);

}
void MainWindow::pb_start()
{
    //获取可用串口
    QList<QSerialPortInfo> port = qspi->availablePorts();
    //选择连接串口
    for (int i=0;i<port.length();i++) {
        if(port.at(i).portName()==ui->combox->currentText())
        {
            //串口号设置
            qsp->setPort(port.at(i));
            //串口波特率设置
            qsp->setBaudRate(ui->baudbox->currentText().toInt());
            //串口数据位设置
            qsp->setDataBits(QSerialPort::DataBits(ui->databox->currentText().toInt()));
            //串口停止位设置
            qsp->setStopBits(QSerialPort::StopBits(ui->stopbox->currentIndex()+1));
            //串口校验位设置
            qsp->setParity( (QSerialPort::Parity)(ui->checkbox->currentIndex()<1 ? \
                                                      ui->checkbox->currentIndex() : \
                                                      ui->checkbox->currentIndex()+1) );
            //开启串口
            qsp->open(QIODevice::ReadWrite);
            //修改按钮颜色
            ui->pb_start->setStyleSheet("background-color: #607D8B; color: white;");
            //监听串口 读取数据
            connect(qsp,&QSerialPort::readyRead,this,&MainWindow::read_port);
        }
    }
}
void MainWindow::pb_stop()
{
    //串口关闭
    qsp->close();
    //修改按钮颜色
    ui->pb_start->setStyleSheet("background-color: #E1E1E1; color: black;");
}
void MainWindow::pb_send()
{
    //判断发送类型
    if(ui->hexsend->isChecked()){
        //16进制转文本发送
        QString msg = ui->textEdit->toPlainText();
        QByteArray arr;
        bool ok = HexToString(msg,arr);
        if(ok){
            qsp->write(arr);
        }
    }else{
        //直接发送文本
        QString msg = ui->textEdit->toPlainText();
        qsp->write(msg.toUtf8());
    }
}
void MainWindow::read_port()
{
    //读取收到的串口数据
    QByteArray msg = qsp->readAll();

    if(msg.length() != 0)
    {
        //实现自动显示到最新内容
        ui->textBrowser->moveCursor(QTextCursor::End);
        //判断输出类型（16进制 or 文本）
        if(ui->hexdisplay->isChecked()){
            ui->textBrowser->append(msg.toHex(' '));
        }else{
            ui->textBrowser->insertPlainText(QString::fromLocal8Bit(msg));
        }
    }
}

void MainWindow::pb_clear()
{
    //清屏保留样式表
    ui->textBrowser->clear();
    theme();
}
void MainWindow::theme()
{
    //修改样式表实现不同显示效果
    if(ui->theme->isChecked())
    {
        ui->textBrowser->setStyleSheet("background-color: black; color: white;");
    }else{
        ui->textBrowser->setStyleSheet("background-color: white; color: black;");
    }
}
void MainWindow::hexsend()
{
    //修改内容实现不同显示
    if(ui->hexsend->isChecked())
    {
        //16进制显示
        QString msg = ui->textEdit->toPlainText();
        ui->textEdit->setText(msg.toUtf8().toHex(' '));
    }else{
        //文本显示
        QString msg = ui->textEdit->toPlainText();
        QByteArray arr;
        bool ok = HexToString(msg,arr);
        if(ok){
          ui->textEdit->setText(arr);
        }
    }
}
//实现16进制转为字符串//
//返回值 bool类型 ：true转换成功  false转换失败并且弹出提示框
//形参 QString,QByteArray 类型：
//                  msg 需要转换的16进制字符串，如 "33 34 35"
//                  ret 返回转换结果 ,  如 "345"
bool MainWindow::HexToString(QString &msg,QByteArray &ret)
{
    QByteArray arr;
    bool ok;
    QStringList sl = msg.split(' ');
    foreach (QString s,sl){
        if(!s.isEmpty()){
            char c = s.toInt(&ok,16)&0xFF;
            if(ok){
                arr.append(c);
            }else{
                QMessageBox::warning(this,"错误",QString("无效的16进制\'%1\'").arg(s));
                ok = false;
                break;
            }
            ok = true;
        }
    }
    if(ok){
        ret = arr;
    }
    return ok;
}
