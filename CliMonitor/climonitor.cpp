#include "climonitor.h"
#include "ui_climonitor.h"

CliMonitor::CliMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CliMonitor)
{
    ui->setupUi(this);

    m_tcpCli = new QTcpSocket(this);
    m_tcpCli->abort();                 //取消原有连接
    connect(m_tcpCli, SIGNAL(readyRead()), this, SLOT(readData()));
    //connect(m_tcpCli, SIGNAL(error(QAbstractSocket::SocketError)),
    //        this, SLOT(ReadError(QAbstractSocket::SocketError)));
    connect(m_tcpCli, SIGNAL(disconnected()),this,SLOT(clientDis()));


    m_connectFlag = false;
    m_serverConnect = false;
    m_vecCpuRate.clear();
    m_vecMemUse.clear();
    m_memTotal = 0;

    m_cpuRate = 40;
    m_memUse = 7000;

    m_wholeCpuRate = 20;
    m_wholeMenUse = 30;


    m_str.clear();

    //11个数据
    m_vecCpuRate.push_front(50);
    m_vecCpuRate.push_front(40);
    m_vecCpuRate.push_front(30);
    m_vecCpuRate.push_front(50);
    m_vecCpuRate.push_front(60);
    m_vecCpuRate.push_front(70);
    m_vecCpuRate.push_front(50);
    m_vecCpuRate.push_front(20);
    m_vecCpuRate.push_front(50);
    m_vecCpuRate.push_front(60);
    m_vecCpuRate.push_front(40);


    //11个数据
    m_memTotal = 8000;
//    ui->lb_mem_w->text() = m_memTotal;
//    ui->lb_mem_h->text() = m_memTotal / 2;

    m_vecMemUse.push_front(4000);
    m_vecMemUse.push_front(2000);
    m_vecMemUse.push_front(2500);
    m_vecMemUse.push_front(2700);
    m_vecMemUse.push_front(3600);
    m_vecMemUse.push_front(4500);
    m_vecMemUse.push_front(5000);
    m_vecMemUse.push_front(7000);
    m_vecMemUse.push_front(7100);
    m_vecMemUse.push_front(7500);
    m_vecMemUse.push_front(6000);


    this->resize(1280, 800);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(slotUpdate()));

    m_timer->start(100);
}

CliMonitor::~CliMonitor()
{
    delete m_tcpCli;
    delete m_timer;
    delete ui;
}


void CliMonitor::paintEvent(QPaintEvent *ev)
{
//    QPalette pa;
//    pa.setColor(QPalette::WindowText, Qt::green);
//    ui->label->setPalette(pa);
//    ui->label_2->setPalette(pa);

    QPen pen;
    QPainter p(this);

    pen.setColor(QColor(99, 97, 99));
    p.setBrush(QColor(99, 97, 99));
    p.setPen(pen);
    p.drawRect(0, 0, 1280, 800);


    pen.setColor(Qt::black);
    pen.setWidth(3);
    p.setBrush(QColor(120, 120, 120));
    p.setPen(pen);

    p.setBrush(Qt::white);
    p.drawRect(50, 100, 100, 600);
    p.drawRect(200, 100, 100, 600);

    p.drawRect(380, 100, 400, 400);
    p.drawRect(830, 100, 400, 400);

    p.drawRect(380, 650, 400, 50);
    p.drawRect(830, 650, 400, 50);

    QFont font1("宋体", 19);
    p.setFont(font1);

    if (m_cpuRate < 70)
        p.setBrush(Qt::green);
    else p.setBrush(Qt::red);
    p.drawRect(50, 700-m_cpuRate*6, 100, m_cpuRate*6);
    p.drawText(50, 350, 100, 20, Qt::AlignCenter, QString::number(m_cpuRate) + "%");

    if (m_memUse < 6000)
        p.setBrush(Qt::green);
    else p.setBrush(Qt::red);
    p.drawRect(200, 700-m_memUse/80*6, 100, m_memUse/80*6);
    p.drawText(200, 350, 100, 20, Qt::AlignCenter, QString::number(m_memUse) + "M");



    p.setBrush(Qt::green);
    p.drawRect(380, 650, 400, 50);
    p.setBrush(Qt::red);
    p.drawRect(380, 650, m_wholeCpuRate*4, 50);
    p.drawText(530, 650, 100, 50, Qt::AlignCenter, QString::number(m_wholeCpuRate) + "%");


    p.setBrush(Qt::green);
    p.drawRect(830, 650, 400, 50);
    p.setBrush(Qt::red);
    p.drawRect(830, 650, m_wholeMenUse*4, 50);
    p.drawText(980, 650, 100, 50, Qt::AlignCenter, QString::number(m_wholeMenUse) + "%");


    pen.setColor(Qt::black);
    pen.setWidth(2);
    p.setPen(pen);
    if (m_vecMemUse.size() > 1)
    {
        for (int i = 0; i < m_vecMemUse.size() - 1; i++)
        {
            //p.drawRect(50, 50, 50, 50);

            p.drawLine(QPoint(830+40*i, 100+400-400*m_vecMemUse[i]/m_memTotal),
                       QPoint(830+40*(i+1), 100+400-400*m_vecMemUse[i+1]/m_memTotal));
        }

    }

    pen.setColor(Qt::black);
    pen.setWidth(2);
    p.setPen(pen);
    if (m_vecCpuRate.size() > 1)
    {
        for (int i = 0; i < m_vecCpuRate.size() - 1; i++)
        {
            //p.drawRect(50, 50, 50, 50);

            //p.drawRect();
            p.drawLine(QPoint(380+40*i, 100+400-40*m_vecCpuRate[i]/10),
                       QPoint(380+40*(i+1), 100+400-40*m_vecCpuRate[i+1]/10));
        }

    }

}

