/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009-2010, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "tracia.h"
//#include "precomp.hpp"
#include <algorithm>
#include <vector>

#include <iostream>
#include <iomanip>

using namespace cv;
int compSeq[32*8*4];

inline int boxFilter(const Mat& sum, const KeyPoint& pt, int y0, int x0,int y1,int x1)
{
    int img_y0 = (int)(pt.pt.y + 0.5) + y0;
    int img_x0 = (int)(pt.pt.x + 0.5) + x0;
    int img_y1 = (int)(pt.pt.y + 0.5) + y1;
    int img_x1 = (int)(pt.pt.x + 0.5) + x1;
    return   sum.at<int>(img_y1, img_x1)
           - sum.at<int>(img_y1, img_x0)
           - sum.at<int>(img_y0, img_x1)
           + sum.at<int>(img_y0, img_x0);
}

inline int smoothedSum(const Mat& sum, const KeyPoint& pt, int y, int x)
{
    static const int HALF_KERNEL = TraciaDescriptorExtractor::KERNEL_SIZE / 2;

    int img_y = (int)(pt.pt.y + 0.5) + y;
    int img_x = (int)(pt.pt.x + 0.5) + x;
    return   sum.at<int>(img_y + HALF_KERNEL + 1, img_x + HALF_KERNEL + 1)
           - sum.at<int>(img_y + HALF_KERNEL + 1, img_x - HALF_KERNEL)
           - sum.at<int>(img_y - HALF_KERNEL, img_x + HALF_KERNEL + 1)
           + sum.at<int>(img_y - HALF_KERNEL, img_x - HALF_KERNEL);
}

static void pixelTests16(const Mat& sum, const std::vector<KeyPoint>& keypoints, Mat& descriptors)
{
    for (int i = 0; i < (int)keypoints.size(); ++i)
    {
        uchar* desc = descriptors.ptr(i);
        const KeyPoint& pt = keypoints[i];
#include "generated_16.i"
    }
}

/*static void pixelTests32(const Mat& sum, const std::vector<KeyPoint>& keypoints, Mat& descriptors)
{
	for (int i = 0; i < (int)keypoints.size(); ++i)
	{
		uchar* desc = descriptors.ptr(i);
		const KeyPoint& pt = keypoints[i];
#include "generated_32.i"
		//float x = 10/0;
	}
}*/

static void pixelTests32(const Mat& sum, const std::vector<KeyPoint>& keypoints, Mat& descriptors)
{
	static const int HALF_KERNEL = TraciaDescriptorExtractor::KERNEL_SIZE / 2;
	for (int i = 0; i < (int)keypoints.size(); ++i)
	{
		uchar* desc = descriptors.ptr(i);
		const KeyPoint& pt = keypoints[i];
		int *compSeqPtr;
		for (int j = 0; j < 32; j++)
		{
			unsigned char a = 0;
			for (int k = 0; k < 8; k++){
				a = a*2;
				compSeqPtr = &compSeq[(j*8+k)*4];
			//	printf("%i %i %i %i %i %i\n",compSeqPtr[1], compSeqPtr[0],compSeqPtr[3], compSeqPtr[2],smoothedSum(sum, pt, compSeqPtr[1], compSeqPtr[0]),(smoothedSum(sum, pt, compSeqPtr[1], compSeqPtr[0]) < smoothedSum(sum, pt, compSeqPtr[3], compSeqPtr[2])));
				//a = a+(boxFilter(sum, pt, compSeqPtr[1]-HALF_KERNEL, compSeqPtr[0]-HALF_KERNEL, compSeqPtr[1]+HALF_KERNEL+1, compSeqPtr[0]+HALF_KERNEL+1) < boxFilter(sum, pt, compSeqPtr[3]-HALF_KERNEL, compSeqPtr[2]-HALF_KERNEL, compSeqPtr[3]+HALF_KERNEL+1, compSeqPtr[2]+HALF_KERNEL+1));
				a = a+(boxFilter(sum, pt, compSeqPtr[1], compSeqPtr[0], compSeqPtr[1], compSeqPtr[0]) < boxFilter(sum, pt, compSeqPtr[3], compSeqPtr[2], compSeqPtr[3], compSeqPtr[2]));
			}
			//printf("%i\n",a);
			//float x = 10/0;
			desc[j] = a;
		}
	}
}
static void pixelTests32a(const Mat& sum, const std::vector<KeyPoint>& keypoints, Mat& descriptors)
{
	for (int i = 0; i < (int)keypoints.size(); ++i)
	{
		uchar* desc = descriptors.ptr(i);
		const KeyPoint& pt = keypoints[i];
		int *compSeqPtr;
		for (int j = 0; j < 32; j++)
		{
			unsigned char a = 0;
			for (int k = 0; k < 8; k++){
				a = a*2;
				compSeqPtr = &compSeq[(j*8+k)*4];
			//	printf("%i %i %i %i %i %i\n",compSeqPtr[1], compSeqPtr[0],compSeqPtr[3], compSeqPtr[2],smoothedSum(sum, pt, compSeqPtr[1], compSeqPtr[0]),(smoothedSum(sum, pt, compSeqPtr[1], compSeqPtr[0]) < smoothedSum(sum, pt, compSeqPtr[3], compSeqPtr[2])));
				a = a+(smoothedSum(sum, pt, compSeqPtr[1], compSeqPtr[0]) < smoothedSum(sum, pt, compSeqPtr[3], compSeqPtr[2]));
			}
			//printf("%i\n",a);
			//float x = 10/0;
			desc[j] = a;
		}
	}
}

