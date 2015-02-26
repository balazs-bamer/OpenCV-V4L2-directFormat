/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
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

#ifndef __OPENCV_VIDEOIO_MOD_HPP__
#define __OPENCV_VIDEOIO_MOD_HPP__

#include <opencv2/core.hpp>
#include "opencv2/retrieve.hpp"

////////////////////////////////// video io /////////////////////////////////

typedef struct CvCapture CvCapture;
typedef struct CvVideoWriter CvVideoWriter;

namespace cv
{

class IVideoCapture;
class CV_EXPORTS_W VideoCapture_mod
{
public:
    CV_WRAP VideoCapture_mod();
    CV_WRAP VideoCapture_mod(const String& filename);
    CV_WRAP VideoCapture_mod(int device);

    virtual ~VideoCapture_mod();
    CV_WRAP virtual bool open(const String& filename);
    CV_WRAP virtual bool open(int device);
    CV_WRAP virtual bool isOpened() const;
    CV_WRAP virtual void release();

    CV_WRAP virtual bool grab();
    CV_WRAP virtual bool retrieve(OutputArray image, int flag = 0);
    virtual VideoCapture_mod& operator >> (CV_OUT Mat& image);
    virtual VideoCapture_mod& operator >> (CV_OUT UMat& image);
    CV_WRAP virtual bool read(OutputArray image);

    CV_WRAP virtual bool set(int propId, double value);
    CV_WRAP virtual double get(int propId);
	void set(RetrieveProps &props);

protected:
    Ptr<CvCapture> cap;
    Ptr<IVideoCapture> icap;
	RetrieveProps retrieveProps;
private:
    static Ptr<IVideoCapture> createCameraCapture(int index);
};

template<> CV_EXPORTS void DefaultDeleter<CvCapture>::operator ()(CvCapture* obj) const;

} // cv

#endif //__OPENCV_VIDEOIO_HPP__
