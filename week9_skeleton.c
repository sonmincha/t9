#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <math.h>
#include <stdio.h>
#include <float.h>


void downsampling(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void upsampling(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void bilinear(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size);

int main()
{

	int height;
	int width;
	int channel;

	float PSNR_up, PSNR_bilinear;

	unsigned char* imgIn = stbi_load("test_img2.bmp", &width, &height, &channel, 3);
	unsigned char* imgOut_down0 = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 320 * 240);
	unsigned char* imgOut_up0 = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 960 * 720);
	unsigned char* imgOut_up1 = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 960 * 720);

	downsampling(imgIn, height, width, channel, imgOut_down0);
	upsampling(imgOut_down0, height, width, channel, imgOut_up0);
	bilinear(imgOut_down0, height, width, channel, imgOut_up1);
	PSNR_up = imagePSNR(imgIn, imgOut_up0, 3 * 960 * 720);
	PSNR_bilinear = imagePSNR(imgIn, imgOut_up1, 3 * 960 * 720);
	printf("%f, %f\n", PSNR_up, PSNR_bilinear);

	stbi_write_bmp("image_down.bmp", width / 3, height / 3, channel, imgOut_down0);
	stbi_write_bmp("image_up.bmp", width, height, channel, imgOut_up0);
	stbi_write_bmp("image_bi.bmp", width, height, channel, imgOut_up1);

	stbi_image_free(imgIn);
	free(imgOut_down0);
	free(imgOut_up0);
	free(imgOut_up1);


	return 0;
}

void downsampling(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	for (int i = 0; i < (height / 3); i++) {
		for (int j = 0; j < (width / 3); j++) {
			for (int k = 0; k < channel; k++) {
				out[i * (width / 3) * channel + j * channel + k] = in[i * (width * 3) * channel + j * (channel * 3) + k];
			}
		}
	}
}

void upsampling(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < channel; k++) {
				out[i * width * channel + j * channel + k] = in[(i / 3) * (width / 3) * channel + (j / 3) * channel + k];
			}
		}
	}

}

void bilinear(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	float colrat1, colrat2;
	float rowrat1, rowrat2;
	float hei1, hei2;

	for (int i = 0; i < (height * width * channel); i++) {
		out[i] = 0;
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < channel; k++) {
				colrat1 = (float)(3 - (i % 3)) / 3;
				colrat2 = (float)(i % 3) / 3;

				rowrat1 = (float)(3 - (j % 3)) / 3;
				rowrat2 = (float)(j % 3) / 3;

				hei1 = (float){ colrat1 * in[k + (j / 3) * channel + (i / 3) * (width / 3) * channel] + colrat2 * in[k + (j / 3) * channel + ((i + 3) / 3) * (width / 3) * channel] };
				hei2 = (float){ colrat1 * in[k + ((j + 3) / 3) * channel + (i / 3) * (width / 3) * channel] + colrat2 * in[k + ((j + 3) / 3) * channel + ((i + 3) / 3) * (width / 3) * channel] };

				out[k + j * channel + i * width * channel] = (float){ rowrat1 * hei1 + rowrat2 * hei2 };
			}
		}
	}
}

double imagePSNR(unsigned char* frame1, unsigned char* frame2, unsigned int size) {

	double sum = 0;
	double mse = 0;
	double psnr = 0;

	for (int i = 0; i < 720; i++) {
		for (int j = 0; j < 960; j++) {
			for (int k = 0; k < 3; k++) {
				sum += (double)pow(frame1[i * 960 * 3 + j * 3 + k] - frame2[i * 960 * 3 + j * 3 + k], 2);
			}
		}
	}

	mse = (double)(sum / (3 * 960 * 720));
	psnr = 20.0 * log10(255) - 10 * log10(mse);
	return psnr;
}
