#ifndef QUERYRUNNER_H
#define QUERYRUNNER_H

#include <QObject>
#include <QSqlQuery>
#include <QVariant>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

// The setup of QSql means we can't pass querys across thread boundarys which sucks
// Couple of data structures for storing the data which we can pass across

typedef QList<QVariant> TomSqlRow ;
typedef QList<TomSqlRow> TomSqlResult ;


class QueryRunner : public QObject
{
    Q_OBJECT
public:
    explicit QueryRunner(QObject *parent = 0);

    QString getLastError() { return m_lastError;};
signals:
    void querySuccess(int count, int tag);
    void queryError(int tag);
    void queryStatus(bool running);
    void statusText(QString text);
    void queryText(QString text);
    void connectOk();
    void minDate(QDateTime date);
    void maxDate(QDateTime date);
public slots:
    void connectToDatabase(QString hostname, QString database, QString user, QString pass);
    void runQuery(const QString &queryString, int tag);
    void openFile(const QString &filename);
    void closeFile();
private:
    QString m_lastError;
    QSqlDatabase m_database;
    QFile *m_file;
    QTextStream m_textStream;
};

#endif // QUERYRUNNER_H
