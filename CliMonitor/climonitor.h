#ifndef CLIMONITOR_H
#define CLIMONITOR_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QVector>
#include <QQueue>

#include <QTcpSocket>
#include <QHostAddress>


namespace Ui {
class CliMonitor;
}

class CliMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit CliMonitor(QWidget *parent = 0);
    ~CliMonitor();

protected:
    void paintEvent(QPaintEvent *ev);
    void keyPressEvent(QKeyEvent *ev);

private slots:
    void slotUpdate();
    void readData();
    void clientDis();
    void addToVec(QVector<int> &vec, int value);
    void deCode();

    void on_pb_connect_clicked();

    void on_pb_disconnect_clicked();

    void on_pb_upDateCliIndex_clicked();

private:
    Ui::CliMonitor *ui;

    QTimer *m_timer;
    QTcpSocket *m_tcpCli;
    bool m_connectFlag;
    bool m_serverConnect;

    QVector<int> m_vecCpuRate;
    QVector<int> m_vecMemUse;
    int m_memTotal;

    int m_cpuRate;
    int m_memUse;

    int m_wholeCpuRate;
    int m_wholeMenUse;

    QString m_str;
};

#endif // CLIMONITOR_H
