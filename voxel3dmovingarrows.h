#ifndef VOXEL3DMOVINGARROWS_H
#define VOXEL3DMOVINGARROWS_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QVector3D>
#include <Qt3DExtras>

#include "voxel3dcone.h"
#include "voxel3dtext.h"

class Voxel3DMovingArrows : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit Voxel3DMovingArrows(Qt3DCore::QEntity *parent = nullptr);

    void MoveArrows(QVector3D arrowPos);

    Voxel3DCone* getXConeArrow();
    Voxel3DCone* getYConeArrow();
    Voxel3DCone* getZConeArrow();

    Voxel3DText* getXText();
    Voxel3DText* getYText();
    Voxel3DText* getZText();

    void setXArrowColor(QColor color);
    void setYArrowColor(QColor color);
    void setZArrowColor(QColor color);
    void setArrowColorDefault();

    void setZoom(float);
    void setArrowLength(int l, int cubeY);
signals:

public slots:

private:
    Qt3DCore::QEntity*                  m_pRootEntity;

    QVector3D                           m_centerPos;
    Voxel3DCone*                        m_pXConeArrow;
    Voxel3DCone*                        m_pXConeStick;
    Voxel3DCone*                        m_pYConeArrow;
    Voxel3DCone*                        m_pYConeStick;
    Voxel3DCone*                        m_pZConeArrow;
    Voxel3DCone*                        m_pZConeStick;

    Voxel3DText*                        m_pXValueText;
    Voxel3DText*                        m_pYValueText;
    Voxel3DText*                        m_pZValueText;

    float                                 m_fArrowLength;
};

#endif // VOXEL3DMOVINGARROWS_H
