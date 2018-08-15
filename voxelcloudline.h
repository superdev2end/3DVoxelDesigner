#ifndef VOXELCLOUDLINE_H
#define VOXELCLOUDLINE_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

#include "custommeshgeometry.h"

class VoxelCloudLine : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit VoxelCloudLine(Qt3DCore::QEntity* parent);

    void initialize(int sizeX, int sizeY, int sizeZ, int spaceSize);
    void updateVertex(int x, int y, int z, QVector3D pos);
    void addLine(int x1, int y1, int z1, int x2, int y2, int z2);
    void removeLine(int x1, int y1, int z1, int x2, int y2, int z2);
    void addLineReactorCandidates(int x1, int y1, int z1, int x2, int y2, int z2);
    void clearLineReactorCandidates();
    void addLineStraightEdge(int x1, int y1, int z1, int x2, int y2, int z2);
    void clearStraightEdge();

    void setColor(QColor color);
    void setAlpha(float a);
signals:

public slots:

private:
    Qt3DCore::QEntity*                  m_pRootEntity;

    Qt3DCore::QEntity*                  m_pChildEntity_line;
    Qt3DExtras::QPhongMaterial*         m_pMaterial;
    CustomeMeshGeometry*                m_pGeometry;
    QVector<QVector3D>                  m_vertics;
    QVector<int>                        m_indices;

    Qt3DCore::QEntity*                  m_pChildEntity_line_reactor_candidates;
    Qt3DExtras::QPhongMaterial*         m_pMaterial_reactor_candidates;
    CustomeMeshGeometry*                m_pGeometry_reactor_candidates;
    QVector<QVector3D>                  m_vertics_reactor_candidates;
    QVector<int>                        m_indices_reactor_candidates;

    Qt3DCore::QEntity*                  m_pChildEntity_line_StraightEdge;
    Qt3DExtras::QPhongMaterial*         m_pMaterial_StraightEdge;
    CustomeMeshGeometry*                m_pGeometry_StraightEdge;
    QVector<QVector3D>                  m_vertics_StraightEdge;
    QVector<int>                        m_indices_StraightEdge;

    int                                 m_workspaceSizeX;
    int                                 m_workspaceSizeY;
    int                                 m_workspaceSizeZ;
    int                                 m_spacingSize;

};

#endif // VOXELCLOUDLINE_H
