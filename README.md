# jpeg-decoder
A jpeg decoder originally made to solve a [Kattis problem](https://open.kattis.com/problems/coincounter).

The `test` program prints out the supplied image using spaces colored with [ANSI 24-bit color escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#24-bit).

The examples are my own images. [Krita](https://krita.org/en/) was used to scale and remove chroma subsampling from the `no_subsampling` examples. [FFmpeg](https://ffmpeg.org/) was used otherwise.

## Currently Supported
* Baseline DCT

## Usage
```bash
# setup
make

# print image
./bin/test <file>
```

## Resources
* [JPEG Huffman Coding Tutorial](https://www.impulseadventure.com/photo/jpeg-huffman-coding.html) \[[archive](https://web.archive.org/web/20211205035857/https://www.impulseadventure.com/photo/jpeg-huffman-coding.html)\] by [Calvin Hass (ImpulseAdventure)](https://www.impulseadventure.com) \[[archive](https://web.archive.org/web/20211202094539/https://www.impulseadventure.com)\] \[[github](https://github.com/ImpulseAdventure)\], 2009
* [Understanding and Decoding a JPEG Image using Python](https://yasoob.me/posts/understanding-and-writing-jpeg-decoder-in-python) \[[github](https://github.com/yasoob/Baseline-JPEG-Decoder)\] by [Yasoob Khalid](https://yasoob.me/) \[[github](https://github.com/yasoob)\], 2020
* Let\'s Write a Simple JPEG Library, [Part-I: The Basics](https://koushtav.me/jpeg/tutorial/2017/11/25/lets-write-a-simple-jpeg-library-part-1) \[[github](https://github.com/TheIllusionistMirage/simple-jpeg-decoder)\] and [Part-II: The Decoder](https://koushtav.me/jpeg/tutorial/c++/decoder/2019/03/02/lets-write-a-simple-jpeg-library-part-2) \[[github](https://github.com/TheIllusionistMirage/libKPEG)\], by [Koushtav Chakrabarty (TheIllusionistMirage)](https://koushtav.me/) \[[github](https://github.com/TheIllusionistMirage)\], 2017 and 2019 respectively
* [THE JPEG COMPRESSION and THE JPG FILE FORMAT](https://www.opennet.ru/docs/formats/jpeg.txt) by Cristy Cuturicu, 1999
* [JPEG](https://en.wikipedia.org/wiki/JPEG) and [JPEG File Interchange Format](https://en.wikipedia.org/wiki/JPEG_File_Interchange_Format) by Wikipedia contributors in [Wikipedia](https://en.wikipedia.org), The Free Encyclopedia, both 2022
* [JPEG File Interchange Format](https://www.w3.org/Graphics/JPEG/jfif3.pdf) by Eric Hamilton, 1992
* [Recommendation ITU-T T.81 | ISO/IEC 10918-1: Information technology - Digital compression and coding of continuous-tone still images - Requirements and guidelines](https://www.w3.org/Graphics/JPEG/itu-t81.pdf) by the International Telecommunication Union (ITU), the International Organization for Standardization (ISO), and the International Electrotechnical Commission (IEC), 1992
* [Recommendation ITU-T T.871 | ISO/IEC 10918-5: Information technology – Digital compression and coding of continuous-tone still images: JPEG File Interchange Format (JFIF)](https://www.itu.int/rec/T-REC-T.871) by the ITU, the ISO, and the IEC, 2011