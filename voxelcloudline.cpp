#include "voxelcloudline.h"

VoxelCloudLine::VoxelCloudLine(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;

    // Child Entities
    m_pChildEntity_line = new Qt3DCore::QEntity(this);
    m_pChildEntity_line_reactor_candidates = new Qt3DCore::QEntity(this);
    m_pChildEntity_line_StraightEdge = new Qt3DCore::QEntity(this);


    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongMaterial(m_pChildEntity_line);
    m_pMaterial->setAmbient(Qt::blue);
    m_pMaterial->setDiffuse(Qt::white);

    m_pChildEntity_line->addComponent(m_pMaterial);

    m_pMaterial_reactor_candidates = new Qt3DExtras::QPhongMaterial(m_pChildEntity_line_reactor_candidates);
    m_pMaterial_reactor_candidates->setAmbient(Qt::green);
    m_pMaterial_reactor_candidates->setDiffuse(Qt::white);

    m_pChildEntity_line_reactor_candidates->addComponent(m_pMaterial_reactor_candidates);

    m_pMaterial_StraightEdge = new Qt3DExtras::QPhongMaterial(m_pChildEntity_line_StraightEdge);
    m_pMaterial_StraightEdge->setAmbient(Qt::red);
    m_pMaterial_StraightEdge->setDiffuse(Qt::white);

    m_pChildEntity_line_StraightEdge->addComponent(m_pMaterial_StraightEdge);

    // Line Mesh Geometry
    Qt3DRender::QGeometryRenderer *customMeshRenderer1 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_line);
    customMeshRenderer1->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    m_pGeometry = new CustomeMeshGeometry(m_pChildEntity_line);
    customMeshRenderer1->setGeometry(m_pGeometry);
    m_pChildEntity_line->addComponent(customMeshRenderer1);

    Qt3DRender::QGeometryRenderer *customMeshRenderer2 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_line_reactor_candidates);
    customMeshRenderer2->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    m_pGeometry_reactor_candidates = new CustomeMeshGeometry(m_pChildEntity_line_reactor_candidates);
    customMeshRenderer2->setGeometry(m_pGeometry_reactor_candidates);
    m_pChildEntity_line_reactor_candidates->addComponent(customMeshRenderer2);

    Qt3DRender::QGeometryRenderer *customMeshRenderer3 = new Qt3DRender::QGeometryRenderer(m_pChildEntity_line_StraightEdge);
    customMeshRenderer3->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    m_pGeometry_StraightEdge = new CustomeMeshGeometry(m_pChildEntity_line_StraightEdge);
    customMeshRenderer3->setGeometry(m_pGeometry_StraightEdge);
    m_pChildEntity_line_StraightEdge->addComponent(customMeshRenderer3);
}

void VoxelCloudLine::initialize(int sizeX, int sizeY, int sizeZ, int spaceSize)
{
    m_workspaceSizeX = sizeX;
    m_workspaceSizeY = sizeY;
    m_workspaceSizeZ = sizeZ;
    m_spacingSize = spaceSize;

    for(int x = 0 ; x < m_workspaceSizeX; x ++)
    {
        for(int y = 0 ; y < m_workspaceSizeY; y ++)
        {
            for(int z = 0 ; z < m_workspaceSizeZ; z ++)
            {
                QVector3D pos = (QVector3D(x , y, z) - QVector3D(static_cast<float>(m_workspaceSizeX - 1) / 2.0f,
                                                                 static_cast<float>(m_workspaceSizeY - 1) / 2.0f,
                                                                 static_cast<float>(m_workspaceSizeZ - 1) / 2.0f)) * m_spacingSize;
                m_vertics << pos;
                m_vertics_reactor_candidates << pos;
                m_vertics_StraightEdge << pos;
            }
        }
    }

//    for (int x = 0; x < m_workspaceSizeX; x++) {
//        for (int y = 0; y < m_workspaceSizeY; y++) {
//            for( int z = 0; z < m_workspaceSizeZ; z++){
//                int i = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;

//                if(x + 1 < m_workspaceSizeX)
//                {
//                    int i_right = z + y * m_workspaceSizeZ + (x + 1) * m_workspaceSizeY * m_workspaceSizeZ; // x + 1
//                    m_indices << i << i_right;
//                }
//                if(y + 1 < m_workspaceSizeY)
//                {
//                    int i_up = z + (y + 1) * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ; // y + 1
//                    m_indices << i << i_up;
//                }
//                if(z + 1 < m_workspaceSizeZ)
//                {
//                    int i_front = (z + 1) + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ; // z + 1
//                    m_indices << i << i_front;
//                }
//            }
//        }
//    }
//    m_pGeometry->setVertics(m_vertics, m_indices);
    m_pGeometry->setVertics(m_vertics);
    m_pGeometry_reactor_candidates->setVertics(m_vertics);
    m_pGeometry_StraightEdge->setVertics(m_vertics);
}

