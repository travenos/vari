/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include "VImageTextWriters.h"

#include <QPixmap>
#include <QPainter>

#include "sim/VSimulationFacade.h"

/**
 * VAbstractImageTextWriter implementation
 */

const QString VAbstractImageTextWriter::FONT_NAME{QStringLiteral("Times New Roman")};
const int VAbstractImageTextWriter::FONT_SIZE{16};
const int VAbstractImageTextWriter::LINE_OFFSET{18};
const int VAbstractImageTextWriter::STRING_LENGTH{250};
const int VAbstractImageTextWriter::UP_OFFSET{36};
const int VAbstractImageTextWriter::LEFT_OFFSET{10};

void VAbstractImageTextWriter::writeText(QPixmap &pixmap) const
{
    QPainter painter( &pixmap );
    const QFont font{getFont()};
    const int lineOffset{getLineOffset()};
    const int stringLength{getLineStringLength()};
    painter.setFont(font);
    const QStringList infoSplitted{createString().split('\n')};
    QPoint position{getTextPosition(pixmap)};
    foreach(const QString &infoLine, infoSplitted)
    {
        QPoint horisontalPos(position);
        const QStringList lineSplitted{infoLine.split('\t')};
        foreach (const QString &infoString, lineSplitted)
        {
            painter.drawText( horisontalPos , infoString );
            horisontalPos.setX(horisontalPos.x() + stringLength);
        }
        position.setY(position.y() + font.pointSize() + lineOffset);
    }
}

QString VAbstractImageTextWriter::createString() const
{
    return QStringLiteral("");
}

QFont VAbstractImageTextWriter::getFont() const
{
    QFont font(FONT_NAME);
    font.setPixelSize(FONT_SIZE);
    return font;
}

int VAbstractImageTextWriter::getLineOffset() const
{
    return LINE_OFFSET;
}

int VAbstractImageTextWriter::getLineStringLength() const
{
    return STRING_LENGTH;
}

QPoint VAbstractImageTextWriter::getTextPosition(const QPixmap &pixmap) const
{
    return QPoint(LEFT_OFFSET, pixmap.height() - UP_OFFSET);
}

/**
 * VSimInfoImageTextWriter implementation
 */


const QString VSimInfoImageTextWriter::INFO_STRING{"Время моделирования (с): %1\t"
                                                   "Время процесса (с): %2\t"
                                                   "Готовность эксперимента (%): %3\n"
                                                   "Фактор реального времни: %4\t"
                                                   "Номер итерации: %5\t"
                                                   "Среднее давление (Па): %6"};

const int VSimInfoImageTextWriter::PRECISION{6};

VSimInfoImageTextWriter::VSimInfoImageTextWriter(const std::shared_ptr<const VSimulationFacade>
                                                 p_facade):
    m_pFacade{p_facade}
{
}

QString VSimInfoImageTextWriter::createString() const
{
    VSimulationInfo inf{m_pFacade->getSimulationInfo()};

    return INFO_STRING.arg(QString::number(inf.realTime, 'g', PRECISION),
                          QString::number(inf.simTime, 'g', PRECISION),
                          QString::number(inf.filledPercent, 'g', PRECISION),
                          QString::number(inf.realtimeFactor, 'g', PRECISION),
                          QString::number(inf.iteration),
                          QString::number(inf.averagePressure, 'g', PRECISION)
                          );
}
