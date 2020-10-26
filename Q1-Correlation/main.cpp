// Q1-Correlation/main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "../Common/image.h"

/*
  Quantizes an image based on the quantization level
  @Param: image - the input image that will be quantized
  @Param: quantization_level - the number of gray level values to use
  @Return: void
*/
void correlation(Image& image, Image mask){



	Image originalImage = Image(image);

    for(int i = 0; i < image.cols; i++){
   		for(int j = 0; j < image.rows; j++) {

   			int sum = 0;

   			/*for (int k = -mask.cols/2; k < mask.cols/2; k++)
   			{
   				for (int l = -mask.rows/2; l < mask.rows/2; l++)
   				{
   					//std::cout << i << " " << i + k << std::endl;
   					//std::cout << l << " " << j + l << std::endl;

   					if(i + k < 0 || i + k == image.cols || j + l < 0 || j + l == image.rows)
   						sum += 0;
   					else
   						sum += originalImage[i + k][j + l] * mask[k + mask.rows/2][l + mask.cols/2];
   				}
   			}*/

   			//image[i][j] = (int) (sum / (mask.cols * mask.rows));
   			image[i][j] = image[i][j] / 2;
   		}
   	}

   	//std::ofstream outFile;
	//outFile.open("correlated.pgm");

	//outFile << image;
	//outFile.close();
   
}


int main(int argc, char** argv) {
	int M, N, Q;
 	bool type;
 	int val;
 	int quantization_level;
 	//std::istringstream ss(argv[3]);

  // Read original image
 	std::ifstream inFile(argv[1]);

 	Image image = Image::read(inFile);

 	std::ifstream inMaskFile(argv[2]);

 	Image mask = Image::read(inMaskFile);

	std::cout << "Question 1: Correlation." << std::endl;

  // Quantize the image
	correlation(image, mask);
	std::cout << "Finished" << std::endl;

	// Save output image
	std::ofstream outFile;
	outFile.open(argv[3]);

	outFile << image;
	outFile.close();

	return 0;
}