void VoxelCloudLine::updateVertex(int x, int y, int z, QVector3D pos)
{
    int i = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;

    if(i < m_vertics_StraightEdge.length()) {
        m_vertics_StraightEdge.replace(i, pos);
        m_pGeometry_StraightEdge->setVertics(m_vertics_StraightEdge);
    }
    if(i < m_vertics_reactor_candidates.length()) {
        m_vertics_reactor_candidates.replace(i, pos);
        m_pGeometry_reactor_candidates->setVertics(m_vertics_reactor_candidates);
    }
    if(i < m_vertics.length()) {
        m_vertics.replace(i, pos);
        m_pGeometry->setVertics(m_vertics);
    }
}

void VoxelCloudLine::addLine(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int i1 = z1 + y1 * m_workspaceSizeZ + x1 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i1 >= m_vertics.length())
        return;

    int i2 = z2 + y2 * m_workspaceSizeZ + x2 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i2 >= m_vertics.length())
        return;

    bool exist = false;
    for(int i = 1 ; i < m_indices.length(); i += 2)
    {
        if((m_indices.at(i - 1) == i1 && m_indices.at(i) == i2) ||
                (m_indices.at(i - 1) == i2 && m_indices.at(i) == i1))
        {
            exist = true;
            break;
        }
    }

    if(!exist){
        m_indices << i1 << i2;
        m_pGeometry->setIndices(m_indices);
    }
}

void VoxelCloudLine::removeLine(int x1, int y1, int z1, int x2, int y2, int z2)
{
    qDebug() << "Remove Line";
    int i1 = z1 + y1 * m_workspaceSizeZ + x1 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i1 >= m_vertics.length())
        return;

    int i2 = z2 + y2 * m_workspaceSizeZ + x2 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i2 >= m_vertics.length())
        return;

    for(int i = 1 ; i < m_indices.length(); i += 2)
    {
        if((m_indices.at(i - 1) == i1 && m_indices.at(i) == i2) ||
                (m_indices.at(i - 1) == i2 && m_indices.at(i) == i1))
        {
            m_indices.remove(i - 1, 2);

            m_pGeometry->setIndices(m_indices);
        }
    }
}


void VoxelCloudLine::addLineReactorCandidates(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int i1 = z1 + y1 * m_workspaceSizeZ + x1 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i1 >= m_vertics_reactor_candidates.length())
        return;

    int i2 = z2 + y2 * m_workspaceSizeZ + x2 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i2 >= m_vertics_reactor_candidates.length())
        return;

    bool exist = false;
    for(int i = 1 ; i < m_indices_reactor_candidates.length(); i += 2)
    {
        if((m_indices_reactor_candidates.at(i - 1) == i1 && m_indices_reactor_candidates.at(i) == i2) ||
                (m_indices_reactor_candidates.at(i - 1) == i2 && m_indices_reactor_candidates.at(i) == i1))
        {
            exist = true;
            break;
        }
    }

    if(!exist){
        m_indices_reactor_candidates << i1 << i2;
        m_pGeometry_reactor_candidates->setIndices(m_indices_reactor_candidates);
    }
}

void VoxelCloudLine::clearLineReactorCandidates()
{
    qDebug() << "Clear Reactor Line";
    foreach(int item, m_indices_reactor_candidates){
        m_indices_reactor_candidates.removeAll(item);
    }

    m_indices_reactor_candidates.clear();
    m_pGeometry_reactor_candidates->setIndices(m_indices_reactor_candidates);
}

void VoxelCloudLine::addLineStraightEdge(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int i1 = z1 + y1 * m_workspaceSizeZ + x1 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i1 >= m_vertics_StraightEdge.length())
        return;

    int i2 = z2 + y2 * m_workspaceSizeZ + x2 * m_workspaceSizeY * m_workspaceSizeZ;
    if(i2 >= m_vertics_StraightEdge.length())
        return;

    bool exist = false;
    for(int i = 1 ; i < m_indices_StraightEdge.length(); i += 2)
    {
        if((m_indices_StraightEdge.at(i - 1) == i1 && m_indices_StraightEdge.at(i) == i2) ||
                (m_indices_StraightEdge.at(i - 1) == i2 && m_indices_StraightEdge.at(i) == i1))
        {
            exist = true;
            break;
        }
    }

    if(!exist){
        m_indices_StraightEdge << i1 << i2;
        m_pGeometry_StraightEdge->setIndices(m_indices_StraightEdge);
    }
}

void VoxelCloudLine::clearStraightEdge()
{
    qDebug() << "Clear Reactor Line";
    foreach(int item, m_indices_StraightEdge){
        m_indices_StraightEdge.removeAll(item);
    }

    m_indices_StraightEdge.clear();
    m_pGeometry_StraightEdge->setIndices(m_indices_StraightEdge);
}

void VoxelCloudLine::setColor(QColor color)
{
    m_pMaterial->setAmbient(color);
    m_pMaterial->setSpecular(color);
}

void VoxelCloudLine::setAlpha(float a)
{
//    m_pMaterial->setAlpha(a);
}
