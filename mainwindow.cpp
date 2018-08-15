#include "mainwindow.h"
#include "ui_mainwindow.h"

///////////////////////////////////////////
///                                     ///
///             ^                       ///
///      Up     | Y                     ///
///       ^     |                       ///
///       v     |                       ///
///     Down    |                       ///
///             |         Left<->Right  ///
///             O-------------->        ///
///            /            X           ///
///    Back   /                         ///
///     /    /                          ///
///  Front  /  Z                        ///
///        v                            ///
///                                     ///
/// ///////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sldGridOpacity->setVisible(false);
    ui->lblGridOpacity->setVisible(false);

    initApplication();

    createInitialWorkspace();
    createInitialNodes();
    linkNodes();
    initSlots();

    // Reactor Guide
    ui->btnSelectReactorGuide->setChecked(false);
    createReactorView();
    onSelectReactorGuide_triggered(false);

    ui->widMainVoxelView->setMouseTracking(true);

    m_objRotX = m_objRotY = m_objRotZ = 0;
    m_objAxisX = QVector3D(1.0, 0.0, 0.0);
    m_objAxisY = QVector3D(0.0, 1.0, 0.0);
    m_objAxisZ = QVector3D(0.0, 0.0, 1.0);

}

MainWindow::~MainWindow()
{
    deleteEntityRecursively(m_pRootEntity);
    //    m_nodeList.clear();
    delete m_pArrow;

    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Pressed a key";

    if(event->key() == Qt::Key_Alt) {
        // Alt -> Click Select Button
        emit ui->btnSelectReactorGuide->toggled(true);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "Released a key";
    if(event->matches(QKeySequence::Undo))
    {
        // Ctrl + Z
        onUndo();
    } else if(event->matches(QKeySequence::Redo) || (event->key() == Qt::Key_R && event->modifiers() == Qt::ControlModifier))
    {
        // Shift + Ctrl + Z, Ctrl + R
        onRedo();
    } else if(event->key() == Qt::Key_T && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + T
        onSelectTransparent();
    } else if(event->key() == Qt::Key_Up) {
        // UpArrow
        onUpOpacity();
    } else if(event->key() == Qt::Key_Down) {
        // DownArrow
        onDownOpacity();
    } else if(event->key() == Qt::Key_1 && event->modifiers() == Qt::ControlModifier) {
        // Control + 1
        onViewTop_triggered();
    } else if(event->key() == Qt::Key_2 && event->modifiers() == Qt::ControlModifier) {
        // Control + 2
        onViewBottom_triggered();
    } else if(event->key() == Qt::Key_3 && event->modifiers() == Qt::ControlModifier) {
        // Control + 3
        onViewRight_triggered();
    } else if(event->key() == Qt::Key_4 && event->modifiers() == Qt::ControlModifier) {
        // Control + 4
        onViewLeft_triggered();
    } else if(event->key() == Qt::Key_5 && event->modifiers() == Qt::ControlModifier) {
        // Control + 5
        onViewFront_triggered();
    } else if(event->key() == Qt::Key_6 && event->modifiers() == Qt::ControlModifier) {
        // Control + 6
        onViewBack_triggered();
    } else if(event->key() == Qt::Key_7 && event->modifiers() == Qt::ControlModifier) {
        // Control + 7
        onViewDefault_triggered();
    } else if(event->key() == Qt::Key_L && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + L
        onTriggerShowHide();
    } else if(event->key() == Qt::Key_R && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + R
        onResetPoint_triggered();
    } else if(event->key() == Qt::Key_C && event->modifiers() == Qt::ShiftModifier) {
        // Shift + C
        onClearBoxColor_triggered();
    } else if(event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier) {
        // Ctrl + C
        onChooseBoxColor_triggered();
    } else if(event->key() == Qt::Key_Alt) {
        // Alt -> Click Select Button
        emit ui->btnSelectReactorGuide->toggled(false);
    }
}

void MainWindow::initApplication()
{
    qDebug() << ">> InitApplication";
    // Initialize workspace property
    m_workspaceSizeX = 10;
    m_workspaceSizeY = 10;
    m_workspaceSizeZ = 10;
    m_spacingSize = 85;

    //============== Default Line Color =================//
    ui->sldGridOpacity->setValue(80);

    m_reactorBox = nullptr;
    m_boxList.clear();

    m_pLines = nullptr;
    m_lineColor = Qt::blue;
    m_pNodes = nullptr;
    m_nodeColor = Qt::red;

    QString buttonStyle = QString("QPushButton{ \
                                  background-color: %1; \
            border:none; \
    }\
    QPushButton:hover{ \
        background-color: %2; \
    border:none; \
    } \
    QPushButton:pressed{ \
        background-color: %3; \
    border:none; \
    }").arg(m_lineColor.name(), m_lineColor.lighter(115).name(), m_lineColor.lighter(120).name());
    ui->btnLineColor->setStyleSheet(buttonStyle);
    ////////////////////////////////////////////////////////

    m_boxColor1 = QColor(255, 153, 0);
    m_boxColor2 = QColor(102, 255, 51);
    m_boxColor3 = QColor(51, 153, 255);
    m_boxColor4 = QColor(153, 0, 255);
    m_boxColor5 = QColor(0, 153, 51);
    m_boxColor6 = QColor(102, 51, 0);
    m_boxColor7 = QColor(204, 102, 153);
    m_boxColor8 = QColor(153, 102, 0);
    m_boxColor = m_boxColor1;
    updateBoxColorUI();

    m_nodeList.clear();
    m_reactorNodeList.clear();
    m_reactorTotalNodeList.clear();
    m_pSelectedNode = nullptr;
    m_pReactorRootEntity = nullptr;
    m_pReactorContentsRootEntity = nullptr;
    m_isRangeSelected = false;

    //============= Object ==================//
    m_hasObj = false;

    // Create Root Entity
    m_pRootEntity = new Qt3DCore::QEntity();
    ui->widMainVoxelView->setupRootEntity(m_pRootEntity);
    connect(ui->widMainVoxelView, SIGNAL(undo()), this, SLOT(onUndo()));
    connect(ui->widMainVoxelView, SIGNAL(redo()), this, SLOT(onRedo()));
    connect(ui->widMainVoxelView, SIGNAL(selectTransparent()), this, SLOT(onSelectTransparent()));
    connect(ui->widMainVoxelView, SIGNAL(upOpacity()), this, SLOT(onUpOpacity()));
    connect(ui->widMainVoxelView, SIGNAL(downOpacity()), this, SLOT(onDownOpacity()));
    connect(ui->widMainVoxelView, SIGNAL(topView()), this, SLOT(onViewTop_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(bottomView()), this, SLOT(onViewBottom_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(rightView()), this, SLOT(onViewRight_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(leftView()), this, SLOT(onViewLeft_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(frontView()), this, SLOT(onViewFront_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(backView()), this, SLOT(onViewBack_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(defaultView()), this, SLOT(onViewDefault_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(triggerShowHide()), this, SLOT(onTriggerShowHide()));
    connect(ui->widMainVoxelView, SIGNAL(triggerResetPoint()), this, SLOT(onResetPoint_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(triggerColorVoxel()), this, SLOT(onChooseBoxColor_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(triggerClearColor()), this, SLOT(onClearBoxColor_triggered()));
    connect(ui->widMainVoxelView, SIGNAL(triggerSelectReactorGuide(bool)), this, SLOT(onSelectReactorGuide_triggered(bool)));
    // Create 3D Object Entity
    m_pObjEntity = new Qt3DCore::QEntity(m_pRootEntity);

    // Mesh of Obj
//    m_pObjMesh = new Qt3DRender::QMesh(m_pRootEntity);
    m_pObjSceneLoader = new Qt3DRender::QSceneLoader(m_pRootEntity);

    // Material of Obj
    m_pObjMaterial = new Qt3DExtras::QPhongMaterial(m_pRootEntity);
    m_pObjMaterial->setAmbient(QColor(250, 0, 0)); // Qt::gray
//    m_pObjMaterial->setDiffuse(QColor(100, 100, 100));
    m_pObjMaterial->setDiffuse(Qt::blue);

    m_pObjMaterialAlpha = new Qt3DExtras::QPhongAlphaMaterial(m_pRootEntity);
    m_pObjMaterialAlpha->setAlpha(0.8f);
    m_pObjMaterialAlpha->setAmbient(QColor(250, 0, 0)); //Qt::gray
//    m_pObjMaterialAlpha->setDiffuse(QColor(100, 100, 100));
    m_pObjMaterialAlpha->setDiffuse(Qt::blue);

    // Transform of Obj
    m_pObjTransform = new Qt3DCore::QTransform();

    // Add Components to Obj Entity
//    m_pObjEntity->addComponent(m_pObjMesh);
    m_pObjEntity->addComponent(m_pObjSceneLoader);
    m_pObjEntity->addComponent(m_pObjTransform);
    m_pObjEntity->addComponent(ui->widMainVoxelView->getOpaqueLayer());
    objTransparentPropertyChanged(false);

    // Reactor Entity
    m_pReactorRootEntity = new Qt3DCore::QEntity();
    ui->widReactorView->setupRootEntity(m_pReactorRootEntity);

    m_pArrow = new Voxel3DMovingArrows(m_pRootEntity);
    m_pArrow->setArrowLength(m_spacingSize, m_workspaceSizeY);
    m_pArrow->setZoom(ui->widMainVoxelView->getZoom());
    m_pArrow->MoveArrows(BEHIND_CAMERA);

    // Picker Components
    m_pXArrowPicker = new Qt3DRender::QObjectPicker(m_pRootEntity);
    m_pXArrowPicker->setDragEnabled(true);
    m_pXArrowPicker->setHoverEnabled(true);
    QObject::connect(m_pXArrowPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(onXArrowPressed(Qt3DRender::QPickEvent*)));
    QObject::connect(m_pXArrowPicker, SIGNAL(entered()), this, SLOT(onXArrowEntered()));
    QObject::connect(m_pXArrowPicker, SIGNAL(exited()), this, SLOT(onXArrowExited()));
    m_pArrow->getXConeArrow()->addComponent(m_pXArrowPicker);

    m_pYArrowPicker = new Qt3DRender::QObjectPicker(m_pRootEntity);
    m_pYArrowPicker->setDragEnabled(true);
    m_pYArrowPicker->setHoverEnabled(true);
    QObject::connect(m_pYArrowPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(onYArrowPressed(Qt3DRender::QPickEvent*)));
    QObject::connect(m_pYArrowPicker, SIGNAL(entered()), this, SLOT(onYArrowEntered()));
    QObject::connect(m_pYArrowPicker, SIGNAL(exited()), this, SLOT(onYArrowExited()));
    m_pArrow->getYConeArrow()->addComponent(m_pYArrowPicker);

    m_pZArrowPicker = new Qt3DRender::QObjectPicker(m_pRootEntity);
    m_pZArrowPicker->setDragEnabled(true);
    m_pZArrowPicker->setHoverEnabled(true);
    QObject::connect(m_pZArrowPicker, SIGNAL(pressed(Qt3DRender::QPickEvent*)), this, SLOT(onZArrowPressed(Qt3DRender::QPickEvent*)));
    QObject::connect(m_pZArrowPicker, SIGNAL(entered()), this, SLOT(onZArrowEntered()));
    QObject::connect(m_pZArrowPicker, SIGNAL(exited()), this, SLOT(onZArrowExited()));
    m_pArrow->getZConeArrow()->addComponent(m_pZArrowPicker);

    // Scale Line
    m_pScaleLine = new Voxel3DCone(m_pRootEntity);
    m_pScaleLine->getTransform()->setTranslation(BEHIND_CAMERA);
    m_bArrowHovered = false;

    // Overlay Image Init
    m_pOverlayImage = nullptr;

    ui->edtScaleLength->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtPointPosX->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtPointPosY->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtPointPosZ->setValidator(new QIntValidator(-65535, 65535, this));

//    ui->widSelectedCoordinates->setVisible(false);
    ui->edtCoor1X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor1Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor1Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor2X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor2Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor2Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor3X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor3Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor3Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor4X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor4Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor4Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor5X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor5Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor5Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor6X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor6Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor6Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor7X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor7Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor7Z->setValidator(new QIntValidator(-65535, 65535, this));

    ui->edtCoor8X->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor8Y->setValidator(new QIntValidator(-65535, 65535, this));
    ui->edtCoor8Z->setValidator(new QIntValidator(-65535, 65535, this));

    // Overlay Image
    ui->edtOverlayOpacity->setValidator(new QIntValidator(0, 100, this));
    ui->edtOverlayScale->setValidator(new QIntValidator(0, 400, this));
    ui->edtOverlayRotate->setValidator(new QIntValidator(-180, 180, this));
    ui->edtOverlayHorizontal->setValidator(new QIntValidator(-500, 500, this));
    ui->edtOverlayVertical->setValidator(new QIntValidator(-500, 500, this));


    m_nStraightEdgeFlag = 0;

    qDebug() << "<< InitApplication";
}

void MainWindow::initSlots()
{
    qDebug() << ">> InitSlots";
    // Button's event-handler
    connect(ui->btnNew, SIGNAL(released()), this, SLOT(onNew_triggered()));
    connect(ui->btnOpen, SIGNAL(released()), this, SLOT(onOpen_triggered()));
    connect(ui->btnSave, SIGNAL(released()), this, SLOT(onSave_triggered()));
    connect(ui->btnImportOBJ, SIGNAL(released()), this, SLOT(onImportOBJ_triggered()));
    connect(ui->btnClearOBJ, SIGNAL(released()), this, SLOT(onClearOBJ_triggered()));
    connect(ui->btnMoveLight, SIGNAL(released()), this, SLOT(onMoveLight_triggered()));

    // Line
    connect(ui->btnLineColor, SIGNAL(released()), this, SLOT(onChooseLineColor_triggered()));
    connect(ui->sldGridOpacity, SIGNAL(valueChanged(int)), this, SLOT(onLineAlphaChanged(int)));

    // Box
    connect(ui->btn_ChooseBoxColor, SIGNAL(released()), this, SLOT(onChooseBoxColor_triggered()));
    connect(ui->btnBoxColor, SIGNAL(released()), this, SLOT(onBoxColor_triggered()));
    connect(ui->btnClearBoxColor, SIGNAL(released()), this, SLOT(onClearBoxColor_triggered()));

    // Object Controller
    connect(ui->spnObjScale, SIGNAL(valueChanged(int)), this, SLOT(objScaleChanged(int)));
    connect(ui->chkOBJTransparent, SIGNAL(toggled(bool)), this, SLOT(objTransparentPropertyChanged(bool)));
    connect(ui->sldOBJOpacity, SIGNAL(valueChanged(int)), this, SLOT(objOpacityChanged(int)));
    connect(ui->spnObjOpacity, SIGNAL(valueChanged(int)), this, SLOT(objOpacityChanged(int)));
    // => move
    connect(ui->sldOBJPosX, SIGNAL(valueChanged(int)), this, SLOT(objPositionXChanged(int)));
    connect(ui->sldOBJPosY, SIGNAL(valueChanged(int)), this, SLOT(objPositionYChanged(int)));
    connect(ui->sldOBJPosZ, SIGNAL(valueChanged(int)), this, SLOT(objPositionZChanged(int)));
    connect(ui->spnOBJPosX, SIGNAL(valueChanged(int)), this, SLOT(objPositionXChanged(int)));
    connect(ui->spnOBJPosY, SIGNAL(valueChanged(int)), this, SLOT(objPositionYChanged(int)));
    connect(ui->spnOBJPosZ, SIGNAL(valueChanged(int)), this, SLOT(objPositionZChanged(int)));
    // => rotate
    connect(ui->dialOBJRotationX,SIGNAL(valueChanged(int)),this, SLOT(objRotationXChanged(int)));
    connect(ui->dialOBJRotationY,SIGNAL(valueChanged(int)),this, SLOT(objRotationYChanged(int)));
    connect(ui->dialOBJRotationZ,SIGNAL(valueChanged(int)),this, SLOT(objRotationZChanged(int)));
    connect(ui->spnOBJRotationX,SIGNAL(valueChanged(int)),this, SLOT(objRotationXChanged(int)));
    connect(ui->spnOBJRotationY,SIGNAL(valueChanged(int)),this, SLOT(objRotationYChanged(int)));
    connect(ui->spnOBJRotationZ,SIGNAL(valueChanged(int)),this, SLOT(objRotationZChanged(int)));

    // Node Controller
    connect(ui->sldNodePosX, SIGNAL(valueChanged(int)), this, SLOT(nodePositionXChanged(int)));
    connect(ui->sldNodePosY, SIGNAL(valueChanged(int)), this, SLOT(nodePositionYChanged(int)));
    connect(ui->sldNodePosZ, SIGNAL(valueChanged(int)), this, SLOT(nodePositionZChanged(int)));
    connect(ui->sldNodePosX, SIGNAL(sliderReleased()), this, SLOT(nodePositionReleased()));
    connect(ui->sldNodePosY, SIGNAL(sliderReleased()), this, SLOT(nodePositionReleased()));
    connect(ui->sldNodePosZ, SIGNAL(sliderReleased()), this, SLOT(nodePositionReleased()));

    connect(ui->btnResetPoint, SIGNAL(released()), this, SLOT(onResetPoint_triggered()));
    connect(ui->chkShowPoint, SIGNAL(toggled(bool)), this, SLOT(nodeVisiblePropertyChanged(bool)));
    connect(ui->chkAddMarker, SIGNAL(stateChanged(int)), this, SLOT(nodeMarkerVisibleChanged(int)));
    connect(ui->edtMarkerName, SIGNAL(editingFinished()), this, SLOT(nodeMarkerTextChanged()));

    // View Controller
    connect(ui->btnViewTop, SIGNAL(released()), this, SLOT(onViewTop_triggered()));
    connect(ui->btnViewBottom, SIGNAL(released()), this, SLOT(onViewBottom_triggered()));
    connect(ui->btnViewFront, SIGNAL(released()), this, SLOT(onViewFront_triggered()));
    connect(ui->btnViewBack, SIGNAL(released()), this, SLOT(onViewBack_triggered()));
    connect(ui->btnViewLeft, SIGNAL(released()), this, SLOT(onViewLeft_triggered()));
    connect(ui->btnViewRight, SIGNAL(released()), this, SLOT(onViewRight_triggered()));
    connect(ui->btnViewDefault, SIGNAL(released()), this, SLOT(onViewDefault_triggered()));
    connect(ui->btnViewScale, SIGNAL(pressed()), this, SLOT(onViewScale_pressed()));
    connect(ui->btnViewScale, SIGNAL(released()), this, SLOT(onViewScale_released()));

    // Reactor Guide
    connect(ui->btnSelectReactorGuide, SIGNAL(toggled(bool)), this, SLOT(onSelectReactorGuide_triggered(bool)));
    connect(ui->btnSelectReactorMode1, SIGNAL(released()), this, SLOT(onSelectReactorMode1_triggered()));
    connect(ui->btnSelectReactorMode2, SIGNAL(released()), this, SLOT(onSelectReactorMode2_triggered()));
    connect(ui->btnSelectReactorMode3, SIGNAL(released()), this, SLOT(onSelectReactorMode3_triggered()));
    connect(ui->btnSelectReactorMode4, SIGNAL(released()), this, SLOT(onSelectReactorMode4_triggered()));
    connect(ui->btnSelectReactorMode5, SIGNAL(released()), this, SLOT(onSelectReactorMode5_triggered()));
    connect(ui->btnSelectReactorMode6, SIGNAL(released()), this, SLOT(onSelectReactorMode6_triggered()));
    connect(ui->btnSelectReactorMode7, SIGNAL(released()), this, SLOT(onSelectReactorMode7_triggered()));
    connect(ui->btnSelectReactorMode8, SIGNAL(released()), this, SLOT(onSelectReactorMode8_triggered()));
    connect(ui->chkNegetiveCopier, SIGNAL(toggled(bool)), this, SLOT(updateReactorView()));
    connect(ui->btnCopyReactorPoints, SIGNAL(released()), this, SLOT(copyReactorPointsToClipboard()));
    // Transfer Points
    connect(ui->btnCopyPoints, SIGNAL(released()), this, SLOT(onCopyPoints_triggered()));
    connect(ui->btnPastePoints, SIGNAL(released()), this, SLOT(onPastePoints_triggered()));

    // Arrow Moving Signal
    connect(ui->widMainVoxelView, SIGNAL(ArrowMoving(QPoint)), this, SLOT(onArrowMoving_changed(QPoint)));
    connect(ui->widMainVoxelView, SIGNAL(zoomChanged(float)), this, SLOT(onZoomChanged(float)));
    connect(ui->widMainVoxelView, SIGNAL(MarkerMoving(QPoint)), this, SLOT(onMarkerMoving_changed(QPoint)));

    connect(ui->widMainVoxelView, SIGNAL(MousePressed()), this, SLOT(onMousePressed()));
    connect(ui->widMainVoxelView, SIGNAL(MouseRelease()), this, SLOT(onMouseRelease()));
    connect(ui->widMainVoxelView, SIGNAL(MouseMove()), this, SLOT(onMouseMove()));

    // Markers Rotating Signal
    connect(ui->widMainVoxelView, SIGNAL(MarkersRotating()), this, SLOT(onMarkersRotating_changed()));

    // Scale Line
    connect(ui->btnScaleLineAdd, SIGNAL(released()), this, SLOT(onScaleLineAdd_triggered()));
    connect(ui->btnScaleLineRemove, SIGNAL(released()), this, SLOT(onScaleLineRemove_triggered()));

    qDebug() << "<< InitSlots";
}

void MainWindow::clearWorkSpace()
{
    qDebug() << ">> ClearWorkSpace";
    m_pSelectedNode = nullptr;
    m_isRangeSelected = false;
    deleteEntityRecursively(m_pTmpRootEntity);
    deleteEntityRecursively(m_pPointRootEntity);
    m_nodeList.clear();

    deleteEntityRecursively(m_pReactorContentsRootEntity);
    m_reactorNodeList.clear();
    m_reactorTotalNodeList.clear();

    m_pLines = nullptr;
    m_pNodes = nullptr;
    if(m_hasObj){
//        m_pObjEntity->removeComponent(m_pObjMesh);
        m_pObjEntity->removeComponent(m_pObjSceneLoader);
        m_hasObj = false;
        enableObjController();
    }
    ui->widReactorView->setCameraOff();
//    ui->widSelectedCoordinates->setVisible(false);
    enableNodeController();

    m_historiesBefore.clear();
    m_historiesAfter.clear();

    ui->widMainVoxelView->BuildCustomFrameGraphRenderer();
    qDebug() << "<< ClearWorkSpace";
}

void MainWindow::deleteEntityRecursively(Qt3DCore::QNode *node){
    Qt3DCore::QEntity* entity = dynamic_cast<Qt3DCore::QEntity*>(node);
    if(entity == nullptr){
        return;
    }
    auto components_to_delete = entity->components();

    foreach(auto *component,components_to_delete){
        if(component != nullptr){
            entity->removeComponent(component);
            component->deleteLater();
            component = nullptr;
        }
    }
    auto children_nodes = entity->childNodes();
    foreach(auto *child_node, children_nodes){
        deleteEntityRecursively(child_node);
    }

    entity->deleteLater();
    entity = nullptr;
}

void MainWindow::onNew_triggered(){
    // Open New-Dialog
    qDebug() << ">> New";
    on_btnOverlayClose_clicked();               //Remove old Overlay image
    NewSceneDlg dlg(this);
    if(dlg.exec() == QDialog::Accepted){
        m_workspaceSizeX = dlg.getCubeSizeX();
        m_workspaceSizeY = dlg.getCubeSizeY();
        m_workspaceSizeZ = dlg.getCubeSizeZ();
        m_spacingSize = dlg.getSpacingSize();
        int nMaxObjMovement = dlg.getObjMovementField();
        ui->sldOBJPosX->setMinimum(-nMaxObjMovement);
        ui->sldOBJPosX->setMaximum(nMaxObjMovement);
        ui->sldOBJPosY->setMinimum(-nMaxObjMovement);
        ui->sldOBJPosY->setMaximum(nMaxObjMovement);
        ui->sldOBJPosZ->setMinimum(-nMaxObjMovement);
        ui->sldOBJPosZ->setMaximum(nMaxObjMovement);

        clearWorkSpace();
        createInitialWorkspace();
        createInitialNodes();
        linkNodes();

        // Reactor Guide
        ui->btnSelectReactorGuide->setChecked(false);
        createReactorView();
        onSelectReactorGuide_triggered(false);

        m_pArrow->setArrowLength(m_spacingSize, m_workspaceSizeY);
        m_pArrow->setZoom(ui->widMainVoxelView->getZoom());
        m_pArrow->MoveArrows(BEHIND_CAMERA);
        m_pArrow->getXText()->getTextMesh()->setText("X " + QString::number(0));
        m_pArrow->getYText()->getTextMesh()->setText("Y " + QString::number(0));
        m_pArrow->getZText()->getTextMesh()->setText("Z " + QString::number(0));
        RemoveAllMarkers();
        m_pScaleLine->getTransform()->setTranslation(BEHIND_CAMERA);
    }
}

void MainWindow::onSave_triggered(){
    // Open Save-Dialog
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save File"),
                nullptr,
                tr("Voxel3D Files (*.vl3)"),
                nullptr,
                nullptr);

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);

        if(file.isOpen())
        {
            QTextStream outStream(&file);
            outStream << m_workspaceSizeX << " " <<
                         m_workspaceSizeY << " " <<
                         m_workspaceSizeZ << endl;
            outStream << m_spacingSize << endl;
            outStream << m_nodeList.count() << endl;

            foreach(Voxel3DNode* node, m_nodeList){
                outStream << node->isVisible() << " " <<
                             node->localPosition().x() << " " <<
                             node->localPosition().y() << " " <<
                             node->localPosition().z() << " " << endl;
            }

            outStream << m_boxColor1.red() << " " << m_boxColor1.green() << " " << m_boxColor1.blue() << endl;
            outStream << m_boxColor2.red() << " " << m_boxColor2.green() << " " << m_boxColor2.blue() << endl;
            outStream << m_boxColor3.red() << " " << m_boxColor3.green() << " " << m_boxColor3.blue() << endl;
            outStream << m_boxColor4.red() << " " << m_boxColor4.green() << " " << m_boxColor4.blue() << endl;
            outStream << m_boxColor5.red() << " " << m_boxColor5.green() << " " << m_boxColor5.blue() << endl;
            outStream << m_boxColor6.red() << " " << m_boxColor6.green() << " " << m_boxColor6.blue() << endl;
            outStream << m_boxColor7.red() << " " << m_boxColor6.green() << " " << m_boxColor6.blue() << endl;
            outStream << m_boxColor8.red() << " " << m_boxColor6.green() << " " << m_boxColor6.blue() << endl;

            int cnt_box = 0;
            for(int i = 0 ; i < m_boxList.count(); i++){
                if(m_boxList.at(i)->getColor() != Qt::transparent){
                    if(m_boxList.at(i)->getNodeIndexList().size() == 8){
                        cnt_box++;
                    }
                }
            }
            outStream << cnt_box << endl;
            qDebug() << "==============================";
            qDebug() << "Count of box : " << cnt_box;

            for(int i = 0 ; i < cnt_box; i++){
                if(m_boxList.at(i)->getColor() != Qt::transparent){
                    if(m_boxList.at(i)->getNodeIndexList().size() == 8){
                        outStream << m_boxList.at(i)->getColor().red() << " " <<
                                     m_boxList.at(i)->getColor().green() << " " <<
                                     m_boxList.at(i)->getColor().blue() << endl;

                        for(int j = 0 ; j < 8; j++){
                            outStream << m_boxList.at(i)->getNodeIndexList().at(j).x() << " "
                                      << m_boxList.at(i)->getNodeIndexList().at(j).y() << " "
                                      << m_boxList.at(i)->getNodeIndexList().at(j).z() << endl;

                        }
                    }
                }
            }

            outStream << m_listNodeMarkers.count() << endl;
            foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
                outStream << pNodeMarker->getMarkerText()->text().toUtf8().toBase64() << " " <<
                             (int)(pNodeMarker->getMarkerTransform()->translation().x()) << " " <<
                             (int)(pNodeMarker->getMarkerTransform()->translation().y()) << " " <<
                             (int)(pNodeMarker->getMarkerTransform()->translation().z()) << " " <<
                             pNodeMarker->getNode()->nodeIndexX() << " " <<
                             pNodeMarker->getNode()->nodeIndexY() << " " <<
                             pNodeMarker->getNode()->nodeIndexZ() << " " << endl;
            }

            if(m_hasObj){
//                outStream << m_pObjMesh->source().url(QUrl::PreferLocalFile) << endl;
                outStream << m_pObjSceneLoader->source().url(QUrl::PreferLocalFile) << endl;
                outStream << ui->spnObjScale->value() << " " <<
                             ui->dialOBJRotationX->value() << " " <<
                             ui->dialOBJRotationY->value() << " " <<
                             ui->dialOBJRotationZ->value() << " " <<
                             ui->sldOBJPosX->value() << " " <<
                             ui->sldOBJPosY->value() << " " <<
                             ui->sldOBJPosZ->value() << " " <<
                             ui->sldOBJOpacity->value() << endl;
            }

            file.close();
        }
    }
}

void MainWindow::onOpen_triggered(){
    on_btnOverlayClose_clicked();       //Remove old Overlay image
    // Open Open-Dialog
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                nullptr,
                tr("Voxel3D Files (*.vl3)"),
                nullptr,
                nullptr);


    if (!fileName.isEmpty()){
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);

        if(file.isOpen())
        {
            clearWorkSpace();
            qDebug() << "OPEN VL3";
            int cnt, vsble, local_x, local_y, local_z;

            QTextStream inStream(&file);
            inStream >> m_workspaceSizeX >>
                    m_workspaceSizeY >>
                    m_workspaceSizeZ;
            inStream >> m_spacingSize;
            inStream >> cnt;

            createInitialWorkspace();

            for (int x = 0; x < m_workspaceSizeX; x++) {
                for (int y = 0; y < m_workspaceSizeY; y++) {
                    for( int z = 0; z < m_workspaceSizeZ; z++){
                        inStream >> vsble >>
                                local_x >>
                                local_y >>
                                local_z;

                        Voxel3DNode* node = new Voxel3DNode(m_pTmpRootEntity);

                        QVector3D pos = (QVector3D(x , y, z) - QVector3D(static_cast<float>(m_workspaceSizeX - 1) / 2.0f,
                                                                         static_cast<float>(m_workspaceSizeY - 1) / 2.0f,
                                                                         static_cast<float>(m_workspaceSizeZ - 1) / 2.0f)) * m_spacingSize;
                        node->setNodeIndex(static_cast<int8_t>(x),
                                           static_cast<int8_t>(y),
                                           static_cast<int8_t>(z));
                        node->updateCenterPosition(pos);
                        node->updateLocalPosition(QVector3D(local_x, local_y, local_z));

                        connect(node, SIGNAL(nodeClicked()), this, SLOT(onNodeClicked()));
                        connect(node, SIGNAL(nodeCtrlClicked()), this, SLOT(onNodeCtrlClicked()));
                        connect(node, SIGNAL(nodeShiftClicked()), this, SLOT(onNodeShiftClicked()));

                        node->setVisible(vsble);

                        m_nodeList.push_back(node);
                    }
                }
            }

            // Reactor Color Table
            int r, g, b;
            QString colorStyle;

            inStream >> r >> g >> b;
            m_boxColor1.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor2.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor3.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor4.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor5.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor6.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor7.setRgb(r, g, b);
            inStream >> r >> g >> b;
            m_boxColor8.setRgb(r, g, b);
            updateBoxColorUI();

            // Reactor Boxes
            m_boxList.clear();
            int cnt_box;
            inStream >> cnt_box;
            qDebug() << "Boxes : " << cnt_box;
            for(int i = 0 ; i < cnt_box; i++){
                qDebug() << "Load Boxe" << i;
                inStream >> r >> g >> b;
                QColor color(r, g, b);
                QList<Voxel3DNode*> nodeList;
                for(int j = 0 ; j < 8 ; j++){
                    int x, y, z;
                    inStream >> x >> y >> z;
                    int indx = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;
                    nodeList.push_back(m_nodeList.at(indx));
                }
                VoxelBox* box = new VoxelBox(m_pPointRootEntity);
                box->initialize();
                box->updateNodes(nodeList);
                box->setColor(color);
                box->addComponent(ui->widMainVoxelView->getTransparentLayer());
                m_boxList.push_back(box);
            }

            int cnt_markers;
            inStream >> cnt_markers;

            qDebug() << "Markers : " << cnt_markers;
            RemoveAllMarkers();
            for(int i = 0 ; i < cnt_markers; i++){
                qDebug() << "Load Marker" << i;
                QString strMarkerText;
                inStream >> strMarkerText;
                strMarkerText = QString::fromUtf8(QByteArray::fromBase64(strMarkerText.toUtf8()).data());
                float markerPosX, markerPosY, markerPosZ;
                int nodeIndexX, nodeIndexY, nodeIndexZ;
                inStream >> markerPosX >> markerPosY >> markerPosZ;
                inStream >> nodeIndexX >> nodeIndexY >> nodeIndexZ;
                QVector3D markerPosVector(markerPosX, markerPosY, markerPosZ);
                VoxelNodeMarker* pNodeMarker = new VoxelNodeMarker(m_pPointRootEntity);\
                pNodeMarker->setMarkerSize(m_spacingSize, m_workspaceSizeY);
                pNodeMarker->getMarkerTransform()->setTranslation(markerPosVector);
                pNodeMarker->getMarkerText()->setText(strMarkerText);
                Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
                pNodeMarker->getMarkerTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
                pNodeMarker->getCuboidMesh()->setXExtent(pNodeMarker->getMarkerText()->text().length());
                pNodeMarker->getCuboidMesh()->setYExtent(1.5f);
                AdjustCuboidOfMarker(pNodeMarker);
                foreach(Voxel3DNode* pNode, m_nodeList) {
                    if (pNode->nodeIndexX() == nodeIndexX && pNode->nodeIndexY() == nodeIndexY && pNode->nodeIndexZ() == nodeIndexZ) {
                        pNodeMarker->setNode(pNode);
                        break;
                    }
                }

                pNodeMarker->getPresentLine()->setPoints(pNodeMarker->getNode()->position(), pNodeMarker->getMarkerTransform()->translation());
                QObject::connect(pNodeMarker, SIGNAL(NodeMarkerPressed(Voxel3DNode*)), this, SLOT(onNodeMarkerPressed(Voxel3DNode*)));
                m_listNodeMarkers.push_back(pNodeMarker);

            }

            QString objName = inStream.readLine().trimmed();

            // Because of "endl"
            if(objName.isEmpty())
                objName = inStream.readLine();

            if(!objName.isEmpty()){
                if(QFile::exists(objName)){
//                    m_pObjMesh->setSource(QUrl::fromLocalFile(objName));
//                    m_pObjEntity->addComponent(m_pObjMesh);
                    m_pObjSceneLoader->setSource(QUrl::fromLocalFile(objName));
                    m_pObjEntity->addComponent(m_pObjSceneLoader);
                    QObject::connect(m_pObjSceneLoader, &Qt3DRender::QSceneLoader::statusChanged, this, &MainWindow::onObjSceneLoaderStatusChanged);

                    m_hasObj = true;
                    enableObjController();

                    int scale, rot_x, rot_y, rot_z, pos_x, pos_y, pos_z, alpha;
                    inStream >> scale >> rot_x >> rot_y >> rot_z >> pos_x >> pos_y >> pos_z >> alpha;

                    ui->spnObjScale->setValue(scale);
                    ui->dialOBJRotationX->setValue(rot_x);
                    ui->dialOBJRotationY->setValue(rot_y);
                    ui->dialOBJRotationZ->setValue(rot_z);
                    ui->sldOBJPosX->setValue(pos_x);
                    ui->sldOBJPosY->setValue(pos_y);
                    ui->sldOBJPosZ->setValue(pos_z);
                    ui->sldOBJOpacity->setValue(alpha);
                }
            }
            file.close();

            m_pLines = new VoxelCloudLine(m_pPointRootEntity);
            m_pLines->initialize(m_workspaceSizeX, m_workspaceSizeY, m_workspaceSizeZ, m_spacingSize);
            m_pLines->setColor(m_lineColor);
            m_pLines->setAlpha(static_cast<float>(ui->sldGridOpacity->value()) / 100.0f);
            m_pLines->addComponent(ui->widMainVoxelView->getOpaqueLayer());

            m_pNodes = new VoxelCloudNode(m_pPointRootEntity);
            m_pNodes->setCamera(ui->widMainVoxelView->camera());
            m_pNodes->setMainView(ui->widMainVoxelView);
            m_pNodes->initialize(m_workspaceSizeX, m_workspaceSizeY, m_workspaceSizeZ, m_spacingSize);
            m_pNodes->addComponent(ui->widMainVoxelView->getOpaqueLayer());

            connect(m_pNodes, SIGNAL(voxelClicked(int, int, int, QVector3D)), this, SLOT(onVoxelClicked(int, int, int, QVector3D)));
            connect(m_pNodes, SIGNAL(voxelCtrlClicked(int, int, int, QVector3D)), this, SLOT(onVoxelCtrlClicked(int, int, int, QVector3D)));
            connect(m_pNodes, SIGNAL(voxelShiftClicked(int, int, int, QVector3D)), this, SLOT(onVoxelShiftClicked(int, int, int, QVector3D)));

            foreach(Voxel3DNode* node, m_nodeList)
            {
                m_pNodes->updateVertex(node->nodeIndexX(),
                                       node->nodeIndexY(),
                                       node->nodeIndexZ(),
                                       node->position());
            }
            foreach(Voxel3DNode* node, m_nodeList)
            {
                if(node->isVisible())
                    m_pNodes->setVisible(node->nodeIndexX(), node->nodeIndexY(), node->nodeIndexZ());
            }

            foreach(Voxel3DNode* node, m_nodeList)
            {
                m_pLines->updateVertex(node->nodeIndexX(),
                                       node->nodeIndexY(),
                                       node->nodeIndexZ(),
                                       node->position());
            }
            linkNodes();

            // Reactor Guide
            ui->btnSelectReactorGuide->setChecked(false);
            createReactorView();
            onSelectReactorGuide_triggered(false);

            m_pArrow->setArrowLength(m_spacingSize, m_workspaceSizeY);
            m_pArrow->setZoom(ui->widMainVoxelView->getZoom());
            m_pArrow->MoveArrows(BEHIND_CAMERA);
            m_pArrow->getXText()->getTextMesh()->setText("X " + QString::number(0));
            m_pArrow->getYText()->getTextMesh()->setText("Y " + QString::number(0));
            m_pArrow->getZText()->getTextMesh()->setText("Z " + QString::number(0));

            m_pScaleLine->getTransform()->setTranslation(BEHIND_CAMERA);
        }
    }
}

void MainWindow::onImportOBJ_triggered(){
    // Open Open-Dialog
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Import an obj"),
                nullptr,
                tr("Image Files (*.obj *.fbx *.dae)"),
                nullptr,
                nullptr);

    if (!fileName.isEmpty()){
        onClearOBJ_triggered();
        QUrl objUrl = QUrl::fromLocalFile(fileName);
//        m_pObjMesh->setSource(objUrl);
//        m_pObjEntity->addComponent(m_pObjMesh);

        m_pObjSceneLoader->setSource(objUrl);
        m_pObjEntity->addComponent(m_pObjSceneLoader);

        QObject::connect(m_pObjSceneLoader, &Qt3DRender::QSceneLoader::statusChanged, this, &MainWindow::onObjSceneLoaderStatusChanged);

        m_hasObj = true;
        enableObjController();
    }
}

void MainWindow::onObjSceneLoaderStatusChanged(Qt3DRender::QSceneLoader::Status status)
{
    qDebug() << "<<<<<<<<<<<< ENTITY COUNT : " << m_pObjSceneLoader->entities().length();
    if(status == Qt3DRender::QSceneLoader::Ready){
        foreach(Qt3DCore::QEntity* entity, m_pObjSceneLoader->entities()){
            mapObjSceneMaterial(entity);
        }
    }
}

void MainWindow::mapObjSceneMaterial(Qt3DCore::QEntity* entity)
{
    for(auto comp: entity->components())
    {
//        Qt3DRender::QAbstractLight *light = qobject_cast<Qt3DRender::QAbstractLight *>(comp);
//        if(light){
//            qDebug() << "removed old light";
//            entity->removeComponent(comp);
//        }

        Qt3DExtras::QPhongMaterial *mat = qobject_cast<Qt3DExtras::QPhongMaterial *>(comp);
        Qt3DExtras::QPhongAlphaMaterial *mat_alpha = qobject_cast<Qt3DExtras::QPhongAlphaMaterial *>(comp);
        if(mat || mat_alpha){
            qDebug() << "removed old phong material";
            entity->removeComponent(comp);

            if(ui->sldOBJOpacity->isEnabled()){
                entity->addComponent(m_pObjMaterialAlpha);
            }else{
                entity->addComponent(m_pObjMaterial);
            }
        }
    }

    for(auto childNode: entity->childNodes())
    {
        auto childEntity = qobject_cast<Qt3DCore::QEntity *>(childNode);
        if (childEntity)
            mapObjSceneMaterial(childEntity);
    }
}

void MainWindow::onClearOBJ_triggered(){
    // Remove OBJ
//    m_pObjEntity->removeComponent(m_pObjMesh);
    m_pObjEntity->removeComponent(m_pObjSceneLoader);
    qDebug() << "Remvoed OBJ";
    m_hasObj = false;
    enableObjController();
}

void MainWindow::onMoveLight_triggered(){
    ui->widMainVoxelView->updateLightPosition();
}

void MainWindow::onChooseLineColor_triggered()
{
    m_lineColor = QColorDialog::getColor(m_lineColor,parentWidget());

    if(m_lineColor.isValid()){
        QString buttonStyle = QString("QPushButton{ \
                                        background-color: %1; \
                                                border:none; \
                                        }\
                                        QPushButton:hover{ \
                                                background-color: %2; \
                                                border:none; \
                                        } \
                                        QPushButton:pressed{ \
                                                background-color: %3; \
                                                border:none; \
                                        }").arg(m_lineColor.name(), m_lineColor.lighter(115).name(), m_lineColor.lighter(120).name());
    ui->btnLineColor->setStyleSheet(buttonStyle);

    if(m_pLines != nullptr)
        m_pLines->setColor(m_lineColor);
}
}

void MainWindow::onChooseBoxColor_triggered()
{
    if(!ui->btnSelectReactorGuide->isChecked())
        return;

    m_boxColor = QColorDialog::getColor(m_boxColor, parentWidget());

    if(m_boxColor.isValid()){
        QString colorStyle = QString("background-color: %1;").arg(m_boxColor.name());
        if(ui->btnSelectReactorMode1->isChecked()){
            m_boxColor1 = m_boxColor;
        }else if(ui->btnSelectReactorMode2->isChecked()){
            m_boxColor2 = m_boxColor;
        }else if(ui->btnSelectReactorMode3->isChecked()){
            m_boxColor3 = m_boxColor;
        }else if(ui->btnSelectReactorMode4->isChecked()){
            m_boxColor4 = m_boxColor;
        }else if(ui->btnSelectReactorMode5->isChecked()){
            m_boxColor5 = m_boxColor;
        }else if(ui->btnSelectReactorMode6->isChecked()){
            m_boxColor6 = m_boxColor;
        }else if(ui->btnSelectReactorMode7->isChecked()){
            m_boxColor7 = m_boxColor;
        }else if(ui->btnSelectReactorMode8->isChecked()){
            m_boxColor8 = m_boxColor;
        }

        updateBoxColorUI();
    }
}

void MainWindow::onBoxColor_triggered() {
    if(m_reactorBox != nullptr) {
        m_reactorBox->setColor(m_boxColor);
        foreach(VoxelBox* pBox, m_boxList) {
            if (pBox == m_reactorBox) {
                qDebug() << "\t Already Exist Reactor Box";
                return;
            }
        }

        qDebug() << "\t Added Reactor Box";
        m_boxList.push_back(m_reactorBox);
    }else{
        updateReactorNodes();
    }
}

void MainWindow::onClearBoxColor_triggered()
{
    if(!ui->btnSelectReactorGuide->isChecked())
        return;

    if(m_reactorBox != nullptr){
        m_boxList.removeAll(m_reactorBox);
        m_reactorBox->deleteLater();
        m_reactorBox = nullptr;
    }
}

void MainWindow::onUpdateBox(){
    foreach(VoxelBox* box, m_boxList){
        if(box->hasNode(m_pSelectedNode)){
            box->updateNode(m_pSelectedNode);
        }
    }
}

void MainWindow::updateBoxColorUI(){
    QString colorStyle;

    colorStyle = QString("background-color: %1;").arg(m_boxColor1.name());
    ui->lblBoxColor1->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor2.name());
    ui->lblBoxColor2->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor3.name());
    ui->lblBoxColor3->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor4.name());
    ui->lblBoxColor4->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor5.name());
    ui->lblBoxColor5->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor6.name());
    ui->lblBoxColor6->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor7.name());
    ui->lblBoxColor7->setStyleSheet(colorStyle);

    colorStyle = QString("background-color: %1;").arg(m_boxColor8.name());
    ui->lblBoxColor8->setStyleSheet(colorStyle);

    if(ui->btnSelectReactorMode1->isChecked()){
        m_boxColor = m_boxColor1;
    }else if(ui->btnSelectReactorMode2->isChecked()){
        m_boxColor = m_boxColor2;
    }else if(ui->btnSelectReactorMode3->isChecked()){
        m_boxColor = m_boxColor3;
    }else if(ui->btnSelectReactorMode4->isChecked()){
        m_boxColor = m_boxColor4;
    }else if(ui->btnSelectReactorMode5->isChecked()){
        m_boxColor = m_boxColor5;
    }else if(ui->btnSelectReactorMode6->isChecked()){
        m_boxColor = m_boxColor6;
    }else if(ui->btnSelectReactorMode7->isChecked()){
        m_boxColor = m_boxColor7;
    }else if(ui->btnSelectReactorMode8->isChecked()){
        m_boxColor = m_boxColor8;
    }
    colorStyle = QString("background-color: %1;").arg(m_boxColor.name());
    QString buttonStyle = QString("QPushButton{ \
                                      background-color: %1; \
                border:none; \
        }\
        QPushButton:hover{ \
            background-color: %2; \
        border:none; \
        } \
        QPushButton:pressed{ \
            background-color: %3; \
        border:none; \
        }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(buttonStyle);
}

