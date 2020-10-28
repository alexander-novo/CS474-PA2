// Q2-Smoothing/main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../Common/image.h"

// 7x7 Gaussian mask
static const int mask_7x7[7][7] = {

		{1, 1, 2, 2, 2, 1, 1},
		{1, 2, 2, 4, 2, 2, 1},
		{2, 2, 4, 8, 4, 2, 2},
		{2, 4, 8, 16, 8, 4, 2},
		{2, 2, 4, 8, 4, 2, 2},
		{1, 2, 2, 4, 2, 2, 1},
		{1, 1, 2, 2, 2, 1, 1}
							
						};

//15x15 Gaussian mask
static const int mask_15x15[15][15] = {

	{2, 2,  3,  4,  5,  5,  6,  6,  6,  5,  5,  4,  3, 2, 2},
	{2, 3,  4,  5,  7,  7,  8,  8,  8,  7,  7,  5,  4, 3, 2},
	{3, 4,  6,  7,  9, 10, 10, 11, 10, 10,  9,  7,  6, 4, 3},
	{4, 5,  7,  9, 10, 12, 13, 13, 13, 12, 10,  9,  7, 5, 4},
	{5, 7,  9, 11, 13, 14, 15, 16, 15, 14, 13, 11,  9, 7, 5},
	{5, 7, 10, 12, 14, 16, 17, 18, 17, 16, 14, 12, 10, 7, 5},
	{6, 8, 10, 13, 15, 17, 19, 19, 19, 17, 15, 13, 10, 8, 6},
	{6, 8, 11, 13, 16, 18, 19, 20, 19, 18, 16, 13, 11, 8, 6},
	{6, 8, 10, 13, 15, 17, 19, 19, 19, 17, 15, 13, 10, 8, 6},
	{5, 7, 10, 12, 14, 16, 17, 18, 17, 16, 14, 12, 10, 7, 5},
	{5, 7,  9, 11, 13, 14, 15, 16, 15, 14, 13, 11,  9, 7, 5},
	{4, 5,  7,  9, 10, 12, 13, 13, 13, 12, 10,  9,  7, 5, 4},
	{3, 4,  6,  7,  9, 10, 10, 11, 10, 10,  9,  7,  6, 4, 3},
	{2, 3,  4,  5,  7,  7,  8,  8,  8,  7,  7,  5,  4, 3, 2},
	{2, 2,  3,  4,  5,  5,  6,  6,  6,  5,  5,  4,  3, 2, 2},
};

/*
  Smoothes an image based on averaging
  @Param: image - the input image that will be smoothed
  @Param: mask_size - the width and height of the mask
  @Return: void
*/
void smooth_image_average(Image& image, int mask_size){

	Image originalImage = Image(image);

	// Iterate through image pixels
  	for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			int average = 0;

   			//iterate through mask
   			for (int k = -mask_size/2; k < mask_size/2; k++)
   			{
   				for (int l = -mask_size/2; l < mask_size/2; l++)
   				{
   					// calcualte average
   					if(i + k < 0 || i + k >= image.cols || j + l < 0 || j + l >= image.rows)
   						average += 0;
   					else
   						average += originalImage[i + k][j + l];
   				}
   			}

   			image[i][j] = (int) (average / (mask_size * mask_size));
   		}
   	}
}

/*
  Smoothes an image based on the Gaussian mask
  @Param: image - the input image that will be smoothed
  @Param: mask_size - the width and height of the mask
  @Return: void
*/
void smooth_image_gaussian(Image& image, int mask_size){

	int normalizion_factor = 0;

	Image originalImage = Image(image);

	// calculate normalizion factor
	for(int i = 0; i < mask_size; i++){
		for (int j = 0; j < mask_size; j++){

			if(mask_size == 7)
				normalizion_factor += mask_7x7[i][j];
			else
				normalizion_factor += mask_15x15[i][j];
		}
	}

	// iterate through image pixels
    for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			int output_pixel_value = 0;

   			// iterate through mask
   			for (int k = -mask_size/2; k < mask_size/2; k++)
   			{
   				for (int l = -mask_size/2; l < mask_size/2; l++)
   				{
   					//bounds checking and padding
   					if(i + k < 0 || i + k == image.cols || j + l < 0 || j + l == image.rows){
   						output_pixel_value += 0;
   					}
   					//calculate output value
   					else if(mask_size == 7)
   						output_pixel_value += originalImage[i + k][j + l] * mask_7x7[k + mask_size/2][l + mask_size/2];
   					else
   						output_pixel_value += originalImage[i + k][j + l] * mask_15x15[k + mask_size/2 - 1][l + mask_size/2 - 1];
   				}
   			}

   			// update image pixel
   			image[i][j] = (int) (output_pixel_value / normalizion_factor);
   		}
   	}
}

int main(int argc, char** argv) {

 	int mask_size;
 	std::istringstream ss(argv[3]);

  // Get mask size
 	if(ss >> mask_size) {
 		if(mask_size != 7 && mask_size != 15){
 			std::cout << mask_size << std::endl;
 			std::cout << "Error: Mask size should be 7 or 15" << std::endl;
 			return 1;

 		}
 	}

 	// Get type of smoothing
 	std::string filter_type = argv[4];
 	if(filter_type != "average" && filter_type != "gaussian"){

 		std::cout << "Error: Mask type should be average or gaussian" << std::endl;
 		std::cout << filter_type << std::endl;
 			return 1;
 	}

  // Read original image
 	std::ifstream inFile(argv[1]);

 	Image image = Image::read(inFile);

	std::cout << "Question 2: Smoothing." << std::endl;

  // Smooth the image
	if(filter_type == "average")
		smooth_image_average(image, mask_size);
	else
		smooth_image_gaussian(image, mask_size);

	// Save output image
	std::ofstream outFile;
	outFile.open(argv[2]);
	outFile << image;
	outFile.close();

	return 0;
}