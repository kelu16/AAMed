#include "aameditor.h"
#include "ui_aameditor.h"

#define fl at<float>

AAMed::AAMed(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AAMed)
{
    ui->setupUi(this);


    //this->setCentralWidget(ui->cvImage);

    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionLoadImage, SIGNAL(triggered()), this, SLOT(loadImage()));
    connect(ui->actionLoadModel, SIGNAL(triggered()), this, SLOT(loadModel()));
    connect(ui->actionLoadTrainingData, SIGNAL(triggered()), this, SLOT(loadTrainingData()));
    connect(ui->actionSaveTrainingData, SIGNAL(triggered()), this, SLOT(saveTrainingData()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(ui->actionMoveLeft, SIGNAL(triggered()), this, SLOT(moveViewportLeft()));
    connect(ui->actionMoveRight, SIGNAL(triggered()), this, SLOT(moveViewportRight()));
    connect(ui->actionMoveUp, SIGNAL(triggered()), this, SLOT(moveViewportUp()));
    connect(ui->actionMoveDown, SIGNAL(triggered()), this, SLOT(moveViewportDown()));
    connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
    connect(ui->cvImage, SIGNAL(clicked(QMouseEvent*, QPoint)), this, SLOT(mouseHandler(QMouseEvent*, QPoint)));
    connect(ui->cvImage, SIGNAL(wheel(QWheelEvent*)), this, SLOT(wheelHandler(QWheelEvent*)));
    connect(ui->actionSelect_Mouth, SIGNAL(triggered()), this, SLOT(selectMouth()));
    connect(ui->actionSelect_Nose, SIGNAL(triggered()), this, SLOT(selectNose()));
    connect(ui->actionSelect_Left_Eye, SIGNAL(triggered()), this, SLOT(selectLeftEye()));
    connect(ui->actionSelect_Right_Eye, SIGNAL(triggered()), this, SLOT(selectRightEye()));
    connect(ui->actionDeselect_All_Points, SIGNAL(triggered()), this, SLOT(deselectAllPoints()));
    connect(ui->actionSelect_All_Points, SIGNAL(triggered()), this, SLOT(selectAllPoints()));
    connect(ui->actionScale_Down, SIGNAL(triggered()), this, SLOT(scaleSelectedDown()));
    connect(ui->actionScale_Up, SIGNAL(triggered()), this, SLOT(scaleSelectedUp()));
    connect(ui->actionAlign_Eyes, SIGNAL(triggered()), this, SLOT(alignEyes()));
    connect(ui->actionAlign_Mouth, SIGNAL(triggered()), this, SLOT(alignMouth()));
    connect(ui->actionAlign_Nose, SIGNAL(triggered()), this, SLOT(alignNose()));

    //this->setImage(Mat::zeros(ui->cvImage->height(), ui->cvImage->width(), CV_8UC3));
    /*
    this->loadModel("/home/lucas/Dropbox/Diplomarbeit/Code/candide.xml");

    this->model.placeModelInBounds(Rect(ui->cvImage->width()/2-100,ui->cvImage->height()/2-100,200,200));
    this->showImage();
    */
    this->mouseSelection = false;
    this->mouseMovePoints = false;
    this->mouseMoveViewport = false;
}

AAMed::~AAMed()
{
    delete ui;
}

void AAMed::zoomIn() {
    if(this->zoom < 3) {
        this->zoom++;
        //this->setViewWindow(this->viewX*2+320, this->viewY*2+240);
        this->viewX = this->viewX*2+ui->cvImage->width()/2;
        this->viewY = this->viewY*2+ui->cvImage->height()/2;
    }
    cout<<this->zoom<<endl;
    zoomedImage = this->image.clone();
    for(int i=0; i<this->zoom; i++) {
        pyrUp(zoomedImage, zoomedImage, Size(zoomedImage.cols*2, zoomedImage.rows*2));
    }
    this->showImage();
}

void AAMed::zoomOut() {
    if(this->zoom > 0) {
        this->zoom--;
        //this->setViewWindow((this->viewX-320)/2,(this->viewY-240)/2);
        this->viewX = max((int)min(floor((this->viewX-ui->cvImage->width()/2)/2),this->image.cols*pow(2,this->zoom)-ui->cvImage->width()),0);
        this->viewY = max((int)min(floor((this->viewY-ui->cvImage->height()/2)/2),this->image.rows*pow(2,this->zoom)-ui->cvImage->height()),0);
    }
    cout<<this->zoom<<endl;
    zoomedImage = this->image.clone();
    for(int i=0; i<this->zoom; i++) {
        pyrUp(zoomedImage, zoomedImage, Size(zoomedImage.cols*2, zoomedImage.rows*2));
    }
    this->showImage();
}

void AAMed::moveViewportLeft() {
    if(this->viewX > 0) {
        this->viewX -= this->zoom+1;
    }
    this->showImage();
}

void AAMed::moveViewportRight() {
    if(this->viewX < this->image.cols*pow(2,this->zoom)-ui->cvImage->width()) {
        this->viewX += this->zoom+1;
    }
    this->showImage();
}

void AAMed::moveViewportUp() {
    if(this->viewY > 0) {
        this->viewY -= this->zoom+1;
    }
    this->showImage();
}

void AAMed::moveViewportDown() {
    cout<<this->image.rows*pow(2,this->zoom)<<" "<<ui->cvImage->height()<<endl;
    if(this->viewY < this->image.rows*pow(2,this->zoom)-ui->cvImage->height()) {
        this->viewY += this->zoom+1;
    }
    this->showImage();
}

void AAMed::updateSelection() {
    this->model.unselectAllPoints();
    int listSize = ui->listWidget->count();
    for(int i=0; i<listSize; i++) {
        if(ui->listWidget->item(i)->isSelected()) {
            this->model.selectPoint(i);
        }
    }
    showImage();
}

void AAMed::selectAllPoints() {
    int size = ui->listWidget->count();

    for(int i=0; i<size; i++) {
        ui->listWidget->item(i)->setSelected(true);
    }
}


void AAMed::deselectAllPoints() {
    int size = ui->listWidget->count();

    for(int i=0; i<size; i++) {
        ui->listWidget->item(i)->setSelected(false);
    }
}

void AAMed::setViewWindow(double x, double y) {
    //cout<<min(x, this->image.cols*pow(2,this->zoom)-640)<<endl;
    this->viewX = max(min(x, this->image.cols*pow(2,this->zoom)-ui->cvImage->width()),0.0d);
    this->viewY = max(min(y, this->image.rows*pow(2,this->zoom)-ui->cvImage->height()),0.0d);
}

void AAMed::loadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "", tr("Image Files (*.png *.jpg *.gif)"));
    if(!fileName.isEmpty()) {
        loadImage(fileName.toLocal8Bit().data());
    }
}

