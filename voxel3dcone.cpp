#include "voxel3dcone.h"

Voxel3DCone::Voxel3DCone(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;
    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongMaterial(m_pRootEntity);
    m_pMaterial->setAmbient(Qt::red);
    m_pMaterial->setDiffuse(Qt::red);
    m_pMaterial->setShininess(0);
    this->addComponent(m_pMaterial);

    m_pConeMesh = new Qt3DExtras::QConeMesh;
    m_pConeMesh->setRings(20);
    m_pConeMesh->setSlices(20);
    m_pConeMesh->setLength(15);
    m_pConeMesh->setTopRadius(0);
    m_pConeMesh->setBottomRadius(6);
    m_pConeMesh->setHasTopEndcap(true);
    m_pConeMesh->setHasBottomEndcap(true);

    m_pConeTransform = new Qt3DCore::QTransform;
    m_pConeTransform->setScale3D(QVector3D(1, 1, 1));
    m_pConeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 0), 0.0f));

    this->addComponent(m_pConeMesh);
    this->addComponent(m_pConeTransform);
    this->addComponent(m_pMaterial);

}

Qt3DExtras::QPhongMaterial* Voxel3DCone::getMaterial()
{
    return m_pMaterial;
}

CustomeMeshGeometry* Voxel3DCone::getGeometry() {
    return m_pGeometry;
}

Qt3DExtras::QConeMesh* Voxel3DCone::getConeMesh() {
    return m_pConeMesh;
}

Qt3DCore::QTransform* Voxel3DCone::getTransform() {
    return m_pConeTransform;
}