static void pixelTests64(const Mat& sum, const std::vector<KeyPoint>& keypoints, Mat& descriptors)
{
    for (int i = 0; i < (int)keypoints.size(); ++i)
    {
        uchar* desc = descriptors.ptr(i);
        const KeyPoint& pt = keypoints[i];

#include "generated_64.i"
    }
}

TraciaDescriptorExtractor::TraciaDescriptorExtractor(int bytes) :
    bytes_(bytes), test_fn_(NULL)
{
	loadFromFile("/home/gestom/test_pairs.grief");
	switch (bytes)
	{
		case 16:
			test_fn_ = pixelTests16;
			break;
		case 32:
			test_fn_ = pixelTests32;
			break;
		case 64:
			test_fn_ = pixelTests64;
			break;
		default:
			CV_Error(CV_StsBadArg, "bytes must be 16, 32, or 64");
	}
}

int TraciaDescriptorExtractor::descriptorSize() const
{
    return bytes_;
}

int TraciaDescriptorExtractor::descriptorType() const
{
    return CV_8UC1;
}

void TraciaDescriptorExtractor::loadFromFile(const char* name)
{	
	FILE *file = fopen(name,"r");
	int *compSeqPtr;
	for (int i = 0;i<32*8;i++)
	{
		compSeqPtr = &(compSeq[i*4]);
		fscanf(file,"%i %i %i %i\n",&compSeqPtr[0],&compSeqPtr[1],&compSeqPtr[2],&compSeqPtr[3]);
	}	
	fclose(file);
}

void TraciaDescriptorExtractor::read( const FileNode& fn)
{
    int dSize = fn["descriptorSize"];
    switch (dSize)
    {
        case 16:
            test_fn_ = pixelTests16;
            break;
        case 32:
            test_fn_ = pixelTests32;
            break;
        case 64:
            test_fn_ = pixelTests64;
            break;
        default:
            CV_Error(CV_StsBadArg, "descriptorSize must be 16, 32, or 64");
    }
    bytes_ = dSize;
}

void TraciaDescriptorExtractor::write( FileStorage& fs) const
{
    fs << "descriptorSize" << bytes_;
}

void TraciaDescriptorExtractor::computeImpl(const Mat& image, std::vector<KeyPoint>& keypoints, Mat& descriptors) const
{
    // Construct integral image for fast smoothing (box filter)
    Mat sum;

    Mat grayImage = image;
    if( image.type() != CV_8U ) cvtColor( image, grayImage, CV_BGR2GRAY );

    ///TODO allow the user to pass in a precomputed integral image
    //if(image.type() == CV_32S)
    //  sum = image;
    //else

    integral( grayImage, sum, CV_32S);

    //Remove keypoints very close to the border
    KeyPointsFilter::runByImageBorder(keypoints, image.size(), PATCH_SIZE/2 + KERNEL_SIZE/2 + 1 );

    descriptors = Mat::zeros((int)keypoints.size(), bytes_, CV_8U);
    test_fn_(sum, keypoints, descriptors);
}

