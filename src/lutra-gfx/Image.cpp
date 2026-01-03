#include <efvk/Image.h>
#include <spng.h>
#include <assert.h>
#include <fstream>

namespace efvk
{
	static std::vector<char> load_png_data(const char* path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		assert(file.is_open());

		const int file_size = static_cast<int>(file.tellg());

		std::vector<char> data(file_size);
		file.seekg(0);
		file.read(reinterpret_cast<char*>(data.data()), file_size);

		return data;
	}

	void Image::Load(const char* filename)
	{
		std::vector<char> png_data = load_png_data(filename);

		spng_ctx* ctx = spng_ctx_new(0);
		spng_set_png_buffer(ctx, png_data.data(), png_data.size());

		spng_ihdr image_header{};
		spng_get_ihdr(ctx, &image_header);
		width = image_header.width;
		height = image_header.height;

		size_t png_decoded_size = 0;
		spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &png_decoded_size);

		data.resize(png_decoded_size);
		spng_decode_image(ctx, data.data(), png_decoded_size, SPNG_FMT_RGBA8, 0);

		spng_ctx_free(ctx);
	}
}