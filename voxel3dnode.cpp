#include "voxel3dnode.h"

Voxel3DNode::Voxel3DNode(Qt3DCore::QEntity* parent) : Qt3DCore::QEntity(parent)
{
    // Initialize Neighor Nodes
    m_pNeighborNodes.clear();
    for(int i = 0 ; i < 6; i++){
        m_pNeighborNodes.push_back(nullptr);
    }
    ///////////////////////////////
    // Initialize Neighor Lines
    m_pNeighborLines.clear();
    for(int i = 0 ; i < 6; i++){
        m_pNeighborLines.push_back(nullptr);
    }

    ///////////////////////////////
    m_visible = false;
    m_selected = false;
    m_active = true;
    m_centerPosition = new QVector3D(0, 0, 0);
    m_position = new QVector3D(0, 0, 0);
    m_startPosition = new QVector3D(0, 0, 0);

    // Transform Components
    m_pTransform = new Qt3DCore::QTransform();
    this->addComponent(m_pTransform);

    // Picker Components
//    m_pPicker = new Qt3DRender::QObjectPicker(this);
//    QObject::connect(m_pPicker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(onClicked(Qt3DRender::QPickEvent*)));
//    this->addComponent(m_pPicker);

    /*
     * Custom Mesh Component
     * */
    Qt3DRender::QGeometryRenderer *customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry *customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

    // 2 distinct vertices
    QByteArray vertexBufferData;
    vertexBufferData.resize(6 * sizeof(float));

    // Vertices
    QVector<QVector3D> vertices = QVector<QVector3D>()
            << QVector3D(0, 0, 0)
            << QVector3D(INT_MAX / 2, INT_MAX / 2, INT_MAX /2);

    float *rawVertexArray = reinterpret_cast<float *>(vertexBufferData.data());
    int idx = 0;

    Q_FOREACH (const QVector3D &v, vertices) {
        rawVertexArray[idx++] = v.x();
        rawVertexArray[idx++] = v.y();
        rawVertexArray[idx++] = v.z();
    }

    // Indices
    QByteArray indexBufferData;
    indexBufferData.resize(2 * sizeof(ushort));
    ushort *rawIndexArray = reinterpret_cast<ushort *>(indexBufferData.data());

    rawIndexArray[0] = 0;
    rawIndexArray[1] = 1;

    vertexDataBuffer->setData(vertexBufferData);
    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);
    indexAttribute->setVertexSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(2);

    customGeometry->addAttribute(positionAttribute);
    customGeometry->addAttribute(indexAttribute);

    customMeshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    customMeshRenderer->setGeometry(customGeometry);
    this->addComponent(customMeshRenderer);

    // Material Component
    m_pMaterial = new Qt3DExtras::QPhongAlphaMaterial(parent);
    m_reactorIndex = -1;
    m_pMaterial->setAmbient(Qt::blue);
    m_pMaterial->setDiffuse(Qt::white);
    auto effect = m_pMaterial->effect();
    for (auto t : effect->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(5.0f);
            rp->addRenderState(pointSize);
        }
    }
    this->addComponent(m_pMaterial);

    paintColor();

    m_historyPositionBefore.clear();
    m_historyPositionAfter.clear();
    m_historyPositionBefore.push_back(QVector3D(0,0,0));

    m_indexX = m_indexY = m_indexZ = 0;
}

Voxel3DNode::~Voxel3DNode()
{

}

void Voxel3DNode::updateCenterPosition(QVector3D ps)
{
    m_centerPosition->setX(ps.x());
    m_centerPosition->setY(ps.y());
    m_centerPosition->setZ(ps.z());

    m_position->setX(ps.x());
    m_position->setY(ps.y());
    m_position->setZ(ps.z());

    m_pTransform->setTranslation(*m_position);
}

void Voxel3DNode::setStartPosition(QVector3D ps)
{
    m_startPosition->setX(ps.x());
    m_startPosition->setY(ps.y());
    m_startPosition->setZ(ps.z());
}

void Voxel3DNode::setNodeIndex(int8_t x, int8_t y, int8_t z)
{
    m_indexX = x;
    m_indexY = y;
    m_indexZ = z;
}

void Voxel3DNode::updateLocalPosition(QVector3D ps)
{
    m_position->setX(m_centerPosition->x() + ps.x());
    m_position->setY(m_centerPosition->y() + ps.y());
    m_position->setZ(m_centerPosition->z() + ps.z());

    m_pTransform->setTranslation(*m_position);

    updateLinePosition();
}

void Voxel3DNode::updateLocalPositionX(float pos)
{
    m_position->setX(m_centerPosition->x() + pos);
    m_pTransform->setTranslation(*m_position);

    updateLinePosition();
}

void Voxel3DNode::updateLocalPositionY(float pos)
{
    m_position->setY(m_centerPosition->y() + pos);
    m_pTransform->setTranslation(*m_position);

    updateLinePosition();
}

void Voxel3DNode::updateLocalPositionZ(float pos)
{
    m_position->setZ(m_centerPosition->z() + pos);
    m_pTransform->setTranslation(*m_position);

    updateLinePosition();
}

void Voxel3DNode::resetPosition()
{
    updateLocalPosition(QVector3D(0, 0, 0));
}

void Voxel3DNode::createHistoryPosition()
{
    m_historyPositionBefore.push_back(localPosition());
    m_historyPositionAfter.clear();
    if(m_historyPositionBefore.length() > 30)
        m_historyPositionBefore.pop_front();
}

