/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VIMAGETEXTWRITERS_H
#define _VIMAGETEXTWRITERS_H

#include <memory>

class QPixmap;
class QString;
class QFont;
class QPoint;

class VSimulationFacade;

class VAbstractImageTextWriter
{
public:
    VAbstractImageTextWriter() = default;
    virtual void writeText(QPixmap &pixmap) const;
protected:
    static const QString FONT_NAME;
    static const int FONT_SIZE;
    static const int LINE_OFFSET;
    static const int STRING_LENGTH;

    virtual QString createString() const;
    virtual QFont getFont() const;
    virtual int getLineOffset() const;
    virtual int getLineStringLength() const;
    virtual QPoint getTextPosition(const QPixmap &pixmap) const;
};

class VSimInfoImageTextWriter : public VAbstractImageTextWriter
{
public:
    VSimInfoImageTextWriter(const std::shared_ptr<const VSimulationFacade> p_facade);
protected:
    virtual QString createString() const override;
private:
    static const QString INFO_STRING;
    static const int PRECISION;

    std::shared_ptr<const VSimulationFacade> m_pFacade;
};

#endif // _VIMAGETEXTWRITERS_H
