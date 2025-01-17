#include "dedistortion.h"
#include "mainwindow.h"
#include <QApplication>
#include "common.h"
dedistortionImageWindow::dedistortionImageWindow(QWidget *parent)
    : QWidget(parent)
      
{
    initUI();

    importCompletedButton->setEnabled(false); // 初始时禁用 "导入完成" 按钮
}
void dedistortionImageWindow::initUI()
{
    this->resize(800, 200); // 设置窗口大小

    // 创建按钮和标签
    chooseVisibleImageButton = new QPushButton("选择包含可见光文件夹的路径", this);
    chooseInfraredImageButton = new QPushButton("选择红外图片路径", this);
    importCompletedButton = new QPushButton("导入完成", this);

    visibleImagePathLabel1 = new QLabel("未选择可见光图片1路径", this);
    visibleImagePathLabel2 = new QLabel("未选择可见光图片2路径", this);
    visibleImagePathLabel3 = new QLabel("未选择可见光图片3路径", this);
    visibleImagePathLabel4 = new QLabel("未选择可见光图片4路径", this);
    infraredImagePathLabel = new QLabel("未选择红外图片路径", this);

    // 设置按钮和标签的几何位置
    chooseVisibleImageButton->setGeometry(100, 100, 200, 40);
    visibleImagePathLabel1->setGeometry(100, 160, 200, 40);
    visibleImagePathLabel2->setGeometry(100, 220, 200, 40);
    visibleImagePathLabel3->setGeometry(100, 280, 200, 40);
    visibleImagePathLabel4->setGeometry(100, 340, 200, 40);
    chooseInfraredImageButton->setGeometry(100, 400, 300, 40);
    infraredImagePathLabel->setGeometry(100, 460, 300, 40);
    importCompletedButton->setGeometry(100, 520, 200, 40);

    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chooseVisibleImageButton);
    layout->addWidget(visibleImagePathLabel1);
    layout->addWidget(visibleImagePathLabel2);
    layout->addWidget(visibleImagePathLabel3);
    layout->addWidget(visibleImagePathLabel4);
    layout->addWidget(chooseInfraredImageButton);
    layout->addWidget(infraredImagePathLabel);
    layout->addWidget(importCompletedButton);

    // 连接按钮点击事件
    connect(chooseVisibleImageButton, &QPushButton::clicked, this, &dedistortionImageWindow::onSelectVisibleImagePathClicked);
    connect(chooseInfraredImageButton, &QPushButton::clicked, this, &dedistortionImageWindow::onSelectInfraredImagePathClicked);
    connect(importCompletedButton, &QPushButton::clicked, this, &dedistortionImageWindow::onImportCompleted);
}

dedistortionImageWindow::~dedistortionImageWindow()
{
}

void dedistortionImageWindow::dedis_image(QString visibleImagePath, std::string intrinsics,std::string number)
{
    std::string svisibleImagePath = visibleImagePath.toStdString();
    cv::Mat src_img = cv::imread(svisibleImagePath);
    std::string intrinsicspath = intrinsics;

    // 检查图像是否成功加载
    if (src_img.empty())
    {
        std::cerr << "无法加载图像!" << std::endl;
        return;
    }

    std::vector<float> intrinsic, distortion;
    getIntrinsic(intrinsicspath, intrinsic);
    getDistortion(intrinsicspath, distortion);

    cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    camera_matrix.at<double>(0, 0) = intrinsic[0]; // 焦距 fx
    camera_matrix.at<double>(0, 2) = intrinsic[2]; // 主点 x 坐标
    camera_matrix.at<double>(1, 1) = intrinsic[4]; // 焦距 fy
    camera_matrix.at<double>(1, 2) = intrinsic[5]; // 主点 y 坐标

    // 畸变系数向量，通常包括 5 个系数：k1, k2, k3, p1, p2
    cv::Mat distortion_coef = cv::Mat::zeros(5, 1, CV_64F);
    distortion_coef.at<double>(0, 0) = distortion[0]; // 径向畸变系数 k1
    distortion_coef.at<double>(1, 0) = distortion[1]; // 径向畸变系数 k2
    distortion_coef.at<double>(2, 0) = distortion[2]; // 径向畸变系数 k3
    distortion_coef.at<double>(3, 0) = distortion[3]; // 切向畸变系数 p1
    distortion_coef.at<double>(4, 0) = distortion[4]; // 切向畸变系数 p2

    // 校正后的图像
    cv::Mat corrected_img;

    // 获取图像的尺寸
    cv::Size imageSize = src_img.size();

    // 定义映射矩阵
    cv::Mat map1, map2;

    // 获取去畸变的映射
    cv::initUndistortRectifyMap(camera_matrix, distortion_coef, cv::Mat(),
                                cv::getOptimalNewCameraMatrix(camera_matrix, distortion_coef, imageSize, 1, imageSize, 0),
                                imageSize, CV_16SC2, map1, map2);

    // 使用映射进行图像去畸变
    cv::remap(src_img, corrected_img, map1, map2, cv::INTER_LINEAR);
    std::string fullname = file_path+"/distort/"+number+".bmp";
    cv::imwrite(fullname, corrected_img);
    
}