void MainWindow::onLineAlphaChanged(int k)
{
    if(m_pLines != nullptr)
        m_pLines->setAlpha(static_cast<float>(k) / 100.0f);
}

void MainWindow::onResetPoint_triggered()
{
    qDebug() << "Reset Point";

    Voxel3DNode* pTmpNode = m_pSelectedNode;

    if (m_selectedNodeList.count() >= 1) {
        foreach(auto *pNode, m_selectedNodeList){
            m_pSelectedNode = pNode;
            m_pSelectedNode->resetPosition();
            displaySelectedNodeData();
            if(m_pLines != nullptr)
                m_pLines->updateVertex(m_pSelectedNode->nodeIndexX(),
                                       m_pSelectedNode->nodeIndexY(),
                                       m_pSelectedNode->nodeIndexZ(),
                                       m_pSelectedNode->position());
            if(m_pNodes != nullptr)
                m_pNodes->updateVertex(m_pSelectedNode->nodeIndexX(),
                                       m_pSelectedNode->nodeIndexY(),
                                       m_pSelectedNode->nodeIndexZ(),
                                       m_pSelectedNode->position());
            nodePositionReleased();
        }
    }

    m_pSelectedNode = pTmpNode;

}

void MainWindow::createInitialWorkspace()
{
    qDebug() << ">> Create InitialWorkspace";
    m_pPointRootEntity = new Qt3DCore::QEntity(m_pRootEntity);
    m_pTmpRootEntity = new Qt3DCore::QEntity(nullptr);

    //=================== UI Property ==================//
    ui->widMainVoxelView->initWorkspace(m_workspaceSizeY, m_spacingSize);

    initNodeParameters();
    initObjParameters();

    ui->btnViewDefault->setChecked(true);
    /////////////////////////////////////////


    enableObjController();
    enableNodeController();

    m_reactorBox = nullptr;
    m_boxList.clear();

    qDebug() << "<< Create InitialWorkspace";
}

