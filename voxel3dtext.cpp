#include "voxel3dtext.h"

Voxel3DText::Voxel3DText(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;

    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongMaterial(m_pRootEntity);
    m_pMaterial->setAmbient(Qt::red);
    m_pMaterial->setDiffuse(Qt::black);
    m_pMaterial->setShininess(0);
    this->addComponent(m_pMaterial);

    m_pTextMesh = new Qt3DExtras::QExtrudedTextMesh(m_pRootEntity);
    m_pTextMesh->setDepth(.1f);
    QFont font("Arial", 32, QFont::Bold, false);
    m_pTextMesh->setFont(font);
    m_pTextMesh->setText("0");

    m_pTextTransform = new Qt3DCore::QTransform;
    m_pTextTransform->setScale3D(QVector3D(1, 1, 1));
    m_pTextTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 0), 0.0f));
    m_pTextTransform->setScale(10);

    this->addComponent(m_pTextMesh);
    this->addComponent(m_pTextTransform);
    this->addComponent(m_pMaterial);

}

Qt3DExtras::QPhongMaterial* Voxel3DText::getMaterial() {
    return m_pMaterial;
}

void Voxel3DText::setMaterial(Qt3DExtras::QPhongMaterial* pMaterial) {
    m_pMaterial = pMaterial;
}

Qt3DExtras::QExtrudedTextMesh* Voxel3DText::getTextMesh() {
    return m_pTextMesh;
}

Qt3DCore::QTransform* Voxel3DText::getTransform() {
    return m_pTextTransform;
}

void Voxel3DText::setTransform(Qt3DCore::QTransform* pTransform) {
    m_pTextTransform = pTransform;
}
