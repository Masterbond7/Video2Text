#include <Windows.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

using std::string;
using std::filesystem::current_path;
using std::cout;
using std::to_string;
using std::ifstream;

namespace fs = std::filesystem;

// Setting Settings
string framesFolder = "AsciiFrames";
string asciiName = "";
string asciiExtention = "txt";
float framesPerSecond = 15;

void setCursorPosition(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

void displayFrame(string framePath) {
	string frameData;
	ifstream openfile(framePath);
	if (openfile.is_open())
	{
		while (!openfile.eof())
		{
			getline(openfile, frameData);
			cout << frameData << std::endl;
		}
	}
}

int main() {
	// Initializing Variables
	int frameNum = 0;
	string asciiFile;
	DWORD aimedTime;
	DWORD timeDifference;

	// Getting number of frames
	int frames = 0;
	fs::path path(current_path().string() + "/" + framesFolder);
	for (auto& p : fs::directory_iterator(path)) {
		frames++;
	}

	// TODO add ability to play wav audio files

	// Main loop to display the ASCII frames
	while (frameNum < frames) {
		// Getting time data for perfect frame delays
		aimedTime = GetTickCount64()+(int)(1000/framesPerSecond);

		// Making the path to the ASCII frame
		asciiFile = framesFolder + "/" + asciiName + to_string(frameNum) + "." + asciiExtention;

		// Set the cursor position to the top-left of the console and display the frame
		setCursorPosition(0, 0);
		displayFrame(asciiFile);

		// Adding a delay between drawing frames
		timeDifference = aimedTime - GetTickCount64();
		if (timeDifference > 0) {
			Sleep(1000 / framesPerSecond); // TODO add an improved method where it accounts for the time it took for the frame to draw
		}
		else {
			cout << "System Slow";
		}

		// Adding 1 to the current frame count
		frameNum++;
	}

	// Returning 0
	return 0;
}