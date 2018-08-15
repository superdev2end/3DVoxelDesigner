#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEntity>
#include <QUrl>
#include <Qt3DExtras>
#include <Qt3DRender>
#include <QFileDialog>
#include <QColorDialog>
#include <QSceneLoader>

#include "constants.h"
#include "voxel3dnode.h"
#include "voxel3dline.h"
#include "newscenedlg.h"
#include "voxelcloudline.h"
#include "voxelcloudnode.h"
#include "voxelbox.h"
#include "voxel3dmovingarrows.h"
#include "voxelnodemarker.h"
#include "voxeloverlayimage.h"

#define BEHIND_CAMERA QVector3D(10000000, 10000000, 10000000)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int getSelectedArrowNumber();
    void setSelectedArrowNumber(int n);
    void AdjustCuboidOfMarker(VoxelNodeMarker*);
    void RotateArrowTexts();
    void RemoveAllMarkers();
    QVector3D getCameraRightVector();

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

private:
    bool checkNodeMarkerExisted(Voxel3DNode*);
    bool isReactorCandidate(Voxel3DNode*);

private:
    Ui::MainWindow *ui;

private:
    // Root Entity
    Qt3DCore::QEntity*                  m_pRootEntity;

    // Root Entity for All Points
    Qt3DCore::QEntity*                  m_pPointRootEntity;
    Qt3DCore::QEntity*                  m_pTmpRootEntity;
    QList<Voxel3DNode*>                 m_nodeList;
    QList<Voxel3DNode*>                 m_selectedActiveNodeList;
    VoxelCloudLine*                     m_pLines;
    VoxelCloudNode*                     m_pNodes;

    // Object Entity and its Components
    Qt3DCore::QEntity*                  m_pObjEntity;
    Qt3DRender::QSceneLoader*           m_pObjSceneLoader;

    Qt3DRender::QMesh*                  m_pObjMesh;
    Qt3DExtras::QPhongMaterial*         m_pObjMaterial;
    Qt3DExtras::QPhongAlphaMaterial*    m_pObjMaterialAlpha;
    Qt3DCore::QTransform*               m_pObjTransform;
    bool                                m_hasObj;
    QQuaternion                         m_objRotation;
    float                               m_objRotX;
    float                               m_objRotY;
    float                               m_objRotZ;
    QVector3D                           m_objAxisX;
    QVector3D                           m_objAxisY;
    QVector3D                           m_objAxisZ;

    // Line Property
    QColor                              m_lineColor;
    QColor                              m_nodeColor;

    // Box Property
    QColor                              m_boxColor;
    QColor                              m_boxColor1;
    QColor                              m_boxColor2;
    QColor                              m_boxColor3;
    QColor                              m_boxColor4;
    QColor                              m_boxColor5;
    QColor                              m_boxColor6;
    QColor                              m_boxColor7;
    QColor                              m_boxColor8;

    // Work Spacing Properties
    int                                 m_workspaceSizeX;
    int                                 m_workspaceSizeY;
    int                                 m_workspaceSizeZ;
    int                                 m_spacingSize;
    Voxel3DNode*                        m_pSelectedNode;
    QList<Voxel3DNode*>                 m_selectedNodeList;
    bool                                m_isRangeSelected;

    // Reactor Nodes
    Qt3DCore::QEntity*                  m_pReactorRootEntity;
    Qt3DCore::QEntity*                  m_pReactorContentsRootEntity;
    QList<Voxel3DNode*>                 m_reactorNodeList;
    QList<Voxel3DNode*>                 m_reactorTotalNodeList;

    VoxelBox*                           m_reactorBox;
    QList<VoxelBox*>                    m_boxList;

    QList<Voxel3DNode*>                 m_historiesBefore;
    QList<Voxel3DNode*>                 m_historiesAfter;

    // Arrow
    Voxel3DMovingArrows*                m_pArrow;
    Qt3DRender::QObjectPicker*          m_pXArrowPicker;
    Qt3DRender::QObjectPicker*          m_pYArrowPicker;
    Qt3DRender::QObjectPicker*          m_pZArrowPicker;
    int                                 m_nSelectedArrowNumber;     //X -> 1, Y -> 2, Z -> 3, O -> 0
    bool                                m_bArrowHovered;
    // Node labels
    QList<VoxelNodeMarker*>             m_listNodeMarkers;

    // Scale Line
    Voxel3DCone*                        m_pScaleLine;

    // Temporary current zoom value;
    float                               m_currentZoom;

    // Arrow Released Flag (when selected node position changed)
    bool                                m_bNodePosChanged;

    QString                             m_reactor_position;

    // Overlay Image Entity;
    VoxelOverlayImage*                  m_pOverlayImage;

    // Straight Edge
    int                                 m_nStraightEdgeFlag;    // 0 -> NO, 1 -> Clicked 'Add', 2 -> One Selected
                                                                // 3 -> Both Selected , 4 -> Hide
    QList<Voxel3DNode*>                 m_StraightEdgeNodes;

    void initApplication();
    void initSlots();
    void createInitialWorkspace();
    void createInitialNodes();
    void linkNodes();
    void clearWorkSpace();
    void deleteEntityRecursively(Qt3DCore::QNode *node);
    void displaySelectedNodeData();
    void MoveMarker(Voxel3DNode*);
    void MoveReactorNodeByEdit(int nValue, int nPoint, int xyz);
    void enableOverlayControls(bool fEnabled);
    void rotateOverlayImage();
    void nodeClickForStraight(Voxel3DNode* pNode);

