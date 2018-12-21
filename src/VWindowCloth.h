#ifndef _VWINDOWCLOTH_H
#define _VWINDOWCLOTH_H

#include <QObject>
#include "VWindowMaterials.h"

class VDatabaseCloth;
class QDoubleValidator;

class VWindowCloth : public VWindowMaterials
{
    Q_OBJECT
public:
    static const QString TITLE;

    explicit VWindowCloth(QWidget *parent=nullptr);
    virtual ~VWindowCloth();

protected:
    virtual void selectMaterial( );
    virtual void saveMaterial( );
    virtual void accept();

private:
    virtual VDatabaseInteractor* databaseInteractor();
    bool getInputs(QString &name, float &cavityheight, float &permeability, float &porosity);

    VDatabaseCloth* const m_pDatabaseCloth;
    QDoubleValidator * const m_pValidator;
    QDoubleValidator * const m_pPartValidator;

signals:
    void gotMaterial(const QString &name, float cavityheight, float permeability, float porosity);
};

#endif // _VWINDOWCLOTH_H
