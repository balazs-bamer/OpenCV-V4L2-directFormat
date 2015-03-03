# VideoCapture class with direct format, region and downsampling options

## Motivation

Environments with limited CPU power like Raspberry Pi computers need algorithms without unnecessary computations. I have noticed some facts signing OpenCV video capture solutions are suboptimal. The test results of my modifications are promising, I describe them later. The starting points are:

* They emit BGR encoded images. Although I'm novice in image processing, I think this alone is hardly a good starting point for many tasks. Grayscale or some other colorspace having luminosity and some sort of chroma info (YCrCb, HSV etc) seem to be far more versatile for edge and movement detection, etc.

* Many sources, like webcams can offer or produce only such formats. This is first converted into BGR, then the user can convert it into the desired format. However, instead of two costly conversions requiring colorspace matrix multiplication for each point each time, a copying or a much simpler conversion could be enough. Some sources, like libv4l do this conversion to BGR internally, while others retain the possibility to efficiently implement other output formats.

* Although Video for Linux supports of change of resolution and cropping, I could not make the change work in an open capture session. Even if I suspend the video stream using VIDIOC_STREAMOFF and VIDIOC_STREAMON around the format change call to *icvSetVideoSize* the whole session is corrupted. The situation is the same if I try to set the same resoluiton as before. There is other [mention](http://comments.gmane.org/gmane.linux.drivers.video-input-infrastructure/30910) of similar problem. I don't know if it is a driver problem or if there is a workaround for it. For my aims more investigation just does not worth. Moreover, this tradiotional way does not allow accessing the same frame in full resolution, which feature will be important for me.

* For many problems it could be enough to start an image processing algorithm on a image with far less resolution than the original one. Either the results are accurate enough, or if it finds something, subsequent processing with full resolution or using an other algorithm could make the hard work. To be short, it would be important, that during the above copy or convert step in VideoCapture the resolution could be decreased as well.

* An other case is when only a portion of the image contains interesting details. By copying only this region of interest in VideoCapture the CPU demand can be further decreased.

## Implementation

I have extracted the files interesting for my problem to avoid collision with existing OpenCV class structures and be able to use them in my developments. I have implemented it only for my webcam format, so this is only a proof of concept.

OpenCV out of the box favours libv4l over bare V4L2. In my CMakeLists.txt I have forced the capture system to use V4L2.

I have decided to modify only the *retreive* routines, as cap_v4l.cpp does its conversions there. The other cap_ files are intact, and I didn't even included them here. This was enough in my case. My webcam does not support BGR mode, so the fallback case YUYV in *autosetup_capture_mode_v4l2* was fine for me to start from, but in full implementation a smarter pre-selection of color formats would be desirable in this function.

Moreover, I focused on V4L2. I didn't care what this modified library would do with only V4L.

The modified VideoCapture class is called VideoCapture_mod now. I have created a *RetrieveProps* structure in opencv2/retrieve.hpp which can be passed in VideoCapture_mod.set to prescribe the output of further retrieve calls. Important is, that any image can be retrieved with modified properties later. This means a fast algorythm can watch for interesting frames in reduced resolution or color depth, than if an event comes, it can handle the same image in full details to an other task. 

This description structure contains the following fields:

* *sampling* describes the downsampling during retrieve. Now it can leave the image in original size, half or quarter the resolution. It operates by computing an average of 2x2 or 4x4 adjacent cells.

* *region* is an unsigned OpenCV Rect describing the region to extract. It is considered only without downsampling (it takes original size region).

* *colorspace* colorspace or format to be used in the original image. It can be grayscale or YCrCb. The class understands BGR, too, but it is not implemented yet. The routine for YCrCb is used instead.

There are two source files providing V4L functionality, cap_v4l-diag.cpp and cap_v4l-nodiag.cpp. The latter one is the production version, the former one writes ioctl call info in /tmp/diag.log at the end of the session. Logfile contents are accumulated in an *stringstream* instance. An example log file is attached. This version modifies the first byte in each frame to an incrementing counter to be able to track them.

I used the compiler flag -std=c++11

## Test results

### Test programs

I have modified an OpenCV capture demo program to test and show the capabilities. There are two programs, both use Curses and OpenCV 3.0 and both with a pure Curses output or a window showing the captured frame. Both use a resolution of 640x480. Its maximal frame rate is 30 FPS, and was set on this rate.

* *test-original* (source is main-cv.cpp) demonstrates the original OpenCV solution. By default it leaves the BGR format (key 0), or convert it to grayscale (key 1), or YCrCb (key 2). The time measured for retrieve contains these conversions, so the results are compareble to the other one.
* *test-modified* (source is main-mod.cpp) uses the modified V4L2 interface and is capable of retrieving grayscale (key 0-4) or YCrCb images (key 5-9).

Possible images:
* *0 or 5*: 640x480 full frame
* *1 or 6*: the full frame resized to 320x240
* *2 or 7*: the full frame resized to 160x120
* *3 or 8*: the bottom part of the full frame with size 640x80
* *4 or 9*: the right part of the full frame with size 40x480

### Measured performance

I have made all measurements on a 700 Mhz Raspberry Pi B+ with factory settings (no overclock or overvoltage). I have compiled both OpenCV 3.0.0 and my routines with *g++ (Raspbian 4.8.2-21~rpi3rpi1) 4.8.2* using *-O1*. I use the Logitech C250 webcam via USB.

Grab, retrieve and total times are in s. CPU% per frame is an approximate number signing the required computations per frame. Sometimes grabbing was somehow disturbed, resulting 0.01 - 0.02 s longer grab times. CPU usage was not influenced, but FPS was significantly lower. I do not know the cause.

#### Results of the original solution

Key|grab|retrieve|total|FPS|CPU usage %|CPU% per frame
--:|---:|-------:|----:|--:|----------:|-------------:
0|0.046|0.007|0.053|17|93|5.5
1|0.044|0.033|0.077|12|94|7.8
2|0.040|0.085|0.125|8|96|12.0

During this run the libv4l interface was used. Note the almost totally busy CPU, which is a result of the heavy calculations during retrieve. The grab times are longer, too. This should be the result of the libv4l transforming YUV to RGB.

#### Results of the modified algorithm:

Key|grab|retrieve|total|FPS|CPU usage %|CPU% per frame
--:|---:|-------:|----:|--:|----------:|-------------:
0|0.025|0.011|0.036|24|35|1.46
1|0.022|0.014|0.036|23|40|1.74
2|0.025|0.010|0.035|25|35|1.40
3|0.032|0.002|0.034|26|16|0.62
4|0.033|0.001|0.034|27|15|0.56
5|0.012|0.021|0.033|24|61|2.54
6|0.016|0.021|0.037|25|61|2.44
7|0.022|0.012|0.034|25|41|1.64
8|0.033|0.004|0.037|25|21|0.84
9|0.033|0.002|0.035|25|17|0.68

The achieved FPS is substantially higher with much lower CPU usage. During grab there is a long blocking wait on the system call, but it does not consume CPU, so a parallel task could perform further calculations. This solution will fit my aims of watching the camera video stream for something interesting using a quick and simple algorithm. If something happens, a parallel task could perform something complicated if it is finished with the previous frame or use an any-time algorithm if applicable.
