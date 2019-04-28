// PointGrey.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "PointGrey.h"

//CameraInfo camInfo;
//
//const Mode k_fmt7Mode = MODE_0;
//const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_MONO8;
//const int k_numImages = 10;
//
//Error error;
//
//BusManager busMgr;
//unsigned int numCameras;
//
//PGRGuid guid;
//Camera cam;
//
//Format7ImageSettings fmt7ImageSettings;
//Format7Info fmt7Info;
//bool supported;
//
//bool valid;
//Format7PacketInfo fmt7PacketInfo;
//Image rawImage;
//
//Property frmRate;




void PrintBuildInfo()
{
	FC2Version fc2Version;
	Utilities::GetLibraryVersion(&fc2Version);

	ostringstream version;
	version << "FlyCapture2 library version: " << fc2Version.major << "."
		<< fc2Version.minor << "." << fc2Version.type << "."
		<< fc2Version.build;
	cout << version.str() << endl;

	ostringstream timeStamp;
	timeStamp << "Application build date: " << __DATE__ << " " << __TIME__;
	cout << timeStamp.str() << endl << endl;
}

void PrintCameraInfo(CameraInfo *pCamInfo)
{
	cout << endl;
	cout << "*** CAMERA INFORMATION ***" << endl;
	cout << "Serial number - " << pCamInfo->serialNumber << endl;
	cout << "Camera model - " << pCamInfo->modelName << endl;
	cout << "Camera vendor - " << pCamInfo->vendorName << endl;
	cout << "Sensor - " << pCamInfo->sensorInfo << endl;
	cout << "Resolution - " << pCamInfo->sensorResolution << endl;
	cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
	cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl
		<< endl;
}

void PrintFormat7Capabilities(Format7Info fmt7Info)
{
	cout << "Max image pixels: (" << fmt7Info.maxWidth << ", "
		<< fmt7Info.maxHeight << ")" << endl;
	cout << "Image Unit size: (" << fmt7Info.imageHStepSize << ", "
		<< fmt7Info.imageVStepSize << ")" << endl;
	cout << "Offset Unit size: (" << fmt7Info.offsetHStepSize << ", "
		<< fmt7Info.offsetVStepSize << ")" << endl;
	cout << "Pixel format bitfield: 0x" << fmt7Info.pixelFormatBitField << endl;
}

void PrintError(Error error) { error.PrintErrorTrace(); }

Camera cam;

int PT_Init()
{

	CameraInfo camInfo;

	const Mode k_fmt7Mode = MODE_0;
	const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_MONO8;
	const int k_numImages = 10;

	Error error;

	BusManager busMgr;
	unsigned int numCameras;

	PGRGuid guid;


	Format7ImageSettings fmt7ImageSettings;
	Format7Info fmt7Info;
	bool supported;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;


	Property frmRate;




	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	cout << "Number of cameras detected: " << numCameras << endl;

	if (numCameras < 1)
	{
		cout << "Insufficient number of cameras... exiting" << endl;
		return -1;
	}

	error = busMgr.GetCameraFromIndex(0, &guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Connect to a camera

	error = cam.Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Get the camera information

	error = cam.GetCameraInfo(&camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	PrintCameraInfo(&camInfo);

	// Query for available Format 7 modes

	fmt7Info.mode = k_fmt7Mode;
	error = cam.GetFormat7Info(&fmt7Info, &supported);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	PrintFormat7Capabilities(fmt7Info);

	if ((k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0)
	{
		// Pixel format not supported!
		cout << "Pixel format is not supported" << endl;
		return -1;
	}


	fmt7ImageSettings.mode = k_fmt7Mode;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	fmt7ImageSettings.width = fmt7Info.maxWidth;
	fmt7ImageSettings.height = fmt7Info.maxHeight;
	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;



	// Validate the settings to make sure that they are valid
	error = cam.ValidateFormat7Settings(
		&fmt7ImageSettings, &valid, &fmt7PacketInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	if (!valid)
	{
		// Settings are not valid
		cout << "Format7 settings are not valid" << endl;
		return -1;
	}

	// Set the settings to the camera
	error = cam.SetFormat7Configuration(
		&fmt7ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Start capturing images
	error = cam.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Retrieve frame rate property

	frmRate.type = FRAME_RATE;
	error = cam.GetProperty(&frmRate);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

		cout << "Frame rate is " << fixed << setprecision(2) << frmRate.absValue
			<< " fps" << endl;

		cout << "Grabbing " << k_numImages << " images" << endl;

	return 0;

}
int PT_Grab(char* FileName)
{
	Error error;
	Image rawImage;


		// Retrieve an image
		error = cam.RetrieveBuffer(&rawImage);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			//continue;
			return -1;
		}

		//cout << ".";

		// Get the raw image dimensions
		PixelFormat pixFormat;
		unsigned int rows, cols, stride;
		rawImage.GetDimensions(&rows, &cols, &stride, &pixFormat);

		// Create a converted image
		Image convertedImage;

		// Convert the raw image
		error = rawImage.Convert(PIXEL_FORMAT_BGRU, &convertedImage);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}



		error = convertedImage.Save(FileName);

		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

	return 0;
}
int PT_Stop()
{
	Error error;
	// Stop capturing images
	error = cam.StopCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Disconnect the camera
	error = cam.Disconnect();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	cin.ignore();

	return 0;
}