void MainWindow::createInitialNodes()
{
    qDebug() << ">> Create InitialNodes";
    m_nodeList.clear();

    /////////////////////////////////////////////////////////////////////////
    for (int x = 0; x < m_workspaceSizeX; x++) {
        for (int y = 0; y < m_workspaceSizeY; y++) {
            for( int z = 0; z < m_workspaceSizeZ; z++){
                Voxel3DNode* node = new Voxel3DNode(m_pTmpRootEntity);
                QVector3D pos = (QVector3D(x , y, z) - QVector3D(static_cast<float>(m_workspaceSizeX - 1) / 2.0f,
                                                                 static_cast<float>(m_workspaceSizeY - 1) / 2.0f,
                                                                 static_cast<float>(m_workspaceSizeZ - 1) / 2.0f)) * m_spacingSize;
                node->setNodeIndex(static_cast<int8_t>(x),
                                   static_cast<int8_t>(y),
                                   static_cast<int8_t>(z));
                node->updateCenterPosition(pos);

                connect(node, SIGNAL(nodeClicked()), this, SLOT(onNodeClicked()));
                connect(node, SIGNAL(nodeCtrlClicked()), this, SLOT(onNodeCtrlClicked()));
                connect(node, SIGNAL(nodeShiftClicked()), this, SLOT(onNodeShiftClicked()));

                int tmp_cnt = 0;
                tmp_cnt = x == 0 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = y == 0 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = z == 0 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = x == m_workspaceSizeX - 1 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = y == m_workspaceSizeY - 1 ? tmp_cnt + 1: tmp_cnt;
                tmp_cnt = z == m_workspaceSizeZ - 1 ? tmp_cnt + 1: tmp_cnt;

                // Lines of Cube is visible
                if(tmp_cnt >= 2)
                {
                    node->setVisible(true);
                }

                m_nodeList.push_back(node);
            }
        }
    }

    m_pLines = new VoxelCloudLine(m_pPointRootEntity);
    m_pLines->initialize(m_workspaceSizeX, m_workspaceSizeY, m_workspaceSizeZ, m_spacingSize);
    m_pLines->setColor(m_lineColor);
    m_pLines->setAlpha(static_cast<float>(ui->sldGridOpacity->value()) / 100.0f);
    m_pLines->addComponent(ui->widMainVoxelView->getOpaqueLayer());

    m_pNodes = new VoxelCloudNode(m_pPointRootEntity);
    m_pNodes->setCamera(ui->widMainVoxelView->camera());
    m_pNodes->setMainView(ui->widMainVoxelView);
    m_pNodes->initialize(m_workspaceSizeX, m_workspaceSizeY, m_workspaceSizeZ, m_spacingSize);
    m_pNodes->addComponent(ui->widMainVoxelView->getOpaqueLayer());

    connect(m_pNodes, SIGNAL(voxelClicked(int, int, int, QVector3D)), this, SLOT(onVoxelClicked(int, int, int, QVector3D)));
    connect(m_pNodes, SIGNAL(voxelCtrlClicked(int, int, int, QVector3D)), this, SLOT(onVoxelCtrlClicked(int, int, int, QVector3D)));
    connect(m_pNodes, SIGNAL(voxelShiftClicked(int, int, int, QVector3D)), this, SLOT(onVoxelShiftClicked(int, int, int, QVector3D)));

    qDebug() << "<< Create InitialNodes";
}

void MainWindow::linkNodes()
{
    qDebug() << ">> LinkNodes";
    //    //////////////////////////////////

    //    QTime dieTime= QTime::currentTime().addMSecs(100);
    //    while (QTime::currentTime() < dieTime)
    //        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    for (int x = 0; x < m_workspaceSizeX; x++) {
        for (int y = 0; y < m_workspaceSizeY; y++) {
            for( int z = 0; z < m_workspaceSizeZ; z++){
                int i = z + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ;

                if(x + 1 < m_workspaceSizeX)
                {
                    int i_right = z + y * m_workspaceSizeZ + (x + 1) * m_workspaceSizeY * m_workspaceSizeZ; // x + 1
                    m_nodeList.at(i)->setRightNode(m_nodeList.at(i_right));
                    m_nodeList.at(i_right)->setLeftNode(m_nodeList.at(i));

                    if(m_nodeList.at(i)->isVisible() &&
                            m_nodeList.at(i_right)->isVisible())
                    {
                        if(m_pLines != nullptr)
                            m_pLines->addLine(x, y, z, x + 1, y, z);
                    }
                }
                if(y + 1 < m_workspaceSizeY)
                {
                    int i_up = z + (y + 1) * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ; // y + 1
                    m_nodeList.at(i)->setUpNode(m_nodeList.at(i_up));
                    m_nodeList.at(i_up)->setDownNode(m_nodeList.at(i));

                    if(m_nodeList.at(i)->isVisible() &&
                            m_nodeList.at(i_up)->isVisible())
                    {
                        if(m_pLines != nullptr)
                            m_pLines->addLine(x, y, z, x, y + 1, z);
                    }
                }
                if(z + 1 < m_workspaceSizeZ)
                {
                    int i_front = (z + 1) + y * m_workspaceSizeZ + x * m_workspaceSizeY * m_workspaceSizeZ; // z + 1
                    m_nodeList.at(i)->setFrontNode(m_nodeList.at(i_front));
                    m_nodeList.at(i_front)->setBackNode(m_nodeList.at(i));

                    if(m_nodeList.at(i)->isVisible() &&
                            m_nodeList.at(i_front)->isVisible())
                    {
                        if(m_pLines != nullptr)
                            m_pLines->addLine(x, y, z, x, y, z + 1);
                    }
                }
            }
        }
    }

    ui->widMainVoxelView->setCameraOrthographicProjection();
    qDebug() << "<< LinkNodes";
}

void MainWindow::objScaleChanged(int k)
{
    if(m_pObjTransform != nullptr)
        m_pObjTransform->setScale(static_cast<float>(k) / 100.f);
}

void MainWindow::enableObjController()
{
    ui->spnObjScale->setEnabled(m_hasObj);

    ui->sldOBJPosX->setEnabled(m_hasObj);
    ui->sldOBJPosY->setEnabled(m_hasObj);
    ui->sldOBJPosZ->setEnabled(m_hasObj);

    ui->spnOBJPosX->setEnabled(m_hasObj);
    ui->spnOBJPosY->setEnabled(m_hasObj);
    ui->spnOBJPosZ->setEnabled(m_hasObj);

    ui->dialOBJRotationX->setEnabled(m_hasObj);
    ui->dialOBJRotationY->setEnabled(m_hasObj);
    ui->dialOBJRotationZ->setEnabled(m_hasObj);

    ui->spnOBJRotationX->setEnabled(m_hasObj);
    ui->spnOBJRotationY->setEnabled(m_hasObj);
    ui->spnOBJRotationZ->setEnabled(m_hasObj);

    ui->chkOBJTransparent->setEnabled(m_hasObj);

    if(!m_hasObj)
        initObjParameters();

    ui->chkOBJTransparent->setChecked(false);
    ui->sldOBJOpacity->setEnabled(false);
    ui->spnObjOpacity->setEnabled(false);
}

void MainWindow::initObjParameters()
{
    ui->sldOBJPosX->setMinimum(-20000);
    ui->sldOBJPosX->setMaximum(20000);
    ui->sldOBJPosY->setMinimum(-20000);
    ui->sldOBJPosY->setMaximum(20000);
    ui->sldOBJPosZ->setMinimum(-20000);
    ui->sldOBJPosZ->setMaximum(20000);
    ui->sldOBJPosX->setValue(0);
    ui->sldOBJPosY->setValue(0);
    ui->sldOBJPosZ->setValue(0);
    ui->dialOBJRotationX->setValue(0);
    ui->dialOBJRotationY->setValue(0);
    ui->dialOBJRotationZ->setValue(0);
    ui->spnObjScale->setValue(100);
    ui->sldOBJOpacity->setValue(80);

    ui->chkOBJTransparent->setChecked(false);
    ui->sldOBJOpacity->setEnabled(false);
    ui->spnObjOpacity->setEnabled(false);
}

void MainWindow::objTransparentPropertyChanged(bool f)
{
    ui->sldOBJOpacity->setEnabled(f);
    ui->spnObjOpacity->setEnabled(f);

    if(m_pObjEntity == nullptr)
        return;

    if(m_pObjMaterial == nullptr && m_pObjMaterialAlpha == nullptr)
        return;

    if(f){
        m_pObjEntity->removeComponent(m_pObjMaterial);
        m_pObjEntity->addComponent(m_pObjMaterialAlpha);
    }else{
        m_pObjEntity->addComponent(m_pObjMaterial);
        m_pObjEntity->removeComponent(m_pObjMaterialAlpha);
    }

    if(m_pObjSceneLoader){
        foreach(Qt3DCore::QEntity* entity, m_pObjSceneLoader->entities()){
            mapObjSceneMaterial(entity);
        }
    }
}

void MainWindow::objOpacityChanged(int k)
{
    ui->spnObjOpacity->setValue(k);
    ui->sldOBJOpacity->setValue(k);

    if(m_pObjMaterialAlpha != nullptr)
        m_pObjMaterialAlpha->setAlpha(static_cast<float>(k) / 100.0f);
}

void MainWindow::objPositionChanged()
{
    if(m_pObjTransform != nullptr){
        m_pObjTransform->setTranslation(QVector3D(ui->sldOBJPosX->value(), ui->sldOBJPosY->value(), ui->sldOBJPosZ->value()));
    }
}

void MainWindow::objPositionXChanged(int k)
{
    if(k > ui->sldOBJPosX->maximum())
        k = ui->sldOBJPosX->maximum();

    ui->spnOBJPosX->setValue(k);
    ui->sldOBJPosX->setValue(k);
    objPositionChanged();
}

void MainWindow::objPositionYChanged(int k)
{
    if(k > ui->sldOBJPosY->maximum())
        k = ui->sldOBJPosY->maximum();
    ui->spnOBJPosY->setValue(k);
    ui->sldOBJPosY->setValue(k);
    objPositionChanged();
}

void MainWindow::objPositionZChanged(int k)
{
    if(k > ui->sldOBJPosZ->maximum())
        k = ui->sldOBJPosZ->maximum();
    ui->spnOBJPosZ->setValue(k);
    ui->sldOBJPosZ->setValue(k);
    objPositionChanged();
}

void MainWindow::objRotationXChanged(int k)
{
    float rValue = k / 2.0f;

    if(m_pObjTransform != nullptr)
    {
//        m_pObjTransform->setRotationX(k);

        QQuaternion rot = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rValue - m_objRotX);
        m_objRotation = rot * m_objRotation;
        m_pObjTransform->setRotation(m_objRotation);

//        QQuaternion rot1 = QQuaternion::fromAxisAndAngle(m_objAxisX, ui->dialOBJRotationX->value());
//        QQuaternion rot2 = QQuaternion::fromAxisAndAngle(m_objAxisY, ui->dialOBJRotationY->value());
//        QQuaternion rot3 = QQuaternion::fromAxisAndAngle(m_objAxisZ, ui->dialOBJRotationZ->value());

//        m_objRotation = rot1 * rot2 * rot3;
//        m_pObjTransform->setRotation(m_objRotation);
////        m_objAxisX = m_objRotation * m_objAxisX;
////        m_objAxisY = m_objRotation * m_objAxisY;
////        m_objAxisZ = m_objRotation * m_objAxisZ;

//        qDebug() << m_objAxisX << m_objAxisY << m_objAxisZ;

        ui->spnOBJRotationX->setValue(k);
        ui->dialOBJRotationX->setValue(k);
    }
    m_objRotX = rValue;
}