void CliMonitor::keyPressEvent(QKeyEvent *ev)
{

}

void CliMonitor::slotUpdate()
{
    //qDebug() << "repaint";
    if (m_connectFlag == true && m_serverConnect == false)
    {
        m_tcpCli->write("M");
        qDebug() << "send M";
    }

    deCode();


    update();
}


void CliMonitor::addToVec(QVector<int> &vec, int value)
{
    if (vec.size() < 11)
    {
        vec.push_front(value);
    }
    else
    {
        for (int i = 10; i > 0; i--)
        {
            vec[i] = vec[i - 1];
        }
        vec[0] = value;
    }
}

bool decode_once = false;
void CliMonitor::deCode()
{
    if (m_serverConnect == true && decode_once == true)
    {
        decode_once = false;
        //"..R..U..T..R..U..T..E..D"
        int temp = 0;
        int temp_value[8] = {0};
        int k = 0;

        for (int i = 0; i < m_str.length() && m_str[i] != '\0'; i++)
        {
            if (m_str[i] >= '0' && m_str[i] <= '9')
            {
                temp = temp * 10 + m_str[i].unicode() - '0';
            }
            else if (m_str[i] == 'R' || m_str[i] == 'U' || m_str[i] == 'T'
                     || m_str[i] == 'E')
            {
                temp_value[k++] = temp;
                temp = 0;
            }
            else if (m_str[i] == 'D')
            {
                temp_value[k++] = temp;
                temp = 0;
                break;
            }
        }
        m_cpuRate = temp_value[0];
        m_memUse = temp_value[1];
        //temp_value[2];

        addToVec(m_vecCpuRate, temp_value[3]);
        addToVec(m_vecMemUse, temp_value[4]);
        //temp_value[5];

        m_wholeCpuRate = temp_value[6];
        m_wholeMenUse = temp_value[7];



//        for (int i = 0; i < 8; i++)
//            qDebug() << temp_value[i];
//        qDebug() << "--------------";


        m_str.clear();
    }
}


void CliMonitor::readData()
{
    m_str.clear();

    QByteArray buffer = m_tcpCli->readAll();
    m_str.prepend(buffer);
    if(m_serverConnect == false && m_str== "M")
    {
        m_serverConnect = true;
        qDebug() << "received M";
    }
    else
    {
        qDebug() << "server data is " << m_str;
        decode_once = true;
    }
}

void CliMonitor::clientDis()
{
    if (m_connectFlag == true)
    {
        m_tcpCli->disconnectFromHost();
        if (m_tcpCli->state() == QAbstractSocket::UnconnectedState \
                || m_tcpCli->waitForDisconnected(1000))  //已断开连接则进入if{}
        {
            m_connectFlag = false;
            m_serverConnect = false;
            qDebug() << "disconnect successful!";
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->lb_state->setText("已断开");
            ui->lb_state->setPalette(pa);

            m_vecCpuRate.clear();
            m_vecMemUse.clear();
        }

    }
}

void CliMonitor::on_pb_connect_clicked()
{
    if (m_connectFlag == false)
    {
        m_tcpCli->abort();
        m_tcpCli->connectToHost(ui->le_IP->text(), ui->le_port->text().toInt());
        if (m_tcpCli->waitForConnected(1000))  // 连接成功则进入if{}
        {
            m_connectFlag = true;
            qDebug() << "connect successful!";
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::green);
            ui->lb_state->setText("已连接");
            ui->lb_state->setPalette(pa);
        }
        else
        {
            qDebug() << "connect failed!";
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->lb_state->setText("已断开");
            ui->lb_state->setPalette(pa);
        }
    }
}

void CliMonitor::on_pb_disconnect_clicked()
{
    if (m_connectFlag == true)
    {
        m_tcpCli->disconnectFromHost();
        if (m_tcpCli->state() == QAbstractSocket::UnconnectedState \
                || m_tcpCli->waitForDisconnected(1000))  //已断开连接则进入if{}
        {
            m_connectFlag = false;
            m_serverConnect = false;
            qDebug() << "disconnect successful!";
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->lb_state->setText("已断开");
            ui->lb_state->setPalette(pa);

            m_vecCpuRate.clear();
            m_vecMemUse.clear();
        }
    }
    //emit m_tcpCli->disconnect();
}

void CliMonitor::on_pb_upDateCliIndex_clicked()
{
    qDebug() << " update ci index";
}
