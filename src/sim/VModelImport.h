/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifndef _VMODELIMPORT_H
#define _VMODELIMPORT_H

#include "VModelImportExport.h"
#include "VLayersProcessor.h"

class QXmlStreamReader;

class VModelImport : public VModelImportExport
{
public:
    VModelImport();
    const VSimulationInfo& getInfo() const;
    const VSimulationParametres::ptr& getSimulationParametres() const;
    const VLayersProcessor::ptr& getLayersProcessor() const;
    bool getPaused() const;
    void loadFromFile(const QString &filename);

protected:
    VSimulationInfo m_info;
    VSimulationParametres::ptr m_pParam;
    VLayersProcessor::ptr m_pLayersProcessor;
    bool m_paused;

private:
    void loadInfo(QXmlStreamReader& xmlReader);
    void loadParametres(QXmlStreamReader &xmlReader);
    void loadPaused(QXmlStreamReader& xmlReader);
    void loadLayers(QXmlStreamReader &xmlReader);
    void loadConnections(QXmlStreamReader& xmlReader);

    VResin loadResin(QXmlStreamReader& xmlReader) const;
};

#endif // _VMODELIMPORT_H
