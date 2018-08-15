#ifndef VOXEL3DNODE_H
#define VOXEL3DNODE_H

#include <QObject>
#include <QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <QTransform>
#include <QVector3D>
#include <Qt3DRender>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras>
#include <Qt3DRender/QPointSize>

#include "voxel3dline.h"

#define NODE_COLOR_SELECTED QColor(200, 20, 20)
#define NODE_COLOR_VISIBLE QColor(20, 20, 200)
#define NODE_COLOR_UNVISIBLE QColor(200, 200, 200)
#define NODE_COLOR_REACTOR QColor(20, 200, 20)
#define NODE_COLOR_REACTOR_SELECTED Qt::black

#define REACTOR_COLOR_1 QColor(255, 153, 0)
#define REACTOR_COLOR_2 QColor(102, 255, 51)
#define REACTOR_COLOR_3 QColor(51, 153, 255)
#define REACTOR_COLOR_4 QColor(153, 0, 255)
#define REACTOR_COLOR_5 QColor(0, 153, 51)
#define REACTOR_COLOR_6 QColor(102, 51, 0)
#define REACTOR_COLOR_7 QColor(204, 102, 153)
#define REACTOR_COLOR_8 QColor(153, 102, 0)

class Voxel3DNode : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit Voxel3DNode(Qt3DCore::QEntity* parent);
    ~Voxel3DNode();

    Voxel3DNode*    leftNode(){ return m_pNeighborNodes.at(0); }
    Voxel3DNode*    rightNode(){ return m_pNeighborNodes.at(1); }
    Voxel3DNode*    upNode(){ return m_pNeighborNodes.at(2); }
    Voxel3DNode*    downNode(){ return m_pNeighborNodes.at(3); }
    Voxel3DNode*    frontNode(){ return m_pNeighborNodes.at(4); }
    Voxel3DNode*    backNode(){ return m_pNeighborNodes.at(5); }
    void            setLeftNode(Voxel3DNode* node){ m_pNeighborNodes.replace(0, node); }
    void            setRightNode(Voxel3DNode* node){ m_pNeighborNodes.replace(1, node); }
    void            setUpNode(Voxel3DNode* node){ m_pNeighborNodes.replace(2, node); }
    void            setDownNode(Voxel3DNode* node){ m_pNeighborNodes.replace(3, node); }
    void            setFrontNode(Voxel3DNode* node){ m_pNeighborNodes.replace(4, node); }
    void            setBackNode(Voxel3DNode* node){ m_pNeighborNodes.replace(5, node); }

    Voxel3DLine*    leftLine(){ return m_pNeighborLines.at(0); }
    Voxel3DLine*    rightLine(){ return m_pNeighborLines.at(1); }
    Voxel3DLine*    upLine(){ return m_pNeighborLines.at(2); }
    Voxel3DLine*    downLine(){ return m_pNeighborLines.at(3); }
    Voxel3DLine*    frontLine(){ return m_pNeighborLines.at(4); }
    Voxel3DLine*    backLine(){ return m_pNeighborLines.at(5); }
    void            setLeftLine(Voxel3DLine* line){ m_pNeighborLines.replace(0, line); }
    void            setRightLine(Voxel3DLine* line){ m_pNeighborLines.replace(1, line); }
    void            setUpLine(Voxel3DLine* line){ m_pNeighborLines.replace(2, line); }
    void            setDownLine(Voxel3DLine* line){ m_pNeighborLines.replace(3, line); }
    void            setFrontLine(Voxel3DLine* line){ m_pNeighborLines.replace(4, line); }
    void            setBackLine(Voxel3DLine* line){ m_pNeighborLines.replace(5, line); }

    void            updateLinePosition();
    void            updateLineColor(QColor color);
    void            updateLineAlpha(float a);

    // Properties
    void            updateCenterPosition(QVector3D);
    void            updateLocalPosition(QVector3D);
    void            updateLocalPositionX(float pos);
    void            updateLocalPositionY(float pos);
    void            updateLocalPositionZ(float pos);
    void            createHistoryPosition();
    void            setNodeIndex(int8_t x, int8_t y, int8_t z);
    int8_t          nodeIndexX(){ return m_indexX; }
    int8_t          nodeIndexY(){ return m_indexY; }
    int8_t          nodeIndexZ(){ return m_indexZ; }

    void            resetPosition();
    QVector3D       centerPosition(){ return *m_centerPosition; }
    QVector3D       localPosition(){ return *m_position - *m_centerPosition; }
    QVector3D       position(){return *m_position; }
    QVector3D       startPosition(){return *m_startPosition; }
    void            setStartPosition(QVector3D);

    bool            isVisible(){ return m_visible; }
    void            setVisible(bool f = true);

    bool            isSelected(){ return m_selected; }
    void            setSelected(bool f = true);

    bool            isActive(){ return m_active; }
    void            setActive(bool f = true);

    void            setReactor(int8_t reactor_index = -1);
    bool            isReactor(){ return m_reactorIndex >= 0; }
    void            removePickerComponent();

    void            undo();
    void            redo();
    void            emitClicked();
    void            emitCtrlClicked();
    void            emitShiftClicked();
signals:
    void            nodeClicked();
    void            nodeCtrlClicked();
    void            nodeShiftClicked();
private:
    void            paintColor();

public slots:

private slots:
    void            onClicked(Qt3DRender::QPickEvent*);

private:
    // Neighbor Nodes
    QList<Voxel3DNode*>                 m_pNeighborNodes; // 0: Left, 1: Right, 2: Up, 3: Down, 4: Front, 5: Back

    // Arm Lines
    QList<Voxel3DLine*>                 m_pNeighborLines; // 0: Left, 1: Right, 2: Up, 3: Down, 4: Front, 5: Back

    // Components
    Qt3DExtras::QPhongAlphaMaterial*         m_pMaterial;
    Qt3DCore::QTransform*               m_pTransform;
    Qt3DRender::QObjectPicker*          m_pPicker;

    // Properties
    QVector3D*                          m_centerPosition;
    QVector3D*                          m_position;
    QVector3D*                          m_startPosition;
    bool                                m_visible;
    bool                                m_selected;
    bool                                m_active;

    QList<QVector3D>                    m_historyPositionBefore;
    QList<QVector3D>                    m_historyPositionAfter;

    int8_t                                m_indexX;
    int8_t                                m_indexY;
    int8_t                                m_indexZ;

    int8_t                                m_reactorIndex;
};

#endif // VOXEL3DNODE_H
