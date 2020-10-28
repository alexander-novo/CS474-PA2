// Q3-Median/main.cpp
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <utility>

#include "../Common/image.h"

/*
  Smoothes an image based on the Gaussian mask
  @Param: image - the input image that will be smoothed
  @Param: mask_size - the width and height of the mask
  @Param: noise_percentage - the amount of noise
  @Param: out - the output path
  @Return: void
*/
void smooth_image_average(Image& image, int mask_size, int noise_percentage, char* out){

  // Iterate though image pixels
  for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			int average = 0;

        // Iterate through mask
   			for (int k = -mask_size/2; k < mask_size/2; k++)
   			{
   				for (int l = -mask_size/2; l < mask_size/2; l++)
   				{
            // calcualte average
            if(i + k < 0 || i + k == image.cols || j + l < 0 || j + l == image.rows)
              average += 0;
            else
   					  average += image[i + k][j + l];
   				}
   			}

        // update pixel
   			image[i][j] = (int) (average / (mask_size * mask_size));
   		}
   	}

    // save smoothed image
    std::string path(out);
    path = path.substr(path.find('/') + 1);
    path = path.substr(0, path.find('-'));
    path += "-smoothed-" + std::to_string(mask_size) + '-' + std::to_string(noise_percentage) + ".pgm";
    std::ofstream outFile;
    outFile.open("out/" + path);
    outFile << image;
    outFile.close();
}

/*
  Filters an image based median filtering
  @Param: image - the input image that will be filtered
  @Param: mask_size - the width and height of the mask
  @Return: void
*/
void median_filter(Image& image, int mask_size){

	  Image orignalImage = Image(image);

    // Iterate though image pixels
    for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			std::vector<int> pixel_values;

        // Iterate though mask
   			for (int k = -mask_size/2; k < mask_size/2; k++)
   			{
   				for (int l = -mask_size/2; l < mask_size/2; l++)
   				{
   					//check bounds and pad zeros if necessary
   					if(i + k < 0 || i + k == image.cols || j + l < 0 || j + l == image.rows)
   						pixel_values.push_back(0);
   					else
   						pixel_values.push_back(orignalImage[i + k][j + l]);
   					
   				}
   			}

        // sort and get median value
   			std::sort(pixel_values.begin(), pixel_values.end());

   			image[i][j] = pixel_values[(mask_size * mask_size) / 2 + 1];
   		}
   	}
}

/*
  Adds noise to an image
  @Param: image - the input image to add noise to
  @Param: noise_percentage - the amount of noise
  @Param: out - the output path
  @Return: void
*/
void add_noise(Image& image, int noise_percentage, char* out){

    std::vector<int> indicies;
    int num_corrupt_pixels = (int) image.cols * image.rows * (noise_percentage / 100.0);

    // save each image location
    for (int i = 0; i < image.cols * image.rows; i++)
    {
      indicies.push_back(i);
    }

    // shuffle indecies
    std::random_shuffle(indicies.begin(), indicies.end());

    // pick first X% of ranodm pixels
    for (int k = 0; k < num_corrupt_pixels; k++)
    {
      int i = indicies[k] / image.cols;
      int j = indicies[k] % image.cols;

      // generate noise value
      int noise_value = 0;
      if(std::rand() % 100 < 50)
        noise_value = 255;

      image[i][j] = noise_value;
    }

    // save noisy image
    std::string path(out);
    path = path.substr(path.find('/') + 1);
    path = path.substr(0, path.find('-'));
    path += "-noise-" + std::to_string(noise_percentage) + ".pgm";
    std::ofstream outFile;
    outFile.open("out/" + path);
    outFile << image;
    outFile.close();
}

int main(int argc, char** argv) {

 	int mask_size;
 	int noise_percentage;
 	std::istringstream ss(argv[3]);

  // Get mask size
 	if(ss >> mask_size) {
 		if(mask_size != 7 && mask_size != 15){
 			std::cout << mask_size << std::endl;
 			std::cout << "Error: Mask size should be 7 or 15" << std::endl;
 			return 1;

 		}
 	}

 	std::istringstream ss2(argv[4]);

  // Get noise level
 	if(ss2 >> noise_percentage) {
 		if(noise_percentage > 100 || noise_percentage < 0){
 			std::cout << noise_percentage << std::endl;
 			std::cout << "Error: noise_percentage should be between 0 and 100" << std::endl;
 			return 1;

 		}
 	}

  // Read original image
 	std::ifstream inFile(argv[1]);

 	Image image = Image::read(inFile);

	std::cout << "Question 3: Median Filtering." << std::endl;

  // Add noise
	add_noise(image, noise_percentage, argv[2]);
  Image imageCopy = Image(image);

  // filter image
	median_filter(image, mask_size);

  // smooth image
  smooth_image_average(imageCopy, mask_size, noise_percentage, argv[2]);

	// Save output image
	std::ofstream outFile;
	outFile.open(argv[2]);
	outFile << image;
	outFile.close();

	return 0;
}