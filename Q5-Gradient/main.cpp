// Q5-Gradient/main.cpp
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../Common/image.h"
#include "../Common/mask.h"

// Struct for inputting arguments from command line
struct Arguments {
	char *inputImagePath, *outImagePath;
	enum MaskType { PREWITT, SOBEL, LAPLACIAN } maskType;
	enum MaskDir { X, Y, MAGNITUDE, UNSPECIFIED } maskDir = UNSPECIFIED;
	Image inputImage;
	std::ofstream outFile;
};

static const Mask<int, 3> prewitt[2] = {{{{1, 0, -1}, {1, 0, -1}, {1, 0, -1}}},
                                        {{{1, 1, 1}, {0, 0, 0}, {-1, -1, -1}}}};
static const Mask<int, 3> sobel[2]   = {{{{1, 0, -1}, {2, 0, -2}, {1, 0, -1}}},
                                      {{{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}}}};
static const Mask<int, 3> laplacian  = {{{0, 1, 0}, {1, -4, 1}, {0, 1, 0}}};

int gradient(Arguments& arg);
bool verifyArguments(int argc, char** argv, Arguments& arg, int& err);
void printHelp();

int main(int argc, char** argv) {
	int err;
	Arguments arg;

	if (!verifyArguments(argc, argv, arg, err)) { return err; }

	return gradient(arg);
}

int gradient(Arguments& arg) {
	Image out;
	switch (arg.maskType) {
		case Arguments::PREWITT:
			switch (arg.maskDir) {
				case Arguments::X:
					out = prewitt[0] * arg.inputImage;
					break;
				case Arguments::Y:
					out = prewitt[1] * arg.inputImage;
					break;
				case Arguments::MAGNITUDE:
					Image x = prewitt[0] * arg.inputImage;
					out     = prewitt[1] * arg.inputImage;
// Compute magnitude
#pragma omp parallel for
					for (unsigned i = 0; i < out.cols * out.rows; i++) {
						// Subtract out.maxVal / 2.0, since we have negative gradient
						// values, but we mapped to [0, 255]. Divide by 2.0 to map to
						// [0, 255] again
						out.pixels[i] = sqrt(((out.pixels[i] - out.maxVal / 2.0) *
						                          (out.pixels[i] - out.maxVal / 2.0) +
						                      (x.pixels[i] - out.maxVal / 2.0) *
						                          (x.pixels[i] - out.maxVal / 2.0)) /
						                     2.0);
					}
					break;
			}
			break;
		case Arguments::SOBEL:
			switch (arg.maskDir) {
				case Arguments::X:
					out = sobel[0] * arg.inputImage;
					break;
				case Arguments::Y:
					out = sobel[1] * arg.inputImage;
					break;
				case Arguments::MAGNITUDE:
					Image x = sobel[0] * arg.inputImage;
					out     = sobel[1] * arg.inputImage;
					// Compute magnitude
#pragma omp parallel for
					for (unsigned i = 0; i < out.cols * out.rows; i++) {
						// Subtract out.maxVal / 2.0, since we have negative gradient
						// values, but we mapped to [0, 255]. Divide by 2.0 to map to
						// [0, 255] again
						out.pixels[i] = sqrt(((out.pixels[i] - out.maxVal / 2.0) *
						                          (out.pixels[i] - out.maxVal / 2.0) +
						                      (x.pixels[i] - out.maxVal / 2.0) *
						                          (x.pixels[i] - out.maxVal / 2.0)) /
						                     2.0);
					}
					break;
			}
			break;
		case Arguments::LAPLACIAN:
			out = laplacian * arg.inputImage;
			break;
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

	// Find optional argument switches
	for (unsigned i = 4; i < argc; i++) {
		if (!strcmp(argv[i], "-d")) {
			if (i + 1 >= argc) {
				std::cout << "Missing direction";
				err = 1;
				printHelp();
				return false;
			}

			using namespace std::string_literals;
			std::string currentArg = argv[i + 1];
			if (std::equal(currentArg.begin(), currentArg.end(), "x"s.begin(),
			               [](char a, char b) { return tolower(a) == tolower(b); }))
				arg.maskDir = Arguments::X;
			else if (std::equal(
			             currentArg.begin(), currentArg.end(), "y"s.begin(),
			             [](char a, char b) { return tolower(a) == tolower(b); }))
				arg.maskDir = Arguments::Y;
			else if (std::equal(
			             currentArg.begin(), currentArg.end(), "mag"s.begin(),
			             [](char a, char b) { return tolower(a) == tolower(b); }))
				arg.maskDir = Arguments::MAGNITUDE;
			else {
				std::cout << "direction \"" << argv[i + 1]
				          << "\" must be one of {x, y, mag}\n";
				printHelp();
				err = 2;
				return false;
			}

			i++;
		}
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

	using namespace std::string_literals;
	std::string currentArg = argv[2];
	if (std::equal(currentArg.begin(), currentArg.end(), "prewitt"s.begin(),
	               [](char a, char b) { return tolower(a) == tolower(b); }))
		arg.maskType = Arguments::PREWITT;
	else if (std::equal(currentArg.begin(), currentArg.end(), "sobel"s.begin(),
	                    [](char a, char b) { return tolower(a) == tolower(b); }))
		arg.maskType = Arguments::SOBEL;
	else if (std::equal(currentArg.begin(), currentArg.end(), "laplacian"s.begin(),
	                    [](char a, char b) { return tolower(a) == tolower(b); }))
		arg.maskType = Arguments::LAPLACIAN;
	else {
		std::cout << "mask type \"" << argv[2]
		          << "\" must be one of {prewitt, sobel, laplacian}\n";
		err = 2;
		return false;
	}

	if ((arg.maskType == Arguments::PREWITT || arg.maskType == Arguments::SOBEL) &&
	    arg.maskDir == Arguments::UNSPECIFIED) {
		std::cout << "When using mask type \"" << argv[2]
		          << "\", optional flag -d must be specified.\n";
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
	std::cout
	    << "Usage: gradient <image> <mask type> <output> [options]   (1)\n"
	    << "   or: gradient -h                                       (2)\n\n"
	    << "(1) Take an image file as input, apply a sharpening mask,\n"
	    << "    and save output to output file.\n"
	    << "    Mask types are:\n"
	    << "    - prewitt\n"
	    << "    - sobel\n"
	    << "    - laplacian\n"
	    << "    If prewitt or sobel is chosen, the optional switch -d is mandatory.\n"
	    << "(2) Print this help menu\n\n"
	    << "Options:\n"
	    << "  -d <direction>   Specify the gradient direction.\n"
	    << "                   direction can be one of {x, y, mag}.\n";
}