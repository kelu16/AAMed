#ifndef AAMEDITOR_H
#define AAMEDITOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QListView>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>
#include "../AAMlib/model.h"
#include "../AAMlib/trainingdata.h"

using namespace cv;
using namespace std;


namespace Ui {
class AAMed;
}

class AAMed : public QMainWindow
{
    Q_OBJECT

public:
    explicit AAMed(QWidget *parent = 0);
    ~AAMed();

public slots:
    void loadImage(std::string fileName);
    void loadImage();
    void loadModel(std::string fileName);
    void loadModel();
    void loadTrainingData(std::string fileName);
    void loadTrainingData();
    void saveTrainingData(std::string fileName);
    void saveTrainingData();
    void zoomIn();
    void zoomOut();
    void moveViewportLeft();
    void moveViewportRight();
    void moveViewportUp();
    void moveViewportDown();
    void updateSelection();
    void mouseHandler(QMouseEvent* ev, QPoint pos);
    void wheelHandler(QWheelEvent * ev);

    void alignModel();
    void alignMouth();
    void alignNose();
    void alignEyes();

    void selectAllPoints();
    void deselectAllPoints();
    void selectMouth();
    void selectNose();
    void selectLeftEye();
    void selectRightEye();
    void selectEyes();

    void scaleSelectedUp();
    void scaleSelectedDown();

private:
    Ui::AAMed *ui;
    Mat image;
    Mat zoomedImage;
    Model model;

    Point mouseSource;
    bool mouseSelection;
    bool mouseMovePoints;
    bool mouseMoveViewport;
    Rect selection;

    string modelFile;
    string imageName;

    int zoom, viewX, viewY;

    void setViewWindow(double x, double y);
    void setImage(Mat img);
    void drawPoints(Mat &img, Mat p);
    void drawTriangles(Mat &img, Mat p, Mat t);
    void drawSelectionBox(Mat &img);
    void showImage();
    void selectGroup(int group);
};

#endif // AAMed_H
