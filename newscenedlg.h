#ifndef NEWSCENEDLG_H
#define NEWSCENEDLG_H

#include <QDialog>
#include <QIntValidator>

namespace Ui {
class NewSceneDlg;
}

class NewSceneDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewSceneDlg(QWidget *parent = nullptr);
    ~NewSceneDlg();

public:
    int getCubeSizeX();
    int getCubeSizeY();
    int getCubeSizeZ();
    int getSpacingSize();
    int getObjMovementField();

private slots:
    void on_rad_8VertexInc_toggled(bool checked);

    void on_rad_16VertexInc_toggled(bool checked);

    void on_rad_32VertexInc_toggled(bool checked);

    void on_rad_80VertexInc_toggled(bool checked);

    void on_rad_43VertexInc_toggled(bool checked);

    void on_rad_85VertexInc_toggled(bool checked);

private:
    Ui::NewSceneDlg *ui;
};

#endif // NEWSCENEDLG_H
