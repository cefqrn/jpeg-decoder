#ifndef PARSING_H
#define PARSING_H

#include "jpeg.h"

#include <stddef.h>

typedef enum SegmentMarker {
    SOI  = 0xFFD8,  // Start of Image
    COM  = 0xFFFE,  // Comment
    APP0 = 0xFFE0,  // Application Marker (JFIF)
    SOF0 = 0xFFC0,  // Start of Frame (Baseline)
    DHT  = 0xFFC4,  // Define Huffman Table
    DQT  = 0xFFDB,  // Define Quantization Table
    SOS  = 0xFFDA,  // Start of Scan
} SegmentMarker;

void parse_APP0(jpeg_info *info, const unsigned char *data);
void parse_SOF0(jpeg_info *info, const unsigned char *data);
void parse_DHT(jpeg_info *info, const unsigned char *data, unsigned short length);
void parse_DQT(jpeg_info *info, const unsigned char *data, unsigned short length);
void parse_SOS(jpeg_info *info, const unsigned char *data);

#endif
