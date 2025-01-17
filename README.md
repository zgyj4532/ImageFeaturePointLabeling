# ImageFeaturePointLabeling

_✨ 一个简单的图片处理项目 ✨_

## 📖 介绍

通过相机内参拼接图片并在可视化界面上实现红外图片与可见光的融合

## 💿 配置

> [!note]
>
> 请确保 安装Qt5，Opencv与Eigen库实现程序的正常运行

- 在inrinsics文件夹中导入inrinsics.txt 相机内参参数格式如：

<summary></summary>

    intrinsic
    900 0 600
    0 900.3 300
    0 0 1
    distorsion
    -0.3 0.1 -0.0003 -0.001 0

- 准备与相机对应的图片文件夹(.bmp,默认4张)

- 和红外图像图片（.bmp）

## 📝 使用

- 在命令行执行

<summary></summary>

    cmake .
    make
    ./build/ImageFeaturePointLabeling

- 之后按照要求选择可见光文件夹与红外图像
- 如果按下导入完成按钮后可见光图片在左上角，红外图片在右下角则说明导入成功
- 有关特征点：先点击可见光图像中的位置，再点击红外图像的相似位置。
> [!note]
>
> 按下按钮后未出现请单击界面左下角