void MainWindow::objRotationYChanged(int k)
{
    float rValue = k / 2.0f;

    if(m_pObjTransform != nullptr)
    {
//        m_pObjTransform->setRotationY(k);
        QQuaternion rot = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rValue - m_objRotY);
        m_objRotation = rot * m_objRotation;
        m_pObjTransform->setRotation(m_objRotation);

//        QQuaternion rot1 = QQuaternion::fromAxisAndAngle(m_objAxisX, ui->dialOBJRotationX->value());
//        QQuaternion rot2 = QQuaternion::fromAxisAndAngle(m_objAxisY, ui->dialOBJRotationY->value());
//        QQuaternion rot3 = QQuaternion::fromAxisAndAngle(m_objAxisZ, ui->dialOBJRotationZ->value());

//        m_objRotation = rot1 * rot2 * rot3;
//        m_pObjTransform->setRotation(m_objRotation);
//        //        m_objAxisX = m_objRotation * m_objAxisX;
//        //        m_objAxisY = m_objRotation * m_objAxisY;
//        //        m_objAxisZ = m_objRotation * m_objAxisZ;

//        qDebug() << m_objAxisX << m_objAxisY << m_objAxisZ;

        ui->spnOBJRotationY->setValue(k);
        ui->dialOBJRotationY->setValue(k);
    }
    m_objRotY = rValue;
}

void MainWindow::objRotationZChanged(int k)
{
    float rValue = k / 2.0f;

    if(m_pObjTransform != nullptr)
    {
//        m_pObjTransform->setRotationZ(k);
        QQuaternion rot = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rValue - m_objRotZ);
        m_objRotation = rot * m_objRotation;
        m_pObjTransform->setRotation(m_objRotation);

//        QQuaternion rot1 = QQuaternion::fromAxisAndAngle(m_objAxisX, ui->dialOBJRotationX->value());
//        QQuaternion rot2 = QQuaternion::fromAxisAndAngle(m_objAxisY, ui->dialOBJRotationY->value());
//        QQuaternion rot3 = QQuaternion::fromAxisAndAngle(m_objAxisZ, ui->dialOBJRotationZ->value());

//        m_objRotation = rot1 * rot2 * rot3;
//        m_pObjTransform->setRotation(m_objRotation);
//        //        m_objAxisX = m_objRotation * m_objAxisX;
//        //        m_objAxisY = m_objRotation * m_objAxisY;
//        //        m_objAxisZ = m_objRotation * m_objAxisZ;

//        qDebug() << m_objAxisX << m_objAxisY << m_objAxisZ;

        ui->spnOBJRotationZ->setValue(k);
        ui->dialOBJRotationZ->setValue(k);
    }
    m_objRotZ = rValue;
}

void MainWindow::onNodeClicked()
{
    Voxel3DNode* pNode = static_cast<Voxel3DNode*>(sender());
    if (isReactorCandidate(pNode) == true) {
        return;
    }

    if (m_nStraightEdgeFlag >= 1 && m_nStraightEdgeFlag <=2) {  //Straight Add Clicked
        nodeClickForStraight(pNode);
        return;
    }

    if(m_pSelectedNode != nullptr){
        foreach(Voxel3DNode* node, m_selectedNodeList)
        {
            node->setSelected(false);
        }
    }

    m_pSelectedNode = pNode;
    m_selectedNodeList.clear();
    m_selectedNodeList.push_back(m_pSelectedNode);

    m_pArrow->MoveArrows(m_pSelectedNode->position());
    RotateArrowTexts();

    ui->chkAddMarker->setChecked(checkNodeMarkerExisted(m_pSelectedNode));

    enableNodeController();
    updateReactorNodes();
    displaySelectedNodeData();
    m_isRangeSelected = false;

    updateSelectionCloudNodes();
}

void MainWindow::onNodeCtrlClicked()
{
    if(ui->btnSelectReactorGuide->isChecked()){
        if(m_pSelectedNode != nullptr){
            foreach(Voxel3DNode* node, m_selectedNodeList)
            {
                node->setSelected(false);
            }
        }

        m_pSelectedNode = static_cast<Voxel3DNode*>(sender());

        m_selectedNodeList.clear();
        m_selectedNodeList.push_back(m_pSelectedNode);

        enableNodeController();
        updateReactorNodes();
        displaySelectedNodeData();

    }else{
        m_pSelectedNode = static_cast<Voxel3DNode*>(sender());
        m_selectedNodeList.push_back(m_pSelectedNode);
        enableNodeController();
        displaySelectedNodeData();
    }
    m_isRangeSelected = false;

    m_pArrow->MoveArrows(BEHIND_CAMERA);

    updateSelectionCloudNodes();
}

void MainWindow::onNodeShiftClicked()
{
    m_isRangeSelected = false;
    if(ui->btnSelectReactorGuide->isChecked()){
        if(m_pSelectedNode != nullptr){
            foreach(Voxel3DNode* node, m_selectedNodeList)
            {
                node->setSelected(false);
            }
        }

        m_pSelectedNode = static_cast<Voxel3DNode*>(sender());

        m_selectedNodeList.clear();
        m_selectedNodeList.push_back(m_pSelectedNode);

        enableNodeController();
        updateReactorNodes();
        displaySelectedNodeData();

    }else{
        if(m_selectedNodeList.length() == 1){
            Voxel3DNode* startNode = m_pSelectedNode;
            QVector3D startPos = startNode->centerPosition();
            qDebug() << "Start Node : " << startNode->nodeIndexX() << "," << startNode->nodeIndexY() << "," << startNode->nodeIndexZ();

            m_pSelectedNode = static_cast<Voxel3DNode*>(sender());
            QVector3D endPos = m_pSelectedNode->centerPosition();
            qDebug() << "End Node : " << m_pSelectedNode->nodeIndexX() << "," << m_pSelectedNode->nodeIndexY() << "," << m_pSelectedNode->nodeIndexZ();

            m_selectedNodeList.clear();
            foreach(Voxel3DNode* node, m_nodeList){
                if(node != nullptr){
                    if(((node->centerPosition().x() >= startPos.x() && node->centerPosition().x() <= endPos.x())
                        || (node->centerPosition().x() >= endPos.x() && node->centerPosition().x() <= startPos.x())) &&
                            ((node->centerPosition().y() >= startPos.y() && node->centerPosition().y() <= endPos.y())
                             || (node->centerPosition().y() >= endPos.y() && node->centerPosition().y() <= startPos.y())) &&
                            ((node->centerPosition().z() >= startPos.z() && node->centerPosition().z() <= endPos.z())
                             || (node->centerPosition().z() >= endPos.z() && node->centerPosition().z() <= startPos.z()))){
                        node->setSelected();
                        m_selectedNodeList.push_back(node);

                        qDebug() << "PUSHED INDEX : " << node->nodeIndexX() << "," << node->nodeIndexY() << "," << node->nodeIndexZ();
                    }
                }
            }

            enableNodeController();
            displaySelectedNodeData();
            m_isRangeSelected = true;
        }else{
            onNodeClicked();
        }
    }

    m_pArrow->MoveArrows(BEHIND_CAMERA);

    updateSelectionCloudNodes();
}

void MainWindow::updateSelectionCloudNodes()
{
    if(m_pNodes != nullptr){
        m_pNodes->clearSelectedVoxels();
        qDebug() << "Selected Count : " << m_selectedNodeList.length();
        foreach(Voxel3DNode* node, m_selectedNodeList){
            m_pNodes->addSelectedVoxel(node->nodeIndexX(), node->nodeIndexY(), node->nodeIndexZ());
        }
    }
}

void MainWindow::displaySelectedNodeData()
{
    if(m_pSelectedNode != nullptr)
    {
        if(m_pSelectedNode->isReactor())
            updateReactorView();

        // Display node's property
        if(m_selectedNodeList.count() == 1)
            ui->chkShowPoint->setChecked(m_pSelectedNode->isVisible());

        ui->lblOutPointPosX->setText(QString::number(abs(static_cast<double>(m_pSelectedNode->localPosition().x()))));
        ui->lblOutPointPosY->setText(QString::number(abs(static_cast<double>(m_pSelectedNode->localPosition().y()))));
        ui->lblOutPointPosZ->setText(QString::number(abs(static_cast<double>(m_pSelectedNode->localPosition().z()))));

        ui->edtPointPosX->setText(QString::number(static_cast<double>(m_pSelectedNode->localPosition().x())));
        ui->edtPointPosY->setText(QString::number(static_cast<double>(m_pSelectedNode->localPosition().y())));
        ui->edtPointPosZ->setText(QString::number(static_cast<double>(m_pSelectedNode->localPosition().z())));

        ui->sldNodePosX->setValue(static_cast<int>(m_pSelectedNode->localPosition().x()));
        ui->sldNodePosY->setValue(static_cast<int>(m_pSelectedNode->localPosition().y()));
        ui->sldNodePosZ->setValue(static_cast<int>(m_pSelectedNode->localPosition().z()));

    }else
    {
        initNodeParameters();
    }
}

void MainWindow::enableNodeController()
{
    ui->sldNodePosX->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() == 1);
    ui->sldNodePosY->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() == 1);
    ui->sldNodePosZ->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() == 1);
    ui->edtPointPosX->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() >= 1);
    ui->edtPointPosY->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() >= 1);
    ui->edtPointPosZ->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() >= 1);

    ui->chkShowPoint->setEnabled(m_pSelectedNode != nullptr);
    ui->chkAddMarker->setEnabled(m_pSelectedNode != nullptr);
    ui->edtMarkerName->setEnabled(ui->chkAddMarker->isChecked());
    ui->lblMarkerName->setEnabled(ui->chkAddMarker->isChecked());

    ui->btnResetPoint->setEnabled(m_pSelectedNode != nullptr && m_selectedNodeList.count() >= 1);

    if(m_pSelectedNode == nullptr)
        initNodeParameters();
}

void MainWindow::initNodeParameters()
{
    ui->sldNodePosX->setMinimum(static_cast<int>(-m_spacingSize * 1.5));
    ui->sldNodePosX->setMaximum(static_cast<int>(m_spacingSize * 1.5));
    ui->sldNodePosY->setMinimum(static_cast<int>(-m_spacingSize * 1.5));
    ui->sldNodePosY->setMaximum(static_cast<int>(m_spacingSize * 1.5));
    ui->sldNodePosZ->setMinimum(static_cast<int>(-m_spacingSize * 1.5));
    ui->sldNodePosZ->setMaximum(static_cast<int>(m_spacingSize * 1.5));
    ui->sldNodePosX->setValue(0);
    ui->sldNodePosY->setValue(0);
    ui->sldNodePosZ->setValue(0);

    ui->chkShowPoint->setChecked(false);

    ui->edtPointPosX->setText(QString::number(0));
    ui->edtPointPosY->setText(QString::number(0));
    ui->edtPointPosZ->setText(QString::number(0));

    ui->lblOutPointPosX->setText(QString::number(0));
    ui->lblOutPointPosX->setText(QString::number(0));
    ui->lblOutPointPosX->setText(QString::number(0));
}

void MainWindow::nodePositionXChanged(int k)
{
    if(m_pSelectedNode != nullptr)
    {
        m_pSelectedNode->updateLocalPositionX(k);
        displaySelectedNodeData();

        if(m_pLines != nullptr)
            m_pLines->updateVertex(m_pSelectedNode->nodeIndexX(),
                                   m_pSelectedNode->nodeIndexY(),
                                   m_pSelectedNode->nodeIndexZ(),
                                   m_pSelectedNode->position());
        if(m_pNodes != nullptr)
            m_pNodes->updateVertex(m_pSelectedNode->nodeIndexX(),
                                   m_pSelectedNode->nodeIndexY(),
                                   m_pSelectedNode->nodeIndexZ(),
                                   m_pSelectedNode->position());

        m_pArrow->MoveArrows(m_pSelectedNode->position());
        RotateArrowTexts();
        m_pArrow->getXText()->getTextMesh()->setText("X " + QString::number(k));
        MoveMarker(m_pSelectedNode);
        onUpdateBox();

        m_bNodePosChanged = true;
    }
}

void MainWindow::nodePositionYChanged(int k)
{
    if(m_pSelectedNode != nullptr)
    {
        m_pSelectedNode->updateLocalPositionY(k);
        displaySelectedNodeData();

        if(m_pLines != nullptr)
            m_pLines->updateVertex(m_pSelectedNode->nodeIndexX(),
                                   m_pSelectedNode->nodeIndexY(),
                                   m_pSelectedNode->nodeIndexZ(),
                                   m_pSelectedNode->position());

        if(m_pNodes != nullptr)
            m_pNodes->updateVertex(m_pSelectedNode->nodeIndexX(),
                                   m_pSelectedNode->nodeIndexY(),
                                   m_pSelectedNode->nodeIndexZ(),
                                   m_pSelectedNode->position());

        m_pArrow->MoveArrows(m_pSelectedNode->position());
        RotateArrowTexts();
        m_pArrow->getYText()->getTextMesh()->setText("Y " + QString::number(k));
        MoveMarker(m_pSelectedNode);

        onUpdateBox();

        m_bNodePosChanged = true;
    }

}

void MainWindow::nodePositionZChanged(int k)
{
    if(m_pSelectedNode != nullptr)
    {
        m_pSelectedNode->updateLocalPositionZ(k);
        displaySelectedNodeData();

        if(m_pLines != nullptr)
            m_pLines->updateVertex(m_pSelectedNode->nodeIndexX(),
                                   m_pSelectedNode->nodeIndexY(),
                                   m_pSelectedNode->nodeIndexZ(),
                                   m_pSelectedNode->position());

        if(m_pNodes != nullptr)
            m_pNodes->updateVertex(m_pSelectedNode->nodeIndexX(),
                                   m_pSelectedNode->nodeIndexY(),
                                   m_pSelectedNode->nodeIndexZ(),
                                   m_pSelectedNode->position());

        m_pArrow->MoveArrows(m_pSelectedNode->position());
        RotateArrowTexts();
        m_pArrow->getZText()->getTextMesh()->setText("Z " + QString::number(k));
        MoveMarker(m_pSelectedNode);

        onUpdateBox();

        m_bNodePosChanged = true;
    }
}

void MainWindow::nodePositionReleased()
{
    if(m_pSelectedNode != nullptr && m_selectedNodeList.length() >= 1)
    {
        m_pSelectedNode->createHistoryPosition();

        m_historiesBefore.push_back(m_pSelectedNode);
        m_historiesAfter.clear();
        if(m_historiesBefore.length() > 30)
            m_historiesBefore.pop_front();

        copyReactorPointsToClipboard();
    }
}

void MainWindow::nodeVisiblePropertyChanged(bool f)
{
    if(m_pPointRootEntity == nullptr)
        return;

    if(m_pSelectedNode != nullptr)
    {
        foreach(Voxel3DNode* node, m_selectedNodeList){
            if(f == node->isVisible())
                continue;

            node->setVisible(f);
            changeNodeVisibleProperty(node);
        }
    }
}

void MainWindow::changeNodeVisibleProperty(Voxel3DNode* node)
{
    if(node != nullptr)
    {
        if(node->isVisible()){
            if(m_pNodes != nullptr)
                m_pNodes->setVisible(node->nodeIndexX(), node->nodeIndexY(), node->nodeIndexZ());
            // Create Line
            if(node->leftNode() != nullptr)
            {
                if(node->leftNode()->isVisible())
                {
                    if(m_pLines != nullptr)
                        m_pLines->addLine(node->nodeIndexX(),
                                          node->nodeIndexY(),
                                          node->nodeIndexZ(),
                                          node->leftNode()->nodeIndexX(),
                                          node->leftNode()->nodeIndexY(),
                                          node->leftNode()->nodeIndexZ());
                }
            }

            if(node->rightNode() != nullptr)
            {
                if(node->rightNode()->isVisible())
                {
                    if(m_pLines != nullptr)
                        m_pLines->addLine(node->nodeIndexX(),
                                          node->nodeIndexY(),
                                          node->nodeIndexZ(),
                                          node->rightNode()->nodeIndexX(),
                                          node->rightNode()->nodeIndexY(),
                                          node->rightNode()->nodeIndexZ());
                }
            }

            if(node->upNode() != nullptr)
            {
                if(node->upNode()->isVisible())
                {
                    if(m_pLines != nullptr)
                        m_pLines->addLine(node->nodeIndexX(),
                                          node->nodeIndexY(),
                                          node->nodeIndexZ(),
                                          node->upNode()->nodeIndexX(),
                                          node->upNode()->nodeIndexY(),
                                          node->upNode()->nodeIndexZ());
                }
            }

            if(node->downNode() != nullptr)
            {
                if(node->downNode()->isVisible())
                {
                    if(m_pLines != nullptr)
                        m_pLines->addLine(node->nodeIndexX(),
                                          node->nodeIndexY(),
                                          node->nodeIndexZ(),
                                          node->downNode()->nodeIndexX(),
                                          node->downNode()->nodeIndexY(),
                                          node->downNode()->nodeIndexZ());
                }
            }

            if(node->frontNode() != nullptr)
            {
                if(node->frontNode()->isVisible())
                {
                    if(m_pLines != nullptr)
                        m_pLines->addLine(node->nodeIndexX(),
                                          node->nodeIndexY(),
                                          node->nodeIndexZ(),
                                          node->frontNode()->nodeIndexX(),
                                          node->frontNode()->nodeIndexY(),
                                          node->frontNode()->nodeIndexZ());
                }
            }

            if(node->backNode() != nullptr)
            {
                if(node->backNode()->isVisible())
                {
                    if(m_pLines != nullptr)
                        m_pLines->addLine(node->nodeIndexX(),
                                          node->nodeIndexY(),
                                          node->nodeIndexZ(),
                                          node->backNode()->nodeIndexX(),
                                          node->backNode()->nodeIndexY(),
                                          node->backNode()->nodeIndexZ());
                }
            }
        }else
        {
            if(m_pNodes != nullptr)
                m_pNodes->setUnvisible(node->nodeIndexX(), node->nodeIndexY(), node->nodeIndexZ());
            // Remove Line
            if(node->leftNode() != nullptr)
            {
                if(m_pLines != nullptr)
                    m_pLines->removeLine(node->nodeIndexX(),
                                         node->nodeIndexY(),
                                         node->nodeIndexZ(),
                                         node->leftNode()->nodeIndexX(),
                                         node->leftNode()->nodeIndexY(),
                                         node->leftNode()->nodeIndexZ());
            }

            if(node->rightNode() != nullptr)
            {
                if(m_pLines != nullptr)
                    m_pLines->removeLine(node->nodeIndexX(),
                                         node->nodeIndexY(),
                                         node->nodeIndexZ(),
                                         node->rightNode()->nodeIndexX(),
                                         node->rightNode()->nodeIndexY(),
                                         node->rightNode()->nodeIndexZ());
            }

            if(node->upNode() != nullptr)
            {
                if(m_pLines != nullptr)
                    m_pLines->removeLine(node->nodeIndexX(),
                                         node->nodeIndexY(),
                                         node->nodeIndexZ(),
                                         node->upNode()->nodeIndexX(),
                                         node->upNode()->nodeIndexY(),
                                         node->upNode()->nodeIndexZ());
            }

            if(node->downNode() != nullptr)
            {
                if(m_pLines != nullptr)
                    m_pLines->removeLine(node->nodeIndexX(),
                                         node->nodeIndexY(),
                                         node->nodeIndexZ(),
                                         node->downNode()->nodeIndexX(),
                                         node->downNode()->nodeIndexY(),
                                         node->downNode()->nodeIndexZ());
            }

            if(node->frontNode() != nullptr)
            {
                if(m_pLines != nullptr)
                    m_pLines->removeLine(node->nodeIndexX(),
                                         node->nodeIndexY(),
                                         node->nodeIndexZ(),
                                         node->frontNode()->nodeIndexX(),
                                         node->frontNode()->nodeIndexY(),
                                         node->frontNode()->nodeIndexZ());
            }

            if(node->backNode() != nullptr)
            {
                if(m_pLines != nullptr)
                    m_pLines->removeLine(node->nodeIndexX(),
                                         node->nodeIndexY(),
                                         node->nodeIndexZ(),
                                         node->backNode()->nodeIndexX(),
                                         node->backNode()->nodeIndexY(),
                                         node->backNode()->nodeIndexZ());
            }
        }
    }
}

void MainWindow::onUndo()
{
    if(m_historiesBefore.length() > 0){
        qDebug() << "<< Undo";
        Voxel3DNode* node = (Voxel3DNode*)m_historiesBefore.last();
        if(node != nullptr){

            m_historiesBefore.pop_back();
            m_historiesAfter.push_front(node);
            node->undo();

            if(m_pLines != nullptr){
                m_pLines->updateVertex(node->nodeIndexX(),
                                       node->nodeIndexY(),
                                       node->nodeIndexZ(),
                                       node->position());
            }

            if(m_pNodes != nullptr){
                m_pNodes->updateVertex(node->nodeIndexX(),
                                       node->nodeIndexY(),
                                       node->nodeIndexZ(),
                                       node->position());
            }
        }
    }
}

