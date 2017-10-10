#include <AuroraFW/Aurora.h>
#include <FreeImage.h>

using namespace AuroraFW;

int width = 800, height = 600, bpp = 24;

Application *app;

afwslot appMainFunction()
{
	// Initializes FreeImage
	FreeImage_Initialise();
	CLI::Log(CLI::Information, "FreeImage has successfully initialized.");
	CLI::Log(CLI::Information, "FreeImage's version: ", FreeImage_GetVersion());
	CLI::Log(CLI::Information, "FreeImage's Copyright message: ", FreeImage_GetCopyrightMessage());

	// Prepares to dray a PNG image
	FIBITMAP *image = FreeImage_Allocate(width, height, bpp);
	RGBQUAD color;

	if(image == nullptr)
		return;
	CLI::Log(CLI::Information, "Allocated space for new image");

	// Starts drawing the puke image

	for(int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			color.rgbRed = (float)j / width * 255.0f;
			color.rgbGreen = (float)abs(j-height) / width * 255.0f;
			color.rgbBlue = (float)i / width * 255.0f;
			FreeImage_SetPixelColor(image, i, j, &color);
		}
	}

	CLI::Log(CLI::Information, "The image was drawn. Saving it now to 'output.png'");

	if(FreeImage_Save(FIF_PNG, image, "output.png", 0))
		CLI::Log(CLI::Information, "Success! The image was saved!");
	else
		CLI::Log(CLI::Information, "Error! The image couldn't be saved!");
	
	FreeImage_DeInitialise();
	CLI::Log(CLI::Information, "FreeImage was deinitialised.");
	
	return;
}

int main()
{
	app = new Application(appMainFunction);
	delete app;

	return EXIT_SUCCESS;
}