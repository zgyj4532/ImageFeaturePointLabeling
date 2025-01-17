#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/features2d.hpp>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>
#include <QDebug>
#include <filesystem> //文件批处理
#include <vector>
#include <iostream>

namespace fs = std::filesystem;
class dedistortionImageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit dedistortionImageWindow(QWidget *parent = nullptr);
    ~dedistortionImageWindow();

private slots:
    void onSelectInfraredImagePathClicked();
    void onImportCompleted();
    void dedis_image(QString visibleImagePath, std::string intrinsics, std::string number);
    void onSelectVisibleImagePathClicked();
    void ReadImage(const std::string &input_dir, std::vector<cv::Mat> &images);
    bool OpenCV_Stitching(std::vector<cv::Mat> total_images, cv::Mat &result);
    void Stitching_main(std::string distort_path);

private:
    // 按钮和标签
    QPushButton *chooseVisibleImageButton, *chooseInfraredImageButton, *importCompletedButton;
    QLabel *visibleImagePathLabel1, *visibleImagePathLabel2, *visibleImagePathLabel3, *visibleImagePathLabel4;
    QLabel *infraredImagePathLabel;

    // 图像路径
    QString visibleImagePath1, visibleImagePath2, visibleImagePath3, visibleImagePath4;
    QString infraredImagePath;

    // 内参路径
    std::string file_path = std::filesystem::path(__FILE__).parent_path().string();
    std::string intrinsicspath1 = file_path + "/intrinsics/intrinsic1.txt";
    std::string intrinsicspath2 = file_path + "/intrinsics/intrinsic2.txt";
    std::string intrinsicspath3 = file_path + "/intrinsics/intrinsic3.txt";
    std::string intrinsicspath4 = file_path + "/intrinsics/intrinsic4.txt";
    // 初始化UI界面
    void initUI();
};