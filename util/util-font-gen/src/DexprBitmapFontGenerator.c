#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


const char* const FontPath = "font.ttf";
const char* const OutputPath = "font256.h";

const unsigned FontSize = 16;
const unsigned FontImageWidth = 256;
const unsigned FontImageHeight = 256;

#define NUM_PRINTABLE_ASCII_CHARACTERS 95


typedef struct FontCharacterData
{
    unsigned imageOffsetX;
    unsigned imageOffsetY;
    unsigned width;
    unsigned height;

    int bitmapLeft;
    int bitmapTop;
    int advanceX;
} FontCharacterData;

typedef struct FontMetricsData
{
    FontCharacterData characters[NUM_PRINTABLE_ASCII_CHARACTERS];
    unsigned lineHeight;
} FontMetricsData;


int loadFontASCII(unsigned characterSize,
                  unsigned char* pImage,
                  unsigned imageWidth,
                  unsigned imageHeight,
                  FontMetricsData* pOutFontMetricsData)
{
    FT_Library freetypeLib;
    if (FT_Init_FreeType(&freetypeLib)){
        fprintf(stderr, "Error: could not initialize freetype library\n");
        return 1;
    }

    FT_Face face;
    if (FT_New_Face(freetypeLib, FontPath, 0, &face)){
        FT_Done_FreeType(freetypeLib);

        fprintf(stderr, "Error: the font must be located in the current working directory and named font.ttf\n");
        return 1;
    }

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    FT_Set_Pixel_Sizes(face, 0, characterSize);


    unsigned currentCopyX = 0;
    unsigned currentCopyY = 0;
    unsigned rowHeight = 0;

    for (unsigned iCharacter = 32; iCharacter <= 126; ++iCharacter)
    {
        if (FT_Load_Char(face, iCharacter, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Error: couldn't load an ASCII character: %x\n", iCharacter);
            continue;
        }

        FT_Bitmap bitmap = face->glyph->bitmap;

        if (bitmap.width > imageWidth || bitmap.rows > imageHeight)
        {
            fprintf(stderr, "Error: invalid size of font character: %x\n", iCharacter);
            continue;
        }

        if (currentCopyX > imageWidth - currentCopyX)
        {
            currentCopyX = 0;
            currentCopyY += rowHeight;
            rowHeight = 0;
        }

        if (bitmap.rows > imageHeight - currentCopyY)
        {
            fprintf(stderr, "Error: not enough space in the bitmap for the character: %x\n", iCharacter);
            continue;
        }

        for (unsigned h = 0; h < bitmap.rows; ++h)
        {
            const unsigned charY = currentCopyY + h;

            memcpy(pImage + imageWidth * charY + currentCopyX,
                   bitmap.buffer + bitmap.pitch * h,
                   bitmap.width);
        }

        FontCharacterData* pFontChar = &pOutFontMetricsData->characters[iCharacter - 32];
        pFontChar->imageOffsetX = currentCopyX;
        pFontChar->imageOffsetY = currentCopyY;
        pFontChar->width = bitmap.width;
        pFontChar->height = bitmap.rows;
        pFontChar->bitmapLeft = face->glyph->bitmap_left;
        pFontChar->bitmapTop = face->glyph->bitmap_top;
        pFontChar->advanceX = (int)(face->glyph->advance.x >> 6);

        rowHeight = bitmap.rows > rowHeight ? bitmap.rows : rowHeight;
        currentCopyX += bitmap.width;
    }

    if (FT_IS_SCALABLE(face))
    {
        pOutFontMetricsData->lineHeight = face->height / (face->units_per_EM / characterSize);
    }
    else
    {
        pOutFontMetricsData->lineHeight = FontSize;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(freetypeLib);

    return 0;
}

int saveFontDataToCFile(const unsigned char* pImage,
                        unsigned imageWidth,
                        unsigned imageHeight,
                        const FontMetricsData* pMetricsData)
{
    FILE* file = fopen(OutputPath, "wb");
    if (file == NULL)
        return 1;


    const char imageHeader[] = "#include \"DexprOS/Drivers/Graphics/GraphicsDriver.h\"\n\n"
                               "static const unsigned char DexprOS_FontImageData[] = {\n";
    const char rgbaPixelHeader[] = "0xFF, 0xFF, 0xFF, 0x";
    const char imageEnding[] = "};\n";

    const char metricsStructDefs[] = "typedef struct DexprOS_FontCharacterData\n"
                                     "{\n"
                                     "    unsigned imageOffsetX;\n"
                                     "    unsigned imageOffsetY;\n"
                                     "    unsigned width;\n"
                                     "    unsigned height;\n"
                                     "\n"
                                     "    int bitmapLeft;\n"
                                     "    int bitmapTop;\n"
                                     "    int advanceX;\n"
                                     "} DexprOS_FontCharacterData;\n"
                                     "\n"
                                     "typedef struct DexprOS_FontMetricsData\n"
                                     "{\n"
                                     "    DexprOS_FontCharacterData characters[95];\n"
                                     "    unsigned lineHeight;\n"
                                     "} DexprOS_FontMetricsData;\n\n";

    const char metricsHeader[] = "static const DexprOS_FontMetricsData g_DexprOS_FontMetrics = {\n"
                                 "    .characters = {\n";

    fwrite(imageHeader, sizeof(imageHeader) - 1, 1, file);

    for (unsigned h = 0; h < imageHeight; ++h)
    {
        for (unsigned w = 0; w < imageWidth; ++w)
        {
            int byteValue = pImage[imageWidth * h + w] & 0xFF;

            fwrite(rgbaPixelHeader, sizeof(rgbaPixelHeader) - 1, 1, file);
            fprintf(file, "%x", byteValue);
            fwrite(", ", 2, 1, file);
        }
        fwrite("\n", 1, 1, file);
    }

    fwrite(imageEnding, sizeof(imageEnding) - 1, 1, file);


    fprintf(file, "\nstatic const unsigned DexprOS_FontImageWidth = %u;\n", FontImageWidth);
    fprintf(file, "static const unsigned DexprOS_FontImageHeight = %u;\n", FontImageHeight);
    fprintf(file, "static const unsigned DexprOS_FontImageRowStride = %u;\n", FontImageWidth * 4);
    fprintf(file, "static const DexprOS_GrImageFormat DexprOS_FontImageFormat = DEXPROS_GR_IMAGE_FORMAT_RGBA8;\n\n");


    fwrite(metricsStructDefs, sizeof(metricsStructDefs) - 1, 1, file);
    fwrite(metricsHeader, sizeof(metricsHeader) - 1, 1, file);

    for (unsigned iCharacter = 0; iCharacter < NUM_PRINTABLE_ASCII_CHARACTERS; ++iCharacter)
    {
        const FontCharacterData pFontChar = pMetricsData->characters[iCharacter];
        fwrite("        {\n", 10, 1, file);
        fprintf(file, "            .imageOffsetX = %u,\n", pFontChar.imageOffsetX);
        fprintf(file, "            .imageOffsetY = %u,\n", pFontChar.imageOffsetY);
        fprintf(file, "            .width = %u,\n", pFontChar.width);
        fprintf(file, "            .height = %u,\n", pFontChar.height);

        fprintf(file, "            .bitmapLeft = %i,\n", pFontChar.bitmapLeft);
        fprintf(file, "            .bitmapTop = %i,\n", pFontChar.bitmapTop);
        fprintf(file, "            .advanceX = %i\n", pFontChar.advanceX);

        if (iCharacter + 1 < NUM_PRINTABLE_ASCII_CHARACTERS)
            fwrite("        },\n", 11, 1, file);
        else
            fwrite("        }\n", 10, 1, file);
    }

    fwrite("    },\n", 7, 1, file);
    fprintf(file, "    .lineHeight = %u\n", pMetricsData->lineHeight);
    fwrite("};\n", 3, 1, file);


    fclose(file);

    return 0;
}


int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;


    unsigned char* pBitmapMemory = (unsigned char*)malloc(FontImageWidth * FontImageHeight);
    if (pBitmapMemory == NULL)
    {
        fprintf(stderr, "Error: memory allocation error\n");
        return EXIT_FAILURE;
    }

    FontMetricsData fontMetrics = {0};

    if (loadFontASCII(FontSize, pBitmapMemory, FontImageWidth, FontImageHeight, &fontMetrics) != 0)
    {
        free(pBitmapMemory);
        return EXIT_FAILURE;
    }


    if (saveFontDataToCFile(pBitmapMemory, FontImageWidth, FontImageHeight, &fontMetrics) != 0)
    {
        free(pBitmapMemory);
        return EXIT_FAILURE;
    }


    free(pBitmapMemory);
}
