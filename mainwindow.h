#pragma once
#include <QDebug> 
#include "opencv2/opencv.hpp"
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <QLabel>
#include <vector>
#include <string> 
#include <filesystem> //文件批处理

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &visibleImgPath, const QString &infraredImgPath, QWidget *parent = nullptr);
    
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;  // 重写 paintEvent 用于绘制图片和标注点
    void mousePressEvent(QMouseEvent *event) override;  // 监听鼠标点击事件

private:
    std::string file_path = std::filesystem::path(__FILE__).parent_path().string();
    Ui::MainWindow *ui;
    QPixmap img1, img2;  // 存储两张图片
    QPixmap blendedImg;  // 存储叠加后的图片
    std::vector<QPoint> map;  // 存储绘制的坐标点
    std::vector<cv::Point2f> map1, map2;  // 存储控制点
    QString visibleImgPath;  // 存储可见光图片路径
    QString infraredImgPath; // 存储红外图片路径
    void loadImages();  // 加载图片
    void drawFeaturePoints(QPainter &painter);  // 绘制标注点
    void blendImages();  // 叠加图像并保存到 blendedImg
    QPushButton *blendButton;  // 新增按钮
    
};