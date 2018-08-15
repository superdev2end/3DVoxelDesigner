#include "voxelnodemarker.h"

VoxelNodeMarker::VoxelNodeMarker(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    m_pRootEntity = parent;

    m_fPercentSize = 1;

    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongMaterial(m_pRootEntity);
    m_pMaterial->setAmbient(Qt::black);
    m_pMaterial->setDiffuse(Qt::black);
    m_pMaterial->setShininess(0);
    this->addComponent(m_pMaterial);

    m_pMarkerText = new Qt3DExtras::QExtrudedTextMesh(m_pRootEntity);
    m_pMarkerText->setDepth(.1f);
    QFont font("Arial", 32, QFont::Bold, false);
    m_pMarkerText->setFont(font);

    m_pCuboidMesh = new Qt3DExtras::QCuboidMesh(m_pRootEntity);
    m_pCuboidMesh->setXExtent(20);
    m_pCuboidMesh->setYExtent(5);
    m_pCuboidMesh->setZExtent(0.05f);

    m_pCuboidEntity = new Qt3DCore::QEntity(m_pRootEntity);
    m_pCuboidEntity->addComponent(m_pCuboidMesh);

    Qt3DExtras::QPhongAlphaMaterial* pMaterial = new Qt3DExtras::QPhongAlphaMaterial(m_pCuboidMesh);
    pMaterial->setAmbient(Qt::yellow);
    pMaterial->setDiffuse(Qt::yellow);
    m_pCuboidEntity->addComponent(pMaterial);

    m_pMarkerTransform = new Qt3DCore::QTransform;
    m_pMarkerTransform->setScale(10);

    m_startPosition = m_pMarkerTransform->translation();

    m_pCuboidTransform = new Qt3DCore::QTransform;
    m_pCuboidTransform->setScale(10);
    m_pCuboidEntity->addComponent(m_pCuboidTransform);

    m_pPresentLine = new Voxel3DLine(m_pRootEntity);
    m_pPresentLine->setColor(Qt::red);

    m_pPicker = new Qt3DRender::QObjectPicker(m_pCuboidEntity);
    QObject::connect(m_pPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(onPressed(Qt3DRender::QPickEvent*)));

    this->addComponent(m_pCuboidMesh);
    this->addComponent(m_pMarkerText);
    this->addComponent(m_pMarkerTransform);
    m_pCuboidEntity->addComponent(m_pPicker);
}

VoxelNodeMarker::~VoxelNodeMarker() {
    delete m_pCuboidMesh;
    delete m_pCuboidEntity;
    delete m_pPresentLine;
}

Qt3DExtras::QExtrudedTextMesh* VoxelNodeMarker::getMarkerText() {
    return m_pMarkerText;
}

void VoxelNodeMarker::setMarkerText(Qt3DExtras::QExtrudedTextMesh* pText) {
    m_pMarkerText = pText;
}

Qt3DCore::QTransform* VoxelNodeMarker::getMarkerTransform() {
    return m_pMarkerTransform;
}

void VoxelNodeMarker::setMarkerTransform(Qt3DCore::QTransform* pTransform) {
    m_pMarkerTransform = pTransform;
}

Qt3DCore::QTransform* VoxelNodeMarker::getCuboidTransform() {
    return m_pCuboidTransform;
}

void VoxelNodeMarker::setCuboidTransform(Qt3DCore::QTransform* pTransform) {
    m_pCuboidTransform = pTransform;
}

Qt3DExtras::QPhongMaterial* VoxelNodeMarker::getMaterial()
{
    return m_pMaterial;
}

Voxel3DNode* VoxelNodeMarker::getNode() {
    return m_pNode;
}

void VoxelNodeMarker::setNode(Voxel3DNode* pNode) {
    m_pNode = pNode;
}

Voxel3DLine* VoxelNodeMarker::getPresentLine() {
    return m_pPresentLine;
}

Qt3DRender::QObjectPicker* VoxelNodeMarker::getPicker() {
    return m_pPicker;
}

void VoxelNodeMarker::onPressed(Qt3DRender::QPickEvent* evt)
{
    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    qDebug() << "Marker Pressed";
    emit NodeMarkerPressed(m_pNode);

}

Qt3DExtras::QCuboidMesh* VoxelNodeMarker::getCuboidMesh() {
    return m_pCuboidMesh;
}

void VoxelNodeMarker::setZoom(float zoom) {
    m_pMarkerTransform->setScale(10 * m_fPercentSize / zoom);
    m_pCuboidTransform->setScale(10 * m_fPercentSize / zoom);
}

QVector3D VoxelNodeMarker::getStartPosition() {
    return m_startPosition;
}

void VoxelNodeMarker::setStartPosition(QVector3D startPos) {
    m_startPosition = startPos;
}

void VoxelNodeMarker::setMarkerSize(int nSpacingSize, int cubeY) {
    float fCubeRate = cubeY * 0.75f / 8 + 0.5f / 8;     // 2 -> 0.25f, 10 -> 1.0f
    m_fPercentSize = (0.7f / 72 * nSpacingSize + 16.0f / 72) * fCubeRate;
}
