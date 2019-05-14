/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VWINDOWCLOTH_H
#define _VWINDOWCLOTH_H

#include <QObject>
#include "VWindowMaterials.h"

class VDatabaseCloth;
class QDoubleValidator;
class QLabel;
class QLineEdit;

class VWindowCloth : public VWindowMaterials
{
    Q_OBJECT
public:
    static const QString TITLE;

    static const QString PARAM1_LABEL;
    static const QString PARAM2_LABEL;
    static const QString PARAM3_LABEL;
    static const QString PARAM4_LABEL;

    explicit VWindowCloth(QWidget *parent=nullptr);
    virtual ~VWindowCloth();

protected:
    virtual void selectMaterial( );
    virtual void saveMaterial( );
    virtual void accept();

private:
    virtual VDatabaseInteractor* databaseInteractor();
    bool getInputs(QString &name, float &cavityheight,
                   float &xpermeability, float &ypermeability, float &porosity);

    void setTitles();
    void assignUiPointers();

    VDatabaseCloth* const m_pDatabaseCloth;
    QDoubleValidator * const m_pPartValidator;

    /**
      * Widgets
      */
    struct UiControls
    {
        QLineEdit * cavityHeightEdit;
        QLineEdit * xPermeabilityEdit;
        QLineEdit * yPermeabilityEdit;
        QLineEdit * porosityEdit;

        QLabel * cavityHeightLabel;
        QLabel * xPermeabilityLabel;
        QLabel * yPermeabilityLabel;
        QLabel * porosityLabel;
    };

    UiControls m_ui;

signals:
    void gotMaterial(const QString &name, float cavityheight,
                     float xpermeability,float ypermeability, float porosity);
};

#endif // _VWINDOWCLOTH_H
