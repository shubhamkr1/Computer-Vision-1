//
// Watermark.cpp : Add watermark to an image, or inspect if a watermark is present.
//
// Based on skeleton code by D. Crandall, Spring 2017
//
// Ritesh Tawde, Shubham Kumar, Ritesh Agarwal
//
//

//Link to the header file
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <SImage.h>
#include <SImageIO.h>
#include <fft.h>
#include <math.h>
#include <fstream>
#include <math.h>

#define PI 3.14
using namespace std;

// This code requires that input be a *square* image, and that each dimension
//  is a power of 2; i.e. that input.width() == input.height() == 2^k, where k
//  is an integer. You'll need to pad out your image (with 0's) first if it's
//  not a square image to begin with. (Padding with 0's has no effect on the FT!)
//
// Forward FFT transform: take input image, and return real and imaginary parts.
//
void fft(const SDoublePlane& input, SDoublePlane& fft_real, SDoublePlane& fft_imag)
{
    fft_real = input;
    fft_imag = SDoublePlane(input.rows(), input.cols());

    FFT_2D(1, fft_real, fft_imag);
}

// Inverse FFT transform: take real and imaginary parts of fourier transform, and return
//  real-valued image.
//
void ifft(const SDoublePlane& input_real,
    const SDoublePlane& input_imag, SDoublePlane& output_real)
{
    output_real = input_real;
    SDoublePlane output_imag = input_imag;

    FFT_2D(0, output_real, output_imag);
}

// Write this in Part 1.1
SDoublePlane fft_magnitude(const SDoublePlane& real_part,
    const SDoublePlane& imag_part)
{
    SDoublePlane output_image = SDoublePlane(real_part.rows(), real_part.cols());

    for (int i = 0; i < real_part.rows(); i++) {
        for (int j = 0; j < real_part.cols(); j++) {
            output_image[i][j] = log10(sqrt(real_part[i][j] * real_part[i][j] + imag_part[i][j] * imag_part[i][j]));
        }
    }

    //normalization
    int min = 9999;
    int max = 0;
    for (int i = 0; i < output_image.rows(); i++) {
        for (int j = 0; j < output_image.cols(); j++) {

            if (output_image[i][j] > max)
                max = output_image[i][j];
            if (output_image[i][j] < min)
                min = output_image[i][j];
        }
    }

    for (int i = 0; i < output_image.rows(); i++) {
        for (int j = 0; j < output_image.cols(); j++) {

            output_image[i][j] = ((output_image[i][j] - min) * 255.0) / (max - min);
        }
    }
    return output_image;
}

// Write this in Part 1.2
SDoublePlane remove_interference(const SDoublePlane& input_image)
{

    SDoublePlane real_part = input_image;
    SDoublePlane imag_part = SDoublePlane(input_image.rows(), input_image.cols());
    SDoublePlane output_image;
    fft(input_image, real_part, imag_part);
    for (int i = 156; i <= 161; i++) {
        for (int j = 0; j < input_image.cols(); j++) {
            real_part[i][j] = 0;
            imag_part[i][j] = 0;
        }
    }
    for (int i = 352; i <= 357; i++) {
        for (int j = 0; j < input_image.rows(); j++) {
            real_part[i][j] = 0;
            imag_part[i][j] = 0;
        }
    }
    ifft(real_part, imag_part, output_image);
    return output_image;
}

//1.3
/*
To check the points on the circle given radius r
http://stackoverflow.com/questions/14096138/find-the-point-on-a-circle-with-given-center-point-radius-and-degree
*/
SDoublePlane mark_image(const SDoublePlane& input_image, int num)
{
    SDoublePlane real_part = input_image;
    SDoublePlane imag_part = SDoublePlane(input_image.rows(), input_image.cols());
    SDoublePlane output_image = input_image;
    //memcpy(&output_image, &input_image, sizeof(input_image));
    int center_of_image = input_image.rows() / 2;
    int radius = 100, i = 0, j = 0, cosine = 0, sine = 0, pos = 0;
    int num_of_points = 220;
    int angle_of_increment = 360 / num_of_points;
    int arr[num_of_points];
    double result, alpha = 0.05;
    srand(num);
    for (i = 0; i < num_of_points; i++) {
        arr[i] = rand() % 2;
    }
    fft(input_image, real_part, imag_part);

    for (i = 1; i <= num_of_points / 2; i++) {
        sine = (radius * sin((angle_of_increment * i) * (PI / 180))) + center_of_image;
        cosine = (radius * cos((angle_of_increment * i) * (PI / 180))) + center_of_image;
        real_part[cosine][sine] = real_part[cosine][sine] + (alpha * fabs(real_part[cosine][sine]) * arr[i]);

        //theta + pi
        sine = center_of_image - (sine - center_of_image);
        cosine = center_of_image - (cosine - center_of_image);
        real_part[cosine][sine] = real_part[cosine][sine] + (alpha * fabs(real_part[cosine][sine]) * arr[i]);
    }

    ifft(real_part, imag_part, output_image);
    return output_image;
}