void AAMed::loadImage(std::string fileName) {
    this->setImage(imread(fileName));

    imageName = fileName.substr(fileName.find_last_of("/\\") + 1);
    this->setWindowTitle(QString::fromStdString(imageName));

    if(this->model.isInitialized()) {
        this->alignModel();
    }
    showImage();
}

void AAMed::alignModel() {
    String faceCascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml";
    CascadeClassifier faceCascade;

    faceCascade.load(faceCascadeName);

    vector<Rect> faces;
    Mat gray;

    cvtColor(this->image, gray, CV_BGR2GRAY);
    equalizeHist(gray, gray);

    faceCascade.detectMultiScale(gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100,100));

    if(!faces.empty()) {
        this->model.placeModelInBounds(faces[0]);
    }
}

void AAMed::alignMouth() {
    Mat face;
    cvtColor(this->image, face, CV_BGR2GRAY);
    equalizeHist(face, face);

    Mat points = this->model.getPoints();
    double minX,minY,maxX,maxY;
    minMaxIdx(points.col(0), &minX, &maxX);
    minMaxIdx(points.col(1), &minY, &maxY);
    Rect bounds = Rect(minX, minY, (maxX-minX), (maxY-minY));
    face = face(bounds);

    //Mat face = gray(faces[0]);

    String mouthCascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_smile.xml";
    CascadeClassifier mouthCascade;

    mouthCascade.load(mouthCascadeName);

    vector<Rect> mouth;
    mouthCascade.detectMultiScale(face, mouth, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100,100));

    if(!mouth.empty()) {
        mouth[0].x += bounds.x;
        mouth[0].y += bounds.y;
        this->model.placeMouthInBounds(mouth[0]);
    }

    this->selectMouth();
}

