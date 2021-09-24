#include "calendarrangewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTableView>
#include <QDebug>
#include <QApplication>

CalendarRangeWidget::CalendarRangeWidget(QWidget *parent) : QCalendarWidget(parent)
{

}

CalendarRangeWidget::~CalendarRangeWidget()
{

}

void CalendarRangeWidget::setTextForDate(QString text, QDate date)
{
    m_data[date] = text;
    update();
}

void CalendarRangeWidget::setRangeStart(QDate date)
{
    m_startRange = date;
    update();
}

void CalendarRangeWidget::setRangeEnd(QDate date)
{
    m_endRange = date;
    update();
}

void CalendarRangeWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    if(date >= m_startRange && date <= m_endRange)
    {
        painter->fillRect(rect, QColor(Qt::green));
    }

    if(date == selectedDate())
    {
        QPen pen(Qt::DashDotDotLine);
        pen.setWidth(1);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(pen);
        painter->drawRect(rect.adjusted(1,1,-1,-1));
    }

    painter->drawText(rect, QString::number(date.day()), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

    if(m_data.contains(date))
    {
        QString text = m_data[date];
        painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
    }
}

