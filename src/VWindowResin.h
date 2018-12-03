#ifndef _VWINDOWRESIN_H
#define _VWINDOWRESIN_H

#include <QObject>
#include "VWindowMaterials.h"

class VDatabaseResin;
class QDoubleValidator;

class VWindowResin : public VWindowMaterials
{
public:
    explicit VWindowResin(QWidget *parent=nullptr);
    ~VWindowResin();
public slots:
    virtual void selectMaterial( );
    virtual void saveMaterial( );
private:
    static const QString TITLE;
    virtual VDatabaseInteractor* databaseInteractor();
    VDatabaseResin* const m_pDatabaseResin;
    QDoubleValidator * const m_pValidator;
};

#endif // _VWINDOWRESIN_H
