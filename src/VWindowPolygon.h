/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWPOLYGON_H
#define _VWINDOWPOLYGON_H

#include <QMainWindow>

#include "sim/layer_builders/VLayerManualBuilder.h"

namespace Ui {
class VWindowPolygon;
}

class VWindowPolygon : public QMainWindow
{
    Q_OBJECT

public:
    VWindowPolygon(QWidget *parent = nullptr);
    virtual ~VWindowPolygon();

private:
    void reset();
    void reject();
    void accept();

    VPolygon m_polygon;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

protected:
    Ui::VWindowPolygon *ui;

    virtual void closeEvent(QCloseEvent *) override;

signals:
    void polygonAvailable(const VPolygon &polygon);
    void creationCanceled();
    void windowClosed();
};

#endif // _VWINDOWPOLYGON_H
