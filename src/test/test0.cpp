#include "../image.h"
#include "../utils.h"

#include <string>

using namespace std;

/*
 * This is an example of a simple test: the image data/dots.png has been created with an image editor that we trust and
 * that allows for the exact coloring of single pixels. Most of the initial tests in this file are like this. The
 * comparison, using an external image, des not check exactness in number representation, but closeness,taking into
 * account numerical imprecision in the digital representation.
 *
 * Note 1: the test function uses "implementation" methods such as clamped_pixel, but it is named after the actual
 * purpose of the method (i.e. get a pixel value). This could be misleading and suggests that you could refactor the
 * image method to "hide" the clamp aspect into the function, or put it as an option selectable trough arguments.
 *
 * Note 2: the function tests clamped_pixel in a random fashion. This is useful, but it does not necessarily cover
 * corner cases. For example, is it testing the access to pixels with negative coordinates? This inputs are possible,
 * and they should have proper tests too.
 *
 * # TDD-HW0 #1 modify non-failing tests: some of the tests pass even when the tested function is not implemented yet.
 * This behaviour does not follow the basic principles of TDD. Modify the tests to make them more meaningful.
 * # TDD-HW0 #2 check invalid inputs. In particular this checks if the clamp functionality works effectively.
 * # TDD-HW0 #3: divide the test function in smaller homogeneous tests.
 */
void test_get_pixel()
  {
  Image im = load_image("data/dots.png");
  // Test within image
  TEST(within_eps(0, im.clamped_pixel(0,0,0)));
  TEST(within_eps(1, im.clamped_pixel(1,0,1)));
  TEST(within_eps(0, im.clamped_pixel(2,0,1)));

  // Test padding
  TEST(within_eps(1, im.clamped_pixel(0,3,1)));
  TEST(within_eps(1, im.clamped_pixel(7,8,0)));
  TEST(within_eps(0, im.clamped_pixel(7,8,1)));
  TEST(within_eps(1, im.clamped_pixel(7,8,2)));
  }


void test_set_pixel()
  {
  Image im = load_image("data/dots.png");
  Image d(4,2,3);
  
  d.set_pixel(0,0,0,0); d.set_pixel(0,0,1,0); d.set_pixel(0,0,2,0);
  d.set_pixel(1,0,0,1); d.set_pixel(1,0,1,1); d.set_pixel(1,0,2,1);
  d.set_pixel(2,0,0,1); d.set_pixel(2,0,1,0); d.set_pixel(2,0,2,0);
  d.set_pixel(3,0,0,1); d.set_pixel(3,0,1,1); d.set_pixel(3,0,2,0);
  
  d.set_pixel(0,1,0,0); d.set_pixel(0,1,1,1); d.set_pixel(0,1,2,0);
  d.set_pixel(1,1,0,0); d.set_pixel(1,1,1,1); d.set_pixel(1,1,2,1);
  d.set_pixel(2,1,0,0); d.set_pixel(2,1,1,0); d.set_pixel(2,1,2,1);
  d.set_pixel(3,1,0,1); d.set_pixel(3,1,1,0); d.set_pixel(3,1,2,1);
  
  // Test images are same
  TEST(same_image(im, d));
  }

/*
 * The following tests load an image from disk, perform some operation (applying the tested function),
 * then check if the transformed image is equal to a reference image. This approach is ok-ish, but, given the
 * possible floating point approximations that could happen in the process of saving and loading back an image, the
 * check is done using the function "within_eps". Another approach could be to generate an artificial image, directly
 * assigning the pixel values to each channel, perform the transformations, then check if the transformed image pixels
 * are coherent with the mathematical definition of the operation, without approximations.
 *
 * TDD_HW1 #1: add deterministic tests cases to the following test functions
 */

void test_grayscale()
  {
  Image im = load_image("data/colorbar.png");
  Image gray = rgb_to_grayscale(im);
  Image g = load_image("data/gray.png");
  TEST(same_image(gray, g));
  }

void test_copy()
  {
  Image im = load_image("data/dog.jpg");
  Image c = im;
  TEST(same_image(im, c));
  }

void test_shift()
  {
  Image im = load_image("data/dog.jpg");
  Image c = im;
  shift_image(c, 1, .1);
  TEST(within_eps(im.data[0], c.data[0]));
  TEST(within_eps(im.data[im.w*im.h+13] + .1,  c.data[im.w*im.h + 13]));
  TEST(within_eps(im.data[2*im.w*im.h+72],  c.data[2*im.w*im.h + 72]));
  TEST(within_eps(im.data[im.w*im.h+47] + .1,  c.data[im.w*im.h + 47]));
  }

void test_scale()
{
    Image im = load_image("data/dog.jpg");
    Image s = im;
    rgb_to_hsv(im);
    rgb_to_hsv(s);
    scale_image(s, 1, 2);
    int ch_size = im.w*im.h;
    TEST(within_eps(im.data[ch_size]*2, s.data[ch_size]));
    TEST(within_eps(im.data[ch_size+72]*2,  s.data[ch_size + 72]));
    TEST(within_eps(im.data[2*ch_size+72],  s.data[2*ch_size + 72]));
    TEST(within_eps(im.data[47],  s.data[47]));
}

void test_rgb_to_hsv()
  {
  Image im = load_image("data/dog.jpg");
  rgb_to_hsv(im);
  Image hsv = load_image("data/dog.hsv.png");
  TEST(same_image(im, hsv));
  }

void test_hsv_to_rgb()
  {
  Image im = load_image("data/dog.jpg");
  Image c = im;
  rgb_to_hsv(im);
  hsv_to_rgb(im);
  TEST(same_image(im, c));
  }

//void test_rgb2lch2rgb()
//  {
//  Image im = load_image("data/dog.jpg");
//  Image c = im;
//
//  rgb_to_lch(im);
//  lch_to_rgb(im);
//  TEST(same_image(im, c));
//  }


void run_tests()
  {
  test_get_pixel();
  test_set_pixel();
  test_copy();
  test_shift();
  test_scale();
  test_grayscale();
  test_rgb_to_hsv();
  test_hsv_to_rgb();
//  test_rgb2lch2rgb();
  printf("%d tests, %d passed, %d failed\n", tests_total, tests_total-tests_fail, tests_fail);
  }

int main(int argc, char **argv)
  {
  // Image manipulation for fun testing.
  
  Image im2 = load_image("data/dog.jpg");
  for (int i=0; i<im2.w; i++)
      for (int j=0; j<im2.h; j++)
          im2(i, j, 0) = 0;
  im2.save_image("output/pixel_modifying_output");
  
  // Running example tests
  
  run_tests();
  
  return 0;
  }