void dedistortionImageWindow::onSelectVisibleImagePathClicked()
{
    // 打开文件夹选择对话框，让用户选择存储图片的文件夹
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择包含可见光图片的文件夹");

    if (!dirPath.isEmpty())
    {
        // 获取文件夹中的所有文件
        QDir dir(dirPath);
        QStringList filter;
        filter << "*.bmp";  // 只选择 bmp 格式的图片
        dir.setNameFilters(filter);

        // 获取文件夹中的所有图片文件
        QStringList imageFiles = dir.entryList(QDir::Files);
        
        if (imageFiles.size() != 4) {
            qDebug() << "错误：文件夹中没有找到 4 张图片。";
            return;
        }

        // 更新路径变量和标签
        visibleImagePath1 = dirPath + "/" + imageFiles.at(0);
        visibleImagePath2 = dirPath + "/" + imageFiles.at(1);
        visibleImagePath3 = dirPath + "/" + imageFiles.at(2);
        visibleImagePath4 = dirPath + "/" + imageFiles.at(3);

        visibleImagePathLabel1->setText(visibleImagePath1);
        visibleImagePathLabel2->setText(visibleImagePath2);
        visibleImagePathLabel3->setText(visibleImagePath3);
        visibleImagePathLabel4->setText(visibleImagePath4);

        qDebug() << "Selected Visible Image Paths:";
        qDebug() << visibleImagePath1;
        qDebug() << visibleImagePath2;
        qDebug() << visibleImagePath3;
        qDebug() << visibleImagePath4;

        // 启用导入完成按钮
        importCompletedButton->setEnabled(true);

        // 调用 dedis_image 处理每张图片
        std::string img1 = "img1";
        std::string img2 = "img2";
        std::string img3 = "img3";
        std::string img4 = "img4";

        dedis_image(visibleImagePath1, intrinsicspath1, img1);
        dedis_image(visibleImagePath2, intrinsicspath2, img2);
        dedis_image(visibleImagePath3, intrinsicspath3, img3);
        dedis_image(visibleImagePath4, intrinsicspath4, img4);
    }
}


void dedistortionImageWindow::onSelectInfraredImagePathClicked()
{
        QString filePath = QFileDialog::getOpenFileName(this, "选择红外图片", "", "Images (*.bmp)");

    if (!filePath.isEmpty()) {
        infraredImagePath = filePath;  // 保存选择的路径
        infraredImagePathLabel->setText(filePath);   // 更新标签显示路径

        qDebug() << "Selected Infrared Image Path: " << filePath;
        importCompletedButton->setEnabled(true);
    }
}

void dedistortionImageWindow::onImportCompleted()
{
    Stitching_main("distort");
    // 跳转到主界面并传递路径
    this->close();
    QString visibleImagePath = QString::fromStdString(file_path+"/stitch/stitch.bmp");
    MainWindow *mainWindow = new MainWindow(visibleImagePath,infraredImagePath,this); // 传递路径到主界面
    mainWindow->show();
}

void dedistortionImageWindow::ReadImage(const std::string &input_dir, vector<cv::Mat> &images)
{
    try {
        vector<string> im_path;
        // 遍历目录
        for (const auto& entry : fs::directory_iterator(input_dir)) {
            // 检查是否是 .bmp 文件
            if (entry.is_regular_file() && entry.path().extension() == ".bmp") {
                std::string file_path = entry.path().string(); // 获取文件路径
                im_path.push_back(file_path);

            }
        }
        sort(im_path.begin(),im_path.end(),greater<string>());
        std::cout << "Total images loaded: " << im_path.size() << std::endl;
        for(const auto &file_path : im_path)
        {
            cv::Mat img = cv::imread(file_path, cv::IMREAD_COLOR);
            if (img.empty()) {
                    std::cerr << "Error reading image: " << file_path << std::endl;
                } else {
                    // imshow("img"+file_path,img);
                    images.push_back(img); // 将图像添加到 vector
                    std::cout << "Loaded image: " << file_path << std::endl;
                }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

bool dedistortionImageWindow::OpenCV_Stitching(std::vector<cv::Mat> total_images, cv::Mat &result)
{
    // 创建一个 Stitcher 实例进行拼接
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);
    stitcher->setPanoConfidenceThresh(0.4); // 增加置信度阈值，避免错误拼接
    // 执行拼接
    cv::Stitcher::Status status = stitcher->stitch(total_images, result);
    cout << "status:" << status << endl;
    if (status == cv::Stitcher::OK)
    {

        return true;
    }
    else
    {
        return false;
    }
}

void dedistortionImageWindow::Stitching_main(std::string distort_path)
{
    cv::Mat result1;
    std::vector<cv::Mat> images1;
    string input_path = file_path+"/"+distort_path;
    ReadImage(input_path,images1);
    // 检查拼接状态
    if (OpenCV_Stitching(images1, result1))
    {

        cout << "全景图拼接成功" << endl;

        imwrite(file_path+"/stitch/stitch.bmp",result1);
        // imshow("stitched",stitched);
        
    }
    else
    {
        cerr << "拼接失败：未知错误" << endl;
    }

}