/*http://www.socscistatistics.com/tests/pearson/*/
void check_image(const SDoublePlane& input_image, int num)
{
    SDoublePlane real_part = input_image;
    SDoublePlane imag_part = SDoublePlane(input_image.rows(), input_image.cols());
    SDoublePlane output_image = input_image;
    //memcpy(&output_image, &input_image, sizeof(input_image));
    int center_of_image = input_image.rows() / 2;
    int radius = 100, i = 0, j = 0, cosine = 0, sine = 0;
    int num_of_points = 220;
    int angle_of_increment = 360 / num_of_points;
    int arr[num_of_points];
    double result, alpha = 0.05, correlation = 0;
    int threshold = 0.003;
    double mean_arr = 0, mean_cvect = 0, mean_cvarr = 0;
    double sum1 = 0, sum2 = 0, sum3 = 0, sumsq1 = 0, sumsq2 = 0, diff = 0, denom = 0;

    //vector c
    double cvector[num_of_points];
    srand(num);
    for (i = 0; i < num_of_points; i++) {
        arr[i] = rand() % 2;
    }
    fft(input_image, real_part, imag_part);
    //SImageIO::write_png_file("girl_fft.png",real_part,real_part,real_part);
    for (i = 1; i <= num_of_points / 2; i++) {
        sine = (radius * sin((angle_of_increment * i) * (PI / 180))) + center_of_image;
        cosine = (radius * cos((angle_of_increment * i) * (PI / 180))) + center_of_image;

        cvector[i] = real_part[cosine][sine]; //+ (alpha * fabs(real_part[cosine][sine]) * arr[i]);

        //theta + pi
        sine = center_of_image - (sine - center_of_image);
        cosine = center_of_image - (cosine - center_of_image);

        cvector[i + 45] = real_part[cosine][sine]; //+ (alpha * fabs(real_part[cosine][sine]) * arr[i]);
    }

    for (i = 0; i < num_of_points; i++) {
        sum1 += arr[i];
        sum2 += cvector[i];
        sum3 += cvector[i] * arr[i];
        sumsq1 += arr[i] * arr[i];
        sumsq2 += cvector[i] * cvector[i];
    }

    mean_arr = sum1 / (sizeof(arr) / sizeof(arr[0]));
    mean_cvect = sum2 / (sizeof(cvector) / sizeof(cvector[0]));
    mean_cvarr = sum3 / (sizeof(arr) / sizeof(arr[0]));

    diff = (mean_cvarr) - (mean_cvect * mean_arr);
    denom = sqrt(sumsq1 - (mean_arr * mean_arr)) * sqrt(sumsq2 - (mean_cvect * mean_cvect));
    correlation = diff / denom;

    if (correlation >= threshold) {
        cout << "\n Watermark is present" << endl;
    }
    else {
        cout << "\nWatermark not detected" << endl;
    }
}

int main(int argc, char** argv)
{
    try {

        if (argc < 4) {
            cout << "Insufficent number of arguments; correct usage:" << endl;
            cout << "    p2 problemID inputfile outputfile" << endl;
            return -1;
        }

        string part = argv[1];
        string inputFile = argv[2];
        string outputFile = argv[3];

        SDoublePlane input_image = SImageIO::read_png_file(inputFile.c_str());
        //printf("resolution input : ",input_image.rows()," | "+input_image.cols());
        //SDoublePlane input_image1 = SImageIO::read_png_file(outputFile.c_str());

        SDoublePlane real_part = input_image;
        SDoublePlane imag_part = SDoublePlane(input_image.rows(), input_image.cols());
        SDoublePlane spectogram;
        int i = 0, j = 0;
        if (part == "1.1") {
            fft(input_image, real_part, imag_part);
            SDoublePlane output_image = fft_magnitude(real_part, imag_part);
            SImageIO::write_png_file(outputFile.c_str(), output_image, output_image, output_image);
        }
        else if (part == "1.2") {
            SDoublePlane output_image = remove_interference(input_image);
            //temp_remove_inter(input_image);
            SImageIO::write_png_file(outputFile.c_str(), output_image, output_image, output_image);
        }
        else if (part == "1.3") {
            if (argc < 6) {
                cout << "Need 6 parameters for watermark part:" << endl;
                cout << "    p2 1.3 inputfile outputfile operation N" << endl;
                return -1;
            }
            string op(argv[4]);
            if (op == "add") {
                // add watermark
                int N = atoi(argv[5]);
                SDoublePlane output_image = mark_image(input_image, N);
                SImageIO::write_png_file(outputFile.c_str(), output_image, output_image, output_image);
            }
            else if (op == "check") {
                int N = atoi(argv[5]);
                check_image(input_image, N);
            }
            else
                throw string("Bad operation!");
        }
        else
            throw string("Bad part!");
    }
    catch (const string& err) {
        cerr << "Error: " << err << endl;
    }
}