void MainWindow::onRedo()
{
    if(m_historiesAfter.length() > 0){
        qDebug() << "<< Redo";
        Voxel3DNode* node = (Voxel3DNode*)m_historiesAfter.first();
        if(node != nullptr){
            m_historiesAfter.pop_front();
            m_historiesBefore.push_back(node);
            node->redo();

            if(m_pLines != nullptr){
                m_pLines->updateVertex(node->nodeIndexX(),
                                       node->nodeIndexY(),
                                       node->nodeIndexZ(),
                                       node->position());
            }

            if(m_pNodes != nullptr){
                m_pNodes->updateVertex(node->nodeIndexX(),
                                       node->nodeIndexY(),
                                       node->nodeIndexZ(),
                                       node->position());
            }
        }
    }
}

void MainWindow::onSelectTransparent() {
    if (ui->chkOBJTransparent->isEnabled()) {
        ui->chkOBJTransparent->setChecked(!ui->chkOBJTransparent->isChecked());
        objTransparentPropertyChanged(ui->chkOBJTransparent->isChecked());
    }
}

void MainWindow::onTriggerShowHide() {
    if (ui->chkShowPoint->isEnabled()) {
        ui->chkShowPoint->setChecked(!ui->chkShowPoint->isChecked());
        nodeVisiblePropertyChanged(ui->chkShowPoint->isChecked());
    }
}

void MainWindow::onUpOpacity() {
    if (ui->sldOBJOpacity->isEnabled()) {
        int nValue = ui->spnObjOpacity->value() + 1;
        ui->spnObjOpacity->setValue(nValue);
        ui->sldOBJOpacity->setValue(nValue);
    }
}

void MainWindow::onDownOpacity() {
    if (ui->sldOBJOpacity->isEnabled()) {
        int nValue = ui->spnObjOpacity->value() - 1;
        ui->spnObjOpacity->setValue(nValue);
        ui->sldOBJOpacity->setValue(nValue);
    }
}

void MainWindow::onViewTop_triggered()
{
    ui->widMainVoxelView->setCameraPositionTop();
    onMarkersRotating_changed();
}

void MainWindow::onViewBottom_triggered()
{
    ui->widMainVoxelView->setCameraPositionBottom();
    onMarkersRotating_changed();
}

void MainWindow::onViewFront_triggered()
{
    ui->widMainVoxelView->setCameraPositionFront();
    onMarkersRotating_changed();
}

void MainWindow::onViewBack_triggered()
{
    ui->widMainVoxelView->setCameraPositionBack();
    onMarkersRotating_changed();
}

void MainWindow::onViewLeft_triggered()
{
    ui->widMainVoxelView->setCameraPositionLeft();
    onMarkersRotating_changed();
}

void MainWindow::onViewRight_triggered()
{
    ui->widMainVoxelView->setCameraPositionRight();
    onMarkersRotating_changed();
}

void MainWindow::onViewDefault_triggered()
{
    ui->widMainVoxelView->setCameraPositionDefault();
    onMarkersRotating_changed();
}

void MainWindow::updateReactorNodes()
{
    if(m_pSelectedNode == nullptr)
        return;

    ////////////////////////////////////////
    ///                                  ///
    ///         2 ------------------6    ///
    ///        /|                  /|    ///
    ///       / |                 / |    ///
    ///      /  |                /  |    ///
    ///     /   |               /   |    ///
    ///    3-------------------7    |    ///
    ///    |    |              |    |    ///
    ///    |    |              |    |    ///
    ///    |    0 -------------|----4    ///
    ///    |   /               |   /     ///
    ///    |  /                |  /      ///
    ///    | /                 | /       ///
    ///    |/                  |/        ///
    ///    1-------------------5         ///
    ///                                  ///
    ////////////////////////////////////////

    // Reactor Candidates

    m_pNodes->clearReactorCandidates();
    m_pLines->clearLineReactorCandidates();

    if(!ui->btnSelectReactorGuide->isChecked())
        return;

    ///
    ///         |
    ///         |
    ///         |
    ///         0--------
    ///        /
    ///       /
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->frontNode()->rightNode()->upNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->rightNode()->nodeIndexX(), m_pSelectedNode->rightNode()->nodeIndexY(), m_pSelectedNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->upNode()->nodeIndexX(), m_pSelectedNode->upNode()->nodeIndexY(), m_pSelectedNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->frontNode()->nodeIndexX(), m_pSelectedNode->frontNode()->nodeIndexY(), m_pSelectedNode->frontNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->rightNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());

        pNode = m_pSelectedNode->rightNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());


    }

    ///
    ///         |
    ///         |
    ///         | /
    ///         |/
    ///         1---------
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->backNode()->rightNode()->upNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->rightNode()->nodeIndexX(), m_pSelectedNode->rightNode()->nodeIndexY(), m_pSelectedNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->upNode()->nodeIndexX(), m_pSelectedNode->upNode()->nodeIndexY(), m_pSelectedNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->backNode()->nodeIndexX(), m_pSelectedNode->backNode()->nodeIndexY(), m_pSelectedNode->backNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->rightNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());

        pNode = m_pSelectedNode->rightNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
    }

    ///
    ///         2--------
    ///       / |
    ///      /  |
    ///     /   |
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->frontNode()->rightNode()->downNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->rightNode()->nodeIndexX(), m_pSelectedNode->rightNode()->nodeIndexY(), m_pSelectedNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->downNode()->nodeIndexX(), m_pSelectedNode->downNode()->nodeIndexY(), m_pSelectedNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->frontNode()->nodeIndexX(), m_pSelectedNode->frontNode()->nodeIndexY(), m_pSelectedNode->frontNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->rightNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());

        pNode = m_pSelectedNode->rightNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
    }

    ///
    ///            /
    ///           /
    ///          /
    ///         3--------
    ///         |
    ///         |
    ///         |
    ///
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->backNode()->rightNode()->downNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->rightNode()->nodeIndexX(), m_pSelectedNode->rightNode()->nodeIndexY(), m_pSelectedNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->downNode()->nodeIndexX(), m_pSelectedNode->downNode()->nodeIndexY(), m_pSelectedNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->backNode()->nodeIndexX(), m_pSelectedNode->backNode()->nodeIndexY(), m_pSelectedNode->backNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->rightNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());

        pNode = m_pSelectedNode->rightNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
    }

    ///
    ///                  |
    ///                  |
    ///                  |
    ///          --------4
    ///                 /
    ///                /
    ///               /
    ///
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->frontNode()->leftNode()->upNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->leftNode()->nodeIndexX(), m_pSelectedNode->leftNode()->nodeIndexY(), m_pSelectedNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->upNode()->nodeIndexX(), m_pSelectedNode->upNode()->nodeIndexY(), m_pSelectedNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->frontNode()->nodeIndexX(), m_pSelectedNode->frontNode()->nodeIndexY(), m_pSelectedNode->frontNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->leftNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());

        pNode = m_pSelectedNode->leftNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
    }

    ///
    ///                  |  /
    ///                  | /
    ///                  |/
    ///          --------5
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->backNode()->leftNode()->upNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->leftNode()->nodeIndexX(), m_pSelectedNode->leftNode()->nodeIndexY(), m_pSelectedNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->upNode()->nodeIndexX(), m_pSelectedNode->upNode()->nodeIndexY(), m_pSelectedNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->backNode()->nodeIndexX(), m_pSelectedNode->backNode()->nodeIndexY(), m_pSelectedNode->backNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->leftNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());

        pNode = m_pSelectedNode->leftNode()->upNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
    }

    ///
    ///         ---------6
    ///                 /|
    ///                / |
    ///               /  |
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->frontNode()->leftNode()->downNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->leftNode()->nodeIndexX(), m_pSelectedNode->leftNode()->nodeIndexY(), m_pSelectedNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->downNode()->nodeIndexX(), m_pSelectedNode->downNode()->nodeIndexY(), m_pSelectedNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->frontNode()->nodeIndexX(), m_pSelectedNode->frontNode()->nodeIndexY(), m_pSelectedNode->frontNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->leftNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());

        pNode = m_pSelectedNode->frontNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());

        pNode = m_pSelectedNode->leftNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
    }

    ///
    ///                     /
    ///                    /
    ///                   /
    ///         ---------7
    ///                  |
    ///                  |
    ///                  |
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        Voxel3DNode* pNode = m_pSelectedNode->backNode()->leftNode()->downNode();
        m_pNodes->setReactorCandidate(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());

        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->leftNode()->nodeIndexX(), m_pSelectedNode->leftNode()->nodeIndexY(), m_pSelectedNode->leftNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->downNode()->nodeIndexX(), m_pSelectedNode->downNode()->nodeIndexY(), m_pSelectedNode->downNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(m_pSelectedNode->nodeIndexX(), m_pSelectedNode->nodeIndexY(), m_pSelectedNode->nodeIndexZ(),
                                           m_pSelectedNode->backNode()->nodeIndexX(), m_pSelectedNode->backNode()->nodeIndexY(), m_pSelectedNode->backNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->leftNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->downNode()->nodeIndexX(), pNode->downNode()->nodeIndexY(), pNode->downNode()->nodeIndexZ());

        pNode = m_pSelectedNode->backNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->frontNode()->nodeIndexX(), pNode->frontNode()->nodeIndexY(), pNode->frontNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->leftNode()->nodeIndexX(), pNode->leftNode()->nodeIndexY(), pNode->leftNode()->nodeIndexZ());

        pNode = m_pSelectedNode->leftNode()->downNode();
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->rightNode()->nodeIndexX(), pNode->rightNode()->nodeIndexY(), pNode->rightNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->upNode()->nodeIndexX(), pNode->upNode()->nodeIndexY(), pNode->upNode()->nodeIndexZ());
        m_pLines->addLineReactorCandidates(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ(),
                                           pNode->backNode()->nodeIndexX(), pNode->backNode()->nodeIndexY(), pNode->backNode()->nodeIndexZ());
    }

    foreach(Voxel3DNode* node, m_nodeList){
        if(node != nullptr)
            node->setReactor(-1);
    }

    //    foreach(Voxel3DNode* node, m_reactorNodeList){
    //        if(node != nullptr){
    //            node->deleteLater();
    //            node = nullptr;
    //        }
    //    }
    m_reactorNodeList.clear();

    if (ui->btnSelectReactorMode1->isChecked())
    {
        ///
        ///         |
        ///         |
        ///         |
        ///         0--------
        ///        /
        ///       /
        ///

        if(m_pSelectedNode->rightNode() != nullptr &&
                m_pSelectedNode->upNode() != nullptr &&
                m_pSelectedNode->frontNode() != nullptr)
        {
            m_pSelectedNode->setReactor(0);
            m_pSelectedNode->frontNode()->setReactor(1);
            m_pSelectedNode->upNode()->setReactor(2);
            m_pSelectedNode->upNode()->frontNode()->setReactor(3);
            m_pSelectedNode->rightNode()->setReactor(4);
            m_pSelectedNode->rightNode()->frontNode()->setReactor(5);
            m_pSelectedNode->rightNode()->upNode()->setReactor(6);
            m_pSelectedNode->rightNode()->upNode()->frontNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->upNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->upNode()->frontNode());
        }
    } else if(ui->btnSelectReactorMode2->isChecked())
    {
        ///
        ///         |
        ///         |
        ///         | /
        ///         |/
        ///         1---------
        ///

        if(m_pSelectedNode->rightNode() != nullptr &&
                m_pSelectedNode->upNode() != nullptr &&
                m_pSelectedNode->backNode() != nullptr)
        {

            m_pSelectedNode->backNode()->setReactor(0);
            m_pSelectedNode->setReactor(1);
            m_pSelectedNode->backNode()->upNode()->setReactor(2);
            m_pSelectedNode->upNode()->setReactor(3);
            m_pSelectedNode->rightNode()->backNode()->setReactor(4);
            m_pSelectedNode->rightNode()->setReactor(5);
            m_pSelectedNode->rightNode()->backNode()->upNode()->setReactor(6);
            m_pSelectedNode->rightNode()->upNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->backNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->backNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->upNode());
        }
    }else if(ui->btnSelectReactorMode3->isChecked())
    {
        ///
        ///         2--------
        ///       / |
        ///      /  |
        ///     /   |
        ///
        if(m_pSelectedNode->rightNode() != nullptr &&
                m_pSelectedNode->downNode() != nullptr &&
                m_pSelectedNode->frontNode() != nullptr)
        {

            m_pSelectedNode->downNode()->setReactor(0);
            m_pSelectedNode->downNode()->frontNode()->setReactor(1);
            m_pSelectedNode->setReactor(2);
            m_pSelectedNode->frontNode()->setReactor(3);
            m_pSelectedNode->rightNode()->downNode()->setReactor(4);
            m_pSelectedNode->rightNode()->downNode()->frontNode()->setReactor(5);
            m_pSelectedNode->rightNode()->setReactor(6);
            m_pSelectedNode->frontNode()->rightNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->downNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->downNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode());
            m_reactorNodeList.push_back(m_pSelectedNode->frontNode()->rightNode());
        }
    } else if(ui->btnSelectReactorMode4->isChecked())
    {
        ///
        ///            /
        ///           /
        ///          /
        ///         3--------
        ///         |
        ///         |
        ///         |
        ///
        ///
        if(m_pSelectedNode->rightNode() != nullptr &&
                m_pSelectedNode->downNode() != nullptr &&
                m_pSelectedNode->backNode() != nullptr)
        {
            m_pSelectedNode->downNode()->backNode()->setReactor(0);
            m_pSelectedNode->downNode()->setReactor(1);
            m_pSelectedNode->backNode()->setReactor(2);
            m_pSelectedNode->setReactor(3);
            m_pSelectedNode->rightNode()->downNode()->backNode()->setReactor(4);
            m_pSelectedNode->rightNode()->downNode()->setReactor(5);
            m_pSelectedNode->rightNode()->backNode()->setReactor(6);
            m_pSelectedNode->rightNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->downNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->downNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->rightNode());
        }
    } else if(ui->btnSelectReactorMode5->isChecked())
    {
        ///
        ///                  |
        ///                  |
        ///                  |
        ///          --------4
        ///                 /
        ///                /
        ///               /
        ///
        ///
        ///

        if(m_pSelectedNode->leftNode() != nullptr &&
                m_pSelectedNode->upNode() != nullptr &&
                m_pSelectedNode->frontNode() != nullptr)
        {

            m_pSelectedNode->leftNode()->setReactor(0);
            m_pSelectedNode->leftNode()->frontNode()->setReactor(1);
            m_pSelectedNode->leftNode()->upNode()->setReactor(2);
            m_pSelectedNode->leftNode()->upNode()->frontNode()->setReactor(3);
            m_pSelectedNode->setReactor(4);
            m_pSelectedNode->frontNode()->setReactor(5);
            m_pSelectedNode->upNode()->setReactor(6);
            m_pSelectedNode->upNode()->frontNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->leftNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->upNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->upNode()->frontNode());
        }
    } else if(ui->btnSelectReactorMode6->isChecked())
    {
        ///
        ///                  |  /
        ///                  | /
        ///                  |/
        ///          --------5
        ///
        ///

        if(m_pSelectedNode->leftNode() != nullptr &&
                m_pSelectedNode->upNode() != nullptr &&
                m_pSelectedNode->backNode() != nullptr)
        {

            m_pSelectedNode->leftNode()->backNode()->setReactor(0);
            m_pSelectedNode->leftNode()->setReactor(1);
            m_pSelectedNode->leftNode()->backNode()->upNode()->setReactor(2);
            m_pSelectedNode->leftNode()->upNode()->setReactor(3);
            m_pSelectedNode->backNode()->setReactor(4);
            m_pSelectedNode->setReactor(5);
            m_pSelectedNode->backNode()->upNode()->setReactor(6);
            m_pSelectedNode->upNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->backNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->backNode()->upNode());
            m_reactorNodeList.push_back(m_pSelectedNode->upNode());
        }
    } else if(ui->btnSelectReactorMode7->isChecked())
    {
        ///
        ///         ---------6
        ///                 /|
        ///                / |
        ///               /  |
        ///
        ///
        if(m_pSelectedNode->leftNode() != nullptr &&
                m_pSelectedNode->downNode() != nullptr &&
                m_pSelectedNode->frontNode() != nullptr)
        {

            m_pSelectedNode->leftNode()->downNode()->setReactor(0);
            m_pSelectedNode->leftNode()->downNode()->frontNode()->setReactor(1);
            m_pSelectedNode->leftNode()->setReactor(2);
            m_pSelectedNode->leftNode()->frontNode()->setReactor(3);
            m_pSelectedNode->downNode()->setReactor(4);
            m_pSelectedNode->downNode()->frontNode()->setReactor(5);
            m_pSelectedNode->setReactor(6);
            m_pSelectedNode->frontNode()->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->downNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->downNode()->frontNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
            m_reactorNodeList.push_back(m_pSelectedNode->frontNode());
        }
    } else if(ui->btnSelectReactorMode8->isChecked())
    {

        ///
        ///                     /
        ///                    /
        ///                   /
        ///         ---------7
        ///                  |
        ///                  |
        ///                  |
        ///
        ///
        if(m_pSelectedNode->leftNode() != nullptr &&
                m_pSelectedNode->downNode() != nullptr &&
                m_pSelectedNode->backNode() != nullptr)
        {

            m_pSelectedNode->leftNode()->downNode()->backNode()->setReactor(0);
            m_pSelectedNode->leftNode()->downNode()->setReactor(1);
            m_pSelectedNode->leftNode()->backNode()->setReactor(2);
            m_pSelectedNode->leftNode()->setReactor(3);
            m_pSelectedNode->downNode()->backNode()->setReactor(4);
            m_pSelectedNode->downNode()->setReactor(5);
            m_pSelectedNode->backNode()->setReactor(6);
            m_pSelectedNode->setReactor(7);

            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->downNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->leftNode());
            m_reactorNodeList.push_back(m_pSelectedNode->downNode()->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode->downNode());
            m_reactorNodeList.push_back(m_pSelectedNode->backNode());
            m_reactorNodeList.push_back(m_pSelectedNode);
        }
    }

    if(m_pSelectedNode != nullptr) {
        if(m_pSelectedNode->isReactor()){
            qDebug() << ">> Creating a box ...";
            VoxelBox* box = nullptr;
            bool box_is_registered = false;
            foreach(VoxelBox* b, m_boxList){
                if(b == m_reactorBox){
                    box_is_registered = true;
                    qDebug() << "\t Box is registered";
                }
                if(b->isEqual(m_reactorNodeList)){
                    box = b;
                    qDebug() << "\t Detected same box";
                }
            }

            if(box == nullptr){
                if(box_is_registered || m_reactorBox == nullptr){
                    box = new VoxelBox(m_pPointRootEntity);
                    box->initialize();

                    qDebug() << "\t Created new box";
                    box->addComponent(ui->widMainVoxelView->getTransparentLayer());
                    m_reactorBox = box;
                }else{
                    qDebug() << "\t Updated Box Nodes";
                    m_reactorBox->updateNodes(m_reactorNodeList);
                }
                if (!ui->chkReactorShow->isChecked()) {
                    m_reactorBox->setColor(m_boxColor);
                } else {
                    m_reactorBox->setColor(Qt::transparent);
                    m_reactorBox->getMaterial()->setAlpha(0);
                }
            }else{
                if(!box_is_registered){
                    qDebug() << "\t Deleted Box";
                    m_reactorBox->deleteLater();
                    m_reactorBox = nullptr;
                }

                if(box_is_registered || m_reactorBox == nullptr){
                    qDebug() << "\t Changed box pointer";
                    m_reactorBox = box;
                }

            }

            qDebug() << "<< Created a box";
            qDebug() << ": Count of box are " << m_boxList.count();
            updateReactorView();
        } else {
            qDebug() << "\t Deleted Box -----   AAAA";
            m_reactorBox->deleteLater();
            m_reactorBox = nullptr;
            updateReactorView();
        }
    }
}

