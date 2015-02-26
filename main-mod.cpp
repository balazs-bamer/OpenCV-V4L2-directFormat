/*
* starter_video.cpp
*
*  Created on: Nov 23, 2010
*      Author: Ethan Rublee
*
*  Modified on: April 17, 2013
*      Author: Kevin Hughes
*
* A starter sample for using OpenCV VideoCapture with capture devices, video files or image sequences
* easy as CV_PI right?
*/

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/videoio_mod.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

#include <curses.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace cv;
using namespace std;

//hide the local functions in an anon namespace
namespace {
    void help(char** av) {
        cout << "The program captures frames from a video file, image sequence (01.jpg, 02.jpg ... 10.jpg) or camera connected to your computer." << endl
             << "Usage:\n" << av[0] << " <video file, image sequence or device number> <any second argument to work without window>" << endl
             << "q,Q,esc -- quit" << endl
             << "b,B     -- change camera brightness" << endl
             << "c,C     -- change camera contrast" << endl
             << "s,S     -- change camera saturation" << endl
             << "0..4    -- select black and white image, downsampling and/or crop" << endl
             << "5..9    -- select YUV color image, downsampling and/or crop" << endl
             << "space   -- save frame" << endl << endl
             << "\tTo capture from a camera pass the device number. To find the device number, try ls /dev/video*" << endl
             << "\texample: " << av[0] << " 0" << endl
             << "\tYou may also pass a video file instead of a device number" << endl
             << "\texample: " << av[0] << " video.avi" << endl
             << "\tYou can also pass the path to an image sequence and OpenCV will treat the sequence just like a video." << endl
             << "\texample: " << av[0] << " right%%02d.jpg" << endl;
    }

RetrieveProps props;

