//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include <iostream>
//#include <stdio.h>
//
//using namespace cv;
//using namespace std;
//
///**  @function main */
//int main( int argc, char** argv )
//{
//  Mat src, dst;
//  Mat histogram, result_histogram;
//
//  char* source_window = "Gray image";
//  char* equalized_window = "Equalized Image";
//
//  /// 加载源图像
//  src = imread("moon.bmp", 0);
//
//  if( !src.data )
//    { cout<<"Usage: ./Histogram_Demo <path_to_image>"<<endl;
//      return -1;}
//  
//  //Histogram
//  int binSize = 255;
//  float range[] = {0, 255};
//  const float* histRange = { range };
//  bool uniform = true; 
//  bool accumulate = false;
//  calcHist( &src, 1, 0, Mat(), histogram, 1, &binSize, &histRange, uniform, accumulate );
//  // 创建直方图画布
// int hist_w = 500; int hist_h = 500;
// int bin_w = cvRound( (double) hist_w/binSize );
// Mat histImage( hist_w, hist_h, CV_8UC3, Scalar( 0,0,0) );
// /// 将直方图归一化到范围 [ 0, histImage.rows ]
// normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
// for( int i = 1; i < binSize; i++ )
//   {
//     line( histImage, Point( bin_w*(i-1), hist_h - cvRound(histogram.at<float>(i-1)) ) ,
//                      Point( bin_w*(i), hist_h - cvRound(histogram.at<float>(i)) ),
//                      Scalar( 0, 0, 255), 2, 8, 0  );
//	}
//
// /// 显示直方图
// namedWindow("Origin Histogram", CV_WINDOW_AUTOSIZE );
// imshow("Origin Histogram", histImage );
//
//
//
//
//  /// 应用直方图均衡化
//  equalizeHist( src, dst );
//
//  calcHist( &dst, 1, 0, Mat(), result_histogram, 1, &binSize, &histRange, uniform, accumulate );
//  Mat result_histImage( hist_w, hist_h, CV_8UC3, Scalar( 0,0,0) );
//  normalize(result_histogram, result_histogram, 0, result_histImage.rows, NORM_MINMAX, -1, Mat() );
//  for( int i = 1; i < binSize; i++ )
//   {
//     line( result_histImage, Point( bin_w*(i-1), hist_h - cvRound(result_histogram.at<float>(i-1)) ) ,
//                      Point( bin_w*(i), hist_h - cvRound(result_histogram.at<float>(i)) ),
//                      Scalar( 255, 0, 0), 2, 8, 0  );
//	}
//  /// 显示直方图
// namedWindow("Equalized Histogram", CV_WINDOW_AUTOSIZE );
// imshow("Equalized Histogram", result_histImage );
//
//  /// 显示结果
//  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
//  namedWindow( equalized_window, CV_WINDOW_AUTOSIZE );
//
//  imshow( source_window, src );
//  imshow( equalized_window, dst );
//
//  /// 等待用户按键退出程序
//  waitKey(0);
//
//  return 0;
//}



#include "cv.h"
#include "highgui.h"
#include <iostream>

using namespace std;

#define HDIM    256    // bin of HIST, default = 256

int main( int argc, char** argv ) 
{
    IplImage *src;
	IplImage *dst;
	IplImage *HistogramImage1;
	CvHistogram *hist;
	CvHistogram *Histogram1;
	CvPoint Point1;
    CvPoint Point2;
    
	src=cvLoadImage("moon.bmp",0);
    int n = 256;     
    double nn[256];
    uchar T[256];
    CvMat *T_mat;
    
    int x;
    int sum = 0; // sum of pixels of the source image 图像中象素点的总和
    double val = 0;
    
    cvNamedWindow( "source", CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
    
    // calculate histgram 计算直方图
    hist = cvCreateHist( 1, &n, CV_HIST_ARRAY, 0, 1 );  
    cvCalcHist( &src, hist, 0, 0 ); 
    
	int histogramBin = 255;
	int histogramBinWidth;
	float range[] = {0, 255};
	//const float* histRange = { range };
	float* histRange = { range };
	Histogram1 = cvCreateHist(1,&histogramBin,CV_HIST_ARRAY,0, 1);
	HistogramImage1 = cvCreateImage(cvSize(400,400),8,3);

	cvSetZero(HistogramImage1);
    HistogramImage1->origin=1;
    histogramBinWidth=256/histogramBin;

    cvCalcHist(&src,Histogram1);
    cvNormalizeHist(Histogram1,5000);
	for(int i=0;i<histogramBin;i++)
    {

        Point1=cvPoint(i*histogramBinWidth,0);
        Point2=cvPoint((i+1)*histogramBinWidth,(int)cvQueryHistValue_1D(Histogram1,i));

        cvRectangle(HistogramImage1,Point1,Point2,CV_RGB(255,0,0));
    }
	cvReleaseHist(&Histogram1);
	cvNamedWindow("Histogram1",1);
	cvShowImage("Histogram1",HistogramImage1);

    // Create Accumulative Distribute Function of histgram
    val = 0;
    for ( x = 0; x < n; x++)
    {
        val = val + cvGetReal1D (hist->bins, x);
		cout<<cvGetReal1D (hist->bins, x)<<"  ";
        nn[x] = val;
    }

    // Compute intensity transformation 计算变换函数的离散形式
    sum = src->height * src->width;
    for( x = 0; x < n; x++ )
    {
        T[x] = (uchar) (255 * nn[x] / sum); // range is [0,255]
    }

    // Do intensity transform for source image
    dst = cvCloneImage( src );
    T_mat = cvCreateMatHeader( 1, 256, CV_8UC1 );
    cvSetData( T_mat, T, 0 );    
    // directly use look-up-table function 直接调用内部函数完成 look-up-table 的过程
    cvLUT( src, dst, T_mat );

    cvShowImage( "source", src );
    cvShowImage( "result", dst );
    cvWaitKey();

    cvDestroyWindow("source");
    cvDestroyWindow("result");
    cvReleaseImage( &src );
    cvReleaseImage( &dst );
    cvReleaseHist ( &hist );
    
    return 0;
}