void MainWindow::createReactorView()
{
    ///////////////////////////////////////////
    ///////         Reactor        ////////////
    // Create Root Entity
    m_reactorNodeList.clear();
    m_reactorTotalNodeList.clear();

    m_pReactorContentsRootEntity = new Qt3DCore::QEntity(m_pReactorRootEntity);
    int size_x, size_y, size_z;
    size_x = size_y = size_z = 4;

    ui->widReactorView->initWorkspace(size_y, m_spacingSize);
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for( int z = 0; z < size_z; z++){
                Voxel3DNode* node = new Voxel3DNode(m_pReactorContentsRootEntity);

                QVector3D pos = (QVector3D(x , y, z) - QVector3D(static_cast<float>(size_x - 1) / 2.0f,
                                                                 static_cast<float>(size_y - 1) / 2.0f,
                                                                 static_cast<float>(size_z - 1) / 2.0f)) * m_spacingSize;
                node->setNodeIndex(static_cast<int8_t>(x),
                                   static_cast<int8_t>(y),
                                   static_cast<int8_t>(z));
                node->updateCenterPosition(pos);
                node->removePickerComponent();
                m_reactorTotalNodeList.push_back(node);
            }
        }
    }

    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            for( int z = 0; z < size_z; z++){
                int i = z + y * size_z + x * size_y * size_z;
                if(x + 1 < size_x)
                {
                    int i_right = z + y * size_z + (x + 1) * size_y * size_z; // x + 1
                    if(m_reactorTotalNodeList.at(i) == nullptr || m_reactorTotalNodeList.at(i_right) == nullptr)
                        return;
                    m_reactorTotalNodeList.at(i)->setRightNode(m_reactorTotalNodeList.at(i_right));
                    m_reactorTotalNodeList.at(i_right)->setLeftNode(m_reactorTotalNodeList.at(i));

                    if(x != 1 || (x == 1 && (y == 1 || y == 2) && (z == 1 || z == 2)))
                    {
                        Voxel3DLine* line = new Voxel3DLine(m_pReactorContentsRootEntity);
                        line->setPoints(m_reactorTotalNodeList.at(i)->position(), m_reactorTotalNodeList.at(i_right)->position());
                        if(x == 1)
                        {
                            m_reactorTotalNodeList.at(i)->setVisible();
                            m_reactorTotalNodeList.at(i_right)->setVisible();
                            line->setColor(Qt::red);
                            if(y == 1 && z == 1){
                                m_reactorTotalNodeList.at(i)->setReactor(0);
                                m_reactorTotalNodeList.at(i_right)->setReactor(4);
                            }else if(y == 1 && z == 2){
                                m_reactorTotalNodeList.at(i)->setReactor(1);
                                m_reactorTotalNodeList.at(i_right)->setReactor(5);
                            }else if(y == 2 && z == 1){
                                m_reactorTotalNodeList.at(i)->setReactor(2);
                                m_reactorTotalNodeList.at(i_right)->setReactor(6);
                            }else if(y == 2 && z == 2){
                                m_reactorTotalNodeList.at(i)->setReactor(3);
                                m_reactorTotalNodeList.at(i_right)->setReactor(7);
                            }
                        }else
                            line->setColor(Qt::gray);
                        m_reactorTotalNodeList.at(i)->setRightLine(line);
                        m_reactorTotalNodeList.at(i_right)->setLeftLine(line);
                    }
                }
                if(y + 1 < size_y)
                {
                    int i_up = z + (y + 1) * size_z + x * size_y * size_z; // y + 1
                    m_reactorTotalNodeList.at(i)->setUpNode(m_reactorTotalNodeList.at(i_up));
                    m_reactorTotalNodeList.at(i_up)->setDownNode(m_reactorTotalNodeList.at(i));
                    if(m_reactorTotalNodeList.at(i) == nullptr || m_reactorTotalNodeList.at(i_up) == nullptr)
                        return;
                    if(y != 1 || (y == 1 && (x == 1 || x == 2) && (z == 1 || z == 2)))
                    {
                        Voxel3DLine* line = new Voxel3DLine(m_pReactorContentsRootEntity);
                        line->setPoints(m_reactorTotalNodeList.at(i)->position(), m_reactorTotalNodeList.at(i_up)->position());
                        if(y == 1)
                        {
                            m_reactorTotalNodeList.at(i)->setVisible();
                            m_reactorTotalNodeList.at(i_up)->setVisible();
                            line->setColor(Qt::red);

                        }
                        else
                            line->setColor(Qt::gray);
                        m_reactorTotalNodeList.at(i)->setUpLine(line);
                        m_reactorTotalNodeList.at(i_up)->setDownLine(line);
                    }
                }
                if(z + 1 < size_z)
                {
                    int i_front = (z + 1) + y * size_z + x * size_y * size_z; // z + 1
                    m_reactorTotalNodeList.at(i)->setFrontNode(m_reactorTotalNodeList.at(i_front));
                    m_reactorTotalNodeList.at(i_front)->setBackNode(m_reactorTotalNodeList.at(i));
                    if(m_reactorTotalNodeList.at(i) == nullptr || m_reactorTotalNodeList.at(i_front) == nullptr)
                        return;

                    if(z != 1 || (z == 1 && (x == 1 || x == 2) && (y == 1 || y == 2)))
                    {
                        Voxel3DLine* line = new Voxel3DLine(m_pReactorContentsRootEntity);
                        line->setPoints(m_reactorTotalNodeList.at(i)->position(), m_reactorTotalNodeList.at(i_front)->position());
                        if(z == 1)
                        {
                            m_reactorTotalNodeList.at(i)->setVisible();
                            m_reactorTotalNodeList.at(i_front)->setVisible();
                            line->setColor(Qt::red);
                        }
                        else
                            line->setColor(Qt::gray);
                        m_reactorTotalNodeList.at(i)->setFrontLine(line);
                        m_reactorTotalNodeList.at(i_front)->setBackLine(line);
                    }
                }
            }
        }
    }
}

void MainWindow::updateReactorView()
{
    qDebug() << ">> UpdateReactor View";
    if(m_reactorNodeList.count() != 8)
    {
//        ui->widReactorView->setCameraOff();
//        ui->widSelectedCoordinates->setVisible(false);
        return;
    }

    if(ui->widReactorView->isCameraOff())
        ui->widReactorView->setCameraOrthographicProjection();

    if(m_reactorBox != nullptr)
        m_reactorBox->updateNodes(m_reactorNodeList);

    int size_x, size_y, size_z;
    size_x = size_y = size_z = 4;

    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            for( int z = 0; z < 2; z++){
                int i = z + y * 2 + x * 2 * 2;
                int i_map = (z + 1) + (y + 1) * size_z + (x + 1) * size_y * size_z;

                if(i_map < m_reactorTotalNodeList.count()){
                    if(m_reactorNodeList.at(i) != nullptr && m_reactorTotalNodeList.at(i_map) != nullptr)
                        m_reactorTotalNodeList.at(i_map)->updateLocalPosition(m_reactorNodeList.at(i)->localPosition());
                }
            }
        }
    }

    ////////////////////////////////////////
    ///                                  ///
    ///         2 ------------------6    ///
    ///        /|                  /|    ///
    ///       / |                 / |    ///
    ///      /  |                /  |    ///
    ///     /   |               /   |    ///
    ///    3-------------------7    |    ///
    ///    |    |              |    |    ///
    ///    |    |              |    |    ///
    ///    |    0 -------------|----4    ///
    ///    |   /               |   /     ///
    ///    |  /                |  /      ///
    ///    | /                 | /       ///
    ///    |/                  |/        ///
    ///    1-------------------5         ///
    ///                                  ///
    ////////////////////////////////////////
    ///

    if(ui->chkNegetiveCopier->isChecked()){
        ui->edtCoor1X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(0)->localPosition().x())));
        ui->edtCoor1Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(0)->localPosition().y())));
        ui->edtCoor1Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(0)->localPosition().z())));

        ui->edtCoor2X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(1)->localPosition().x())));
        ui->edtCoor2Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(1)->localPosition().y())));
        ui->edtCoor2Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(1)->localPosition().z())));

        ui->edtCoor3X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(2)->localPosition().x())));
        ui->edtCoor3Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(2)->localPosition().y())));
        ui->edtCoor3Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(2)->localPosition().z())));

        ui->edtCoor4X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(3)->localPosition().x())));
        ui->edtCoor4Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(3)->localPosition().y())));
        ui->edtCoor4Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(3)->localPosition().z())));

        ui->edtCoor5X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(4)->localPosition().x())));
        ui->edtCoor5Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(4)->localPosition().y())));
        ui->edtCoor5Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(4)->localPosition().z())));

        ui->edtCoor6X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(5)->localPosition().x())));
        ui->edtCoor6Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(5)->localPosition().y())));
        ui->edtCoor6Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(5)->localPosition().z())));

        ui->edtCoor7X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(6)->localPosition().x())));
        ui->edtCoor7Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(6)->localPosition().y())));
        ui->edtCoor7Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(6)->localPosition().z())));

        ui->edtCoor8X->setText(QString::number(static_cast<double>(m_reactorNodeList.at(7)->localPosition().x())));
        ui->edtCoor8Y->setText(QString::number(static_cast<double>(m_reactorNodeList.at(7)->localPosition().y())));
        ui->edtCoor8Z->setText(QString::number(static_cast<double>(m_reactorNodeList.at(7)->localPosition().z())));

    }else{
        ui->edtCoor1X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(0)->localPosition().x()))));
        ui->edtCoor1Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(0)->localPosition().y()))));
        ui->edtCoor1Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(0)->localPosition().z()))));

        ui->edtCoor2X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(1)->localPosition().x()))));
        ui->edtCoor2Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(1)->localPosition().y()))));
        ui->edtCoor2Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(1)->localPosition().z()))));

        ui->edtCoor3X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(2)->localPosition().x()))));
        ui->edtCoor3Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(2)->localPosition().y()))));
        ui->edtCoor3Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(2)->localPosition().z()))));

        ui->edtCoor4X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(3)->localPosition().x()))));
        ui->edtCoor4Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(3)->localPosition().y()))));
        ui->edtCoor4Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(3)->localPosition().z()))));

        ui->edtCoor5X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(4)->localPosition().x()))));
        ui->edtCoor5Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(4)->localPosition().y()))));
        ui->edtCoor5Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(4)->localPosition().z()))));

        ui->edtCoor6X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(5)->localPosition().x()))));
        ui->edtCoor6Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(5)->localPosition().y()))));
        ui->edtCoor6Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(5)->localPosition().z()))));

        ui->edtCoor7X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(6)->localPosition().x()))));
        ui->edtCoor7Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(6)->localPosition().y()))));
        ui->edtCoor7Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(6)->localPosition().z()))));

        ui->edtCoor8X->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(7)->localPosition().x()))));
        ui->edtCoor8Y->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(7)->localPosition().y()))));
        ui->edtCoor8Z->setText(QString::number(abs(static_cast<double>(m_reactorNodeList.at(7)->localPosition().z()))));
    }
}

void MainWindow::copyReactorPointsToClipboard()
{
    if (m_pSelectedNode == nullptr)
        return;

    if(m_pSelectedNode->isReactor())
        updateReactorView();

    QString reactor_position = "";

    if (ui->btnSelectReactorGuide->isChecked() == true && m_reactorNodeList.count() == 8) {
        if(ui->chkNegetiveCopier->isChecked()){
            reactor_position += QString::number((m_reactorNodeList.at(0)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(0)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(0)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(1)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(1)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(1)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(2)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(2)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(2)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(3)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(3)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(3)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(4)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(4)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(4)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(5)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(5)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(5)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(6)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(6)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(6)->localPosition().z())) + ";";
            reactor_position += QString::number((m_reactorNodeList.at(7)->localPosition().x())) + "," +
                    QString::number((m_reactorNodeList.at(7)->localPosition().y())) + "," +
                    QString::number((m_reactorNodeList.at(7)->localPosition().z()));
        }else{
            reactor_position += QString::number(abs(m_reactorNodeList.at(0)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(0)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(0)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(1)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(1)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(1)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(2)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(2)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(2)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(3)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(3)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(3)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(4)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(4)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(4)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(5)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(5)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(5)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(6)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(6)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(6)->localPosition().z())) + ";";
            reactor_position += QString::number(abs(m_reactorNodeList.at(7)->localPosition().x())) + "," +
                    QString::number(abs(m_reactorNodeList.at(7)->localPosition().y())) + "," +
                    QString::number(abs(m_reactorNodeList.at(7)->localPosition().z()));
        }

        QClipboard *p_Clipboard = QApplication::clipboard();
        p_Clipboard->setText(reactor_position);
    }
}

void MainWindow::onSelectReactorGuide_triggered(bool f)
{
    ui->btnSelectReactorGuide->setChecked(f);

    ui->btnSelectReactorMode1->setEnabled(f);
    ui->btnSelectReactorMode2->setEnabled(f);
    ui->btnSelectReactorMode3->setEnabled(f);
    ui->btnSelectReactorMode4->setEnabled(f);
    ui->btnSelectReactorMode5->setEnabled(f);
    ui->btnSelectReactorMode6->setEnabled(f);
    ui->btnSelectReactorMode7->setEnabled(f);
    ui->btnSelectReactorMode8->setEnabled(f);

//    ui->widSelectedCoordinates->setVisible(f);

    updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode1_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode1->setChecked(true);

    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor1;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);

    //////////////////////////////////////

//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode2_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode2->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor2;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode3_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode3->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor3;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode4_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode4->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor4;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode5_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode5->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor5;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode6_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode6->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor6;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode7_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode7->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode8->setChecked(false);

    m_boxColor = m_boxColor7;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onSelectReactorMode8_triggered()
{
    //////////////////////////////////////
    ui->btnSelectReactorMode8->setChecked(true);

    ui->btnSelectReactorMode1->setChecked(false);
    ui->btnSelectReactorMode2->setChecked(false);
    ui->btnSelectReactorMode3->setChecked(false);
    ui->btnSelectReactorMode4->setChecked(false);
    ui->btnSelectReactorMode5->setChecked(false);
    ui->btnSelectReactorMode6->setChecked(false);
    ui->btnSelectReactorMode7->setChecked(false);

    m_boxColor = m_boxColor8;
    QString colorStyle = QString("QPushButton{ \
                                 background-color: %1; \
           border:none; \
   }\
   QPushButton:hover{ \
       background-color: %2; \
   border:none; \
   } \
   QPushButton:pressed{ \
       background-color: %3; \
   border:none; \
   }").arg(m_boxColor.name(), m_boxColor.lighter(115).name(), m_boxColor.lighter(120).name());
    ui->btn_ChooseBoxColor->setStyleSheet(colorStyle);
    //////////////////////////////////////
//    if (!ui->chkReactorShow->isChecked())
        updateReactorNodes();
    ui->widReactorView->setCameraPositionDefault();
}

void MainWindow::onCopyPoints_triggered()
{
    QString fileName = "voxeltemplate.tmp";
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);

        if(file.isOpen())
        {
            QTextStream outStream(&file);

            int minX, minY, minZ, maxX, maxY, maxZ;
            minX = minY = minZ = 1000000;
            maxX = maxY = maxZ = 0;
            foreach(Voxel3DNode* node, m_selectedNodeList){
                if(node->nodeIndexX() < minX)
                    minX = node->nodeIndexX();
                if(node->nodeIndexY() < minY)
                    minY = node->nodeIndexY();
                if(node->nodeIndexZ() < minZ)
                    minZ = node->nodeIndexZ();

                if(node->nodeIndexX() > maxX)
                    maxX = node->nodeIndexX();
                if(node->nodeIndexY() > maxY)
                    maxY = node->nodeIndexY();
                if(node->nodeIndexZ() > maxZ)
                    maxZ = node->nodeIndexZ();
            }

            outStream << maxX - minX + 1 << " " <<
                         maxY - minY + 1 << " " <<
                         maxZ - minZ + 1 << endl;

            qDebug() << "CREATE TEMPLATE";
            foreach(Voxel3DNode* node, m_selectedNodeList){
//                qDebug() << node->nodeIndexX()-minX << "," << node->nodeIndexY()-minY << "," << node->nodeIndexZ() - minZ;
                outStream << node->isVisible() << " " <<
                             node->localPosition().x() << " " <<
                             node->localPosition().y() << " " <<
                             node->localPosition().z() << " " << endl;
            }

            file.close();
        }
    }
}

void MainWindow::onPastePoints_triggered()
{
    if(m_pSelectedNode == nullptr)
        return;

    if(m_selectedNodeList.length() > 1 && !m_isRangeSelected)
        return;

    QString fileName = "voxeltemplate.tmp";

    if (!fileName.isEmpty()){
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);

        if(file.isOpen())
        {
            qDebug() << "RESTORE TEMPLATE";
            int cnt, vsble, local_x, local_y, local_z;

            QTextStream inStream(&file);
            int sizeX, sizeY, sizeZ;
            inStream >> sizeX >>
                    sizeY >>
                    sizeZ;

            int startIndexX, startIndexY, startIndexZ;
            int endIndexX, endIndexY, endIndexZ;
            if(m_isRangeSelected){
                int minX, minY, minZ, maxX, maxY, maxZ;
                minX = minY = minZ = 1000000;
                maxX = maxY = maxZ = 0;
                foreach(Voxel3DNode* node, m_selectedNodeList){
                    if(node->nodeIndexX() < minX)
                        minX = node->nodeIndexX();
                    if(node->nodeIndexY() < minY)
                        minY = node->nodeIndexY();
                    if(node->nodeIndexZ() < minZ)
                        minZ = node->nodeIndexZ();

                    if(node->nodeIndexX() > maxX)
                        maxX = node->nodeIndexX();
                    if(node->nodeIndexY() > maxY)
                        maxY = node->nodeIndexY();
                    if(node->nodeIndexZ() > maxZ)
                        maxZ = node->nodeIndexZ();
                }

                startIndexX = minX;
                startIndexY = minY;
                startIndexZ = minZ;

                endIndexX = maxX;
                endIndexY = maxY;
                endIndexZ = maxZ;
            }else{
                startIndexX = m_pSelectedNode->nodeIndexX();
                startIndexY = m_pSelectedNode->nodeIndexY();
                startIndexZ = m_pSelectedNode->nodeIndexZ();

                endIndexX = m_workspaceSizeX - 1;
                endIndexY = m_workspaceSizeY - 1;
                endIndexZ = m_workspaceSizeZ - 1;
            }


            for (int x = 0; x < sizeX; x++) {
                for (int y = 0; y < sizeY; y++) {
                    for( int z = 0; z < sizeZ; z++){
                        inStream >> vsble >>
                                local_x >>
                                local_y >>
                                local_z;

                        if(startIndexX + x <= endIndexX &&
                                startIndexY + y <= endIndexY &&
                                startIndexZ + z <= endIndexZ)
                        {
                            int index = (startIndexZ + z) + (startIndexY + y) * m_workspaceSizeZ + (startIndexX + x) * m_workspaceSizeY * m_workspaceSizeZ;
                            Voxel3DNode* node = m_nodeList.at(index);
                            node->updateLocalPosition(QVector3D(local_x, local_y, local_z));

                            if(m_pLines != nullptr)
                                m_pLines->updateVertex(node->nodeIndexX(),
                                                       node->nodeIndexY(),
                                                       node->nodeIndexZ(),
                                                       node->position());
                            if(m_pNodes != nullptr)
                                m_pNodes->updateVertex(node->nodeIndexX(),
                                                       node->nodeIndexY(),
                                                       node->nodeIndexZ(),
                                                       node->position());
                            if(node->isVisible() != vsble){
                                node->setVisible(vsble);
                                changeNodeVisibleProperty(node);
                            }
                        }
                    }
                }
            }

        }
        file.close();
    }
}

void MainWindow::onVoxelClicked(int index_x, int index_y, int index_z, QVector3D pos){
    qDebug() << "Clicked Index : " << index_x << "," << index_y << "," << index_z << " : " << pos;

    foreach(Voxel3DNode* node, m_nodeList)
    {
        if(node->nodeIndexX() == index_x && node->nodeIndexY() == index_y && node->nodeIndexZ() == index_z){
            node->emitClicked();
            break;
        }
    }
}

void MainWindow::onVoxelCtrlClicked(int index_x, int index_y, int index_z, QVector3D pos){
    qDebug() << "Ctrl + Clicked : INDEX : " << index_x << "," << index_y << "," << index_z << " : " << pos;

    foreach(Voxel3DNode* node, m_nodeList)
    {
        if(node->nodeIndexX() == index_x && node->nodeIndexY() == index_y && node->nodeIndexZ() == index_z){
            node->emitCtrlClicked();
            break;
        }
    }
}

