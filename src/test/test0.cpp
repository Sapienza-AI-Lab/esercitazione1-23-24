#include "../image.h"
#include "../utils.h"

#include <stdio.h>
#include <string>
#include <cstdint>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <utility>
#include <ranges>

using namespace std;

//TODO inserire un metodo di output formattato
union RGB_color {
    struct {
        std::float_t r, g, b, a;
    };
    std::float_t color[4];

    RGB_color &operator/=(float divisor) {
        this->r /= divisor;
        this->g /= divisor;
        this->b /= divisor;
        return *this;
    }

};

// classical stringstream implementation
//std::ostream& operator<<(std::ostream& os, const RGB_color& color)
//{
//    os << "RGB color: #" << hex << uppercase << (int)color.r << (int)color.g << (int)color.b << endl;
//    return os;
//}

// classical sprintf (should be faster than stream) implementation
std::ostream &operator<<(std::ostream &os, const RGB_color &color) {
//    os << "RGB color: #" << hex << uppercase << (int)color.r << (int)color.g << (int)color.b << endl;
    std::string out(100, '\0');
    sprintf(out.data(), "RGB color: #%02X%02X%02X\n", (int) color.r, (int) color.g, (int) color.b);
    return os << out;
}

// new c++20 implementation with <format>: should be faster and more readable
// TODO: with gcc < 13 is not supported

// this should be a FIXTURE
array<RGB_color, 8> setup_rgb_reference() {
    array<RGB_color, 8> test_color_pattern{{{0x00, 0x00, 0x00, 0xFF},  // black
                                            {0xFF, 0xFF, 0xFF, 0xFF},  // white
                                            {0xFF, 0x00, 0x00, 0xFF},  // red
                                            {0xFF, 0xFF, 0x00, 0xFF},  // yellow
                                            {0x00, 0xFF, 0x00, 0xFF},  // green
                                            {0x00, 0xFF, 0xFF, 0xFF},  // cyan
                                            {0x00, 0x00, 0xFF, 0xFF},  // blue
                                            {0xFF, 0x00, 0xFF, 0xFF}}}; // magenta
    for (auto color_it = test_color_pattern.begin(); color_it < test_color_pattern.end(); ++color_it) {
        *color_it /= 255;
    }
    // the following does not work, uses a temporary object
//    for(auto color : test_color_pattern) color /= 255;
    return test_color_pattern;
}

/*
 * This is an example of a simple test: the image data/dots.png has been created with an image editor that we trust and
 * that allows for the exact coloring of single pixels. Most of the initial tests in this file are like this. The
 * comparison, using an external image, des not check exactness in number representation, but closeness,taking into
 * accontut numerical imprecision in the digital representation.
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
 */
void test_get_pixel_valid() {
    Image im = load_image("data/dots.png");

    /* A systematic test */
    // Test within image:
    // some values used in this test
    int const TEST_SIZE = 8;
    int w = im.w;
    int h = im.h;
    int cs = im.w * im.h;
    auto test_color_pattern = setup_rgb_reference();

    // to avoid a mess of per pixel tests, collect the results in auxiliary variables
    bool test_passed = true;
    bool pixel_tests_results[3 * TEST_SIZE];
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            for (int c = 0; c < 3; ++c) {
                pixel_tests_results[y * w + x + c * cs] =
                        test_color_pattern[y * w + x].color[c] == im.clamped_pixel(x, y, c);
                test_passed &= pixel_tests_results[y * w + x + c * cs];
            }
        }
    }
    TEST(test_passed);
    if (!test_passed) {
        for (int i = 0; i < TEST_SIZE; ++i)
            cerr << "test_get_pixel test at (x,y)=(" << i % w << "," << i / w << ") is " << pixel_tests_results[i]
                 << endl;
    }
}

