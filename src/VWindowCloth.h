#ifndef _VWINDOWCLOTH_H
#define _VWINDOWCLOTH_H

#include <QObject>
#include "VWindowMaterials.h"

class VDatabaseCloth;
class QDoubleValidator;

class VWindowCloth : public VWindowMaterials
{
public:
    explicit VWindowCloth(QWidget *parent=nullptr);
    ~VWindowCloth();
public slots:
    virtual void selectMaterial( );
    virtual void saveMaterial( );
private:
    static const QString TITLE;
    virtual VDatabaseInteractor* databaseInteractor();
    VDatabaseCloth* const m_pDatabaseCloth;
    QDoubleValidator * const m_pValidator;
    QDoubleValidator * const m_pPartValidator;
};

#endif // _VWINDOWCLOTH_H
