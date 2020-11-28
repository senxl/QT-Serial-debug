#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QToolTip>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSerialPort *qsp = new QSerialPort();
    QSerialPortInfo *qspi = new QSerialPortInfo();

    void PushButtonConnect();
public slots:
    bool pb_start();
    void pb_stop();
    void pb_send();
    void read_port();
    void pb_clear();
    void theme();
    void hexsend();
    bool HexToString(QString &msg,QByteArray &arr);

};
#endif // MAINWINDOW_H
