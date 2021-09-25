#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "queryrunner.h"
#include "version.h"
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_queryThread = new QThread(this);
    m_runner = new QueryRunner();
    m_runner->moveToThread(m_queryThread);
    m_queryThread->start();

    connect(m_runner, SIGNAL(statusText(QString)), ui->statusBar, SLOT(showMessage(QString)));
    connect(m_runner, SIGNAL(connectOk()), this, SLOT(connectedOk()));
    connect(m_runner, SIGNAL(querySuccess(int,int)), this, SLOT(onQuerySuccess(int,int)));
    connect(m_runner, SIGNAL(maxDate(QDateTime)), this, SLOT(maxDate(QDateTime)));
    connect(m_runner, SIGNAL(minDate(QDateTime)), this, SLOT(minDate(QDateTime)));

    ui->progressBar->setVisible(false);
    ui->lblResultInfo->setVisible(false);

    ui->progressBar->setMinimum(0);
    ui->stackedWidget->setCurrentIndex(0);

    setWindowTitle(QString("ConductorLogFoo - %1")
    .arg(PROJECT_VERSION));

    // Load saved settings for User, Password and IP
    QSettings settings;
    ui->leIp->setText(settings.value("conductorIp", "10.101.50.60").toString());
    ui->leUser->setText(settings.value("conductorUser", "etcuser").toString());
    ui->lePass->setText(settings.value("conductorPass", "lights").toString());
}

MainWindow::~MainWindow()
{
    delete ui;
    QMetaObject::invokeMethod(m_queryThread, "quit");
    m_queryThread->wait();
    delete m_queryThread;
}

void MainWindow::on_btnConnect_pressed()
{
    QSettings settings;
    settings.setValue("conductorIp", ui->leIp->text());
    settings.setValue("conductorUser", ui->leUser->text());
    settings.setValue("conductorPass", ui->lePass->text());

    QMetaObject::invokeMethod(m_runner,
        "connectToDatabase",
        Q_ARG(QString, ui->leIp->text()),
        Q_ARG(QString, "etcsyslog"),
        Q_ARG(QString, ui->leUser->text()),
        Q_ARG(QString, ui->lePass->text())
        );
}

void MainWindow::connectedOk()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->cwFrom->setSelectedDate(QDate::currentDate().addDays(-7));
    ui->teFrom->setTime(QTime(8,00));
    ui->cwTo->setSelectedDate(QDate::currentDate());
    ui->teTo->setTime(QTime(8,00));
    ui->teDuration->setTime(QTime(8,00));
    updateFileName();
}

void MainWindow::on_btnGetLogs_pressed()
{
    ui->progressBar->setVisible(true);
    ui->btnGetLogs->setEnabled(false);
    ui->leSavePath->setEnabled(false);
    ui->btnSetPath->setEnabled(false);

    updateTimes();

    QDateTime chunkStart = m_start;
    QDateTime chunkEnd = m_start.addSecs( 10 * 60); // 10min chunks

    m_numChunks = m_start.msecsTo(m_end) / (1000 * 60 * 10) ;
    ui->progressBar->setMaximum(m_numChunks);

    QMetaObject::invokeMethod(m_runner, "openFile", Q_ARG(QString,ui->leSavePath->text()));

    int chunk = 0;

    while(chunkStart < m_end)
    {

        if(chunkEnd > m_end)
            chunkEnd = m_end;

        QString rcvOperator(">");

        if(ui->timeSpecTab->currentIndex()==1)
            rcvOperator = QString(">=");


        QString sql = QString(
            "SELECT msg_rcv_time, hostname, msg_facility, msg_priority, msg_program, msg_text FROM msg_extended WHERE NOT ("
                    "hostname LIKE 'conductor'"
                    "AND"
                    "("
                    "msg_priority LIKE 'info'"
                    " OR msg_priority LIKE 'notice'"
                    " OR msg_program LIKE 'monit' "
                    ")"
                    ")"
                    " AND msg_rcv_time%1'%2' AND msg_rcv_time<='%3'"
                    " ORDER BY msg_id ASC;")
                .arg(rcvOperator)
               .arg(chunkStart.toString("yyyy-MM-dd hh:mm:ss"))
               .arg(chunkEnd.toString("yyyy-MM-dd hh:mm:ss"));

        qDebug() << sql;
        QMetaObject::invokeMethod(m_runner, "runQuery", Q_ARG(QString,sql), Q_ARG(int, chunk));

        chunk++;
        chunkStart = chunkEnd;
        chunkEnd = chunkEnd.addSecs( 10 * 60);
    }


    QMetaObject::invokeMethod(m_runner, "closeFile");
}


void MainWindow::onQuerySuccess(int resultCount, int tag)
{
    ui->progressBar->setValue(tag);

    ui->lblResultInfo->setText(tr("%1 results found").arg(resultCount));

    if(tag >= m_numChunks-1)
    {
        ui->btnGetLogs->setEnabled(true);
        ui->progressBar->setVisible(false);

        ui->leSavePath->setEnabled(true);
        ui->btnSetPath->setEnabled(true);
    }
}

