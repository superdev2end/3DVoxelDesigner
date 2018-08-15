#ifndef VOXELCLOUDNODE_H
#define VOXELCLOUDNODE_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

#include "constants.h"
#include "custommeshgeometry.h"
#include "voxel3dwidget.h"

class VoxelCloudNode : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit VoxelCloudNode(Qt3DCore::QEntity* parent);

    void initialize(int sizeX, int sizeY, int sizeZ, int spaceSize);
    void updateVertex(int x, int y, int z, QVector3D pos);

    void setVisible(int index_x, int index_y, int index_z);
    void setUnvisible(int index_x, int index_y, int index_z);
    void addSelectedVoxel(int index_x, int index_y, int index_z);
    void clearSelectedVoxels();
    void setVisibleDeactive(int index_x, int index_y, int index_z);
    void setUnvisibleDeactive(int index_x, int index_y, int index_z);
    void setReactorCandidate(int index_x, int index_y, int index_z);
    void clearReactorCandidates();
    void setStraightEdge(int index_x, int index_y, int index_z);
    void clearStraightEdges();
    void setCamera(Qt3DRender::QCamera*  pCamera);
    void setMainView(Voxel3DWidget *pMainView);
signals:
    void voxelClicked(int index_x, int index_y, int index_z, QVector3D pos);
    void voxelCtrlClicked(int index_x, int index_y, int index_z, QVector3D pos);
    void voxelShiftClicked(int index_x, int index_y, int index_z, QVector3D pos);
private slots:
    void            onClicked(Qt3DRender::QPickEvent*);
    void            onPressed(Qt3DRender::QPickEvent*);
public slots:

private:
    Qt3DCore::QEntity*                  m_pRootEntity;

    Qt3DRender::QObjectPicker*          m_pPicker;

    Qt3DCore::QEntity*                  m_pChildEntity_visible;
    Qt3DCore::QEntity*                  m_pChildEntity_unvisible;
    Qt3DCore::QEntity*                  m_pChildEntity_selected;
    Qt3DCore::QEntity*                  m_pChildEntity_visible_deactive;
    Qt3DCore::QEntity*                  m_pChildEntity_unvisible_deactive;
    Qt3DCore::QEntity*                  m_pChildEntity_reactor_candidates;
    Qt3DCore::QEntity*                  m_pChildEntity_Straight;

    Qt3DExtras::QPhongMaterial*         m_pMaterial_visible;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial_unvisible;
    Qt3DExtras::QPhongMaterial*         m_pMaterial_selected;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial_visible_deactive;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial_unvisible_deactive;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial_reactor_candidates;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial_Straight;

    CustomeMeshGeometry*                m_pGeometry_visible;
    CustomeMeshGeometry*                m_pGeometry_unvisible;
    CustomeMeshGeometry*                m_pGeometry_selected;
    CustomeMeshGeometry*                m_pGeometry_visible_deactive;
    CustomeMeshGeometry*                m_pGeometry_unvisible_deactive;
    CustomeMeshGeometry*                m_pGeometry_reactor_candidates;
    CustomeMeshGeometry*                m_pGeometry_Straight;

    QVector<QVector3D>                  m_vertics_visible;
    QVector<QVector3D>                  m_vertics_unvisible;
    QVector<QVector3D>                  m_vertics_selected;
    QVector<QVector3D>                  m_vertics_visible_deactive;
    QVector<QVector3D>                  m_vertics_unvisible_deactive;
    QVector<QVector3D>                  m_vertics_reactor_candidates;
    QVector<QVector3D>                  m_vertics_Straight;

    QVector<int>                        m_indices_visible;
    QVector<int>                        m_indices_unvisible;
    QVector<int>                        m_indices_selected;
    QVector<int>                        m_indices_visible_deactive;
    QVector<int>                        m_indices_unvisible_deactive;
    QVector<int>                        m_indices_reactor_candidates;
    QVector<int>                        m_indices_Straight;

    int                                 m_workspaceSizeX;
    int                                 m_workspaceSizeY;
    int                                 m_workspaceSizeZ;
    int                                 m_spacingSize;

    float                               m_pointSize;
    int                                 m_pressedIndex;
    Qt3DRender::QCamera*                m_pCamera;
    Voxel3DWidget*                      m_pMainView;

    int                                 m_nMinX;
    int                                 m_nMinY;
    int                                 m_nMinZ;
    QVector3D                           m_LastCameraPos;
};

#endif // VOXELCLOUDNODE_H
