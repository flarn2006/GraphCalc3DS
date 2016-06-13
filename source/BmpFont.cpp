/*
The MIT License (MIT)

Copyright (c) 2016 Michael Norton <flarn2006@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <cstdio>
#include "BmpFont.h"

BmpFont::BmpFont()
{
    texture = nullptr;
    unclip();
}

BmpFont::BmpFont(const char *filename)
    : BmpFont()
{
    load(filename);
}

BmpFont::~BmpFont()
{
    if (texture != nullptr) free();
}

bool BmpFont::load(const char *filename)
{
    if (texture != nullptr) free();
    
    std::FILE *fp = std::fopen(filename, "rb");
    
    if (fp) {
        u8 temp[2];
        std::fread(temp, 1, 2, fp);
        
        if (temp[0] != 0xBF || temp[1] != 0xF2) {
            std::fclose(fp);
            return false;
        }
        
        std::fread(&imgWidth, 4, 1, fp);
        std::fread(&imgHeight, 4, 1, fp);
        std::fread(&cellWidth, 4, 1, fp);
        std::fread(&cellHeight, 4, 1, fp);
        
        u8 bitCount;
        std::fread(&bitCount, 1, 1, fp);
        
        std::fread(&baseChar, 1, 1, fp);
        std::fread(charWidths, 1, 256, fp);
        
        std::size_t byteCount = 4 * imgWidth * imgHeight;
        u8 *data = new u8[byteCount];
        
        if (bitCount == 8) {
            u8 *ptr = data;
            for (std::size_t i=0; i<byteCount/4; ++i) {
                ptr[0] = ptr[1] = ptr[2] = 0xFF;
                std::fread(&ptr[3], 1, 1, fp);
                ptr += 4;
            }
        } else if (bitCount == 24) {
            u8 *ptr = data;
            for (std::size_t i=0; i<byteCount/4; ++i) {
                std::fread(ptr, 1, 3, fp);
                ptr[3] = 0xFF;
                ptr += 4;
            }
        } else if (bitCount == 32) {
            std::fread(data, 1, byteCount, fp);
        } else {
            std::fclose(fp);
            return false;
        }
        
        texture = sf2d_create_texture_mem_RGBA8(data, imgWidth, imgHeight, TEXFMT_RGBA8, SF2D_PLACE_RAM);
        delete[] data;
        
        std::fclose(fp);
        
        return true;
    } else {
        return false;
    }
}

u8 BmpFont::drawChar(char ch, int x, int y, u32 color) const
{
    u8 cols = imgWidth / cellWidth;
    u8 rows = imgHeight / cellHeight;
    unsigned char maxChar = std::min((int)baseChar + (cols * rows - 1), 255);
    unsigned char &uc = *(unsigned char *)&ch;
    
    if (baseChar <= uc && uc <= maxChar) {
        int tx = ((uc - baseChar) % cols) * cellWidth;
        int ty = ((uc - baseChar) / cols) * cellHeight;
        int width = cellWidth;
        int height = cellHeight;
        
        if (isClipped()) {
            if (x < clipLeft) {
                tx += (clipLeft - x);
                width -= (clipLeft - x);
                x = clipLeft;
            }
            if (y < clipTop) {
                ty += (clipTop - y);
                height -= (clipTop - y);
                y = clipTop;
            }
            if (x + width > clipRight)
                width = clipRight - x;
            if (y + height > clipBottom)
                height = clipBottom - y;
        }
        
        if (width > 0 && height > 0)
            sf2d_draw_texture_part_blend(texture, x, y, tx, ty, width, height, color);
        
        return charWidths[uc];
    } else {
        return 0;
    }
}

void BmpFont::splitToLines(const std::string &str, std::vector<std::string> &lines, int wrapWidth) const
{
    lines.clear();
    if (wrapWidth == 0) {
        // No wrapping
        std::string curLine = "";
        for (auto i = str.begin(); i != str.end(); ++i) {
            if (*i == '\n') {
                lines.push_back(curLine);
                curLine = "";
            } else {
                curLine += *i;
            }
        }
        lines.push_back(curLine);
    } else if (wrapWidth > 0) {
        // Word wrap
        std::vector<std::string> words;
        std::string curWord = "";
        u32 curX = 0;
        
        for (auto i = str.begin(); i != str.end(); ++i) {
            u8 curCharWidth = charWidths[*(unsigned char *)&*i];
            bool ignoreWhitespace = false;
            
            if (curX + curCharWidth > (unsigned)wrapWidth) {
                words.push_back(curWord);
                curWord = "";
                curX = 0;
                ignoreWhitespace = true;
            }
            
            if ((!ignoreWhitespace || *i != ' ') && *i != '\n') {
                curWord += *i;
                curX += curCharWidth;
            }
            
            if (*i == ' ' || *i == '\n' || *i == '-') {
                words.push_back(curWord);
                curWord = "";
                curX = 0;
                if (*i == '\n')
                    words.push_back("\n");
            }
        }
        
        if (curWord.size() > 0)
            words.push_back(curWord);
        
        std::string curLine = "";
        curX = 0;
        
        for (auto i = words.begin(); i != words.end(); ++i) {
            u32 curWidth = getLineWidth(*i);
            
            if (curX + curWidth > (unsigned)wrapWidth || (*i)[0] == '\n') {
                lines.push_back(curLine);
                curLine = "";
                curX = 0;
            }
            
            if ((*i)[0] != '\n') {
                curLine += *i;
                curX += curWidth;
            }
        }
        
        if (curLine.size() > 0)
            lines.push_back(curLine);
    } else {
        // Character wrap
        wrapWidth = -wrapWidth;
        std::string curLine;
        u32 curX = 0;
        
        for (auto i = str.begin(); i != str.end(); ++i) {
            unsigned char &uc = *(unsigned char *)&*i;
            if (*i == '\n' || curX + charWidths[uc] > (unsigned)wrapWidth) {
                lines.push_back(curLine);
                curLine = "";
                curX = 0;
            }
            if (*i != '\n') {
                curLine += *i;
                curX += charWidths[uc];
            }
        }
        
        lines.push_back(curLine);
    }
}

u32 BmpFont::drawStrInternal(const std::string &str, int x, int y, u32 color) const
{
    u32 curX = 0;
    u32 width = 0;
    
    if (alignment == ALIGN_RIGHT) {
        x -= getTextWidth(str);
    } else if (alignment == ALIGN_CENTER) {
        x -= getTextWidth(str) / 2;
    }
    
    for (auto i = str.begin(); i != str.end(); ++i) {
        curX += drawChar(*i, x + curX, y, color);
        if (curX > width) width = curX;
    }
    return width;
}

u32 BmpFont::drawStr(const std::string &str, int x, int y, u32 color) const
{
    std::vector<std::string> lines;
    splitToLines(str, lines, 0);
    
    u32 width = 0;
    for (std::size_t i=0; i<lines.size(); ++i) {
        u32 curWidth = drawStrInternal(lines[i], x, y+cellHeight*i, color);
        if (curWidth > width)
            width = curWidth;
    }
    
    return width;
}

u32 BmpFont::drawStrWrap(const std::string &str, int x, int y, int wrapWidth, u32 color) const
{
    std::vector<std::string> lines;
    splitToLines(str, lines, wrapWidth);
    
    for (std::size_t i=0; i<lines.size(); ++i) {
        drawStrInternal(lines[i], x, y+cellHeight*i, color);
    }
    
    return lines.size() * cellHeight;
}

u32 BmpFont::getLineWidth(const std::string &line) const
{
    u32 width = 0;
    for (auto i = line.begin(); i != line.end(); ++i) {
        width += charWidths[*(unsigned char *)&*i];
    }
    return width;
}

void BmpFont::getTextDims(const std::string &str, u32 &width, u32 &height, int wrapWidth) const
{
    std::vector<std::string> lines;
    splitToLines(str, lines, wrapWidth);
    
    width = 0;
    for (auto i = lines.begin(); i != lines.end(); ++i) {
        u32 curWidth = getLineWidth(*i);
        if (curWidth > width)
            width = curWidth;
    }
    
    height = lines.size() * cellHeight;
}

u32 BmpFont::getTextWidth(const std::string &str, int wrapWidth) const
{
    u32 width, height;
    getTextDims(str, width, height, wrapWidth);
    return width;
}

u32 BmpFont::getTextHeight(const std::string &str, int wrapWidth) const
{
    u32 width, height;
    getTextDims(str, width, height, wrapWidth);
    return height;
}

void BmpFont::free()
{
    sf2d_free_texture(texture);
    texture = nullptr;
}

BmpFont &BmpFont::align(TextAlignment alignment)
{
    this->alignment = alignment;
    return *this;
}

BmpFont &BmpFont::clip(int left, int top, int right, int bottom)
{
    if (left > right)
        std::swap(left, right);
    if (top > bottom)
        std::swap(top, bottom);
    
    clipLeft = left;
    clipTop = top;
    clipRight = right;
    clipBottom = bottom;
    
    return *this;
}

BmpFont &BmpFont::unclip()
{
    clipLeft = clipTop = clipRight = clipBottom = 0;
    return *this;
}

bool BmpFont::isClipped() const
{
    return (clipLeft != 0) || (clipTop != 0) || (clipRight != 0) || (clipBottom != 0);
}

BmpFont::operator bool() const
{
    return (texture != nullptr);
}

u32 BmpFont::height() const
{
    return cellHeight;
}