void MainWindow::onVoxelShiftClicked(int index_x, int index_y, int index_z, QVector3D pos){
    qDebug() << "Shift + Clicked : INDEX :  " << index_x << "," << index_y << "," << index_z << " : " << pos;

    foreach(Voxel3DNode* node, m_nodeList)
    {
        if(node->nodeIndexX() == index_x && node->nodeIndexY() == index_y && node->nodeIndexZ() == index_z){
            node->emitShiftClicked();
            break;
        }
    }
}

void MainWindow::onXArrowPressed(Qt3DRender::QPickEvent* evt){
    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    qDebug() << "X arrow pressed";
    m_nSelectedArrowNumber = 1;     //X arrow selected
    m_pSelectedNode->setStartPosition(m_pSelectedNode->position());
    ui->widMainVoxelView->setArrowSelectedFlag(true);
}

void MainWindow::onXArrowEntered(){
    qDebug() << "X arrow hovered";
    if (m_bArrowHovered == false) {
        m_pArrow->setXArrowColor(QColor(150, 150, 50, 127));
        m_nSelectedArrowNumber = 1;     //X arrow selected
        m_pSelectedNode->setStartPosition(m_pSelectedNode->position());
        m_bArrowHovered = true;
    }
}

void MainWindow::onXArrowExited(){
    qDebug() << "X arrow exited";
    if (ui->widMainVoxelView->getArrowSelectedFlag() == false) {
        m_pArrow->setXArrowColor(Qt::red);
        m_bArrowHovered = false;
    }
}

void MainWindow::onYArrowPressed(Qt3DRender::QPickEvent* evt){
    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    qDebug() << "Y arrow Pressed";
    m_nSelectedArrowNumber = 2;     //Y arrow selected
    m_pSelectedNode->setStartPosition(m_pSelectedNode->position());
    ui->widMainVoxelView->setArrowSelectedFlag(true);
}

void MainWindow::onYArrowEntered(){
    qDebug() << "Y arrow hovered";
    if (m_bArrowHovered == false) {
        m_pArrow->setYArrowColor(QColor(150, 150, 50, 127));
        m_nSelectedArrowNumber = 2;     //Y arrow selected
        m_pSelectedNode->setStartPosition(m_pSelectedNode->position());
        m_bArrowHovered = true;
    }
}

void MainWindow::onYArrowExited(){
    qDebug() << "Y arrow exited";
    if (ui->widMainVoxelView->getArrowSelectedFlag() == false) {
        m_pArrow->setYArrowColor(Qt::green);
        m_bArrowHovered = false;
    }
}

void MainWindow::onZArrowPressed(Qt3DRender::QPickEvent* evt){
    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    qDebug() << "Z moved";
    m_nSelectedArrowNumber = 3;     //Z arrow selected
    m_pSelectedNode->setStartPosition(m_pSelectedNode->position());
    ui->widMainVoxelView->setArrowSelectedFlag(true);
}

void MainWindow::onZArrowEntered(){
    qDebug() << "Z arrow hovered";
    if (m_bArrowHovered == false) {
        m_pArrow->setZArrowColor(QColor(150, 150, 50, 127));
        m_nSelectedArrowNumber = 3;     //Z arrow selected
        m_pSelectedNode->setStartPosition(m_pSelectedNode->position());
        m_bArrowHovered = true;
    }
}

void MainWindow::onZArrowExited(){
    qDebug() << "Z arrow exited";
    if (ui->widMainVoxelView->getArrowSelectedFlag() == false) {
        m_pArrow->setZArrowColor(Qt::blue);
        m_bArrowHovered = false;
    }
}

int MainWindow::getSelectedArrowNumber() {
    return m_nSelectedArrowNumber;
}

void MainWindow::setSelectedArrowNumber(int n) {
    m_nSelectedArrowNumber = n;
}

void MainWindow::onArrowMoving_changed(QPoint pos) {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();

    pos.setY(ui->widMainVoxelView->rect().height() - pos.y());      //Change the coordinage to the 3d screeen view. Up->Right
    QPoint posStart = ui->widMainVoxelView->getPosStart();
    posStart.setY(ui->widMainVoxelView->rect().height() - posStart.y());

    if(m_pSelectedNode != nullptr)
    {
        QRect viewRect(ui->widMainVoxelView->rect().left(), ui->widMainVoxelView->rect().top(), ui->widMainVoxelView->rect().width() * ui->widMainVoxelView->getZoom(), ui->widMainVoxelView->rect().height() * ui->widMainVoxelView->getZoom());
        pos.setX(pos.x() * ui->widMainVoxelView->getZoom());
        pos.setY(pos.y() * ui->widMainVoxelView->getZoom());
        posStart.setX(posStart.x() * ui->widMainVoxelView->getZoom());
        posStart.setY(posStart.y() * ui->widMainVoxelView->getZoom());

        QVector3D selectedPos3D = m_pSelectedNode->startPosition().project(pCamera->viewMatrix(), pCamera->projectionMatrix(), viewRect);

        double alpha1 = qAtan2(pos.y() - selectedPos3D.y(), pos.x() - selectedPos3D.x());
        double alpha2 = qAtan2(posStart.y() - selectedPos3D.y(), posStart.x() - selectedPos3D.x());
        double alpha = alpha1 - alpha2;
        double length1 = qSqrt(qPow(pos.y() - selectedPos3D.y(), 2) + qPow(pos.x() - selectedPos3D.x(), 2));
        double length2 = qSqrt(qPow(posStart.y() - selectedPos3D.y(), 2) + qPow(posStart.x() - selectedPos3D.x(), 2));
        double length3 = qCos(alpha) * length1;
        double length = (length3 - length2) / (ui->widMainVoxelView->getZoom() * ui->widMainVoxelView->getZoom());
        float fSpacingRatio = m_spacingSize * (0.7f/72.0f) + (16.0f/72.0f);        //80 => 1; 8 => 0.3
        float fCubeRatio = m_workspaceSizeY * 0.7f / 8 + 1.0f / 8;
        length *= (fSpacingRatio * fCubeRatio);

        if (m_nSelectedArrowNumber == 1) {
            // X Arrow
            double difference = m_pSelectedNode->startPosition().x() - m_pSelectedNode->centerPosition().x() + length;
            difference = ui->sldNodePosX->maximum() < difference ? ui->sldNodePosX->maximum() : difference;
            difference = ui->sldNodePosX->minimum() > difference ? ui->sldNodePosX->minimum() : difference;
            nodePositionXChanged(difference);
        }
        if (m_nSelectedArrowNumber == 2) {
            //Y Arrow
            double difference = m_pSelectedNode->startPosition().y() - m_pSelectedNode->centerPosition().y() + length;
            difference = ui->sldNodePosY->maximum() < difference ? ui->sldNodePosY->maximum() : difference;
            difference = ui->sldNodePosY->minimum() > difference ? ui->sldNodePosY->minimum() : difference;
            nodePositionYChanged(difference);
        }
        if (m_nSelectedArrowNumber == 3) {
            // Z Arrow
            double difference = m_pSelectedNode->startPosition().z() - m_pSelectedNode->centerPosition().z() + length;
            difference = ui->sldNodePosZ->maximum() < difference ? ui->sldNodePosZ->maximum() : difference;
            difference = ui->sldNodePosZ->minimum() > difference ? ui->sldNodePosZ->minimum() : difference;
            nodePositionZChanged(difference);
        }
    }
}

void MainWindow::onMarkerMoving_changed(QPoint pos) {
    if (m_pSelectedNode != nullptr) {
        foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
            if (pNodeMarker->getNode() == m_pSelectedNode) {
                // Move this marker
                QPoint posStart = ui->widMainVoxelView->getPosStart();
                QVector3D startMarkerPos = pNodeMarker->getStartPosition();

                float fRatio = m_spacingSize * (0.7f/72.0f) + (16.0f/72.0f);        //80 => 1; 8 => 0.3
                float diffX = (pos.x() - posStart.x()) * fRatio;
                float diffY = (pos.y() - posStart.y()) * fRatio;

                Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
                QVector3D xDiffVector = getCameraRightVector();

                pNodeMarker->getMarkerTransform()->setTranslation(startMarkerPos - pCamera->upVector() * diffY / ui->widMainVoxelView->getZoom() - xDiffVector * diffX/ ui->widMainVoxelView->getZoom());
                pNodeMarker->getMarkerTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));

                pNodeMarker->getPresentLine()->setPoints(m_pSelectedNode->position(), pNodeMarker->getMarkerTransform()->translation());

                AdjustCuboidOfMarker(pNodeMarker);
                break;
            }
        }
    }
}

void MainWindow::onZoomChanged(float zoom) {
    m_pArrow->setZoom(zoom);

    foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
        pNodeMarker->setZoom(zoom);
        AdjustCuboidOfMarker(pNodeMarker);
    }

    float nRadius = (m_spacingSize * 0.8f + 13.6f) / 80;
    m_pScaleLine->getConeMesh()->setTopRadius(nRadius / zoom);
    m_pScaleLine->getConeMesh()->setBottomRadius(nRadius / zoom);

    if (m_pOverlayImage != nullptr) {
//        m_pOverlayImage->setZoom(zoom);
        rotateOverlayImage();
    }
}

void MainWindow::onMarkersRotating_changed() {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();

    foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
        pNodeMarker->getMarkerTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
        AdjustCuboidOfMarker(pNodeMarker);
    }

    RotateArrowTexts();

    m_pScaleLine->getTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), getCameraRightVector()));

    //Rotate Overlay Image
    rotateOverlayImage();

}

void MainWindow::rotateOverlayImage() {
    if (m_pOverlayImage != nullptr) {
        Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
        QVector3D viewVector = pCamera->viewVector() / pCamera->viewVector().length();

        m_pOverlayImage->getTextureTransform()->setTranslation(QVector3D(0, 0, 0) + viewVector * 5);
        float fRatio = m_spacingSize * (0.7f/72.0f) + (16.0f/72.0f);        //80 => 1; 8 => 0.3
        float diffX = m_pOverlayImage->getHorizontal() * fRatio;
        float diffY = m_pOverlayImage->getVertical() * fRatio;

        QVector3D xDiffVector = getCameraRightVector();

        QVector3D startTexturePos = m_pOverlayImage->getTextureTransform()->translation();

        m_pOverlayImage->getTextureTransform()->setTranslation(startTexturePos + pCamera->upVector() * diffY / ui->widMainVoxelView->getZoom() - xDiffVector * diffX/ ui->widMainVoxelView->getZoom());

        QQuaternion rot = QQuaternion::fromDirection(-pCamera->viewVector(), pCamera->upVector());
        QQuaternion rot90 = QQuaternion::fromAxisAndAngle(getCameraRightVector(), -90);
        rot = rot90 * rot;
        QQuaternion myRot = QQuaternion::fromAxisAndAngle(viewVector, m_pOverlayImage->getRotate());
        rot = myRot * rot;
        m_pOverlayImage->getTextureTransform()->setRotation(rot);
    }
}

void MainWindow::nodeMarkerVisibleChanged(int f)
{
    if(m_pPointRootEntity == nullptr)
        return;

    if(m_pSelectedNode != nullptr)
    {
        ui->edtMarkerName->setEnabled(ui->chkAddMarker->isChecked());
        ui->lblMarkerName->setEnabled(ui->chkAddMarker->isChecked());

        Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();

        bool fExisted = checkNodeMarkerExisted(m_pSelectedNode);

        if (f > 0) {
            // Checked Marker (Add a new marker)

            if (fExisted == false) {
                VoxelNodeMarker* pNodeMarker = new VoxelNodeMarker(m_pRootEntity);
                pNodeMarker->setMarkerSize(m_spacingSize, m_workspaceSizeY);
                pNodeMarker->getMarkerTransform()->setTranslation(m_pSelectedNode->position() - QVector3D(10, 10, 10));
                qDebug() << "Camera pos" << pCamera->position();
                pNodeMarker->getMarkerText()->setText("Marker");
                ui->edtMarkerName->setText("Marker");
                pNodeMarker->getCuboidMesh()->setXExtent(pNodeMarker->getMarkerText()->text().length());
                pNodeMarker->getCuboidMesh()->setYExtent(1.5f);
                pNodeMarker->getMarkerTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
                AdjustCuboidOfMarker(pNodeMarker);
                pNodeMarker->setNode(m_pSelectedNode);
                pNodeMarker->getPresentLine()->setPoints(m_pSelectedNode->position(), pNodeMarker->getMarkerTransform()->translation());
                QObject::connect(pNodeMarker, SIGNAL(NodeMarkerPressed(Voxel3DNode*)), this, SLOT(onNodeMarkerPressed(Voxel3DNode*)));
                m_listNodeMarkers.push_back(pNodeMarker);

                onZoomChanged(ui->widMainVoxelView->getZoom());
            }

        } else {
            // Unchecked Marker (Remove the marker)
            if (fExisted == true) {
                foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
                    if (pNodeMarker->getNode() == m_pSelectedNode) {
                        m_listNodeMarkers.removeOne(pNodeMarker);
                        delete pNodeMarker;
                        ui->edtMarkerName->setText("");
                        break;
                    }
                }
            }
        }
    }
}

bool MainWindow::checkNodeMarkerExisted(Voxel3DNode* pNode) {
    bool fExisted = false;

    if (pNode != nullptr) {
        foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
            if (pNodeMarker->getNode() == pNode) {
                fExisted = true;
                break;
            }
        }
    }

    return fExisted;
}

void MainWindow::nodeMarkerTextChanged() {
    foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
        if (pNodeMarker->getNode() == m_pSelectedNode) {
            pNodeMarker->getMarkerText()->setText(ui->edtMarkerName->text());
            pNodeMarker->getCuboidMesh()->setXExtent(pNodeMarker->getMarkerText()->text().length());
            AdjustCuboidOfMarker(pNodeMarker);
            break;
        }
    }
}

void MainWindow::onNodeMarkerPressed(Voxel3DNode* pNode) {
    if(m_pSelectedNode != nullptr){
        foreach(Voxel3DNode* node, m_selectedNodeList)
        {
            node->setSelected(false);
        }
    }

    m_pSelectedNode = pNode;
    m_selectedNodeList.clear();
    m_selectedNodeList.push_back(m_pSelectedNode);

    m_pArrow->MoveArrows(m_pSelectedNode->position());
    RotateArrowTexts();

    ui->chkAddMarker->setChecked(checkNodeMarkerExisted(m_pSelectedNode));

    enableNodeController();
    updateReactorNodes();
    displaySelectedNodeData();
    m_isRangeSelected = false;

    updateSelectionCloudNodes();

    ui->widMainVoxelView->setMarkerSelectedFlag(true);

    foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
        if (pNodeMarker->getNode() == m_pSelectedNode) {
            pNodeMarker->setStartPosition(pNodeMarker->getMarkerTransform()->translation());
            break;
        }
    }
}

void MainWindow::MoveMarker(Voxel3DNode* pNode) {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();

    foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
        if (pNodeMarker->getNode() == pNode) {
            //            pNodeMarker->getMarkerTransform()->setTranslation(pNode->position() - QVector3D(10, 10, 10));
            pNodeMarker->getMarkerTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
            pNodeMarker->getPresentLine()->setPoints(m_pSelectedNode->position(), pNodeMarker->getMarkerTransform()->translation());
            AdjustCuboidOfMarker(pNodeMarker);
            break;
        }
    }
}

void MainWindow::AdjustCuboidOfMarker(VoxelNodeMarker* pNodeMarker) {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
    float fCubeRate = m_workspaceSizeY * 0.75f / 8 + 0.5f / 8;
    float fPercentSize = (0.7f / 72 * m_spacingSize + 16.0f / 72) * fCubeRate;
    float fWidth = pNodeMarker->getCuboidMesh()->xExtent() * 10 * fPercentSize;
    float fHeight = pNodeMarker->getCuboidMesh()->yExtent() * 10 * fPercentSize;

    qDebug() << "W-H: " << fWidth << fHeight;
    QVector3D cuboidPos = pNodeMarker->getMarkerTransform()->translation();
    QVector3D xDiffVector = -getCameraRightVector() * fWidth / 2 / ui->widMainVoxelView->getZoom();
    QVector3D yDiffVector = pCamera->upVector() / pCamera->upVector().length() * fHeight / 2 / ui->widMainVoxelView->getZoom();
    QVector3D zDiffVector = pCamera->viewVector() / pCamera->viewVector().length() * 0.5f;
    pNodeMarker->getCuboidTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
    pNodeMarker->getCuboidTransform()->setTranslation(cuboidPos + xDiffVector + yDiffVector + zDiffVector);
}

void MainWindow::RotateArrowTexts() {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
    m_pArrow->getXText()->getTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
    m_pArrow->getYText()->getTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
    m_pArrow->getZText()->getTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), pCamera->upVector()));
}

void MainWindow::onScaleLineAdd_triggered() {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
    int len = ui->edtScaleLength->text().toInt();
    if (len < 1)
        return;

    len = m_spacingSize * len;
    m_pScaleLine->getConeMesh()->setLength(len);
    float nRadius = (m_spacingSize * 0.8f + 13.6f) / 80;
    m_pScaleLine->getConeMesh()->setTopRadius(nRadius / ui->widMainVoxelView->getZoom());
    m_pScaleLine->getConeMesh()->setBottomRadius(nRadius / ui->widMainVoxelView->getZoom());
    m_pScaleLine->getTransform()->setTranslation(QVector3D(0, 0, 0));
    m_pScaleLine->getTransform()->setRotation(QQuaternion::fromDirection(pCamera->position(), getCameraRightVector()));
}

void MainWindow::onScaleLineRemove_triggered() {
    m_pScaleLine->getTransform()->setTranslation(BEHIND_CAMERA);
}

void MainWindow::RemoveAllMarkers() {
    foreach(VoxelNodeMarker* pNodeMarker, m_listNodeMarkers){
        delete pNodeMarker;
    }
    m_listNodeMarkers.clear();
}

QVector3D MainWindow::getCameraRightVector() {
    Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
    double x = pCamera->upVector().y() * pCamera->viewVector().z() - pCamera->upVector().z() * pCamera->viewVector().y();
    double y = pCamera->upVector().z() * pCamera->viewVector().x() - pCamera->upVector().x() * pCamera->viewVector().z();
    double z = pCamera->upVector().x() * pCamera->viewVector().y() - pCamera->upVector().y() * pCamera->viewVector().x();
    double length = qSqrt(x*x + y*y + z*z);
    QVector3D yDiffVector(x, y, z);
    yDiffVector = yDiffVector / length;

    return yDiffVector;
}

void MainWindow::onViewScale_pressed() {
    m_currentZoom = ui->widMainVoxelView->getZoom();
    ui->widMainVoxelView->setZoom(.75f);
}

void MainWindow::onViewScale_released() {
    ui->widMainVoxelView->setZoom(m_currentZoom);
}

void MainWindow::onMousePressed() {
    if (m_bArrowHovered == true) {
        ui->widMainVoxelView->setArrowSelectedFlag(true);
    }
}

