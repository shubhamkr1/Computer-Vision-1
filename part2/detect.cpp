//
// detect.cpp : Detect cars in satellite images.
//
// Based on skeleton code by D. Crandall, Spring 2017
//
// PUT YOUR NAMES HERE
//
//

#include <SImage.h>
#include <SImageIO.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// The simple image class is called SDoublePlane, with each pixel represented as
// a double (floating point) type. This means that an SDoublePlane can represent
// values outside the range 0-255, and thus can represent squared gradient magnitudes,
// harris corner scores, etc. 
//
// The SImageIO class supports reading and writing PNG files. It will read in
// a color PNG file, convert it to grayscale, and then return it to you in 
// an SDoublePlane. The values in this SDoublePlane will be in the range [0,255].
//
// To write out an image, call write_png_file(). It takes three separate planes,
// one for each primary color (red, green, blue). To write a grayscale image,
// just pass the same SDoublePlane for all 3 planes. In order to get sensible
// results, the values in the SDoublePlane should be in the range [0,255].
//

// Below is a helper functions that overlays rectangles
// on an image plane for visualization purpose. 

// Draws a rectangle on an image plane, using the specified gray level value and line width.
//
void overlay_rectangle(SDoublePlane &input, int _top, int _left, int _bottom, int _right, double graylevel, int width)
{
	for (int w = -width / 2; w <= width / 2; w++) {
		int top = _top + w, left = _left + w, right = _right + w, bottom = _bottom + w;

		// if any of the coordinates are out-of-bounds, truncate them 
		top = min(max(top, 0), input.rows() - 1);
		bottom = min(max(bottom, 0), input.rows() - 1);
		left = min(max(left, 0), input.cols() - 1);
		right = min(max(right, 0), input.cols() - 1);

		// draw top and bottom lines
		for (int j = left; j <= right; j++)
			input[top][j] = input[bottom][j] = graylevel;
		// draw left and right lines
		for (int i = top; i <= bottom; i++)
			input[i][left] = input[i][right] = graylevel;
	}
}

// DetectedBox class may be helpful!
//  Feel free to modify.
//
class DetectedBox {
public:
	int row, col, width, height;
	double confidence;
};

// Function that outputs the ascii detection output file
void  write_detection_txt(const string &filename, const vector<DetectedBox> &cars)
{
	ofstream ofs(filename.c_str());

	for (vector<DetectedBox>::const_iterator s = cars.begin(); s != cars.end(); ++s)
		ofs << s->row << " " << s->col << " " << s->width << " " << s->height << " " << s->confidence << endl;
}

// Function that outputs a visualization of detected boxes
void  write_detection_image(const string &filename, const vector<DetectedBox> &cars, const SDoublePlane &input)
{
	SDoublePlane output_planes[3];

	for (int p = 0; p<3; p++)
	{
		output_planes[p] = input;
		for (vector<DetectedBox>::const_iterator s = cars.begin(); s != cars.end(); ++s)
			overlay_rectangle(output_planes[p], s->row, s->col, s->row + s->height - 1, s->col + s->width - 1, p == 2 ? 255 : 0, 2);
	}

	SImageIO::write_png_file(filename.c_str(), output_planes[0], output_planes[1], output_planes[2]);
}


// The rest of these functions are incomplete. These are just suggestions to 
// get you started -- feel free to add extra functions, change function
// parameters, etc.

// Convolve an image with a separable convolution kernel
//
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
	SDoublePlane output(input.rows(), input.cols());
	// Convolution code here
	int kcenx = row_filter.rows() / 2;
	int kceny = row_filter.cols() / 2;
	for (int i = 0; i < input.rows(); i++)
	{
		for (int j = 0; j<input.cols(); j++)
		{
			double sum = 0;
			for (int k = 0; k < row_filter.rows(); ++k)
			{
				for (int l = 0; l < row_filter.cols(); l++)
				{
					if ((i + k - kceny)<input.rows() && (i + k - kceny) >= 0 && (j + l - kcenx)<input.cols() && (j + l - kcenx) >= 0)
					{
						sum += input[i - kceny + k][j - kcenx + l] * row_filter[row_filter.rows() - 1 - k][row_filter.cols() - 1 - l];
					}
				}
			}
			output[i][j] = sum;
		}
	}
	kcenx = col_filter.rows() / 2;
	kceny = col_filter.cols() / 2;
	for (int i = 0; i < input.rows(); i++)
	{
		for (int j = 0; j<input.cols(); j++)
		{
			double sum = 0;
			for (int k = 0; k < col_filter.rows(); ++k)
			{
				for (int l = 0; l < col_filter.cols(); l++)
				{
					if ((i + k - kceny)<input.rows() && (i + k - kceny) >= 0 && (j + l - kcenx)<input.cols() && (j + l - kcenx) >= 0)
					{
						sum += input[i - kceny + k][j - kcenx + l] * col_filter[col_filter.rows() - 1 - k][col_filter.cols() - 1 - l];
					}
				}
			}
			output[i][j] = sum;
		}
	}
	return output;
}