    int process(cv::VideoCapture_mod& capture, bool noWindow) {
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
        cout << "press space to save a picture. q or esc to quit" << endl;
		if(!noWindow) {
	        namedWindow(window_name, CV_WINDOW_AUTOSIZE); //resizable window;
		}
		stringstream out;
        Mat frame;
		struct timespec timeStart, timeMid, timeEnd, timeFirst;
		int nSampl = 0;
		clock_gettime( CLOCK_REALTIME, &timeFirst);
		double avgGrab = 0, avgRetr = 0, avgTotal = 0;
        for (;;) {
			clock_gettime( CLOCK_REALTIME, &timeStart);
			if(nSampl == 0) {
				timeFirst = timeStart;
			}
			bool success = capture.grab();
			clock_gettime( CLOCK_REALTIME, &timeMid);
            if(success) {
				success = capture.retrieve(frame, 0);
	            if (frame.empty())
		            break;
			}
			clock_gettime(CLOCK_REALTIME, &timeEnd);
			double diff1 = timeMid.tv_sec + timeMid.tv_nsec / 1000000000.0 - 
				timeStart.tv_sec - timeStart.tv_nsec / 1000000000.0;
			double diff2 = timeEnd.tv_sec + timeEnd.tv_nsec / 1000000000.0 - 
				timeMid.tv_sec - timeMid.tv_nsec / 1000000000.0;
			double fps = 0;
			if(success) {
				if(nSampl >= 0) {
					avgGrab = (avgGrab * nSampl + diff1) / (nSampl + 1);
					avgRetr = (avgRetr * nSampl + diff2) / (nSampl + 1);
					avgTotal = (avgTotal * nSampl + diff1 + diff2) / (nSampl + 1);
					double diff = timeEnd.tv_sec + timeEnd.tv_nsec / 1000000000.0 - 
						timeFirst.tv_sec - timeFirst.tv_nsec / 1000000000.0;
					fps = (nSampl + 1) / diff;
				}
				nSampl++;
			}

            out << "\nGrab : " << avgGrab;
            out << "\nRetr : " << avgRetr;
            out << "\nTotal: " << avgTotal;
            out << "\nFPS  : " << fps;
            out << "\nMat size: " << frame.size().width << " " << frame.size().height;
            out << "\nMat channels: " << frame.channels();
            out << "\nCV_CAP_PROP_BRIGHTNESS: ";
            out << capture.get(CV_CAP_PROP_BRIGHTNESS);
            out << "\nCV_CAP_PROP_CONTRAST: ";
            out << capture.get(CV_CAP_PROP_CONTRAST);
            out << "\nCV_CAP_PROP_SATURATION: ";
            out << capture.get(CV_CAP_PROP_SATURATION);
			out << '\n';

			if(frame.channels() == 3 && !noWindow) {
				cvtColor(frame, frame, CV_YCrCb2BGR);
			}

			int key = '!';
			if(!noWindow) {
				cout << out.str() << flush;
	            imshow(window_name, frame);
	            key = (char)waitKey(200); //delay N millis, usually long enough to display and capture input
			}
			else {
				clear();
				printw(out.str().c_str());
				struct timespec del;
				del.tv_sec = 0;
				del.tv_nsec = 1000000;
				nanosleep(&del, NULL);
				key = getch();	
			}
			out.str("");

            switch (key) {
            case 'q':
            case 'Q':
            case 27: //escape key
                return 0;
            case ' ': //Save an image
                sprintf(filename,"filename%.3d.jpg",n++);
                imwrite(filename,frame);
                cout << "Saved " << filename << endl;
                break;
			case 'b':
				capture.set(CV_CAP_PROP_BRIGHTNESS, 0.3);
				break;
			case 'B':
				capture.set(CV_CAP_PROP_BRIGHTNESS, 0.7);
				break;
			case 'c':
				capture.set(CV_CAP_PROP_CONTRAST, 0.07);
				break;
			case 'C':
				capture.set(CV_CAP_PROP_CONTRAST, 0.2);
				break;
			case 's':
				capture.set(CV_CAP_PROP_SATURATION, 0.07);
				break;
			case 'S':
				capture.set(CV_CAP_PROP_SATURATION, 0.2);
				break;
			case '0':
				props.sampling = DS_ORIGINAL;
				props.region.x = 0;
				props.region.y = 0;
				props.region.width = 640;
				props.region.height = 480;
				props.colorspace = CS_GRAY;
				break;
			case '1':
				props.sampling = DS_HALF;
				props.colorspace = CS_GRAY;
				break;
			case '2':
				props.sampling = DS_QUARTER;
				props.colorspace = CS_GRAY;
				break;
			case '3':
				props.sampling = DS_ORIGINAL;
				props.region.x = 0;
				props.region.y = 400;
				props.region.width = 640;
				props.region.height = 80;
				props.colorspace = CS_GRAY;
				break;
			case '4':
				props.sampling = DS_ORIGINAL;
				props.region.x = 599;
				props.region.y = 0;
				props.region.width = 40;
				props.region.height = 480;
				props.colorspace = CS_GRAY;
				break;
			case '5':
				props.sampling = DS_ORIGINAL;
				props.region.x = 0;
				props.region.y = 0;
				props.region.width = 640;
				props.region.height = 480;
				props.colorspace = CS_YCRCB;
				break;
			case '6':
				props.sampling = DS_HALF;
				props.colorspace = CS_YCRCB;
				break;
			case '7':
				props.sampling = DS_QUARTER;
				props.colorspace = CS_YCRCB;
				break;
			case '8':
				props.sampling = DS_ORIGINAL;
				props.region.x = 0;
				props.region.y = 400;
				props.region.width = 640;
				props.region.height = 80;
				props.colorspace = CS_YCRCB;
				break;
			case '9':
				props.sampling = DS_ORIGINAL;
				props.region.x = 599;
				props.region.y = 0;
				props.region.width = 40;
				props.region.height = 480;
				props.colorspace = CS_YCRCB;
				break;
            default:
                break;
            }
			if(key >= '0' && key <= '9') {
				nSampl = -20; // fill up capture cache
				avgGrab = avgRetr = avgTotal = 0;
				capture.set(props);
			}
        }
        return 0;
    }
}
	
int main(int ac, char** av) {

    if (ac < 2) {
        help(av);
        return 1;
    }
    std::string arg = av[1];
    cv::VideoCapture_mod capture(arg); //try to open string, this will attempt to open it as a video file or image sequence
    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
        capture.open(atoi(arg.c_str()));
    if (!capture.isOpened()) {
        cerr << "Failed to open the video device, video file or image sequence!\n" << endl;
        help(av);
        return 1;
    }
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	props.sampling = DS_ORIGINAL;
	props.colorspace = CS_GRAY;
	props.region.x = -1;
	capture.set(props);
	bool noWindow = ac > 2;
	if(noWindow) {
		initscr();			/* Start curses mode		*/
		cbreak();				/* Line buffering disabled	*/
		keypad(stdscr, true);		/* We get F1, F2 etc..		*/
		noecho();			/* Don't echo() while we do getch */
		nodelay(stdscr, true);
		scrollok(stdscr, false);
	}
	int ret = process(capture, noWindow);
	if(noWindow) {
		endwin();			/* End curses mode		  */
	}
	return ret;
}
