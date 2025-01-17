#include "mainwindow.h"
#include "build/ui_mainwindow.h"

MainWindow::MainWindow(const QString &visibleImgPath, const QString &infraredImgPath, QWidget *parent) : QMainWindow(parent), visibleImgPath(visibleImgPath), infraredImgPath(infraredImgPath),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int button_width=300;
    int button_height=180;
    this->resize(2560, 1440);
    this->setFixedSize(2560, 1440);  // 禁用窗口大小调整
    loadImages(); // 加载图片
    // 创建并设置"开始融合"按钮
    int windowWidth = width();
    int windowHeight = height();
    blendButton = new QPushButton("开始融合", this);
    blendButton->setGeometry(int(windowWidth / 4)-button_width/2,int(windowHeight / 4)-button_height/2,button_width,button_height); // 设置按钮的位置和大小（在左上角的中间）
    QFont font;
    font.setFamily("微软雅黑"); // 设置字体为微软雅黑
    font.setPointSize(14);      // 设置字体大小
    font.setBold(true);         // 设置字体为粗体
    blendButton->setFont(font);
    // 连接按钮的点击信号到槽函数
    connect(blendButton, &QPushButton::clicked, this, &MainWindow::blendImages);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 加载图片
void MainWindow::loadImages()
{
    // 获取窗口的宽度和高度
    int windowWidth = width();
    int windowHeight = height();
    
    // 计算窗口大小的二分之一
    int targetWidth = windowWidth / 2;
    int targetHeight = windowHeight / 2;
    // qDebug()<<"W:"<<targetWidth<<"H:"<<targetHeight;

    img1.load(infraredImgPath); // 图片路径（可以根据需要修改）
    img2.load(visibleImgPath); // 图片路径（可以根据需要修改）
    // 将图像缩放到窗口的四分之一大小
    img1 = img1.scaled(targetWidth, targetHeight, Qt::IgnoreAspectRatio);
    img2 = img2.scaled(targetWidth, targetHeight, Qt::IgnoreAspectRatio);
}
// 叠加图像并生成 blendedImg
void MainWindow::blendImages()
{
    if (map1.size() < 4 || map2.size() < 4)
    {
        std::cerr << "Error: Not enough control points selected!" << std::endl;
        return;
    }
    qDebug() << "融合按钮点击，执行图像融合";
    QSize img1Size = img1.size();                         // 获取 QPixmap 的大小
    cv::Size cvSize(img1Size.width(), img1Size.height()); // 转换为 cv::Size
    std::string sinfraredImgPath = infraredImgPath.toStdString();
    std::string svisibleImgPath = visibleImgPath.toStdString();
    // 通过 QPixmap -> QImage -> cv::Mat 转换 img1
    cv::Mat img1Mat = cv::imread(sinfraredImgPath, cv::IMREAD_COLOR);
    // 通过 QPixmap -> QImage -> cv::Mat 转换 img2
    cv::Mat img2Mat = cv::imread(svisibleImgPath, cv::IMREAD_COLOR);
    cv::resize(img1Mat, img1Mat, cvSize);
    cv::resize(img2Mat, img2Mat, cvSize);
    // 计算透视变换矩阵
    cv::Mat H = cv::findHomography(map1, map2, cv::RANSAC);
    std::cout << "H:"<<endl << H <<"end"<<endl;
    // 使用透视变换矩阵对第一张图进行变换
    cv::Mat warpedImg1;
    cv::warpPerspective(img1Mat,
                        warpedImg1, H, img2Mat.size());

    // 将 warpedImg1 转换为 BGRA（带有 Alpha 通道）
    cv::Mat warpedImg1Alpha;
    cv::cvtColor(warpedImg1, warpedImg1Alpha, cv::COLOR_BGR2BGRA);

    // 设置黑边为透明
    for (int i = 0; i < warpedImg1Alpha.rows; ++i)
    {
        for (int j = 0; j < warpedImg1Alpha.cols; ++j)
        {
            cv::Vec4b &pixel = warpedImg1Alpha.at<cv::Vec4b>(i, j);
            // 如果像素是黑色（即RGB都为0），则将Alpha通道设为0，表示透明
            if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
            {
                pixel[3] = 0; // 设置透明
            }
        }
    }

    // 将 warpedImg1Alpha 转换回 QPixmap
    cv::Mat blendedImgCV = img2Mat.clone();
    cv::Mat img2Alpha;
    cv::cvtColor(img2Mat, img2Alpha, cv::COLOR_BGR2BGRA);
    // 遍历图像的每一个像素，直接融合
    for (int i = 0; i < warpedImg1Alpha.rows; ++i)
    {
        for (int j = 0; j < warpedImg1Alpha.cols; ++j)
        {
            cv::Vec4b &warpedPixel = warpedImg1Alpha.at<cv::Vec4b>(i, j);
            cv::Vec4b &img2Pixel = img2Alpha.at<cv::Vec4b>(i, j);

            // 如果 warpedImg1 的 Alpha 值为0（透明），则使用 img2 的像素
            if (warpedPixel[3] == 0)
            {
                continue; // 不需要替换，保持 img2 原来的像素
            }

            // 否则，用 warpedImg1 的像素覆盖 img2
            img2Alpha.at<cv::Vec4b>(i, j) = warpedPixel;
        }
    }
    // 将图像转换回 BGR 格式
    img2Mat = cv::imread(svisibleImgPath, cv::IMREAD_COLOR);
    cv::cvtColor(img2Alpha, blendedImgCV, cv::COLOR_BGRA2BGR);
    cv::resize(blendedImgCV, blendedImgCV, img2Mat.size(), 1, 1);
    std::string blendedImgpath = file_path + "/output/result.bmp";
    cv::imwrite(blendedImgpath, blendedImgCV);
    cv::resize(blendedImgCV, blendedImgCV, cv::Size(), 0.5, 0.5);
    cv::cvtColor(blendedImgCV, blendedImgCV, cv::COLOR_BGR2RGB);
    QImage blendedQImage((const unsigned char *)blendedImgCV.data, blendedImgCV.cols,
                         blendedImgCV.rows, blendedImgCV.step, QImage::Format_RGB888);

    blendedImg = QPixmap::fromImage(blendedQImage);

}