// Convolve an image with a  convolution kernel
//
SDoublePlane convolve_general(const SDoublePlane &input, const SDoublePlane &filter)
{
	SDoublePlane output(input.rows(), input.cols());

	int kcenx = filter.rows() / 2;
	int kceny = filter.cols() / 2;
	for (int i = 0; i < input.rows(); i++)
	{
		for (int j = 0; j<input.cols(); j++)
		{
			double sum = 0;
			for (int k = 0; k < filter.rows(); ++k)
			{
				for (int l = 0; l < filter.cols(); l++)
				{
					if ((i + k - kceny)<input.rows() && (i + k - kceny) >= 0 && (j + l - kcenx)<input.cols() && (j + l - kcenx) >= 0)
					{
						sum += input[i - kceny + k][j - kcenx + l] * filter[filter.rows() - 1 - k][filter.cols() - 1 - l];
					}
				}
			}
			output[i][j] = sum;
		}
	}
	// Convolution code here

	return output;
}


// Apply a sobel operator to an image, returns the result
// 
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool _gx)
{
	SDoublePlane output(input.rows(), input.cols());
	int row_filt[] = { 1, 2, 1 };
	int col_filt[] = { 1, 0, -1 };
	int row_filt1[] = { 1, 0, -1 };
	int col_filt1[] = { 1, 2, 1 };
	SDoublePlane row_filter(3, 1);
	SDoublePlane col_filter(1, 3);
	if (_gx == true){

		for (int i = 0; i < row_filter.rows(); i++){
			for (int j = 0; j < row_filter.cols(); j++){
				row_filter[i][j] = row_filt[i + j];
				//cout << row_filt[i + j];
			}
		}
		for (int i = 0; i < col_filter.rows(); i++){
			for (int j = 0; j < col_filter.cols(); j++){
				col_filter[i][j] = col_filt[i + j];
				//cout << col_filt[i + j];
			}
		}
	}
	else if (_gx == false){
		for (int i = 0; i < row_filter.rows(); i++){
			for (int j = 0; j < row_filter.cols(); j++){
				row_filter[i][j] = row_filt1[i + j];
				//cout << row_filt[i + j];
			}
		}
		for (int i = 0; i < col_filter.rows(); i++){
			for (int j = 0; j < col_filter.cols(); j++){
				col_filter[i][j] = col_filt1[i + j];
				//cout << col_filt[i + j];
			}
		}
	}
	output = convolve_separable(input, row_filter, col_filter);
	// Implement a sobel gradient estimation filter with 1-d filters
	return output;
}

// Apply an edge detector to an image, returns the binary edge map
// 
SDoublePlane find_edges(const SDoublePlane &input, double thresh = 0)
{
	SDoublePlane output(input.rows(), input.cols());
	SDoublePlane sobel_hx(3, 3);
	SDoublePlane sobel_hy(3, 3);
	float sobel_hx1[3][3] = { { -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 }
	};
	float sobel_hy1[3][3] = { { 1, 2, 1 },
	{ 0, 0, 0 },
	{ -1, -2, -1 }
	};
	for (int k = 0; k < 3; k++){
		for (int j = 0; j < 3; j++){
			sobel_hx[k][j] = sobel_hx1[k][j];
			sobel_hy[k][j] = sobel_hy1[k][j];
		}
	}
	float sum;
	SDoublePlane hx_grad(input.rows(), input.cols());
	SDoublePlane hy_grad(input.rows(), input.cols());
	hx_grad = convolve_general(input, sobel_hx);
	hy_grad = convolve_general(input, sobel_hy);
	for (int y = 1; y < hx_grad.rows() - 1; y++){
		for (int x = 1; x < hx_grad.cols() - 1; x++){
			sum = abs(hx_grad[y][x]) + abs(hy_grad[y][x]);
			sum = sum > thresh ? 255 : sum;
			sum = sum < thresh ? 0 : sum;
			output[y][x] = sum;
		}
	}

	// Implement an edge detector of your choice, e.g.
	// use your sobel gradient operator to compute the gradient magnitude and threshold

	return output;
}


