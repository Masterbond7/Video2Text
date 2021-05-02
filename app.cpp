// Including files
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <codecvt>

// Including "config.h" header file
#include "config.h"

// Declaring used namespaces
using namespace std;
using namespace cv;

// Initializing variables
bool frameFound;
Mat frameData;
int currentFrame = 0;
string outputString;
int pixelBrightness;

// Useful functions for conversion https://codereview.stackexchange.com/a/146738 / https://codereview.stackexchange.com/questions/419/converting-between-stdwstring-and-stdstring - Jamerson
std::wstring utf8ToUtf16(const std::string& utf8Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(utf8Str);
}
std::string utf16ToUtf8(const std::wstring& utf16Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(utf16Str);
}

// Main function
int main() {
	// Loading the video into the video variable
	VideoCapture video = VideoCapture(VIDEO_FILE_NAME);

	// Getting the loaded video's framerate and adding 0.5f because of how c++ converts float to int (truncation)
	uint64 videoFPS = uint64(video.get(5) + 0.5f);
	cout << "The video's FPS is " << videoFPS << " frames per second." << endl;

	// Checking if the videos FPS is lower than the target FPS and setting the target FPS lower to avoid possible division by zero errors later
	if (videoFPS < TARGET_OUTPUT_FPS) { TARGET_OUTPUT_FPS = videoFPS; }

	// Getting the total number of frames in the video
	double videoTotalFrames = video.get(7);
	cout << "The total amount of frames in the video is " << videoTotalFrames << " frames." << endl;

	// Calculating the nTH frame to grab from the video
	int nTHframe = int(((float)videoFPS / (float)TARGET_OUTPUT_FPS) + 0.5f);
	cout << "Every frame that is a multiple of " << nTHframe << " will be used." << endl;
	cout << "This will result in an actual output framerate of " << videoFPS / nTHframe << " FPS." << endl;
	cout << "And will also result in " << uint64(((float)videoTotalFrames / (float)nTHframe) + 0.5) << " text frames." << endl << endl;

	// Creating folders if they do not exist
	CreateDirectoryW(utf8ToUtf16(OUTPUT_FOLDER_NAME).c_str(), NULL);

	// Looping through all the frames in the video
	for (uint64 frameIndex = 6; frameIndex < videoTotalFrames; frameIndex++) {
		// Reading the next frame from the video and storing it in the frameData variable and exiting with code 1 if it can not be read
		frameFound = video.read(frameData);
		if (!frameFound) { exit(1); }

		// Code to be processed on every nTH frame
		if ((frameIndex % nTHframe) == 0) {
			// Resetting variables
			outputString = "";

			// Resizing the images
			resize(frameData, frameData, Size(OUTPUT_WIDTH, OUTPUT_HEIGHT));

			// Grayscaling the image
			cvtColor(frameData, frameData, COLOR_BGR2GRAY);

			// Rotating the image 90 degrees clockwise to solve problem with how the Mat array is read
			//rotate(frameData, frameData, cv::ROTATE_90_CLOCKWISE);

			// Cycling through every pixel and matching with a text charachter
			/*for (int i = 0; i < frameData.rows; i++) {
				for (int j = 0; j < frameData.cols; j++) {
					cout << int(frameData.at<uchar>(i, j));
				}
				cout << endl;
			}*/
			for (int x = 0; x < OUTPUT_HEIGHT; x++) {
				for (int y = 0; y < OUTPUT_WIDTH; y++) {
					pixelBrightness = frameData.at<uchar>(x, y);
					outputString += GRAY_SCALE[int(((((pixelBrightness * (-1)) + 255) + 1) / 256.0f) * (GRAY_SCALE.length())) - 1];
				}
				outputString += "\n";
			}
			
			// Writing the outputString variable to a text file
			ofstream outFile(OUTPUT_FOLDER_NAME + "/" + to_string(currentFrame) + ".txt");
			outFile << outputString;
			outFile.close();

			//// Saving frame as an image
			imwrite("Images/" + to_string(currentFrame) + ".png", frameData);
			
			// Incrementing the currentFrame variable
			currentFrame++;
		}

		// Return to the beginning of this line to overwrite the old progress report as long as this is not the last progress report
		if (frameIndex < videoTotalFrames - 1) { cout << "Frame " << frameIndex+1 << "/" << videoTotalFrames << " processed." << "\r"; }
		else { cout << "Frame " << frameIndex + 1 << "/" << videoTotalFrames << " processed." << endl << endl; }
	}

	// Print the ending message
	cout << "Finished converting the video file into text." << endl;

	// Exiting successfully
	exit(0);
}