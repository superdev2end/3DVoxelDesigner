#ifndef VOXELNODEMARKER_H
#define VOXELNODEMARKER_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>
#include <Qt3DRender>

#include "voxel3dnode.h"
#include "voxel3dline.h"

class VoxelNodeMarker : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit VoxelNodeMarker(Qt3DCore::QEntity *parent = nullptr);

    ~VoxelNodeMarker();
    Qt3DExtras::QExtrudedTextMesh* getMarkerText();
    void setMarkerText(Qt3DExtras::QExtrudedTextMesh* pText);

    Qt3DExtras::QCuboidMesh* getCuboidMesh();

    Qt3DCore::QTransform* getMarkerTransform();
    void setMarkerTransform(Qt3DCore::QTransform* pTransform);

    Qt3DCore::QTransform* getCuboidTransform();
    void setCuboidTransform(Qt3DCore::QTransform* pTransform);

    Qt3DExtras::QPhongMaterial* getMaterial();

    Voxel3DNode* getNode();
    void setNode(Voxel3DNode* pNode);

    Voxel3DLine* getPresentLine();

    Qt3DRender::QObjectPicker* getPicker();

    QVector3D getStartPosition();
    void setStartPosition(QVector3D);

    void setZoom(float);
    void setMarkerSize(int nSpacingSize, int cubeY);

signals:
    void NodeMarkerPressed(Voxel3DNode*);


public slots:


private:
    Qt3DCore::QEntity*                  m_pRootEntity;
    Qt3DCore::QEntity*                  m_pCuboidEntity;
    Qt3DExtras::QPhongMaterial*         m_pMaterial;
    Qt3DExtras::QExtrudedTextMesh*      m_pMarkerText;
    Qt3DExtras::QCuboidMesh*            m_pCuboidMesh;
    Qt3DCore::QTransform*               m_pMarkerTransform;
    Qt3DCore::QTransform*               m_pCuboidTransform;

    Voxel3DNode*                        m_pNode;
    Voxel3DLine*                        m_pPresentLine;
    Qt3DRender::QObjectPicker*          m_pPicker;

    QVector3D                           m_startPosition;

    float                               m_fPercentSize;

private slots:
    void onPressed(Qt3DRender::QPickEvent*);
};

#endif // VOXELNODEMARKER_H
