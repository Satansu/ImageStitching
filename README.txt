OpenCV INSTALLATION INSTRUCTIONS -
(can refer to this if any issues occur: https://youtu.be/v_76zetw950?si=pEwlF1DYhzr7Cjff)

Download OpenCV 4.9.0 from the official website
Download opencv_contrib from https://github.com/opencv/opencv_contrib

Extract them to a common folder (opencv)

Download CMake (select the ADD TO PATH checkbox during installation)

Open Cmake GUI and select the OpenCV 4.9.0 folder as the source, and create a new "build" folder for the build. 

Press Configure. After it has been completed, go to OPENCV_EXTRA_MODULES_PATH and enter the path to opencv_contrib. 

Press Configure again, and then Generate. After that select Open Project. I have used VS 2022.

After opening the project, select the Cmake_Targets folder, right-click ALL_BUILD and select build. Make sure to do this for both Debug and Release.

Repeat the previous step for INSTALL as well.

BUILDING THE PROJECT -
Go to Properties -> Additional Include Directories. Add the path to your OpenCV's include folder.
(Example: E:\opencv\build\install\include)

Go to Linker -> Additional Library Directories. Add the path to your OpenCV's lib folder.
(Example: E:\opencv\build\install\x64\vc17\lib)

Go to Input -> Additional Dependencies. Add the following libraries -
opencv_core490.lib
opencv_highgui490.lib
opencv_imgcodecs490.lib
opencv_xfeatures2d490.lib
opencv_features2d490.lib
opencv_calib3d490.lib
opencv_imgproc490.lib
opencv_calib3d490d.lib

Change the address of the 4 images in the code to it appropriate location as the absolute path has been given.

Design and Algorithms -
There are 3 main steps after creating an empty panorama canvas to load the images.

1) Feature detection using SIFT (Scale-Invariant Feature Transform). This seems to be the most robust algorithm and hence has been chosen.

2) Feature matching using Brute Force algorithm - This is a straightforward algorithm and simple to execute. FLANN can be used as well but I had some difficulty in implementing it.

3) Finding homography using RANSAC (Random Sample Consensus), a tried and tested staple of Computer Vision that can handle outliers very well.

After this the output is transformed and displayed.


