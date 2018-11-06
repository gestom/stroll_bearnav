#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;

class CV_EXPORTS TraciaDescriptorExtractor : public DescriptorExtractor
{
public:
    static const int PATCH_SIZE = 48;
    static const int KERNEL_SIZE = 9;

    // bytes is a length of descriptor in bytes. It can be equal 16, 32 or 64 bytes.
    TraciaDescriptorExtractor( int bytes = 32 );

    virtual void read( const FileNode& );
    virtual void write( FileStorage& ) const;

    virtual int descriptorSize() const;
    virtual int descriptorType() const;

    /// @todo read and write for brief

//    AlgorithmInfo* info() const;

    virtual void computeImpl(const Mat& image, std::vector<KeyPoint>& keypoints, Mat& descriptors) const;
    typedef void(*PixelTestFn)(const Mat&, const std::vector<KeyPoint>&, Mat&);

    int bytes_;
    PixelTestFn test_fn_;
};

