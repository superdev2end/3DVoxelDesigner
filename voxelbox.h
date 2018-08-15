#ifndef VOXELBOX_H
#define VOXELBOX_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

#include "custommeshgeometry.h"
#include "voxel3dnode.h"

class VoxelBox : public Qt3DCore::QEntity
{
    Q_OBJECT

public:
    explicit VoxelBox(Qt3DCore::QEntity* parent);

    void initialize();
    void updateNodes(QList<Voxel3DNode*> nodeList);
    void updateNode(Voxel3DNode* node);
    void updateVertics(QVector<QVector3D> vertics);

    bool isEqual(QList<Voxel3DNode*> nodeList);
    bool hasNode(Voxel3DNode* node);

    void setColor(QColor color);

    QVector<QVector3D> getNodeIndexList(){ return m_nodeIndexList; }
    QColor  getColor(){ return m_color; }

    Qt3DExtras::QPhongAlphaMaterial* getMaterial();

signals:

public slots:

private:
    Qt3DCore::QEntity*                  m_pRootEntity;
    Qt3DExtras::QPhongAlphaMaterial*    m_pMaterial;

    CustomeMeshGeometry*                m_pGeometry;

    QVector<QVector3D>                  m_vertics;
    QVector<int>                        m_indices;
    QVector<QVector3D>                  m_nodeIndexList;

    QColor                              m_color;
};

#endif // VOXELBOX_H
