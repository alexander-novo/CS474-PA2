// Q4-Unsharp/main.cpp
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "../Common/image.h"
#include "../Common/mask.h"

// Struct for inputting arguments from command line
struct Arguments {
	char *inputImagePath, *outImagePath;
	double A;  // High Boost option
	Image inputImage;
	std::ofstream outFile;
};

static const Mask<unsigned, 7> gauss7 = {{{1, 1, 2, 2, 2, 1, 1},
                                          {1, 2, 2, 4, 2, 2, 1},
                                          {2, 2, 4, 8, 4, 2, 2},
                                          {2, 4, 8, 16, 8, 4, 2},
                                          {2, 2, 4, 8, 4, 2, 2},
                                          {1, 2, 2, 4, 2, 2, 1},
                                          {1, 1, 2, 2, 2, 1, 1}}};

int unsharp(Arguments& arg);
bool verifyArguments(int argc, char** argv, Arguments& arg, int& err);
void printHelp();

int main(int argc, char** argv) {
	int err;
	Arguments arg;

	if (!verifyArguments(argc, argv, arg, err)) { return err; }

	return unsharp(arg);
}

int unsharp(Arguments& arg) {
	Image out = gauss7 * arg.inputImage;

	// Apply High Boost filter from original and low pass.
	// Then re-map values to [0, 255]
#pragma omp parallel for collapse(2)
	for (unsigned y = 0; y < out.rows; y++) {
		for (unsigned x = 0; x < out.cols; x++) {
			out[y][x] =
			    (arg.A * arg.inputImage[y][x] - out[y][x] + out.maxVal) / (1 + arg.A);
		}
	}

	arg.outFile << out;
	arg.outFile.close();

	return 0;
}

bool verifyArguments(int argc, char** argv, Arguments& arg, int& err) {
	if (argc < 2 ||
	    (argc < 4 && strcmp(argv[1], "-h") && strcmp(argv[1], "--help"))) {
		std::cout << "Missing operand.\n";
		err = 1;
		printHelp();
		return false;
	}

	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		printHelp();
		return false;
	}

	// Required arguments
	arg.inputImagePath = argv[1];
	std::ifstream inFile(argv[1]);
	try {
		arg.inputImage = Image::read(inFile);
	} catch (std::exception& e) {
		std::cout << "Image \"" << argv[1] << "\"failed to be read: \"" << e.what()
		          << "\"\n";
		err = 2;
		return false;
	}

	char* end;
	arg.A = strtod(argv[2], &end);
	if (end == argv[2]) {
		std::cout << "\"" << argv[2] << "\" could not be interpreted as a number\n";
		err = 2;
		return false;
	} else if (arg.A < 1) {
		std::cout << "A = \"" << arg.A << "\" must be greater than or equal to 1.\n";
		printHelp();
		err = 2;
		return false;
	}

	arg.outImagePath = argv[3];
	arg.outFile.open(argv[3]);
	if (!arg.outFile) {
		std::cout << "Could not open \"" << argv[3] << "\"\n";
		err = 2;
		return false;
	}

	return true;
}

void printHelp() {
	std::cout << "Usage: unsharp <image> <A> <output>   (1)\n"
	          << "   or: unsharp -h                                       (2)\n\n"
	          << "(1) Take an image file as input, apply an unsharp filter,\n"
	          << "    then add back (A - 1) copies of the original image\n"
	          << "    to the output (High Boost). Saves output image to ouput file.\n"
	          << "    Let A = 1 for simple unsharp filter.\n"
	          << "    A must be greater than or equal to 1.\n"
	          << "(2) Print this help menu\n";
}