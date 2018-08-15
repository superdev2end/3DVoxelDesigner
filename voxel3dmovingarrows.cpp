#include "voxel3dmovingarrows.h"

Voxel3DMovingArrows::Voxel3DMovingArrows(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;

    // X coordinage arrow
    m_pXConeArrow = new Voxel3DCone(m_pRootEntity);
    m_pXConeArrow->getMaterial()->setAmbient(Qt::red);
    m_pXConeArrow->getMaterial()->setDiffuse(Qt::red);
    m_pXConeArrow->getConeMesh()->setLength(20);
    m_pXConeArrow->getConeMesh()->setTopRadius(0);
    m_pXConeArrow->getConeMesh()->setBottomRadius(7);
    m_pXConeArrow->getTransform()->setRotationZ(-90);
    m_pXConeStick = new Voxel3DCone(m_pRootEntity);
    m_pXConeStick->getMaterial()->setAmbient(Qt::red);
    m_pXConeStick->getMaterial()->setDiffuse(Qt::red);
    m_pXConeStick->getConeMesh()->setLength(45);
    m_pXConeStick->getConeMesh()->setTopRadius(0.2f);
    m_pXConeStick->getConeMesh()->setBottomRadius(0.2f);
    m_pXConeStick->getTransform()->setRotationZ(-90);
    QVector3D xArrowVector = m_pXConeArrow->getTransform()->translation();
    float xDifference = (m_pXConeArrow->getConeMesh()->length() + m_pXConeStick->getConeMesh()->length()) / 2;
    m_pXConeStick->getTransform()->setTranslation(QVector3D(xArrowVector.x() - xDifference, xArrowVector.y(), xArrowVector.z()));

    m_pXValueText = new Voxel3DText(m_pRootEntity);
    m_pXValueText->getTextMesh()->setText("X 0");
    m_pXValueText->getMaterial()->setAmbient(Qt::red);
    m_pXValueText->getMaterial()->setDiffuse(Qt::red);
    m_pXValueText->getTransform()->setTranslation(m_pXConeStick->getTransform()->translation());

    // Y coordinate arrow
    m_pYConeArrow = new Voxel3DCone(m_pRootEntity);
    m_pYConeArrow->getMaterial()->setAmbient(Qt::green);
    m_pYConeArrow->getMaterial()->setDiffuse(Qt::green);
    m_pYConeArrow->getConeMesh()->setLength(20);
    m_pYConeArrow->getConeMesh()->setTopRadius(0);
    m_pYConeArrow->getConeMesh()->setBottomRadius(7);
    m_pYConeArrow->getTransform()->setRotationY(-90);
    m_pYConeStick = new Voxel3DCone(m_pRootEntity);
    m_pYConeStick->getMaterial()->setAmbient(Qt::green);
    m_pYConeStick->getMaterial()->setDiffuse(Qt::green);
    m_pYConeStick->getConeMesh()->setLength(45);
    m_pYConeStick->getConeMesh()->setTopRadius(1);
    m_pYConeStick->getConeMesh()->setBottomRadius(1);
    m_pYConeStick->getTransform()->setRotationY(-90);
    float yDifference = (m_pYConeArrow->getConeMesh()->length() + m_pYConeStick->getConeMesh()->length()) / 2;
    QVector3D yArrowVector = m_pYConeArrow->getTransform()->translation();
    m_pYConeStick->getTransform()->setTranslation(QVector3D(yArrowVector.x(), yArrowVector.y() - yDifference, yArrowVector.z()));

    m_pYValueText = new Voxel3DText(m_pRootEntity);
    m_pYValueText->getTextMesh()->setText("Y 0");
    m_pYValueText->getMaterial()->setAmbient(Qt::green);
    m_pYValueText->getMaterial()->setDiffuse(Qt::green);
    m_pYValueText->getTransform()->setTranslation(m_pYConeStick->getTransform()->translation());

    // Z coordinate arrow
    m_pZConeArrow = new Voxel3DCone(m_pRootEntity);
    m_pZConeArrow->getMaterial()->setAmbient(Qt::blue);
    m_pZConeArrow->getMaterial()->setDiffuse(Qt::blue);
    m_pZConeArrow->getConeMesh()->setLength(20);
    m_pZConeArrow->getConeMesh()->setTopRadius(0);
    m_pZConeArrow->getConeMesh()->setBottomRadius(7);
    m_pZConeArrow->getTransform()->setRotationX(90);
    m_pZConeStick = new Voxel3DCone(m_pRootEntity);
    m_pZConeStick->getMaterial()->setAmbient(Qt::blue);
    m_pZConeStick->getMaterial()->setDiffuse(Qt::blue);
    m_pZConeStick->getConeMesh()->setLength(45);
    m_pZConeStick->getConeMesh()->setTopRadius(1);
    m_pZConeStick->getConeMesh()->setBottomRadius(1);
    m_pZConeStick->getTransform()->setRotationX(90);
    float zDifference = (m_pZConeArrow->getConeMesh()->length() + m_pZConeStick->getConeMesh()->length()) / 2;
    QVector3D zArrowVector = m_pZConeArrow->getTransform()->translation();
    m_pZConeStick->getTransform()->setTranslation(QVector3D(zArrowVector.x(), zArrowVector.y(), zArrowVector.z() - zDifference));

    m_pZValueText = new Voxel3DText(m_pRootEntity);
    m_pZValueText->getTextMesh()->setText("Z 0");
    m_pZValueText->getMaterial()->setAmbient(Qt::blue);
    m_pZValueText->getMaterial()->setDiffuse(Qt::blue);
    m_pZValueText->getTransform()->setTranslation(m_pZConeStick->getTransform()->translation());
}