void MainWindow::on_cwFrom_clicked(const QDate &date)
{
    ui->cwTo->setMinimumDate(date);
    QDate toDate = ui->cwTo->selectedDate() ;
    if(toDate < date)
    {
        ui->cwTo->setSelectedDate(date);
    }
    updateFileName();
}

void MainWindow::on_cwTo_clicked(const QDate &date)
{
    Q_UNUSED(date)
    updateFileName();
}


void MainWindow::on_teTo_timeChanged(const QTime &time)
{
    Q_UNUSED(time)
    updateFileName();
}

void MainWindow::on_timeSpecTab_currentChanged ( int index )
{
    Q_UNUSED(index)
    updateFileName();
}


void MainWindow::on_teDuration_timeChanged(const QTime &time)
{
    Q_UNUSED(time)
    updateFileName();
}

void MainWindow::on_cwDuration_clicked(const QDate &date)
{
    Q_UNUSED(date)
    updateFileName();
}

void MainWindow::on_teFrom_timeChanged(const QTime &time)
{
    Q_UNUSED(time)
    updateFileName();
}


void MainWindow::on_rbBefore_clicked()
{

    updateFileName();
}

void MainWindow::on_rbAfter_clicked()
{
    updateFileName();
}

void MainWindow::on_rbAround_clicked()
{

    updateFileName();
}


void MainWindow::on_sbDurationHours_valueChanged()
{
    updateFileName();
}


void MainWindow::on_sbDurationMins_valueChanged()
{
    updateFileName();
}

void MainWindow::on_btnJumpNext_pressed()
{

    QDateTime durationSt(ui->cwDuration->selectedDate());
    durationSt.setTime(ui->teDuration->time());

    durationSt = durationSt.addSecs(ui->sbDurationHours->value() * 60 * 60 + ui->sbDurationMins->value() * 60);

    ui->cwDuration->setSelectedDate(durationSt.date());
    ui->teDuration->setTime(durationSt.time());


}

void MainWindow::updateFileName()
{

    updateTimes();

    QString buttonText = tr("From %1 to %2")
            .arg(m_start.toString("MMMM dd,yyyy hh:mm:ss"))
            .arg(m_end.toString("MMMM dd,yyyy hh:mm:ss"));
    ui->btnGetLogs->setDescription(buttonText);

    QString defaultFileName =  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    defaultFileName.append( tr("/system_log_%1_%2.csv")
            .arg(m_start.toString("yyyyMMddhhmmss"))
            .arg(m_end.toString("yyyyMMddhhmmss"))
                            );
    ui->leSavePath->setText(defaultFileName);

}

void MainWindow::on_btnSetPath_pressed()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save CSV File"), ui->leSavePath->text(), tr("CSV Files (*.csv)"));

    if(path.isEmpty()) return;

    ui->leSavePath->setText(path);
}


void MainWindow::updateTimes()
{
    if(ui->timeSpecTab->currentIndex()==0)
    {
        // Time range
        m_start = QDateTime(ui->cwFrom->selectedDate());
        m_end = QDateTime(ui->cwTo->selectedDate());
        m_start.setTime(ui->teFrom->time());
        m_end.setTime(ui->teTo->time());
    }
    else
    {
        // Duration
        QDateTime durationSt(ui->cwDuration->selectedDate());
        durationSt.setTime(ui->teDuration->time());

        int durationSecs = ui->sbDurationHours->value() * 60 * 60 + ui->sbDurationMins->value() * 60;

        if(ui->rbBefore->isChecked())
        {
            m_end = durationSt;
            m_start = durationSt.addSecs(-1 * durationSecs);
        }
        if(ui->rbAfter->isChecked())
        {
            m_start = durationSt;
            m_end = durationSt.addSecs(durationSecs);
        }
        if(ui->rbAround->isChecked())
        {
            m_start = durationSt.addSecs(-1 * durationSecs / 2);
            m_end = durationSt.addSecs(durationSecs / 2);
        }
    }
}


void MainWindow::minDate(const QDateTime &date)
{
    ui->cwDuration->setRangeStart(date.date());
    ui->cwFrom->setRangeStart(date.date());
    ui->cwTo->setRangeStart(date.date());


    ui->cwFrom->setSelectedDate(date.date());
    ui->teFrom->setTime(date.time());
    updateFileName();
}

void MainWindow::maxDate(const QDateTime &date)
{
    ui->cwDuration->setRangeEnd(date.date());
    ui->cwFrom->setRangeEnd(date.date());
    ui->cwTo->setRangeEnd(date.date());

    ui->cwTo->setSelectedDate(date.date());
    ui->teTo->setTime(date.time());
    ui->cwDuration->setSelectedDate(date.date());
    ui->teDuration->setTime(date.time());
}
