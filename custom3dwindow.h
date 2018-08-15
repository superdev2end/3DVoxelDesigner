#ifndef CUSTOM3DWINDOW_H
#define CUSTOM3DWINDOW_H

#include <Qt3DExtras/Qt3DWindow>

class Custom3DWindow : public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT
public:
    Custom3DWindow();

signals:
    void mousePressSignal(QMouseEvent* event);
    void mouseReleaseSignal(QMouseEvent* event);
    void mouseMoveSignal(QMouseEvent* event);
    void wheelSignal(QWheelEvent *event);
    void keyPressSignal(QKeyEvent *event);
    void keyReleaseSignal(QKeyEvent *event);
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // CUSTOM3DWINDOW_H