void Voxel3DMovingArrows::MoveArrows(QVector3D arrowPos) {
    m_centerPos = arrowPos;
    float dotSpace = m_pXConeArrow->getConeMesh()->length();
    QVector3D * xArrowVector = new QVector3D(arrowPos.x() + dotSpace + m_pXConeArrow->getConeMesh()->length() / 2 + m_pXConeStick->getConeMesh()->length(), arrowPos.y(), arrowPos.z());
    m_pXConeArrow->getTransform()->setTranslation(*xArrowVector);
    QVector3D * xStickVector = new QVector3D(arrowPos.x() + dotSpace + m_pXConeStick->getConeMesh()->length() / 2, arrowPos.y(), arrowPos.z());
    m_pXConeStick->getTransform()->setTranslation(*xStickVector);
    m_pXValueText->getTransform()->setTranslation(m_pXConeStick->getTransform()->translation());

    QVector3D * yArrowVector = new QVector3D(arrowPos.x(), arrowPos.y() + dotSpace + m_pXConeArrow->getConeMesh()->length() / 2 + m_pXConeStick->getConeMesh()->length(), arrowPos.z());
    m_pYConeArrow->getTransform()->setTranslation(*yArrowVector);
    QVector3D * yStickVector = new QVector3D(arrowPos.x(), arrowPos.y() + dotSpace + m_pXConeStick->getConeMesh()->length() / 2, arrowPos.z());
    m_pYConeStick->getTransform()->setTranslation(*yStickVector);
    m_pYValueText->getTransform()->setTranslation(m_pYConeStick->getTransform()->translation());

    QVector3D * zArrowVector = new QVector3D(arrowPos.x(), arrowPos.y(), arrowPos.z() + dotSpace + m_pXConeArrow->getConeMesh()->length() / 2 + m_pXConeStick->getConeMesh()->length());
    m_pZConeArrow->getTransform()->setTranslation(*zArrowVector);
    QVector3D * zStickVector = new QVector3D(arrowPos.x(), arrowPos.y(), arrowPos.z() + dotSpace + m_pXConeStick->getConeMesh()->length() / 2);
    m_pZConeStick->getTransform()->setTranslation(*zStickVector);
    m_pZValueText->getTransform()->setTranslation(m_pZConeStick->getTransform()->translation());
}

Voxel3DCone* Voxel3DMovingArrows::getXConeArrow() {
    return m_pXConeArrow;
}

Voxel3DCone* Voxel3DMovingArrows::getYConeArrow() {
    return m_pYConeArrow;
}

Voxel3DCone* Voxel3DMovingArrows::getZConeArrow() {
    return m_pZConeArrow;
}

void Voxel3DMovingArrows::setArrowLength(int l, int cubeY)
{
    float fCubeRate = cubeY * 0.75f / 8 + 0.5f / 8;     // 2 -> 0.25f, 10 -> 1.0f
    m_fArrowLength = (l * 0.8f + 13.6f) * fCubeRate;
}

