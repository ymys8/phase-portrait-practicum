#pragma once

#include "qcustomplot.h"
#include <QMainWindow>
#include "qcustomplot.h"

/// Абстракстный класс для всех виджетов с заданием
class TaskWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// Конструктор
    explicit TaskWindow(QWidget *parent = nullptr) : QMainWindow(parent, Qt::Window)
    {
        setAttribute(Qt::WA_DeleteOnClose);
    }
    /// Деструктор
    virtual ~TaskWindow() = default;

protected:
    /// Инициализация параметров пользовательского интерфейса
    virtual void initUI() = 0;

    /// Перерисовать график
    void redrawPlot(QCustomPlot *plot)
    {
        plot->rescaleAxes();
        plot->replot();
    }

    /// Настроить оси графика с центром в (0,0)
    static void setupZeroCenteredAxes(QCustomPlot* plot, double margin = 1.1)
    {
        if (!plot) return;

        double maxX = 0.0, maxY = 0.0;
        bool hasData = false;

        // Обрабатываем оQCPGraph
        for (int i = 0; i < plot->graphCount(); ++i) {
            auto data = plot->graph(i)->data();
            if (data && !data->isEmpty()) {
                hasData = true;
                for (auto it = data->begin(); it != data->end(); ++it) {
                    maxX = qMax(maxX, qAbs(it->key));
                    maxY = qMax(maxY, qAbs(it->value));
                }
            }
        }

        // Обрабатываем QCPCurve
        for (int i = 0; i < plot->plottableCount(); ++i) {
            if (auto* curve = qobject_cast<QCPCurve*>(plot->plottable(i))) {
                auto data = curve->data();
                if (data && !data->isEmpty()) {
                    hasData = true;
                    for (auto it = data->begin(); it != data->end(); ++it) {
                        maxX = qMax(maxX, qAbs(it->key));
                        maxY = qMax(maxY, qAbs(it->value));
                    }
                }
            }
        }

        if (!hasData) {
            plot->xAxis->setRange(-1, 1);
            plot->yAxis->setRange(-1, 1);
            return;
        }

        maxX = qMax(maxX * margin, 0.1);
        maxY = qMax(maxY * margin, 0.1);

        plot->xAxis->setRange(-maxX, maxX);
        plot->yAxis->setRange(-maxY, maxY);
    }
};
