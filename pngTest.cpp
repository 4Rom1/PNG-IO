
/*
 * A simple libpng example program taken from
 * http://zarb.org/~gc/html/libpng.html
 * https://gist.github.com/niw/5963798
 * Modified by Romain Garnier to read and write from/to
 * different intput/outputs of a png file.
 * Install libpng :
 * https://programmersought.com/article/94135545706/
 * or simply do
 * sudo apt-get install libpng libpng-dev
 * compile with
 * g++ pngTest.cpp `libpng-config --ldflags` -o pngTest.out
 * run with 
 * ./pngTest.out in.png out.png
 */

#include <array>
#include <cstdint>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

// Png image container
class PngImage {
public:
  PngImage(size_t width, size_t height) : width(width), height(height) {

    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
      // 4 channels RGBA by default
      row_pointers[y] = (png_byte *)malloc(width * 4);
    }
  }

  PngImage(char *filename) {

    FILE *fp = fopen(filename, "rb");

    png_structp png =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
      abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
      abort();

    if (setjmp(png_jmpbuf(png)))
      abort();

    png_init_io(png, fp);

    png_read_info(png, info);

    png_byte color_type;
    png_byte bit_depth;

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if (bit_depth == 16)
      png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
    for (int y = 0; y < height; y++) {
      row_pointers[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
    }

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);
  }

  size_t Width() { return width; }

  size_t Height() { return height; }

  png_bytep *Data() { return row_pointers; }

  array<uint8_t, 4> at(const size_t row, const size_t col) {

    for (size_t c = 0; c < 4; ++c) {
      pixel[c] = row_pointers[row][col * 4 + c];
    }

    return pixel;
  }

  void write(const size_t row, const size_t col, array<uint8_t, 4> &px) {

    for (size_t c = 0; c < 4; ++c) {
      row_pointers[row][col * 4 + c] = px[c];
    }
  }

  void write_png_file(char *filename) {
    int y;

    FILE *fp = fopen(filename, "wb");
    if (!fp)
      abort();

    png_structp png =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
      abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
      abort();

    if (setjmp(png_jmpbuf(png)))
      abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    // png_set_filler(png, 0, PNG_FILLER_AFTER);

    if (!row_pointers)
      abort();

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
  }

  // Dtor
  ~PngImage() {
    for (int y = 0; y < height; y++) {
      free(row_pointers[y]);
    }
    free(row_pointers);
  }

private:
  png_bytep *row_pointers;
  array<uint8_t, 4> pixel;
  size_t width;
  size_t height;
};

int main(int argc, char *argv[]) {
  if (argc != 3)
    abort();

  PngImage pngIn = PngImage(argv[1]);
  PngImage pngOut = PngImage(pngIn.Width(), pngIn.Height());

  for (int y = 0; y < pngIn.Height(); y++) {
    for (int x = 0; x < pngIn.Width(); x++) {
      auto px = pngIn.at(y, x);
      // printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1],
      // px[2], px[3]);
      // Modify each R value to 1
      px[0] = 1;
      // Write to output
      pngOut.write(y, x, px);
    }
  }

  pngOut.write_png_file(argv[2]);

  return 0;
}
