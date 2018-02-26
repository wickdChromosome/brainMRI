/* 
 * Function: Varies sharpness and shiftscale of an input image
 * 
 * 
 *
 * Author: Bence Kotis
 * 
 */

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkCastImageFilter.h"
#include <iostream>
#include <string>


int main() {

	float variance;
	float shiftScaleNum;

	float varianceVect[100];
	float shiftScaleVect[100];

	//generate both arrays
	for (int i = 0; i < 100; i++) {
	
		//variance from 
		varianceVect[i] = (float) i/2;	
		shiftScaleVect[i] = (float) i/100;

	}
	std::cout << "Done making value arrays\n";

	int i = 0;
	//main loop to generate images
	while (i <= 200) {

		if (i < 100) {
		
			variance = varianceVect[i];
			shiftScaleNum = 0.5;
	
		} else {
		
			variance = 25;
			shiftScaleNum = shiftScaleVect[i-100];
	
		}

		std::cout << "Set shiftscale to " << shiftScaleNum << "\n";
		std::cout << "Set variance to " << variance << "\n";

		char *inputFileName = "brainMri.dcm";

		typedef float PixelType;
		typedef unsigned char OutputPixelType;

		typedef itk::Image<PixelType, 2> ImageType;
		typedef itk::Image<OutputPixelType, 2> OutputImageType;

		ImageType::SizeType size;
		ImageType::IndexType index;

		typedef itk::ImageFileReader<ImageType> ReaderType;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(inputFileName);
		reader->Update();

		typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType > GaussianFilterType;
		GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
		gaussianFilter->SetMaximumKernelWidth(200); //to avoid max kernel size warning
		gaussianFilter->SetInput(reader->GetOutput());
		gaussianFilter->SetVariance(variance);

		typedef itk::ShiftScaleImageFilter< ImageType, ImageType >  ShiftScaleFilterType;
		ShiftScaleFilterType::Pointer shiftFilter = ShiftScaleFilterType::New();
		shiftFilter->SetInput(gaussianFilter->GetOutput());
		shiftFilter->SetShift(shiftScaleNum);

		typedef itk::SubtractImageFilter< ImageType, ImageType, ImageType > SubtractFilter;
		SubtractFilter::Pointer subtractor = SubtractFilter::New();
		subtractor->SetInput2(shiftFilter->GetOutput());
		subtractor->SetInput1(reader->GetOutput());

		typedef itk::RescaleIntensityImageFilter<ImageType, OutputImageType> RescalerType;
		RescalerType::Pointer rescaler = RescalerType::New();
		rescaler->SetInput(subtractor->GetOutput());
		rescaler->SetOutputMaximum(255);
		rescaler->SetOutputMinimum(0);
		std::cout << "About to define writer\n";

		typedef itk::ImageFileWriter<OutputImageType> WriterType;
		WriterType::Pointer writer = WriterType::New();
		std::string path;	

		std::cout << "Defining path..\n";		

		if (i < 100) {

			std::string base = "out/variance/brainMri";
			std::string ext = ".png";
			path = base.append(std::to_string(i)).append(ext);

		} else {

			std::string base = "out/shiftscale/brainMri";
			std::string ext = ".png";
			path = base.append(std::to_string(i)).append(ext);
		}

		std::cout << "Current path set to " << path << "\n";

		writer->SetFileName(path);		

		writer->SetInput(rescaler->GetOutput());
		writer->Update();
		std::cout << "Another one\n";

		i++;

		//just to make sure
		if (i == 200){ break; }

	}

	return 0;

}



