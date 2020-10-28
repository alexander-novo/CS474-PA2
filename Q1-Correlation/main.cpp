// Q1-Correlation/main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "../Common/image.h"

/*
  Normalizes an image based min-max nomralization
  @Param: image - the input image that will be quantized
  @Param: corr_values - the correlation values
  @Return: void
*/
void normalize_image(Image& image, std::vector<int> corr_values){

	int max = 0;
	int min = 1000000000;
	int value;

	// find min an max correlation values
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			value = corr_values[i * image.cols + j];

			if(value > max){
				max = value;
			}
			if(value < min)
				min = value;
		}
	}

	// scale each pixel and update image
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			double scaled_value = 255.0 * ((corr_values[i*image.cols + j] - min) / (double)(max - min));
			image[i][j] = (int) scaled_value;
		}
	}
}

/*
  Correlates an image based on the image mask
  @Param: image - the input image that will be quantized
  @Param: mask - the image mask
  @Param: size_y - the mask height
  @Param: mask - the mask width
  @Return: void
*/
void correlation(Image& image, Image mask, int size_y, int size_x){


	Image originalImage = Image(image);
	std::vector<int> correlation_values;

	// iterate through image pixels
    for(int i = 0; i < image.rows; i++){
   		for(int j = 0; j < image.cols; j++) {

   			int sum = 0;

   			// iterate over mask
   			for (int k = -size_y/2; k < size_y/2; k++)
   			{
   				for (int l = -size_x/2; l < size_x/2; l++)
   				{

   					//check bounds
   					if(i + k < 0 || i + k >= size_x || j + l < 0 || j + l >= size_y)
   						sum += 0;
   					else
   						sum += originalImage[i + k][j + l] * mask[k + size_y/2][l + size_x/2];
   				}
   			}

   			correlation_values.push_back(sum);
   		}
   	}

   	// normalize image
   	normalize_image(image, correlation_values);
   
}


int main(int argc, char** argv) {
	
 	//std::istringstream ss(argv[3]);

  // Read original image
 	std::ifstream inFile(argv[1]);
 	Image image = Image::read(inFile);

 	std::ifstream inMaskFile(argv[3]);
 	Image mask = Image::read(inMaskFile);

    // Get mask width
    int mask_size_x;
    std::istringstream ss(argv[4]);
 	if(ss >> mask_size_x) {
 		if(mask_size_x < 1 || mask_size_x > image.cols){

 			std::cout << "Error: Mask size should be greater than 0 and smaller than image size" << std::endl;
 				return 1;
 		}
 	}

 	// Get mask height
 	int mask_size_y;
    std::istringstream ss2(argv[5]);
 	if(ss2 >> mask_size_y) {
 		if(mask_size_y < 1 || mask_size_y > image.rows){

 			std::cout << "Error: Mask size should be greater than 0 and smaller than image size" << std::endl;
 				return 1;
 		}
 	}

	std::cout << "Question 1: Correlation." << std::endl;

  // Correlate the image
	correlation(image, mask, mask_size_y, mask_size_x);
	std::cout << "Finished" << std::endl;

	// Save output image
	std::ofstream outFile;
	outFile.open(argv[2]);

	outFile << image;
	outFile.close();

	return 0;
}