void Voxel3DNode::onClicked(Qt3DRender::QPickEvent* evt){
    if(evt->button() != Qt3DRender::QPickEvent::LeftButton)
        return;

    if (m_active == false) {
        return;
    }

    if(evt->modifiers() == Qt::ControlModifier)
    {
        emit nodeCtrlClicked();
    }else if(evt->modifiers() == Qt::ShiftModifier)
    {
        emit nodeShiftClicked();
    }else{
        emit nodeClicked();
    }

    setSelected(true);
}

void Voxel3DNode::emitClicked()
{
    if (m_active == false) {
        return;
    }

    emit nodeClicked();
    setSelected(true);
}

void Voxel3DNode::emitCtrlClicked()
{
    if (m_active == false) {
        return;
    }

    emit nodeCtrlClicked();
    setSelected(true);
}

void Voxel3DNode::emitShiftClicked()
{
    if (m_active == false) {
        return;
    }

    emit nodeShiftClicked();
    setSelected(true);
}

void Voxel3DNode::undo()
{
    if(m_historyPositionBefore.length() > 1){
        QVector3D pt = m_historyPositionBefore.last();
        m_historyPositionBefore.pop_back();
        m_historyPositionAfter.push_front(pt);
        updateLocalPosition(m_historyPositionBefore.last());

        emitClicked();
    }
}

void Voxel3DNode::redo()
{
    if(m_historyPositionAfter.length() > 0){
        QVector3D pt = m_historyPositionAfter.first();
        m_historyPositionAfter.pop_front();
        m_historyPositionBefore.push_back(pt);
        updateLocalPosition(pt);

        emitClicked();
    }
}

void Voxel3DNode::setVisible(bool f)
{
    m_visible = f;
    paintColor();
}

void Voxel3DNode::setSelected(bool f)
{
    m_selected = f;
    paintColor();
}

void Voxel3DNode::setActive(bool f)
{
    m_active = f;
    paintColor();
}

void Voxel3DNode::setReactor(int8_t reactor_index)
{
    m_reactorIndex = reactor_index;
    paintColor();
}

void Voxel3DNode::removePickerComponent()
{
//    if(m_pPicker != nullptr)
//        this->removeComponent(m_pPicker);
}

void Voxel3DNode::paintColor()
{
    if(m_reactorIndex < 0)
    {
        if(m_selected){
            m_pMaterial->setAmbient(NODE_COLOR_SELECTED);
        }else{
            if(m_visible)
            {
                m_pMaterial->setAmbient(NODE_COLOR_VISIBLE);
            }else{
                m_pMaterial->setAmbient(NODE_COLOR_UNVISIBLE);
            }
        }
    }else{
        if(m_selected){
            m_pMaterial->setAmbient(NODE_COLOR_SELECTED);
        }else{
            switch(m_reactorIndex){
            case 0:
                m_pMaterial->setAmbient(REACTOR_COLOR_1);
                break;
            case 1:
                m_pMaterial->setAmbient(REACTOR_COLOR_2);
                break;
            case 2:
                m_pMaterial->setAmbient(REACTOR_COLOR_3);
                break;
            case 3:
                m_pMaterial->setAmbient(REACTOR_COLOR_4);
                break;
            case 4:
                m_pMaterial->setAmbient(REACTOR_COLOR_5);
                break;
            case 5:
                m_pMaterial->setAmbient(REACTOR_COLOR_6);
                break;
            case 6:
                m_pMaterial->setAmbient(REACTOR_COLOR_7);
                break;
            case 7:
                m_pMaterial->setAmbient(REACTOR_COLOR_8);
                break;
            default:
                m_pMaterial->setAmbient(NODE_COLOR_UNVISIBLE);
            }
        }
    }
}

void Voxel3DNode::updateLinePosition()
{
    if(leftLine() != nullptr && leftNode() != nullptr)
        leftLine()->setPoints(position(), leftNode()->position());
    if(rightLine() != nullptr && rightNode() != nullptr)
        rightLine()->setPoints(position(), rightNode()->position());
    if(upLine() != nullptr && upNode() != nullptr)
        upLine()->setPoints(position(), upNode()->position());
    if(downLine() != nullptr && downNode() != nullptr)
        downLine()->setPoints(position(), downNode()->position());
    if(frontLine() != nullptr && frontNode() != nullptr)
        frontLine()->setPoints(position(), frontNode()->position());
    if(backLine() != nullptr && backNode() != nullptr)
        backLine()->setPoints(position(), backNode()->position());
}

void Voxel3DNode::updateLineColor(QColor color)
{
    if(leftLine() != nullptr && leftNode() != nullptr)
        leftLine()->setColor(color);
    if(rightLine() != nullptr && rightNode() != nullptr)
        rightLine()->setColor(color);
    if(upLine() != nullptr && upNode() != nullptr)
        upLine()->setColor(color);
    if(downLine() != nullptr && downNode() != nullptr)
        downLine()->setColor(color);
    if(frontLine() != nullptr && frontNode() != nullptr)
        frontLine()->setColor(color);
    if(backLine() != nullptr && backNode() != nullptr)
        backLine()->setColor(color);
}

void Voxel3DNode::updateLineAlpha(float a)
{
    if(leftLine() != nullptr && leftNode() != nullptr)
        leftLine()->setAlpha(a);
    if(rightLine() != nullptr && rightNode() != nullptr)
        rightLine()->setAlpha(a);
    if(upLine() != nullptr && upNode() != nullptr)
        upLine()->setAlpha(a);
    if(downLine() != nullptr && downNode() != nullptr)
        downLine()->setAlpha(a);
    if(frontLine() != nullptr && frontNode() != nullptr)
        frontLine()->setAlpha(a);
    if(backLine() != nullptr && backNode() != nullptr)
        backLine()->setAlpha(a);
}
