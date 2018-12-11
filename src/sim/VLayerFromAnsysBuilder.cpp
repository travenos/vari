/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <QTextStream>
#include "VLayerFromAnsysBuilder.h"

/**
 * VLayerFromAnsysBuilder implementation
 */


/**
 * @param filename const QString
 * @param param VSimulationParametres::const_ptr
 */
VLayerFromAnsysBuilder::VLayerFromAnsysBuilder(const QString &filename,
                                               const VCloth &material,
                                               const VSimulationParametres::const_ptr &p_simParam,
                                               VUnit units):
    VLayerFromFileBuilder(filename, material, p_simParam, units)
{

}

bool VLayerFromAnsysBuilder::importNodes()  
{
    QString line;
    while (!m_file.atEnd())
    {
        line = m_file.readLine();
        if(line.startsWith("nblock",Qt::CaseInsensitive))
        {
            m_file.readLine(); //read an empty line
            line = m_file.readLine();
            while(!line.contains(QRegExp("[a-z]|[A-D]|[F-Z]")))
            {
                if(line.at(0).isSpace())
                {
                    int nodeId = 0;
                    line.replace('-'," -");
                    line.replace("E -","E-");

                    QStringList tokens = line.split(QRegExp("[^\\d.+-E]"), QString::SkipEmptyParts); // \D = non digit

                    std::vector<double> coords;
                    int pos = 0;
                    for(QStringList::iterator it = tokens.begin(); it != tokens.end(); it++)
                    {
                        bool ok;
                        QString token = (*it);

                        switch(pos){
                                                case 0:
                            nodeId = token.toInt(&ok);
                            if(!ok)
                                return false;
                            break;
                                                case 3:
                                                case 4:
                                                case 5:
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
                }
                line = m_file.readLine();
            }
        }
    }
    m_file.reset();
    return true;
}

bool VLayerFromAnsysBuilder::importConnections()  
{
    QString line;
    while (!m_file.atEnd())
    {
        line = m_file.readLine();
        if(line.startsWith("eblock,19",Qt::CaseInsensitive))
        {
            m_file.readLine(); //read an empty line
            line = m_file.readLine();
            while(!line.contains(QRegExp("[a-z]|[A-Z]")))
            {
                if(line.at(0).isSpace())
                {
                    line = line.trimmed();
                    QTextStream istream(&line);

                    int connectivityId, unknown, connection;
                    int numOfVertices; // tripple or quad
                    std::list<int> connectionList;
                    istream			>> unknown >> unknown
                            >> unknown >> unknown
                            >> unknown >> unknown
                            >> unknown >> unknown //8 unknown parameters in the file
                            >> numOfVertices
                            >> unknown
                            >> connectivityId;
                    while(!istream.atEnd())
                    {
                        istream >> connection;
                        connectionList.push_back(connection);
                    }

                    if(connectionList.size() >= 2)
                    {
                        for(std::list<int>::const_iterator it = connectionList.begin(); it != connectionList.end(); it++)
                        {
                            std::list<int>::const_iterator itNode2 = it;
                            itNode2++;
                            if(itNode2 == connectionList.end())
                                itNode2 = connectionList.begin();

                            makeNeighbours(*it,*itNode2);
                        }
                        addShapePart(&connectionList);
                    }
                }
                line = m_file.readLine();
            }
        }
    }
    m_file.reset();
    return true;
}
