#include <vector>

namespace T3E
{
    /**
    *
    * picoPNG version 20101224
    * Copyright (c) 2005-2010 Lode Vandevenne
    *
    * This software is provided 'as-is', without any express or implied
    * warranty. In no event will the authors be held liable for any damages
    * arising from the use of this software.
    *
    * Permission is granted to anyone to use this software for any purpose,
    * including commercial applications, and to alter it and redistribute it
    * freely, subject to the following restrictions:
    *
    *     1. The origin of this software must not be misrepresented; you must not
    *     claim that you wrote the original software. If you use this software
    *     in a product, an acknowledgment in the product documentation would be
    *     appreciated but is not required.
    *     2. Altered source versions must be plainly marked as such, and must not be
    *     misrepresented as being the original software.
    *     3. This notice may not be removed or altered from any source distribution.
    *
    * picoPNG is a PNG decoder in one C++ function of around 500 lines. Use picoPNG for
    * programs that need only 1 .cpp file. Since it's a single function, it's very limited,
    * it can convert a PNG to raw pixel data either converted to 32-bit RGBA color or
    * with no color conversion at all. For anything more complex, another tiny library
    * is available: LodePNG (lodepng.c(pp)), which is a single source and header file.
    * Apologies for the compact code style, it's to make this tiny.
    *
    *
	*decodePNG: The picoPNG function, decodes a PNG file buffer in memory, into a raw pixel buffer.
	*out_image: output parameter, this will contain the raw pixels after decoding.
	*By default the output is 32-bit RGBA color.
	*The std::vector is automatically resized to the correct size.
	*image_width: output_parameter, this will contain the width of the image in pixels.
	*image_height: output_parameter, this will contain the height of the image in pixels.
	*in_png: pointer to the buffer of the PNG file in memory. To get it from a file on
	*disk, load it and store it in a memory buffer yourself first.
	*in_size: size of the input PNG file in bytes.
	*convert_to_rgba32: optional parameter, true by default.
	*Set to true to get the output in RGBA 32-bit (8 bit per channel) color format
	*no matter what color type the original PNG image had. This gives predictable,
	*useable data from any random input PNG.
	*Set to false to do no color conversion at all. The result then has the same data
	*type as the PNG image, which can range from 1 bit to 64 bits per pixel.
	*Information about the color type or palette colors are not provided. You need
	*to know this information yourself to be able to use the data so this only
	*works for trusted PNG files. Use LodePNG instead of picoPNG if you need this information.
	*return: 0 if success, not 0 if some error occured.
	*/
	extern int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height,
		const unsigned char* in_png, size_t in_size, bool convert_to_rgba32 = true);
}