void Voxel3DMovingArrows::setZoom(float zoom) {    
    m_pXConeArrow->getConeMesh()->setLength(m_fArrowLength * 0.25f / zoom);
    m_pXConeArrow->getConeMesh()->setBottomRadius(m_fArrowLength* 0.08f / zoom);
    m_pXConeStick->getConeMesh()->setLength(m_fArrowLength * 0.6f / zoom);
    m_pXConeStick->getConeMesh()->setTopRadius(m_fArrowLength / 80 / zoom);
    m_pXConeStick->getConeMesh()->setBottomRadius(m_fArrowLength / 80  / zoom);

    m_pYConeArrow->getConeMesh()->setLength(m_fArrowLength * 0.25f / zoom);
    m_pYConeArrow->getConeMesh()->setBottomRadius(m_fArrowLength* 0.08f / zoom);
    m_pYConeStick->getConeMesh()->setLength(m_fArrowLength * 0.6f / zoom);
    m_pYConeStick->getConeMesh()->setTopRadius(m_fArrowLength / 80  / zoom);
    m_pYConeStick->getConeMesh()->setBottomRadius(m_fArrowLength / 80  / zoom);

    m_pZConeArrow->getConeMesh()->setLength(m_fArrowLength * 0.25f / zoom);
    m_pZConeArrow->getConeMesh()->setBottomRadius(m_fArrowLength* 0.08f / zoom);
    m_pZConeStick->getConeMesh()->setLength(m_fArrowLength * 0.6f / zoom);
    m_pZConeStick->getConeMesh()->setTopRadius(m_fArrowLength / 80  / zoom);
    m_pZConeStick->getConeMesh()->setBottomRadius(m_fArrowLength / 80  / zoom);

    m_pXValueText->getTransform()->setScale(m_fArrowLength / 8 / zoom);
    m_pYValueText->getTransform()->setScale(m_fArrowLength / 8 / zoom);
    m_pZValueText->getTransform()->setScale(m_fArrowLength / 8 / zoom);

    MoveArrows(m_centerPos);
}

Voxel3DText* Voxel3DMovingArrows::getXText() {
    return m_pXValueText;
}

Voxel3DText* Voxel3DMovingArrows::getYText() {
    return m_pYValueText;
}

Voxel3DText* Voxel3DMovingArrows::getZText() {
    return m_pZValueText;
}

void Voxel3DMovingArrows::setXArrowColor(QColor color) {
    m_pXConeArrow->getMaterial()->setAmbient(color);
    m_pXConeArrow->getMaterial()->setDiffuse(color);
    m_pXConeStick->getMaterial()->setAmbient(color);
    m_pXConeStick->getMaterial()->setDiffuse(color);
}

void Voxel3DMovingArrows::setYArrowColor(QColor color) {
    m_pYConeArrow->getMaterial()->setAmbient(color);
    m_pYConeArrow->getMaterial()->setDiffuse(color);
    m_pYConeStick->getMaterial()->setAmbient(color);
    m_pYConeStick->getMaterial()->setDiffuse(color);
}

void Voxel3DMovingArrows::setZArrowColor(QColor color) {
    m_pZConeArrow->getMaterial()->setAmbient(color);
    m_pZConeArrow->getMaterial()->setDiffuse(color);
    m_pZConeStick->getMaterial()->setAmbient(color);
    m_pZConeStick->getMaterial()->setDiffuse(color);
}

void Voxel3DMovingArrows::setArrowColorDefault() {
    m_pXConeArrow->getMaterial()->setAmbient(Qt::red);
    m_pXConeArrow->getMaterial()->setDiffuse(Qt::red);
    m_pXConeStick->getMaterial()->setAmbient(Qt::red);
    m_pXConeStick->getMaterial()->setDiffuse(Qt::red);

    m_pYConeArrow->getMaterial()->setAmbient(Qt::green);
    m_pYConeArrow->getMaterial()->setDiffuse(Qt::green);
    m_pYConeStick->getMaterial()->setAmbient(Qt::green);
    m_pYConeStick->getMaterial()->setDiffuse(Qt::green);

    m_pZConeArrow->getMaterial()->setAmbient(Qt::blue);
    m_pZConeArrow->getMaterial()->setDiffuse(Qt::blue);
    m_pZConeStick->getMaterial()->setAmbient(Qt::blue);
    m_pZConeStick->getMaterial()->setDiffuse(Qt::blue);
}
