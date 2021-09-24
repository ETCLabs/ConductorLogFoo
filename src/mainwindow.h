#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "queryrunner.h"

namespace Ui {
class MainWindow;
}

class QueryRunner;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_btnConnect_pressed();
    void connectedOk();
    void on_btnSetPath_pressed();
    void onQuerySuccess(int resultCount, int tag);
    void on_btnGetLogs_pressed();
    void on_cwTo_clicked(const QDate &date);
    void on_cwFrom_clicked(const QDate &date);
    void on_teTo_timeChanged(const QTime &time);
    void on_teFrom_timeChanged(const QTime &time);
    void on_teDuration_timeChanged(const QTime &time);
    void on_cwDuration_clicked(const QDate &date);
    void on_sbDurationHours_valueChanged();
    void on_sbDurationMins_valueChanged();
    void on_rbBefore_clicked();
    void on_rbAfter_clicked();
    void on_rbAround_clicked();
    void on_btnJumpNext_pressed();
    void on_timeSpecTab_currentChanged ( int index );
    void minDate(const QDateTime &date);
    void maxDate(const QDateTime &date);
private:
    void updateFileName();
    void updateTimes();
    Ui::MainWindow *ui;
    QueryRunner *m_runner;
    QThread *m_queryThread;
    int m_numChunks;
    QDateTime m_start;
    QDateTime m_end;
};

#endif // MAINWINDOW_H