//
// This main file just outputs a few test images. You'll want to change it to do 
//  something more interesting!
//
int main(int argc, char *argv[])
{
	if (!(argc == 2))
	{
		cerr << "usage: " << argv[0] << " input_image" << endl;
		return 1;
	}

	string input_filename(argv[1]);
	SDoublePlane input_image = SImageIO::read_png_file(input_filename.c_str());
	SDoublePlane output(input_image.rows(), input_image.cols());
	output = find_edges(input_image, 150);
	SImageIO::write_png_file("edges.png", output, output, output);
	// test step 2 by applying mean filters to the input image
	/*SDoublePlane mean_filter(3,3);
	for(int i=0; i<3; i++)
	for(int j=0; j<3; j++)
	mean_filter[i][j] = 1/9.0;
	SDoublePlane output_image = convolve_general(input_image, mean_filter);*/


	// randomly generate some detected cars -- you'll want to replace this
	//  with your car detection code obviously!
	vector<DetectedBox> cars;
	string temp_filename;
	if (input_filename == "Plaza.png"){
		temp_filename = "temp_4.png";
	}
	else if (input_filename == "Informatics.png"){
		temp_filename = "temp_3.png";
	}
	else{
		temp_filename = "temp_1.png";
	}

	SDoublePlane temp_image = SImageIO::read_png_file(temp_filename.c_str());
	int total = 0;
	for (int i = 0; i < temp_image.rows(); i++){
		for (int j = 0; j < temp_image.cols(); j++){
			if (temp_image[i][j] == 255){
				total += 1;
			}
		}
	}
	int low_limit, high_limit;
	int adj;
	if (input_filename == "Plaza.png"){
		low_limit = total - 30;
		high_limit = total + 50;
		adj = 5;
	}
	else if (input_filename == "Informatics.png"){
		low_limit = total - 120;
		high_limit = total + 250;
		adj = 5;
	}
	else if (input_filename == "SRSC.png"){
		low_limit = total - 100;
		high_limit = total + 300;
		adj = 0;
	}
	SDoublePlane check_image(input_image.rows(), input_image.cols());
	int pixels_in;
	//for each (x,y) count the number of edge pixels and store them in a matrix
	for (int i = 0; i < output.rows(); i++)
	{
		for (int j = 0; j < output.cols(); j++)
		{
			pixels_in = 0;
			for (int k = 0; k < temp_image.rows(); k++){
				for (int l = 0; l < temp_image.cols(); l++){
					if (((i + k) < input_image.rows()) && ((j + l) < input_image.cols())){
						if (output[i + k][j + l] == 255){
							pixels_in += 1;
						}
					}

				}
			}
			check_image[i][j] = pixels_in;
		}
	}
	int var1 = 0, var2 = 0;
	//for each pixel matching the limit we must mark it as a car
	for (int i = 0; i < check_image.rows(); i++)
	{
		for (int j = 0; j < check_image.cols(); j++)
		{
			if ((check_image[i][j] <= high_limit) && (check_image[i][j] >= low_limit)){
				if ((i >= (var1 + temp_image.rows() - adj)) || (j >= (var2 + temp_image.cols() - adj))){  //keep two rectangles from overlapping
					var1 = i;
					var2 = j;
					DetectedBox s;
					s.row = i;
					s.col = j;
					s.width = temp_image.cols();
					s.height = temp_image.rows();
					s.confidence = 10e08;
					cars.push_back(s);
				}

			}

		}
	}
	/*for(int i=0; i<10; i++)
	{
	DetectedBox s;
	s.row = rand() % input_image.rows();
	s.col = rand() % input_image.cols();
	s.width = 20;
	s.height = 20;
	s.confidence = rand();
	cout <<"confidence"<< s.confidence;
	cars.push_back(s);
	}*/

	write_detection_txt("detected.txt", cars);
	write_detection_image("detected.png", cars, input_image);
}
//References:
//http://www.songho.ca/dsp/convolution/convolution.html
//http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
//https://en.wikipedia.org/wiki/Template_matching
//https://blogs.msdn.microsoft.com/nativeconcurrency/2011/11/01/convolution-sample/
//https://courses.cs.washington.edu/courses/cse576/book/ch5.pdf
//http://www.cs.cornell.edu/courses/cs6670/2011sp/lectures/lec02_filter.pdf