private slots:
    // Button event's handler
    void onNew_triggered();
    void onSave_triggered();
    void onOpen_triggered();
    void onImportOBJ_triggered();
    void onObjSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status);
    void mapObjSceneMaterial(Qt3DCore::QEntity* entity);
    void onClearOBJ_triggered();
    void onMoveLight_triggered();

    // Line
    void onChooseLineColor_triggered();
    void onLineAlphaChanged(int k);

    // Object Controller
    void enableObjController();
    void initObjParameters();
    // => scale
    void objScaleChanged(int k);
    // => transparent
    void objTransparentPropertyChanged(bool f);
    void objOpacityChanged(int k);
    // => move
    void objPositionChanged();
    void objPositionXChanged(int k);
    void objPositionYChanged(int k);
    void objPositionZChanged(int k);
    // => rotate
    void objRotationXChanged(int k);
    void objRotationYChanged(int k);
    void objRotationZChanged(int k);

    // Node Controller
    void enableNodeController();
    void initNodeParameters();
    void nodePositionXChanged(int k);
    void nodePositionYChanged(int k);
    void nodePositionZChanged(int k);
    void nodePositionReleased();
    void onNodeClicked();
    void onNodeCtrlClicked();
    void onNodeShiftClicked();
    void updateSelectionCloudNodes();
    void onResetPoint_triggered();
    void nodeVisiblePropertyChanged(bool);
    void changeNodeVisibleProperty(Voxel3DNode* node);
    void onUndo();
    void onRedo();
    void onSelectTransparent();
    void onUpOpacity();
    void onDownOpacity();
    void onTriggerShowHide();

    // View's Handler
    void onViewTop_triggered();
    void onViewBottom_triggered();
    void onViewFront_triggered();
    void onViewBack_triggered();
    void onViewLeft_triggered();
    void onViewRight_triggered();
    void onViewDefault_triggered();
    void onViewScale_pressed();
    void onViewScale_released();

    // Reactor Guide
    void updateReactorNodes();
    void createReactorView();
    void updateReactorView();
    void onSelectReactorGuide_triggered(bool f);
    void onSelectReactorMode1_triggered();
    void onSelectReactorMode2_triggered();
    void onSelectReactorMode3_triggered();
    void onSelectReactorMode4_triggered();
    void onSelectReactorMode5_triggered();
    void onSelectReactorMode6_triggered();
    void onSelectReactorMode7_triggered();
    void onSelectReactorMode8_triggered();
    void copyReactorPointsToClipboard();
    // Box
    void onBoxColor_triggered();
    void onChooseBoxColor_triggered();
    void onClearBoxColor_triggered();
    void onUpdateBox();
    void updateBoxColorUI();

    // Transfer Points
    void onCopyPoints_triggered();
    void onPastePoints_triggered();

    void onVoxelClicked(int index_x, int index_y, int index_z, QVector3D pos);
    void onVoxelCtrlClicked(int index_x, int index_y, int index_z, QVector3D pos);
    void onVoxelShiftClicked(int index_x, int index_y, int index_z, QVector3D pos);

    // Arrow Mouse Event Slots
    void onXArrowPressed(Qt3DRender::QPickEvent*);
    void onXArrowEntered();
    void onXArrowExited();
    void onYArrowPressed(Qt3DRender::QPickEvent*);
    void onYArrowEntered();
    void onYArrowExited();
    void onZArrowPressed(Qt3DRender::QPickEvent*);
    void onZArrowEntered();
    void onZArrowExited();
    void onArrowMoving_changed(QPoint);
    void onMarkerMoving_changed(QPoint);
    void onZoomChanged(float);

    void onMousePressed();
    void onMouseRelease();
    void onMouseMove();
    // Markers Rotating Event Slots
    void onMarkersRotating_changed();

    // Markers Slots
    void nodeMarkerVisibleChanged(int);
    void nodeMarkerTextChanged();
    void onNodeMarkerPressed(Voxel3DNode*);

    // Scale Line
    void onScaleLineAdd_triggered();
    void onScaleLineRemove_triggered();

    void on_edtPointPosX_editingFinished();
    void on_edtPointPosY_editingFinished();
    void on_edtPointPosZ_editingFinished();

    void on_edtCoor1X_editingFinished();
    void on_edtCoor1Y_editingFinished();
    void on_edtCoor1Z_editingFinished();

    void on_edtCoor2X_editingFinished();
    void on_edtCoor2Y_editingFinished();
    void on_edtCoor2Z_editingFinished();

    void on_edtCoor3X_editingFinished();
    void on_edtCoor3Y_editingFinished();
    void on_edtCoor3Z_editingFinished();

    void on_edtCoor4X_editingFinished();
    void on_edtCoor4Y_editingFinished();
    void on_edtCoor4Z_editingFinished();

    void on_edtCoor5X_editingFinished();
    void on_edtCoor5Y_editingFinished();
    void on_edtCoor5Z_editingFinished();

    void on_edtCoor6X_editingFinished();
    void on_edtCoor6Y_editingFinished();
    void on_edtCoor6Z_editingFinished();

    void on_edtCoor7X_editingFinished();
    void on_edtCoor7Y_editingFinished();
    void on_edtCoor7Z_editingFinished();

    void on_edtCoor8X_editingFinished();
    void on_edtCoor8Y_editingFinished();
    void on_edtCoor8Z_editingFinished();

    void on_chkSelectedPointsActive_toggled(bool checked);
    void on_btnGotoOverlay_clicked();
    void on_btnBack_clicked();
    void on_btnOverlayOpen_clicked();
    void on_btnOverlayClose_clicked();
    void on_sldOverlayOpacity_valueChanged(int value);
    void on_edtOverlayOpacity_editingFinished();
    void on_sldOverlayScale_valueChanged(int value);
    void on_edtOverlayScale_editingFinished();
    void on_sldOverlayRotate_valueChanged(int value);
    void on_edtOverlayRotate_editingFinished();
    void on_sldOverlayHorizontal_valueChanged(int value);
    void on_edtOverlayHorizontal_editingFinished();
    void on_sldOverlayVertical_valueChanged(int value);
    void on_edtOverlayVertical_editingFinished();
    void on_btnStraightEdgeAdd_clicked();
    void on_btnStraightEdgeRemove_clicked();
    void on_chkStraightEdgeShow_toggled(bool checked);
};

#endif // MAINWINDOW_H
