/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWRESIN_H
#define _VWINDOWRESIN_H

#include <QObject>
#include "VWindowMaterials.h"

class VDatabaseResin;
class QLineEdit;
class QLabel;

class VWindowResin : public VWindowMaterials
{
    Q_OBJECT
public:
    static const QString TITLE;

    static const QString PARAM1_LABEL;
    static const QString PARAM2_LABEL;
    static const QString PARAM3_LABEL;
    static const QString PARAM4_LABEL;

    explicit VWindowResin(QWidget *parent=nullptr);
    virtual ~VWindowResin();

protected:
    virtual void selectMaterial( );
    virtual void saveMaterial( );
    virtual void accept();

private:
    virtual VDatabaseInteractor* databaseInteractor();
    bool getInputs(QString &name, float &viscosity, float &viscTempcoef, float &lifetime, float &lifetimeTempcoef);

    void setTitles();
    void assignUiPointers();

    VDatabaseResin* const m_pDatabaseResin;

    /**
      * Widgets
      */
    struct UiControls
    {
        QLineEdit * viscosityEdit;
        QLineEdit * viscTempcoefEdit;
        QLineEdit * lifetimeEdit;
        QLineEdit * lifetimeTempcoefEdit;

        QLabel * viscosityLabel;
        QLabel * viscTempcoefLabel;
        QLabel * lifetimeLabel;
        QLabel * lifetimeTempcoefLabel;
    };

    UiControls m_ui;

signals:
    void gotMaterial(const QString &name, float viscosity, float viscTempcoef, float lifetime, float lifetimeTempcoef);
};

#endif // _VWINDOWRESIN_H
