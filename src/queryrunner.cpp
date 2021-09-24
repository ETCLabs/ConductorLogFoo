#include "queryrunner.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDateTime>

QueryRunner::QueryRunner(QObject *parent) : QObject(parent)
{
}

void QueryRunner::connectToDatabase(QString hostname, QString database, QString user, QString pass)
{
    m_database = QSqlDatabase::addDatabase("QPSQL");
    m_database.setHostName(hostname);
    m_database.setDatabaseName(database);
    m_database.setUserName(user);
    m_database.setPassword(pass);

    emit statusText(tr("Opening Database"));
    emit queryStatus(true);
    bool ok = m_database.open();

    if(!ok)
    {
        m_lastError = m_database.lastError().text();
        emit queryError(0);

        emit statusText(tr("Error Opening Database - %1").arg(m_lastError));
        return;
    }
    else
    {
        m_lastError = "";
        emit connectOk();
        emit statusText(tr("Opened Database OK"));
    }

    emit queryStatus(false);

    // Find the minimum and maximum dates in the DB

    QSqlQuery m_query = QSqlQuery(m_database);

    QDateTime minDateTime, maxDateTime;
    QString queryString = "SELECT MIN(msg_rcv_time),MAX(msg_rcv_time) FROM msg_extended";

    emit queryText(queryString);

    ok = m_query.prepare(queryString);
    if(!ok)
    {
        m_lastError  = m_query.lastError().text();
        emit queryError(0);
        emit statusText(tr("Query %1 Failed").arg(queryString));
        return;
    }

    m_query.exec();
    while(m_query.next())
    {
        minDateTime = m_query.record().value(0).toDateTime();
        maxDateTime = m_query.record().value(1).toDateTime();
        emit minDate(minDateTime);
        emit maxDate(maxDateTime);
    }

}

void QueryRunner::runQuery(const QString &queryString, int tag)
{
    QSqlQuery m_query = QSqlQuery(m_database);

    emit queryText(queryString);
    QString shortQuery = queryString.left(10);
    if(queryString.length()>10)
        shortQuery += QString("...");

    emit statusText(tr("Running Query %1").arg(shortQuery));
    bool ok = m_query.prepare(queryString);
    if(!ok)
    {
        m_lastError  = m_query.lastError().text();
        emit queryError(tag);
        emit statusText(tr("Query %1 Failed").arg(shortQuery));
        return;
    }

    emit queryStatus(true);
    m_query.exec();

    if(m_query.lastError().type()!=QSqlError::NoError)
    {
        m_lastError  = m_query.lastError().text();
        emit statusText(tr("Query %1 Failed").arg(shortQuery));
        emit queryError(tag);
    }
    else
    {
        m_lastError = "";
        int resultCount = 0;

        while(m_query.next())
        {
            resultCount++;
            QString value;
            int colCount = m_query.record().count();

            for(int i=0; i<colCount; i++){
                value = m_query.value(i).toString();
                if(i==5)
                    m_textStream << "\"";
                m_textStream << value;
                if(i==5)
                    m_textStream << "\"";
                m_textStream << ",";
            }
            m_textStream << "\r\n";
        }

        emit querySuccess(resultCount, tag);
        emit statusText(tr("Query %1 Succeeded").arg(shortQuery));
    }
    emit queryStatus(false);
}

void QueryRunner::openFile(const QString &filename)
{
    m_file = new QFile(filename);
    bool ok = m_file->open(QIODevice::WriteOnly);
    if(!ok)
    {
        delete m_file;
        m_file = 0;
        return;
    }

    m_textStream.setDevice(m_file);

    //header
    m_textStream << "Receive Time,Hostname,Facility,Priority,Program,Message Text\r\n";
}

void QueryRunner::closeFile()
{
    m_textStream.flush();
    m_textStream.setDevice(0);
    m_file->close();
    m_file = 0;
}
