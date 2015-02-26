/**
Retrieve properties for custom retrieval of V4L2 captured frames.
*/

#ifndef CV_VIDEOIO_RETRIEVE
#define CV_VIDEOIO_RETRIEVE

#include <opencv2/core.hpp>

enum RetrDownsample { DS_ORIGINAL, DS_HALF, DS_QUARTER };

// CS_BGR is not implemented, CS_YCRCB is used instead
enum RetrColorspace { CS_GRAY, CS_YCRCB, CS_BGR };

typedef struct RetrieveProps {
	/** Downsampling of the full frame
	*/
    RetrDownsample sampling;

	/** Region of interest to take in the frame, considered only for DS_ORIGINAL
	*/
    cv::Rect_<unsigned> region;

	/** Resulting colorspace.
	*/	
    RetrColorspace colorspace;

	int getDenominator() {
		return 1 << sampling;
	}

	int getChannels() {
		return colorspace == CS_GRAY ? 1 : 3;
	}
} RetrieveProps;

#endif
