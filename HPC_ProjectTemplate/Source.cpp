#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;

struct Filter
{
	int size;
	int* data;
};
#include <iostream>
#include <iomanip>

int* blur(Filter filter, int* image, int width, int height);

int* create_padded_image(int* w, int* h , int filter_size) {
	int extra_rows_cols = (filter_size / 2);
	int height_new = *h + extra_rows_cols * 2;
	int width_new  = *w + extra_rows_cols * 2;

	int* img = new int[height_new * width_new];

	//add zeros to the upper rows only but with full width
	for (int i = 0; i < extra_rows_cols; i++) {
		for (int j = 0; j < width_new; j++) {
			img[i * width_new + j] = 0;
		}
	}
	//add zeros to the lower rows only but with full width
	for (int i = *h; i < (*h + extra_rows_cols); i++) {
		for (int j = 0; j < width_new; j++) {
			img[i * width_new + j] = 0;
		}
	}
	//add zeros to the left columns only but with full height
	for (int i = extra_rows_cols; i < (height_new - extra_rows_cols); i++) {
		for (int j = 0; j < extra_rows_cols; j++) {
			img[i * width_new + j] = 0;
		}
	}
	//add zeros to the right most columns only but with full height
	for (int i = extra_rows_cols; i < (height_new - extra_rows_cols); i++) {
		for (int j = *w; j < (*w + extra_rows_cols); j++) {
			img[i * width_new + j] = 0;
		}
	}
	*w = width_new; *h = height_new;
	return img;
}

int* inputImage(int* w, int* h, System::String^ imagePath, int filter_size) //put the size of image in w & h
{
	int* input;


	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;


	int extra_rows_cols = (filter_size / 2);
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	//this function modifies the width and height
	input = create_padded_image(w, h, filter_size);

	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[(i + extra_rows_cols) * (*w) + (j + extra_rows_cols)] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	cout << "result Image Saved " << index << endl;
}


int main()
{
	int ImageWidth_padded = 4, ImageHeight_padded = 4;

	int start_s, stop_s, TotalTime = 0;

	System::String^ imagePath;
	std::string img;
	img = "..//Data//Input//lena.png";

	Filter filter;
	cout << "Enter kernel size (must be an odd number)" << endl;
	cin >> filter.size;
	if (filter.size % 2 == 0)
		return 0;
	//fill the filter
	filter.data = new int[filter.size * filter.size];
	for (int i = 0; i < filter.size; i++) {
		for (int j = 0; j < filter.size; j++) {
			filter.data[i * filter.size + j] = 1;
		}
	}

	imagePath = marshal_as<System::String^>(img);
	int* imageData = inputImage(&ImageWidth_padded, &ImageHeight_padded, imagePath, filter.size);


	start_s = clock();

	cout << "Image width = " << ImageWidth_padded << "\n" << "Image height = " << ImageHeight_padded << endl;
	//createImage(imageData, ImageWidth_padded, ImageHeight_padded , 1);
	//return 0;
	int* output_img = blur(filter, imageData, ImageWidth_padded, ImageHeight_padded);
	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	createImage(output_img, (ImageWidth_padded - (filter.size / 2) * 2), (ImageHeight_padded - (filter.size / 2) * 2), 1);
	cout << "time: " << TotalTime << endl;
	
	free(output_img);
	
	free(imageData);
	free(filter.data);

}

int* blur(Filter filter, int* image, int width, int height) {
	int original_hieght = (height - (filter.size / 2) * 2); //this is an integer division which means that the 2s don't cancel each other
	int original_width = (width - (filter.size / 2) * 2);

	int* output = new int[original_hieght * original_width];
	//int offset = 5 / 2;

	for (int i = 0; i < height - filter.size; i++) {
		for (int j = 0; j < width - filter.size; j++) {
			int sop = 0;
			for (int h = 0; h < filter.size; h++) {
				for (int w = 0; w < filter.size; w++) {
					sop += filter.data[h * filter.size + w] * image[(i + h)*width + (j + w)];
				}
			}
			output[i * original_width + j] = sop / (filter.size * filter.size);
		}
	}
	return output;
}
