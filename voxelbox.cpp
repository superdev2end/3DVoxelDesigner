#include "voxelbox.h"

VoxelBox::VoxelBox(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;
    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongAlphaMaterial(m_pRootEntity);
    m_color = Qt::blue;
    m_pMaterial->setAmbient(m_color);
    m_pMaterial->setDiffuse(Qt::white);
    m_pMaterial->setAlpha(0.8f);

    this->addComponent(m_pMaterial);

    // Line Mesh Geometry
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer(this);
    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    m_pGeometry = new CustomeMeshGeometry(this);
    customMeshRenderer->setGeometry(m_pGeometry);
    this->addComponent(customMeshRenderer);
}

void VoxelBox::initialize()
{
    m_vertics.clear();
    m_vertics << QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) <<
                 QVector3D(0, 0, 0) ;
    m_indices << 0 << 1 << 2            // Left
                << 1 << 3 << 2
                << 5 << 4 << 6          // Right
                << 5 << 6 << 7
                << 1 << 5 << 7          // Front
                << 1 << 7 << 3
                << 4 << 0 << 2          // Back
                << 4 << 2 << 6
                << 7 << 6 << 2          // Up
                << 7 << 2 << 3
                << 4 << 5 << 0          // Down
                << 5 << 1 << 0;
    m_pGeometry->setVertics(m_vertics, m_indices);
}

void VoxelBox::updateNodes(QList<Voxel3DNode*> nodeList)
{
    m_nodeIndexList.clear();
    QVector<QVector3D> vertics;
    for(int i = 0 ; i < nodeList.count(); i++){
        vertics << nodeList.at(i)->position();
        m_nodeIndexList.push_back(QVector3D(nodeList.at(i)->nodeIndexX(), nodeList.at(i)->nodeIndexY(), nodeList.at(i)->nodeIndexZ()));
    }
    updateVertics(vertics);
}

void VoxelBox::updateNode(Voxel3DNode *node)
{
    if(m_nodeIndexList.count() != m_vertics.count())
        return;

    for(int i = 0 ; i < m_nodeIndexList.count(); i++){
        if(m_nodeIndexList.at(i) == QVector3D(node->nodeIndexX(), node->nodeIndexY(), node->nodeIndexZ()))
            m_vertics.replace(i, node->position());
    }
    m_pGeometry->setVertics(m_vertics);
}

void VoxelBox::updateVertics(QVector<QVector3D> vertics)
{
    m_vertics.clear();
    if(vertics.length() != 8 )
        return;

    m_vertics = vertics;
    m_pGeometry->setVertics(m_vertics);

}

bool VoxelBox::isEqual(QList<Voxel3DNode *> nodeList)
{
    if(m_nodeIndexList.count() != nodeList.count())
        return false;
    for(int i = 0 ; i < nodeList.count(); i++){
        if(m_nodeIndexList.at(i) != QVector3D(nodeList.at(i)->nodeIndexX(), nodeList.at(i)->nodeIndexY(), nodeList.at(i)->nodeIndexZ()))
            return false;
    }

    return true;
}

bool VoxelBox::hasNode(Voxel3DNode *node)
{
    for(int i = 0 ; i < m_nodeIndexList.count(); i++){
        if(m_nodeIndexList.at(i) == QVector3D(node->nodeIndexX(), node->nodeIndexY(), node->nodeIndexZ()))
            return true;
    }
    return false;
}

void VoxelBox::setColor(QColor color)
{
    m_color = color;
    m_pMaterial->setAmbient(color);
    m_pMaterial->setAlpha(0.8f);
}

Qt3DExtras::QPhongAlphaMaterial* VoxelBox::getMaterial() {
    return m_pMaterial;
}
