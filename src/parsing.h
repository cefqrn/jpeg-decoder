#ifndef PARSING_H
#define PARSING_H

#include "jpeg.h"

typedef enum SegmentMarker {
    SOI  = 0xFFD8,  // Start of Image
    SOF0 = 0xFFC0,  // Start of Frame (Baseline)
    DHT  = 0xFFC4,  // Define Huffman Table
    DQT  = 0xFFDB,  // Define Quantization Table
    SOS  = 0xFFDA,  // Start of Scan
} SegmentMarker;

int parse_SOF0(jpeg_info *info, unsigned short length, const unsigned char *data);
int parse_DHT(jpeg_info *info, unsigned short length, const unsigned char *data);
int parse_DQT(jpeg_info *info, unsigned short length, const unsigned char *data);
int parse_SOS(scan_info *scanInfo, const jpeg_info *jpegInfo, unsigned short length, const unsigned char *data);

#endif
