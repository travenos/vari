/**
 * Project VARI
 * @author Alexey Barashkov
 */


#include "VLayerFromGmeshBuilder.h"

/**
 * VLayerFromGmeshBuilder implementation
 */


/**
 * @param filename
 * @param param VSimParam::const_ptr
 */
VLayerFromGmeshBuilder::VLayerFromGmeshBuilder(const QString &filename,
                                               const VCloth &material,
                                               const VSimulationParametres::const_ptr &p_simParam):
VLayerFromFileBuilder(filename, material, p_simParam)
{

}

bool VLayerFromGmeshBuilder::importNodes() 
{
    QString line;
    while (!m_file.atEnd())
    {
        line = m_file.readLine();
        if(line.startsWith("$Nodes",Qt::CaseInsensitive))
        {
            line = m_file.readLine();
            line = m_file.readLine();
            while(!line.contains("$EndNodes",Qt::CaseInsensitive))
            {
                int nodeId = 0;
                QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

                std::vector<double> coords;
                int pos = 0;
                for(QStringList::iterator it = tokens.begin(); it != tokens.end(); it++)
                {
                    bool ok;
                    QString token = (*it);

                    switch(pos)
                    {
                    case 0:
                        nodeId = token.toInt(&ok);
                        if(!ok)
                            return false;
                        break;
                    case 1:
                    case 2:
                    case 3:
                        double coord = token.toDouble(&ok);
                        if(!ok)
                            return false;
                        else
                        {
                            coords.push_back(coord);
                        }
                        break;
                    }
                    pos++;
                }

                if(coords.size() >= 1)
                {
                    while(coords.size() < 3)
                    {
                        coords.push_back(0);
                    }
                    QVector3D coordsVector(coords[0],coords[1],coords[2]);
                    addNode(nodeId, coordsVector);
                }
                line = m_file.readLine();
            }
        }
    }
    m_file.reset();
    return true;
}

bool VLayerFromGmeshBuilder::importConnections() 
{
    QString line;
    while (!m_file.atEnd())
    {
        line = m_file.readLine();
        if(line.startsWith("$Elements",Qt::CaseInsensitive))
        {
            line = m_file.readLine();
            line = m_file.readLine();
            while(!line.contains("$EndElements"))
            {
                int elementType = 0;
                int numberOfTags = 0;
                std::list<int> vertices;
                int pos=0;
                QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                for(QStringList::iterator it = tokens.begin(); it != tokens.end(); it++)
                {
                    bool ok;
                    QString token = (*it);

                    switch(pos){
                    case 0:
                        token.toInt(&ok);
                        if(!ok)
                            return false;
                        break;
                    case 1:
                        elementType = token.toInt(&ok);
                        if(!ok)
                            return false;
                        break;
                    case 2:
                        numberOfTags = token.toInt(&ok);
                        if(!ok)
                            return false;
                        break;
                    default:
                        int nodeNumber = token.toInt(&ok);
                        if(!ok)
                            return false;
                        if(pos > (2 + numberOfTags))
                        {
                            vertices.push_back(nodeNumber);
                        }
                        break;
                    }
                    pos++;
                }

                if( elementType == 2 || elementType == 3 )
                {
                    // elementType 2: triangle
                    // elementType 3: quadrangle
                    createConnections(&vertices);
                }
                else if( elementType == 4)
                {
                    // elementType 4: tetrahedron
                    if(vertices.size() == 4)
                    {
                        std::list<int> verticesTriangle1(vertices);
                        verticesTriangle1.pop_front();
                        createConnections(&verticesTriangle1);

                        std::list<int> verticesTriangle2(vertices);
                        verticesTriangle2.erase(++(verticesTriangle2.begin()));
                        createConnections(&verticesTriangle2);

                        std::list<int> verticesTriangle3(vertices);
                        verticesTriangle3.erase(++++(verticesTriangle3.begin()));
                        createConnections(&verticesTriangle3);

                        std::list<int> verticesTriangle4(vertices);
                        verticesTriangle4.pop_back();
                        createConnections(&verticesTriangle4);
                    }
                }
                line = m_file.readLine();
            }
        }
    }
    m_file.reset();
    return true;
}
