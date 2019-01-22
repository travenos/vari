/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWRESIN_H
#define _VWINDOWRESIN_H

#include <QObject>
#include "VWindowMaterials.h"

class VDatabaseResin;
class QDoubleValidator;

class VWindowResin : public VWindowMaterials
{
    Q_OBJECT
public:
    static const QString TITLE;

    explicit VWindowResin(QWidget *parent=nullptr);
    virtual ~VWindowResin();

protected:
    virtual void selectMaterial( );
    virtual void saveMaterial( );
    virtual void accept();

private:
    virtual VDatabaseInteractor* databaseInteractor();
    bool getInputs(QString &name, float &viscosity, float &tempcoef);

    VDatabaseResin* const m_pDatabaseResin;
    QDoubleValidator * const m_pValidator;

signals:
    void gotMaterial(const QString &name, float viscosity, float tempcoef);
};

#endif // _VWINDOWRESIN_H