// 重写 paintEvent 用于绘制图片和标注点
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int windowWidth = width();
    int windowHeight = height();

    // 绘制第一张图片 (右上角)
    painter.drawPixmap(windowWidth - img1.width(), 0, img1);

    // 绘制第二张图片 (右下角)
    painter.drawPixmap(windowWidth - img2.width(), windowHeight - img2.height(), img2);

    // 如果 blendedImg 已经计算，绘制它（左下角）
    if (!blendedImg.isNull())
    {
        blendedImg = blendedImg.scaled(img1.size(), Qt::KeepAspectRatio);
        painter.drawPixmap(0, windowHeight - blendedImg.height(), blendedImg);
    }

    // 绘制标注点
    drawFeaturePoints(painter);
    // 设置淡蓝色笔刷
    painter.setPen(QPen(QColor(0, 0, 139), 8));  // 深蓝色 (RGB: 0, 0, 139), 线宽8

    // 绘制竖线（在水平中间位置）
    int centerX = windowWidth / 2;
    painter.drawLine(centerX, 0, centerX, windowHeight);  // 竖线从上到下

    // 绘制横线（在垂直中间位置）
    int centerY = windowHeight / 2;
    painter.drawLine(0, centerY, windowWidth, centerY);  // 横线从左到右
}

// 绘制标注点
void MainWindow::drawFeaturePoints(QPainter &painter)
{
    painter.setPen(QPen(Qt::red));
    painter.setBrush(QBrush(Qt::red));

    // 设置字体样式（可选）
    QFont font = painter.font();
    font.setPointSize(10); // 设置字体大小
    painter.setFont(font);

    // 绘制 map 中存储的每个点击点
    int count = 1; // 用于标注数字
    for (const QPoint &point : map)
    {
        // 绘制红色圆点
        painter.drawEllipse(point, 5, 5); // 半径为 5 的圆

        // 计算文本的位置：在圆的上方（y坐标减去圆的半径 + 2）
        QPoint textPos(point.x() - 5, point.y() - 10); // 位置稍微偏移一下，避免重叠

        // 绘制数字（1, 2, 3 等）
        painter.drawText(textPos, QString::number(count));

        // 增加数字
        count++;
    }
}

// 监听鼠标点击事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 获取窗口的宽度和高度
    int windowWidth = width();
    int windowHeight = height();
    // 检查鼠标点击位置是否在第一张图片上（右上角）
    if (event->x() > windowWidth - img1.width() && event->y() < img1.height())
    {
        // 将点击位置转化为 cv::Point2f 类型，并加入 map1
        int x_left1 = event->x() - (windowWidth - img1.width());
        int y_left1 = event->y();

        // 将转换后的坐标加入 map1
        map1.push_back(cv::Point2f(x_left1, y_left1));
        map.push_back(QPoint(event->x(), event->y()));
    }
    // 检查鼠标点击位置是否在第二张图片上（右下角）
    else if (event->x() > windowWidth - img2.width() && event->y() > windowHeight - img2.height())
    {
        // 将点击位置转化为 cv::Point2f 类型，并加入 map2
        int x_left2 = event->x() - (windowWidth - img2.width());
        int y_left2 = event->y() - (windowHeight - img2.height());

        // 将转换后的坐标加入 map2
        map2.push_back(cv::Point2f(x_left2, y_left2));
        map.push_back(QPoint(event->x(), event->y()));
    }

    update(); // 更新界面，重新绘制
}
