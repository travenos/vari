/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VLAYER_H
#define _VLAYER_H

class VLayer {
public: 
    
/**
 * @param nodes
 * @param triangles
 * @param material
 */
void VLayer(vector<VNodePtr> nodes, vector<VTrianglePtr> triangles, VMaterial& material);
    
/**
 * @param visible
 */
void setVisible(bool visible);
    
bool isVisible();
    
double getMedianDistance();
    
vector<&VNodePtr> getNodes();
    
vector<&VTrianglePtr> getTriangles();
    
void reset();
private: 
    vector<VNodePtr> m_nodes;
    vector<VTrianglePtr> m_triangles;
    VMaterialPtr m_pMaterial;
    bool m_visible;
};

#endif //_VLAYER_H