#include "bitstream.h"
#include "decoding.h"
#include "hufftree.h"
#include "parsing.h"
#include "macros.h"
#include "image.h"
#include "jpeg.h"

#include <stddef.h>
#include <stdlib.h>

#define CHAR_WIDTH  8
#define MARKER_SIZE 2

#define READ_WORD(fp) getc(fp) << CHAR_WIDTH | getc(fp)

/*
    Read the data given by the jpeg segments into info.
    jpeg_free must be called to free the memory allocated in info.
*/
void jpeg_read_info(jpeg_info *info, FILE *fp) {
    {  // Read the file until the start of the image is reached.
        unsigned short word = READ_WORD(fp);
        while (word != SOI && !feof(fp) && !ferror(fp)) {
            word = (word << CHAR_WIDTH) + getc(fp);
        }

        CHECK_FAIL(word != SOI, "Start of image never reached.");
    }

    SegmentMarker marker;
    do {
        marker = READ_WORD(fp);
        size_t length = READ_WORD(fp) - MARKER_SIZE;

        unsigned char *data = malloc(length * sizeof *data);
        CHECK_ALLOC(data, "data");

        CHECK_FAIL(fread(data, sizeof *data, length, fp) != length, "Couldn't read image segment.");

        switch (marker) {
            case APP0: parse_APP0(info, data);        break;
            case SOF0: parse_SOF0(info, data);        break;
            case DHT:  parse_DHT(info, data, length); break;
            case DQT:  parse_DQT(info, data, length); break;
            case SOS:  parse_SOS(info, data);         break;
            default:;  // All other markers are ignored.
        }

        free(data);
    } while (marker != SOS);
}

/*
    Read the pixel values of the image into img.
*/
void jpeg_read_image(pixel *img, jpeg_info *info, FILE *fp) {
    bitstream stream = bitstream_create(fp);

    int dcCoeffs[3] = {0};

    // Note: this assumes both Cb and Cr have the same sampling factors.
    size_t hSamplingFactor = 1;
    size_t vSamplingFactor = 1;
    if (info->componentCount == 3) {
        hSamplingFactor = info->componentData[0].hSamplingFactor / info->componentData[1].hSamplingFactor;
        vSamplingFactor = info->componentData[0].vSamplingFactor / info->componentData[1].vSamplingFactor;
    }
    
    for (int y=0; y < info->height; y += 8 * vSamplingFactor) {
        for (int x=0; x < info->width; x += 8 * hSamplingFactor) {
            for (size_t v=0; v < vSamplingFactor; ++v) {
                for (size_t h=0; h < hSamplingFactor; ++h) {
                    dcCoeffs[0] = decode_MCU(img, info, &stream, info->componentData[0], dcCoeffs[0], x + 8*h, y + 8*v, 1, 1);
                }
            }

            if (info->componentCount == 3) {
                dcCoeffs[1] = decode_MCU(img, info, &stream, info->componentData[1], dcCoeffs[1], x, y, hSamplingFactor, vSamplingFactor);
                dcCoeffs[2] = decode_MCU(img, info, &stream, info->componentData[2], dcCoeffs[2], x, y, hSamplingFactor, vSamplingFactor);
            }
        }
    }
}

/*
    Free the memory taken by info.
*/
void jpeg_free(jpeg_info *info) {
    for (size_t i=0; i < 2; ++i) {
        for (size_t j=0; j < 2; ++j) {
            hufftree_destroy(&info->huffTrees[i][j]);
        }
    }
}