void AAMed::alignNose() {
    Mat face;
    cvtColor(this->image, face, CV_BGR2GRAY);
    equalizeHist(face, face);

    Mat points = this->model.getPoints();
    double minX,minY,maxX,maxY;
    minMaxIdx(points.col(0), &minX, &maxX);
    minMaxIdx(points.col(1), &minY, &maxY);
    Rect bounds = Rect(minX, minY, (maxX-minX), (maxY-minY));
    face = face(bounds);

    //Mat face = gray(faces[0]);

    String noseCascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_mcs_nose.xml";
    CascadeClassifier noseCascade;

    noseCascade.load(noseCascadeName);

    vector<Rect> nose;
    noseCascade.detectMultiScale(face, nose, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100,100));

    cout<<nose[0]<<endl;
    if(!nose.empty()) {
        nose[0].x += bounds.x;
        nose[0].y += bounds.y;
        this->model.placeNoseInBounds(nose[0]);
    }

    this->selectNose();
}

void AAMed::alignEyes() {
    Mat face;
    cvtColor(this->image, face, CV_BGR2GRAY);
    equalizeHist(face, face);

    Mat points = this->model.getPoints();
    double minX,minY,maxX,maxY;
    minMaxIdx(points.col(0), &minX, &maxX);
    minMaxIdx(points.col(1), &minY, &maxY);
    Rect bounds = Rect(minX, minY, (maxX-minX), (maxY-minY));
    face = face(bounds);

    String eyeCascadeName = "/usr/local/share/OpenCV/haarcascades/haarcascade_lefteye_2splits.xml";
    CascadeClassifier eyeCascade;

    eyeCascade.load(eyeCascadeName);

    vector<Rect> eyes;
    eyeCascade.detectMultiScale(face, eyes, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100,100));

    if(!eyes.empty() && eyes.size() >= 2) {
        Rect leftEye, rightEye;
        if(eyes[0].x < eyes[1].x) {
            leftEye = eyes[0];
            rightEye = eyes[1];
        } else {
            leftEye = eyes[1];
            rightEye = eyes[0];
        }
        leftEye.x += bounds.x;
        leftEye.y += bounds.y;
        this->model.placeRightEyeInBounds(leftEye);

        rightEye.x += bounds.x;
        rightEye.y += bounds.y;
        this->model.placeLeftEyeInBounds(rightEye);
    }

    this->selectEyes();
}

void AAMed::selectGroup(int group) {
    this->deselectAllPoints();
    Mat points = this->model.getPoints();
    Mat groups = this->model.getGroups();

    for(int i=0; i<points.rows; i++) {
        if(group & (int)groups.fl(i)) {
            this->ui->listWidget->item(i)->setSelected(true);
        }
    }
}

void AAMed::selectMouth() {
    this->selectGroup(MODEL_MOUTH);
}

void AAMed::selectNose() {
    this->selectGroup(MODEL_NOSE);
}

void AAMed::selectLeftEye() {
    this->selectGroup(MODEL_LEFTEYE);
}

void AAMed::selectRightEye() {
    this->selectGroup(MODEL_RIGHTEYE);
}

void AAMed::selectEyes() {
    this->selectGroup(MODEL_LEFTEYE | MODEL_RIGHTEYE);
}

void AAMed::loadModel() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "", "*.xml");
    if(!fileName.isEmpty()) {
        loadModel(fileName.toLocal8Bit().data());
    }
}

void AAMed::loadModel(std::string fileName) {
    this->model.loadDataFromFile(fileName);

    ui->listWidget->clear();
    vector <string> desc = this->model.getDescriptions();
    int size = desc.size();
    for(int i=0; i<size; i++) {
        ui->listWidget->addItem(QString::fromStdString(desc.at(i)));
    }

    if(!this->image.empty()) {
        this->model.setImage(this->image);
        this->alignModel();
    }
}

void AAMed::loadTrainingData() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "", "*.xml");
    if(!fileName.isEmpty()) {
        loadTrainingData(fileName.toLocal8Bit().data());
    }
}

