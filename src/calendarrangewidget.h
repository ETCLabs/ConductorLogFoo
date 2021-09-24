#ifndef CALENDARRANGEWIDGET_H
#define CALENDARRANGEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QCalendarWidget>

class CalendarRangeWidget : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit CalendarRangeWidget(QWidget *parent = 0);
    ~CalendarRangeWidget();
public slots:
    void setRangeStart(QDate date);
    void setRangeEnd(QDate date);
    void setTextForDate(QString text, QDate date);
protected:
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
private:
    QDate m_startRange;
    QDate m_endRange;
    QDate m_startSelection;
    QDate m_endSelection;
    QHash<QDate, QString> m_data;
};

#endif // CALENDARRANGEWIDGET_H