void test_get_pixel_invalid() {
    /*
        Old tests:
        TEST(within_eps(1, im.clamped_pixel(0, 3, 1)));
        TEST(within_eps(1, im.clamped_pixel(7, 8, 0)));
        TEST(within_eps(0, im.clamped_pixel(7, 8, 1)));
        TEST(within_eps(1, im.clamped_pixel(7, 8, 2)));
    */
    // Test padding
    Image im = load_image("data/dots.png");
    int w = im.w;
    int h = im.h;
    auto test_color_pattern = setup_rgb_reference();

    bool test_passed = true;
    bool pixel_tests_results[4];
    using Pnt = std::pair<int, int>;
    array values_request{Pnt{-1, -1}, Pnt{-1, h}, Pnt{w, -1}, Pnt{w, h}};
    array im_angles{Pnt{0, 0}, Pnt{0, h - 1}, Pnt{w - 1, 0}, Pnt{w - 1, h - 1}};
    test_passed = true;
    for (int i = 0; i < 4; ++i) {
        for (int c = 0; c < 3; ++c) {
            auto xr = values_request[i].first;
            auto yr = values_request[i].second;
            auto xt = im_angles[i].first;
            auto yt = im_angles[i].second;
            pixel_tests_results[i] = test_color_pattern[yt * w + xt].color[c] == im.clamped_pixel(xr, yr, c);
            test_passed &= pixel_tests_results[i];
        }
    }
    TEST(test_passed);
}

// This test is already systematic, since compares a given input image with
// one that is created with the tested function pixel by pixel
void test_set_pixel() {
    Image im = load_image("data/dots.png");
    Image d(4, 2, 3);

    d.set_pixel(0, 0, 0, 0);
    d.set_pixel(0, 0, 1, 0);
    d.set_pixel(0, 0, 2, 0);
    d.set_pixel(1, 0, 0, 1);
    d.set_pixel(1, 0, 1, 1);
    d.set_pixel(1, 0, 2, 1);
    d.set_pixel(2, 0, 0, 1);
    d.set_pixel(2, 0, 1, 0);
    d.set_pixel(2, 0, 2, 0);
    d.set_pixel(3, 0, 0, 1);
    d.set_pixel(3, 0, 1, 1);
    d.set_pixel(3, 0, 2, 0);

    d.set_pixel(0, 1, 0, 0);
    d.set_pixel(0, 1, 1, 1);
    d.set_pixel(0, 1, 2, 0);
    d.set_pixel(1, 1, 0, 0);
    d.set_pixel(1, 1, 1, 1);
    d.set_pixel(1, 1, 2, 1);
    d.set_pixel(2, 1, 0, 0);
    d.set_pixel(2, 1, 1, 0);
    d.set_pixel(2, 1, 2, 1);
    d.set_pixel(3, 1, 0, 1);
    d.set_pixel(3, 1, 1, 0);
    d.set_pixel(3, 1, 2, 1);

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

void test_grayscale() {
    Image im = load_image("data/colorbar.png");
    Image gray = rgb_to_grayscale(im);
    Image g = load_image("data/gray.png");
    TEST(same_image(gray, g));
}

void test_copy() {
    Image im = load_image("data/dog.jpg");
    Image c = im;
    TEST(same_image(im, c));
}

void test_shift() {
    Image im = load_image("data/dog.jpg");
    Image c = im;
    shift_image(c, 1, .1);
    TEST(within_eps(im.data[0], c.data[0]));
    TEST(within_eps(im.data[im.w * im.h + 13] + .1, c.data[im.w * im.h + 13]));
    TEST(within_eps(im.data[2 * im.w * im.h + 72], c.data[2 * im.w * im.h + 72]));
    TEST(within_eps(im.data[im.w * im.h + 47] + .1, c.data[im.w * im.h + 47]));
}

void test_scale() {
    Image im = load_image("data/dog.jpg");
    Image s = im;
    rgb_to_hsv(im);
    rgb_to_hsv(s);
    scale_image(s, 1, 2);
    int ch_size = im.w * im.h;
    TEST(within_eps(im.data[ch_size] * 2, s.data[ch_size]));
    TEST(within_eps(im.data[ch_size + 72] * 2, s.data[ch_size + 72]));
    TEST(within_eps(im.data[2 * ch_size + 72], s.data[2 * ch_size + 72]));
    TEST(within_eps(im.data[47], s.data[47]));
}

void test_rgb_to_hsv() {
    Image im = load_image("data/dog.jpg");
    rgb_to_hsv(im);
    Image hsv = load_image("data/dog.hsv.png");
    TEST(same_image(im, hsv));
}

void test_hsv_to_rgb() {
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


void run_tests() {
    test_get_pixel_valid();
    test_get_pixel_invalid();
//    test_set_pixel();
//    test_copy();
//    test_shift();
//    test_scale();
//    test_grayscale();
//    test_rgb_to_hsv();
//    test_hsv_to_rgb();
//  test_rgb2lch2rgb();
    printf("%d tests, %d passed, %d failed\n", tests_total, tests_total - tests_fail, tests_fail);
}

int main(int argc, char **argv) {
    // Running example tests

    run_tests();

    return 0;
}