void AAMed::loadTrainingData(std::string fileName) {
    Model m;

    m.loadDataFromFile(fileName);
    this->model = m;
    this->setImage(m.getImage());

    ui->listWidget->clear();
    vector <string> desc = this->model.getDescriptions();
    int size = desc.size();
    for(int i=0; i<size; i++) {
        ui->listWidget->addItem(QString::fromStdString(desc.at(i)));
    }

    imageName = fileName.substr(fileName.find_last_of("/\\") + 1);
    this->setWindowTitle(QString::fromStdString(imageName));

    this->showImage();
}

void AAMed::saveTrainingData(string fileName) {
    this->model.saveDataToFile(fileName);
}

void AAMed::saveTrainingData() {
    string file = this->imageName.substr(0,this->imageName.find_last_of('.'))+".xml";
    QString fileName = QFileDialog::getSaveFileName(this, "Save file", QString::fromStdString(file), "*.xml");

    saveTrainingData(fileName.toLocal8Bit().data());
}

void AAMed::setImage(Mat img) {
    this->zoom = 0;
    this->viewX = 0;
    this->viewY = 0;
    this->image = img;
    this->zoomedImage = this->image.clone();
    ui->cvImage->showImage(this->image);
}

void AAMed::drawPoints(Mat &img, Mat p) {
    Scalar color;
    for(int i=0; i<p.rows; i++) {
        if(model.isPointSelected(i)) {
            color = Scalar(0,0,255);
        } else {
            color = Scalar(0,255,0);
        }
        circle(img,Point2f(p.fl(i,0),p.fl(i,1)),2,color,CV_FILLED,0);
    }
}

void AAMed::drawTriangles(Mat &img, Mat p, Mat t) {
    for(int i=0; i<t.rows; i++) {
        Point a,b,c;
        a = this->model.getPointFromMat(p, t.at<int>(i,0));
        b = this->model.getPointFromMat(p, t.at<int>(i,1));
        c = this->model.getPointFromMat(p, t.at<int>(i,2));

        line(img, a, b, Scalar(255,0,0),1);
        line(img, a, c, Scalar(255,0,0),1);
        line(img, b, c, Scalar(255,0,0),1);
    }
}

void AAMed::drawSelectionBox(Mat &img) {
    rectangle(img,selection,Scalar(200,200,200));
}

void AAMed::showImage() {
    if(!this->image.empty()) {
        Mat outImage = zoomedImage.clone();
        if(this->model.isInitialized()) {
            Mat points = this->model.getPoints().clone();
            for(int i=0; i<this->zoom; i++) {
                points.col(0) = points.col(0)*2;
                points.col(1) = points.col(1)*2;
            }
            this->drawTriangles(outImage, points, this->model.getTriangles());
            this->drawPoints(outImage, points);
        }

        Mat view = outImage(Rect(this->viewX, this->viewY, ui->cvImage->width(), ui->cvImage->height()));
        this->drawSelectionBox(view);

        ui->cvImage->showImage(view, true);
    }
}

