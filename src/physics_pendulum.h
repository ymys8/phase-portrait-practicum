#pragma once

#include "taskwindow.h"

namespace Ui {
    class PhysicsPendulumWindow;
}

class QCustomPlot;
class QCPCurve;

namespace pp
{

struct Parameters
{
    double omega0;
    double delta;
    double A;
    double rho;
    double t;
    double phi0;
    double psi0;
};

class PhysicsPendulumWindow : public TaskWindow
{
    Q_OBJECT

public:
    explicit PhysicsPendulumWindow(QWidget *parent = nullptr);
    ~PhysicsPendulumWindow();

private slots:
    void redrawPlots();

private:
    void initUI() override;

    void initEvolutionXPlot();
    void initEvolutionYPlot();
    void initEvolutionXYPlot();
    void initPhasePortretPlot();

    Parameters getParametrsFromUI() const;

    void redrawPlot(QCustomPlot *plot);

    Ui::PhysicsPendulumWindow *ui;
    QCPCurve *phasePortraitCurve;
};

}
