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

void smooth_image_average(Image& image, int mask_size, int noise_percentage, char* out){

  for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			int average = 0;

   			for (int k = -mask_size/2; k < mask_size/2; k++)
   			{
   				for (int l = -mask_size/2; l < mask_size/2; l++)
   				{
            if(i + k < 0 || i + k == image.cols || j + l < 0 || j + l == image.rows)
              average += 0;
            else
   					  average += image[i + k][j + l];
   				}
   			}

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

void median_filter(Image& image, int mask_size){

	  Image orignalImage = Image(image);

    for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			std::vector<int> pixel_values;

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

   			std::sort(pixel_values.begin(), pixel_values.end());

   			image[i][j] = pixel_values[(mask_size * mask_size) / 2 + 1];
   		}
   	}
}

void add_noise(Image& image, int noise_percentage, char* out){

    std::vector<int> indicies;
    int num_corrupt_pixels = (int) image.cols * image.rows * (noise_percentage / 100.0);

    //std::cout << image.rows * image.cols << std::endl;
    //std::cout << num_corrupt_pixels << std::endl;

    for (int i = 0; i < image.cols * image.rows; i++)
    {
      indicies.push_back(i);
    }

    std::random_shuffle(indicies.begin(), indicies.end());

    for (int k = 0; k < num_corrupt_pixels; k++)
    {
      int i = indicies[k] / image.cols;
      int j = indicies[k] % image.cols;

      //std::cout << i << " " << j << std::endl;

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
	int M, N, Q;
 	bool type;
 	int val;
 	int mask_size;
 	int noise_percentage;
 	std::istringstream ss(argv[3]);

  // Get quantization level
 	if(ss >> mask_size) {
 		if(mask_size != 7 && mask_size != 15){
 			std::cout << mask_size << std::endl;
 			std::cout << "Error: Mask size should be 7 or 15" << std::endl;
 			return 1;

 		}
 	}

 	std::istringstream ss2(argv[4]);

  // Get quantization level
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

  // Smooth the image
	add_noise(image, noise_percentage, argv[2]);

  Image imageCopy = Image(image);

	median_filter(image, mask_size);

  smooth_image_average(imageCopy, mask_size, noise_percentage, argv[2]);

	// Save output image
	std::ofstream outFile;
	outFile.open(argv[2]);
	outFile << image;
	outFile.close();

	return 0;
}