void AAMed::mouseHandler(QMouseEvent *ev, QPoint pos) {
    int events;
    int x = pos.x();
    int y = pos.y();
    int id = -1;
    QString statusText = "";
    Qt::CursorShape cursor = Qt::ArrowCursor;

    Point imgPos = Point((this->viewX+x)/pow(2,this->zoom), (this->viewY+y)/pow(2,this->zoom));

    selection = Rect(Point(0,0),Point(0,0));

    if((ev->type()==2) && (ev->button()==1)) {
        events = CV_EVENT_LBUTTONDOWN;
    } else if ((ev->type()==2) && (ev->button()==2)) {
        events = CV_EVENT_RBUTTONDOWN;
    } else if ((ev->type()==3) && (ev->button()==1)) {
        events = CV_EVENT_LBUTTONUP;
    } else if ((ev->type()==3) && (ev->button()==2)) {
        events = CV_EVENT_RBUTTONUP;
    } else if (ev->type()==5) {
        events = CV_EVENT_MOUSEMOVE;
    }

    if(this->model.isInitialized()) {
        id = this->model.findPointToPosition(imgPos, 1);
    }

    switch(events) {
    case CV_EVENT_LBUTTONDOWN:
        if(id > -1) {
            this->mouseSource = Point(x,y);
            this->mouseMovePoints = true;
            ui->listWidget->item(id)->setSelected(true);
        } else {
            this->mouseSource = Point(x,y);
            this->mouseMoveViewport = true;
            cursor = Qt::ClosedHandCursor;
        }
        break;
    case CV_EVENT_LBUTTONUP:
        if(id > -1) {

        } else if(!this->mouseMoveViewport) {
            int size = ui->listWidget->count();

            for(int i=0; i<size; i++) {
                ui->listWidget->item(i)->setSelected(false);
            }
        }

        this->mouseMovePoints = false;
        this->mouseMoveViewport = false;
        break;
    case CV_EVENT_RBUTTONDOWN: {
        mouseSource = Point(x,y);
        this->mouseSelection = true;
        this->model.unselectAllPoints();

        int size = ui->listWidget->count();

        for(int i=0; i<size; i++) {
            ui->listWidget->item(i)->setSelected(false);
        }
        }
        break;
    case CV_EVENT_RBUTTONUP: {
        this->mouseSelection = false;
        this->selection = Rect(mouseSource, Point(x,y));

        Rect selectionInView = this->selection + Point(this->viewX, this->viewY);
        Rect selectionZoomed = Rect(selectionInView.tl()/pow(2,this->zoom), selectionInView.br()/pow(2,this->zoom));

        this->model.selectPointsInRect(selectionZoomed);
        vector <int> selectedPoints = this->model.getSelectedPoints();
        int numSelectedPoints = selectedPoints.size();
        for(int i=0; i<numSelectedPoints; i++) {
            int id = selectedPoints.at(i);
            ui->listWidget->item(id)->setSelected(true);
        }
        }
        break;
    case CV_EVENT_MOUSEMOVE:
        int size = ui->listWidget->count();

        for(int i=0; i<size; i++) {
            ui->listWidget->item(i)->setFont(QFont());
        }

        if(this->mouseSelection) {
            this->selection = Rect(mouseSource, Point(x,y));

            Rect selectionInView = this->selection + Point(this->viewX, this->viewY);
            Rect selectionZoomed = Rect(selectionInView.tl()/pow(2,this->zoom), selectionInView.br()/pow(2,this->zoom));

            this->model.selectPointsInRect(selectionZoomed);
            vector <int> selectedPoints = this->model.getSelectedPoints();
            int numSelectedPoints = selectedPoints.size();
            for(int i=0; i<numSelectedPoints; i++) {
                int id = selectedPoints.at(i);
                ui->listWidget->item(id)->setSelected(true);
            }
        } else if(this->mouseMovePoints) {
            float dx = (mouseSource.x-x)/pow(2,this->zoom);
            float dy = (mouseSource.y-y)/pow(2,this->zoom);
            this->model.moveSelectedVertices(dx, dy);
            this->mouseSource = Point(x,y);
        } else if(this->mouseMoveViewport) {
            float dx = (mouseSource.x-x);
            float dy = (mouseSource.y-y);
            this->mouseSource = Point(x,y);
            this->setViewWindow(this->viewX+dx, this->viewY+dy);
            cursor = Qt::ClosedHandCursor;
        } else if(id > -1) {
            QFont fontBold;
            fontBold.setBold(true);
            ui->listWidget->item(id)->setFont(fontBold);
            statusText = "Point "+QString::number(id)+": "+ui->listWidget->item(id)->text();
            cursor = Qt::PointingHandCursor;
        }

        break;
    }

    this->setCursor(cursor);
    this->ui->statusBar->showMessage("["+QString::number(imgPos.x)+";"+QString::number(imgPos.y)+"] "+statusText);
    showImage();
}

void AAMed::wheelHandler(QWheelEvent *ev) {
    if(ev->delta() > 0) {
        this->zoomIn();
    } else {
        this->zoomOut();
    }
}

void AAMed::scaleSelectedDown() {
    this->model.scaleSelectedPoints(1/1.05);
    showImage();
}

void AAMed::scaleSelectedUp() {
    this->model.scaleSelectedPoints(1.05);
    showImage();
}
