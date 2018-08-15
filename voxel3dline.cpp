#include "voxel3dline.h"

Voxel3DLine::Voxel3DLine(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;    
    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongAlphaMaterial(m_pRootEntity);
    m_pMaterial->setAmbient(Qt::blue);
    m_pMaterial->setDiffuse(Qt::white);
    this->addComponent(m_pMaterial);

    // Line Mesh Geometry
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer(this);
    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    m_pGeometry = new CustomeMeshGeometry(this);
    customMeshRenderer->setGeometry(m_pGeometry);
    this->addComponent(customMeshRenderer);
}

void Voxel3DLine::setPoints(QVector3D start, QVector3D end)
{
    QVector<QVector3D> pos;
    QVector<int> indices;

    pos << start << end;
    indices << 0 << 1;

    m_pGeometry->setVertics(pos, indices);
}

void Voxel3DLine::setColor(QColor color)
{
    m_pMaterial->setAmbient(color);
}

void Voxel3DLine::setAlpha(float a)
{
    m_pMaterial->setAlpha(a);
}