void MainWindow::onMouseRelease() {
    m_pXArrowPicker->setDragEnabled(true);
    m_pXArrowPicker->setHoverEnabled(true);
    m_pYArrowPicker->setDragEnabled(true);
    m_pYArrowPicker->setHoverEnabled(true);
    m_pZArrowPicker->setDragEnabled(true);
    m_pZArrowPicker->setHoverEnabled(true);
    qDebug() << "Mouse Released : MainWindow";
    m_pArrow->setArrowColorDefault();

    if(m_bArrowHovered){
        if(m_nSelectedArrowNumber == 1){
            if(m_pXArrowPicker != nullptr){
                m_pArrow->getXConeArrow()->removeComponent(m_pXArrowPicker);
                delete m_pXArrowPicker;
                Sleep(100);

                m_pXArrowPicker = new Qt3DRender::QObjectPicker(m_pRootEntity);
                m_pXArrowPicker->setHoverEnabled(true);
                m_pXArrowPicker->setDragEnabled(true);

                QObject::connect(m_pXArrowPicker, SIGNAL(entered()), this, SLOT(onXArrowEntered()));
                QObject::connect(m_pXArrowPicker, SIGNAL(exited()), this, SLOT(onXArrowExited()));
                m_pArrow->getXConeArrow()->addComponent(m_pXArrowPicker);
            }
        }else if(m_nSelectedArrowNumber == 2){
            if(m_pYArrowPicker != nullptr){
                m_pArrow->getYConeArrow()->removeComponent(m_pYArrowPicker);
                delete m_pYArrowPicker;
                Sleep(100);

                m_pYArrowPicker = new Qt3DRender::QObjectPicker(m_pRootEntity);
                m_pYArrowPicker->setHoverEnabled(true);
                m_pYArrowPicker->setDragEnabled(true);

                QObject::connect(m_pYArrowPicker, SIGNAL(entered()), this, SLOT(onYArrowEntered()));
                QObject::connect(m_pYArrowPicker, SIGNAL(exited()), this, SLOT(onYArrowExited()));
                m_pArrow->getYConeArrow()->addComponent(m_pYArrowPicker);
            }
        }else if(m_nSelectedArrowNumber == 3){
            if(m_pZArrowPicker != nullptr){
                m_pArrow->getZConeArrow()->removeComponent(m_pZArrowPicker);
                delete m_pZArrowPicker;
                Sleep(100);

                m_pZArrowPicker = new Qt3DRender::QObjectPicker(m_pRootEntity);
                m_pZArrowPicker->setHoverEnabled(true);
                m_pZArrowPicker->setDragEnabled(true);

                QObject::connect(m_pZArrowPicker, SIGNAL(entered()), this, SLOT(onZArrowEntered()));
                QObject::connect(m_pZArrowPicker, SIGNAL(exited()), this, SLOT(onZArrowExited()));
                m_pArrow->getZConeArrow()->addComponent(m_pZArrowPicker);
            }
        }
    }

     m_bArrowHovered = false;

     if (m_bNodePosChanged == true && m_selectedNodeList.count() == 1) {
         nodePositionReleased();
         m_bNodePosChanged = false;
     }
}

void MainWindow::onMouseMove(){
    m_pXArrowPicker->setDragEnabled(false);
    m_pXArrowPicker->setHoverEnabled(false);
    m_pYArrowPicker->setDragEnabled(false);
    m_pYArrowPicker->setHoverEnabled(false);
    m_pZArrowPicker->setDragEnabled(false);
    m_pZArrowPicker->setHoverEnabled(false);
}

void MainWindow::on_edtPointPosX_editingFinished()
{
    int nValue = ui->edtPointPosX->text().toInt();

    if(ui->edtPointPosX->text().toInt() < -m_spacingSize * 1.5f) {
        nValue = -m_spacingSize * 1.5f;
    }

    if(ui->edtPointPosX->text().toInt() > m_spacingSize * 1.5f) {
        nValue = m_spacingSize * 1.5f;
    }

    Voxel3DNode* pTmpNode = m_pSelectedNode;

    if (m_selectedNodeList.count() >= 1) {
        foreach(auto *pNode, m_selectedNodeList){
            m_pSelectedNode = pNode;
            nodePositionXChanged(nValue);
            nodePositionReleased();
        }
    }

    m_pSelectedNode = pTmpNode;

}

void MainWindow::on_edtPointPosY_editingFinished()
{
    int nValue = ui->edtPointPosY->text().toInt();

    if(ui->edtPointPosY->text().toInt() < -m_spacingSize * 1.5) {
        nValue = -m_spacingSize * 1.5;
    }

    if(ui->edtPointPosY->text().toInt() > m_spacingSize * 1.5) {
        nValue = m_spacingSize * 1.5;
    }

    Voxel3DNode* pTmpNode = m_pSelectedNode;

    if (m_selectedNodeList.count() >= 1) {
        foreach(auto *pNode, m_selectedNodeList){
            m_pSelectedNode = pNode;
            nodePositionYChanged(nValue);
            nodePositionReleased();
        }
    }

    m_pSelectedNode = pTmpNode;
}

void MainWindow::on_edtPointPosZ_editingFinished()
{
    int nValue = ui->edtPointPosZ->text().toInt();

    if(ui->edtPointPosZ->text().toInt() < -m_spacingSize * 1.5) {
        nValue = -m_spacingSize * 1.5;
    }

    if(ui->edtPointPosZ->text().toInt() > m_spacingSize * 1.5) {
        nValue = m_spacingSize * 1.5;
    }

    Voxel3DNode* pTmpNode = m_pSelectedNode;

    if (m_selectedNodeList.count() >= 1) {
        foreach(auto *pNode, m_selectedNodeList){
            m_pSelectedNode = pNode;
            nodePositionZChanged(nValue);
            nodePositionReleased();
        }
    }

    m_pSelectedNode = pTmpNode;
}

void MainWindow::MoveReactorNodeByEdit(int nValue, int nPoint, int xyz) {
    if(m_reactorNodeList.count() != 8)
    {
        return;
    }

    if(nValue < -m_spacingSize * 1.5f) {
        nValue = static_cast<int>(-m_spacingSize * 1.5f);
    }

    if(nValue > m_spacingSize * 1.5f) {
        nValue = static_cast<int>(m_spacingSize * 1.5f);
    }

    // Node Selected Action (m_pSelectedNode has been changed)
    if(m_pSelectedNode != nullptr){
        foreach(Voxel3DNode* node, m_selectedNodeList)
        {
            node->setSelected(false);
        }
    }

    m_pSelectedNode = m_reactorNodeList.at(nPoint);
    m_selectedNodeList.clear();
    m_selectedNodeList.push_back(m_pSelectedNode);

    m_pArrow->MoveArrows(m_pSelectedNode->position());
    RotateArrowTexts();

    ui->chkAddMarker->setChecked(checkNodeMarkerExisted(m_pSelectedNode));

    enableNodeController();
    displaySelectedNodeData();
    m_isRangeSelected = false;

    updateSelectionCloudNodes();

    switch (xyz) {
    case 1:
        nodePositionXChanged(nValue);
        break;
    case 2:
        nodePositionYChanged(nValue);
        break;
    case 3:
        nodePositionZChanged(nValue);
        break;
    default:
        break;
    }

    nodePositionReleased();
}

void MainWindow::on_edtCoor1X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor1X->text().toInt(), 0, 1);
}

void MainWindow::on_edtCoor1Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor1Y->text().toInt(), 0, 2);
}

void MainWindow::on_edtCoor1Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor1Z->text().toInt(), 0, 3);
}

void MainWindow::on_edtCoor2X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor2X->text().toInt(), 1, 1);
}

void MainWindow::on_edtCoor2Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor2Y->text().toInt(), 1, 2);
}

void MainWindow::on_edtCoor2Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor2Z->text().toInt(), 1, 3);
}

void MainWindow::on_edtCoor3X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor3X->text().toInt(), 2, 1);
}

void MainWindow::on_edtCoor3Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor3Y->text().toInt(), 2, 2);
}

void MainWindow::on_edtCoor3Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor3Z->text().toInt(), 2, 3);
}

void MainWindow::on_edtCoor4X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor4X->text().toInt(), 3, 1);
}

void MainWindow::on_edtCoor4Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor4Y->text().toInt(), 3, 2);
}

void MainWindow::on_edtCoor4Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor4Z->text().toInt(), 3, 3);
}

void MainWindow::on_edtCoor5X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor5X->text().toInt(), 4, 1);
}

void MainWindow::on_edtCoor5Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor5Y->text().toInt(), 4, 2);
}

void MainWindow::on_edtCoor5Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor5Z->text().toInt(), 4, 3);
}

void MainWindow::on_edtCoor6X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor6X->text().toInt(), 5, 1);
}

void MainWindow::on_edtCoor6Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor6Y->text().toInt(), 5, 2);
}

void MainWindow::on_edtCoor6Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor6Z->text().toInt(), 5, 3);
}

void MainWindow::on_edtCoor7X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor7X->text().toInt(), 6, 1);
}

void MainWindow::on_edtCoor7Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor7Y->text().toInt(), 6, 2);
}

void MainWindow::on_edtCoor7Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor7Z->text().toInt(), 6, 3);
}

void MainWindow::on_edtCoor8X_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor8X->text().toInt(), 7, 1);
}

void MainWindow::on_edtCoor8Y_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor8Y->text().toInt(), 7, 2);
}

void MainWindow::on_edtCoor8Z_editingFinished()
{
    MoveReactorNodeByEdit(ui->edtCoor8Z->text().toInt(), 7, 3);
}



void MainWindow::on_chkSelectedPointsActive_toggled(bool checked)
{
    qDebug() << "SelectedPoints" << checked;
    if (checked == true) {
        if (m_selectedNodeList.count() >= 1) {
            foreach(auto *pNode, m_nodeList) {
                bool bExist = false;
                foreach(auto *pSelectedNode, m_selectedNodeList) {
                    if (pSelectedNode == pNode) {
                        bExist = true;
                        break;
                    }
                }
                if (bExist == false) {
                    pNode->setActive(false);
                    if (pNode->isVisible() == true) {
                        m_pNodes->setVisibleDeactive(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());
                    } else {
                        m_pNodes->setUnvisibleDeactive(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());
                    }

                }
            }
            foreach(auto *pNode, m_selectedNodeList){
                m_selectedActiveNodeList.push_back(pNode);
            }
        }
    } else {
        if (m_selectedActiveNodeList.count() >= 1) {
            foreach(auto *pNode, m_nodeList) {
                bool bExist = false;
                foreach(auto *pSelectedNode, m_selectedActiveNodeList) {
                    if (pSelectedNode == pNode) {
                        bExist = true;
                        break;
                    }
                }
                if (bExist == false) {
                    pNode->setActive(true);
                    if (pNode->isVisible() == true) {
                        m_pNodes->setVisible(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());
                    } else {
                        m_pNodes->setUnvisible(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());
                    }

                }
            }
        }
        m_selectedActiveNodeList.clear();
    }
}

void MainWindow::on_btnGotoOverlay_clicked()
{
    ui->stwRightToolBar->setCurrentIndex(1);
}

void MainWindow::on_btnBack_clicked()
{
    ui->stwRightToolBar->setCurrentIndex(0);
    //ReactorView refresh
    ui->widReactorView->resizeView(ui->widReactorView->size());
}

void MainWindow::on_btnOverlayOpen_clicked()
{
    // Open Open-Dialog
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Import an png"),
                nullptr,
                tr("Image Files (*.png *.jpg *.bmp)"),
                nullptr,
                nullptr);

    if (!fileName.isEmpty()){
        on_btnOverlayClose_clicked();
        m_pOverlayImage = new VoxelOverlayImage(m_pRootEntity);
        m_pOverlayImage->setTextureSource(QUrl::fromLocalFile(fileName));

        Qt3DRender::QCamera* pCamera = ui->widMainVoxelView->camera();
        QVector3D viewVector = pCamera->viewVector() / pCamera->viewVector().length();
        m_pOverlayImage->getTextureTransform()->setTranslation(pCamera->position() + viewVector * 5);
        qDebug() << "Camera pos" << pCamera->position();
        QQuaternion rot = QQuaternion::fromDirection(-viewVector, pCamera->upVector());
        QQuaternion rot90 = QQuaternion::fromAxisAndAngle(getCameraRightVector(), -90);
        rot = rot90 * rot;
        m_pOverlayImage->getTextureTransform()->setRotation(rot);
        m_pOverlayImage->setPercentSize(m_spacingSize);

        enableOverlayControls(true);
        on_edtOverlayOpacity_editingFinished();
        on_edtOverlayScale_editingFinished();
        emit ui->sldOverlayRotate->valueChanged(0);
        emit ui->sldOverlayHorizontal->valueChanged(0);
        emit ui->sldOverlayVertical->valueChanged(0);
        ui->sldOverlayRotate->setValue(0);
        ui->sldOverlayHorizontal->setValue(0);
        ui->sldOverlayVertical->setValue(0);
    }
}

void MainWindow::on_btnOverlayClose_clicked()
{
    if (m_pOverlayImage != nullptr) {
        delete m_pOverlayImage;
        m_pOverlayImage = nullptr;
        enableOverlayControls(false);
    }
}

void MainWindow::enableOverlayControls(bool fEnabled) {
    ui->sldOverlayOpacity->setEnabled(fEnabled);
    ui->sldOverlayScale->setEnabled(fEnabled);
    ui->sldOverlayRotate->setEnabled(fEnabled);
    ui->sldOverlayHorizontal->setEnabled(fEnabled);
    ui->sldOverlayVertical->setEnabled(fEnabled);

    ui->edtOverlayOpacity->setEnabled(fEnabled);
    ui->edtOverlayScale->setEnabled(fEnabled);
    ui->edtOverlayRotate->setEnabled(fEnabled);
    ui->edtOverlayHorizontal->setEnabled(fEnabled);
    ui->edtOverlayVertical->setEnabled(fEnabled);
}
void MainWindow::on_sldOverlayOpacity_valueChanged(int value)
{
    ui->edtOverlayOpacity->setText(QString::number(value));
    m_pOverlayImage->setOpacity(value);
}

void MainWindow::on_edtOverlayOpacity_editingFinished()
{
    int nValue = ui->edtOverlayOpacity->text().toInt();
    ui->sldOverlayOpacity->setValue(nValue);
    m_pOverlayImage->setOpacity(nValue);
}

void MainWindow::on_sldOverlayScale_valueChanged(int value)
{
    ui->edtOverlayScale->setText(QString::number(value));
    m_pOverlayImage->setScale(value);
    m_pOverlayImage->setZoom(ui->widMainVoxelView->getZoom());
}

void MainWindow::on_edtOverlayScale_editingFinished()
{
    int nValue = ui->edtOverlayScale->text().toInt();
    ui->sldOverlayScale->setValue(nValue);
    m_pOverlayImage->setScale(nValue);
    m_pOverlayImage->setZoom(ui->widMainVoxelView->getZoom());
}

void MainWindow::on_sldOverlayRotate_valueChanged(int value)
{
    ui->edtOverlayRotate->setText(QString::number(value));
    m_pOverlayImage->setRotate(value);
    rotateOverlayImage();
}

void MainWindow::on_edtOverlayRotate_editingFinished()
{
    int nValue = ui->edtOverlayRotate->text().toInt();
    ui->sldOverlayRotate->setValue(nValue);
    m_pOverlayImage->setRotate(nValue);
    rotateOverlayImage();
}

void MainWindow::on_sldOverlayHorizontal_valueChanged(int value)
{
    ui->edtOverlayHorizontal->setText(QString::number(value));
    m_pOverlayImage->setHorizontal(value);
    rotateOverlayImage();
}

void MainWindow::on_edtOverlayHorizontal_editingFinished()
{
    int nValue = ui->edtOverlayHorizontal->text().toInt();
    ui->sldOverlayHorizontal->setValue(nValue);
    m_pOverlayImage->setHorizontal(nValue);
    rotateOverlayImage();
}

void MainWindow::on_sldOverlayVertical_valueChanged(int value)
{
    ui->edtOverlayVertical->setText(QString::number(value));
    m_pOverlayImage->setVertical(value);
    rotateOverlayImage();
}

void MainWindow::on_edtOverlayVertical_editingFinished()
{
    int nValue = ui->edtOverlayVertical->text().toInt();
    ui->sldOverlayVertical->setValue(nValue);
    m_pOverlayImage->setVertical(nValue);
    rotateOverlayImage();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   //ReactorView refresh
   ui->widReactorView->resizeView(ui->widReactorView->size());
}

bool MainWindow::isReactorCandidate(Voxel3DNode* pNode) {
    if(!ui->btnSelectReactorGuide->isChecked() || m_pSelectedNode == nullptr)
        return false;

    ///
    ///         |
    ///         |
    ///         |
    ///         0--------
    ///        /
    ///       /
    ///
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        if(m_pSelectedNode->frontNode()->rightNode()->upNode() == pNode) {
            onSelectReactorMode1_triggered();
            return true;
        }
    }

    ///
    ///         |
    ///         |
    ///         | /
    ///         |/
    ///         1---------
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        if(m_pSelectedNode->backNode()->rightNode()->upNode() == pNode) {
            onSelectReactorMode2_triggered();
            return true;
        }
    }

    ///
    ///         2--------
    ///       / |
    ///      /  |
    ///     /   |
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        if(m_pSelectedNode->frontNode()->rightNode()->downNode() == pNode) {
            onSelectReactorMode3_triggered();
            return true;
        }
    }

    ///
    ///            /
    ///           /
    ///          /
    ///         3--------
    ///         |
    ///         |
    ///         |
    ///
    ///
    if(m_pSelectedNode->rightNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        if(m_pSelectedNode->backNode()->rightNode()->downNode() == pNode) {
            onSelectReactorMode4_triggered();
            return true;
        }
    }

    ///
    ///                  |
    ///                  |
    ///                  |
    ///          --------4
    ///                 /
    ///                /
    ///               /
    ///
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        if(m_pSelectedNode->frontNode()->leftNode()->upNode() == pNode) {
            onSelectReactorMode5_triggered();
            return true;
        }
    }

    ///
    ///                  |  /
    ///                  | /
    ///                  |/
    ///          --------5
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->upNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        if(m_pSelectedNode->backNode()->leftNode()->upNode() == pNode) {
            onSelectReactorMode6_triggered();
            return true;
        }
    }

    ///
    ///         ---------6
    ///                 /|
    ///                / |
    ///               /  |
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->frontNode() != nullptr) {
        if(m_pSelectedNode->frontNode()->leftNode()->downNode() == pNode) {
            onSelectReactorMode7_triggered();
            return true;
        }
    }

    ///
    ///                     /
    ///                    /
    ///                   /
    ///         ---------7
    ///                  |
    ///                  |
    ///                  |
    ///
    ///
    if(m_pSelectedNode->leftNode() != nullptr &&
            m_pSelectedNode->downNode() != nullptr &&
            m_pSelectedNode->backNode() != nullptr) {
        if(m_pSelectedNode->backNode()->leftNode()->downNode() == pNode) {
            onSelectReactorMode8_triggered();
            return true;
        }
    }

    return false;
}

void MainWindow::on_btnStraightEdgeAdd_clicked()
{
//    if (m_nStraightEdgeFlag != 0) {
//        QMessageBox messageBox;
//        messageBox.warning(this, "Warning!","You should remove the old straight edge!");
//        messageBox.setFixedSize(500,200);
//        return;
//    }
    m_nStraightEdgeFlag = 1;
}

void MainWindow::nodeClickForStraight(Voxel3DNode* pNode) {
    if (m_nStraightEdgeFlag == 1) {
        on_btnStraightEdgeRemove_clicked();
        m_nStraightEdgeFlag = 2;
        m_pNodes->setStraightEdge(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());
        m_StraightEdgeNodes.push_back(pNode);
    }else if (m_nStraightEdgeFlag == 2) {
        m_nStraightEdgeFlag++;
        m_pNodes->setStraightEdge(pNode->nodeIndexX(), pNode->nodeIndexY(), pNode->nodeIndexZ());
        m_StraightEdgeNodes.push_back(pNode);
    }

    if (m_nStraightEdgeFlag == 3) {
        if (ui->chkStraightEdgeShow->isChecked() == true) {
            m_pLines->addLineStraightEdge(m_StraightEdgeNodes[0]->nodeIndexX(), m_StraightEdgeNodes[0]->nodeIndexY(), m_StraightEdgeNodes[0]->nodeIndexZ(),
                                           m_StraightEdgeNodes[1]->nodeIndexX(), m_StraightEdgeNodes[1]->nodeIndexY(), m_StraightEdgeNodes[1]->nodeIndexZ());
        }
    }
}

void MainWindow::on_btnStraightEdgeRemove_clicked()
{
    m_nStraightEdgeFlag = 0;
    m_pNodes->clearStraightEdges();
    m_StraightEdgeNodes.clear();
    m_pLines->clearStraightEdge();
}

void MainWindow::on_chkStraightEdgeShow_toggled(bool checked)
{
    if (checked == true) {
        if (m_nStraightEdgeFlag == 3) {
            m_pLines->clearStraightEdge();
            m_pLines->addLineStraightEdge(m_StraightEdgeNodes[0]->nodeIndexX(), m_StraightEdgeNodes[0]->nodeIndexY(), m_StraightEdgeNodes[0]->nodeIndexZ(),
                                               m_StraightEdgeNodes[1]->nodeIndexX(), m_StraightEdgeNodes[1]->nodeIndexY(), m_StraightEdgeNodes[1]->nodeIndexZ());
        }
    } else {
        m_pLines->clearStraightEdge();
    }
}
