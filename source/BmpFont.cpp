/*
The MIT License (MIT)

Copyright (c) 2016 Michael Norton <flarn2006@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <cstdio>
#include <sstream>
#include "BmpFont.h"

BmpFont::BmpFont()
{
    unclip();
}

BmpFont::BmpFont(const char *filename)
    : BmpFont()
{
    load(filename);
}

BmpFont::FontData::~FontData()
{
    sf2d_free_texture(texture);
}

bool BmpFont::load(const char *filename)
{
    data.reset(new FontData());
    
    std::FILE *fp = std::fopen(filename, "rb");

    if (fp) {
        u8 temp[2];
        std::fread(temp, 1, 2, fp);

        if (temp[0] != 0xBF || temp[1] != 0xF2) {
            std::fclose(fp);
            data.reset();
            return false;
        }

        std::fread(&data->imgWidth, 4, 1, fp);
        std::fread(&data->imgHeight, 4, 1, fp);
        std::fread(&data->cellWidth, 4, 1, fp);
        std::fread(&data->cellHeight, 4, 1, fp);

        u8 bitCount;
        std::fread(&bitCount, 1, 1, fp);

        std::fread(&data->baseChar, 1, 1, fp);
        std::fread(data->charWidths, 1, 256, fp);

        std::size_t byteCount = 4 * data->imgWidth * data->imgHeight;
        u8 *texdata = new u8[byteCount];

        if (bitCount == 8) {
            u8 *ptr = texdata;
            for (std::size_t i=0; i<byteCount/4; ++i) {
                ptr[0] = ptr[1] = ptr[2] = 0xFF;
                std::fread(&ptr[3], 1, 1, fp);
                ptr += 4;
            }
        } else if (bitCount == 24) {
            u8 *ptr = texdata;
            for (std::size_t i=0; i<byteCount/4; ++i) {
                std::fread(ptr, 1, 3, fp);
                ptr[3] = 0xFF;
                ptr += 4;
            }
        } else if (bitCount == 32) {
            std::fread(texdata, 1, byteCount, fp);
        } else {
            std::fclose(fp);
            data.reset();
            return false;
        }

        data->texture = sf2d_create_texture_mem_RGBA8(texdata, data->imgWidth, data->imgHeight, TEXFMT_RGBA8, SF2D_PLACE_RAM);
        delete[] texdata;

        std::fclose(fp);

        return true;
    } else {
        data.reset();
        return false;
    }
}

u8 BmpFont::drawChar(char ch, int x, int y, u32 color) const
{
    u8 cols = data->imgWidth / data->cellWidth;
    u8 rows = data->imgHeight / data->cellHeight;
    unsigned char maxChar = std::min((int)data->baseChar + (cols * rows - 1), 255);
    unsigned char uc = ch;

    if (data->baseChar <= uc && uc <= maxChar) {
        int tx = ((uc - data->baseChar) % cols) * data->cellWidth;
        int ty = ((uc - data->baseChar) / cols) * data->cellHeight;
        int width = data->cellWidth;
        int height = data->cellHeight;

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
            sf2d_draw_texture_part_blend(data->texture, x, y, tx, ty, width, height, color);

        return data->charWidths[uc];
    } else {
        return 0;
    }
}

void BmpFont::splitToLines(const std::string &str, std::vector<std::string> &lines, int wrapWidth) const
{
    lines.clear();
    if (wrapWidth == 0) {
        // No wrapping
        std::stringstream curLine;
        for (const auto &ch : str) {
            if (ch == '\n') {
                lines.push_back(curLine.str());
                curLine.str("");
            } else {
                curLine << ch;
            }
        }
        lines.push_back(curLine.str());
    } else if (wrapWidth > 0) {
        // Word wrap
        std::vector<std::string> words;
        std::stringstream curWord;
        u32 curX = 0;

        for (const auto &ch : str) {
            u8 curCharWidth = data->charWidths[(unsigned char)ch];
            bool ignoreWhitespace = false;

            if (curX + curCharWidth > (unsigned)wrapWidth) {
                words.push_back(curWord.str());
                curWord.str("");
                curX = 0;
                ignoreWhitespace = true;
            }

            if ((!ignoreWhitespace || (ch != ' ' && ch != '\t')) && ch != '\n') {
                curWord << ch;
                curX += curCharWidth;
            }

            if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '-') {
                words.push_back(curWord.str());
                curWord.str("");
                curX = 0;
                if (ch == '\n')
                    words.push_back("\n");
            }
        }

        std::string lastWord = curWord.str();
        if (!lastWord.empty())
            words.push_back(lastWord);

        std::stringstream curLine;
        curX = 0;

        for (const auto &word : words) {
            u32 curWidth = getLineWidth(word);

            if (curX + curWidth > (unsigned)wrapWidth || word[0] == '\n') {
                lines.push_back(curLine.str());
                curLine.str("");
                curX = 0;
            }

            if (word[0] != '\n') {
                curLine << word;
                curX += curWidth;
            }
        }

        std::string lastLine = curLine.str();
        if (!lastLine.empty())
            lines.push_back(lastLine);
    } else {
        // Character wrap
        wrapWidth = -wrapWidth;
        std::stringstream curLine;
        u32 curX = 0;

        for (const auto &ch : str) {
            unsigned char uc = ch;
            if (ch == '\n' || curX + data->charWidths[uc] > (unsigned)wrapWidth) {
                lines.push_back(curLine.str());
                curLine.str("");
                curX = 0;
            }
            if (ch != '\n') {
                curLine << ch;
                curX += data->charWidths[uc];
            }
        }

        lines.push_back(curLine.str());
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

    for (const auto &ch : str) {
		if (ch == '\t')
			curX = nextTabPos(curX);
		else
			curX += drawChar(ch, x + curX, y, color);
        if (curX > width) width = curX;
    }
    return width;
}

u32 BmpFont::nextTabPos(u32 currentPos) const
{
	u32 newPos = currentPos;
	if (newPos % tabWidth == 0)
		newPos += tabWidth;
	else
		// Move ahead to the next multiple of tabWidth
		newPos += tabWidth - newPos % tabWidth;
	return newPos;
}

u32 BmpFont::drawStr(const std::string &str, int x, int y, u32 color) const
{
    std::vector<std::string> lines;
    splitToLines(str, lines, 0);

    u32 width = 0;
    for (std::size_t i=0; i<lines.size(); ++i) {
        u32 curWidth = drawStrInternal(lines[i], x, y+data->cellHeight*i, color);
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
        drawStrInternal(lines[i], x, y+data->cellHeight*i, color);
    }

    return lines.size() * data->cellHeight;
}

u32 BmpFont::getLineWidth(const std::string &line) const
{
    u32 width = 0;
    for (const auto &ch : line) {
		if (ch == '\t')
			width = nextTabPos(width);
		else
			width += data->charWidths[(unsigned char)ch];
    }
    return width;
}

void BmpFont::getTextDims(const std::string &str, u32 &width, u32 &height, int wrapWidth) const
{
    std::vector<std::string> lines;
    splitToLines(str, lines, wrapWidth);

    width = 0;
    for (const auto &line : lines) {
        u32 curWidth = getLineWidth(line);
        if (curWidth > width)
            width = curWidth;
    }

    height = lines.size() * data->cellHeight;
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
    data.reset();
}

BmpFont &BmpFont::align(TextAlignment alignment)
{
    this->alignment = alignment;
    return *this;
}

BmpFont BmpFont::align(TextAlignment alignment) const
{
    BmpFont bf = *this;
    bf.align(alignment);
    return bf;
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

BmpFont BmpFont::clip(int left, int top, int right, int bottom) const
{
    BmpFont bf = *this;
    bf.clip(left, top, right, bottom);
    return bf;
}

BmpFont &BmpFont::unclip()
{
    clipLeft = clipTop = clipRight = clipBottom = 0;
    return *this;
}

BmpFont BmpFont::unclip() const
{
    BmpFont bf = *this;
    bf.unclip();
    return bf;
}

bool BmpFont::isClipped() const
{
    return (clipLeft != 0) || (clipTop != 0) || (clipRight != 0) || (clipBottom != 0);
}

BmpFont &BmpFont::setTabWidth(u32 width)
{
	tabWidth = width;
	return *this;
}

BmpFont BmpFont::setTabWidth(u32 width) const
{
	BmpFont bf = *this;
	bf.setTabWidth(width);
	return bf;
}

u32 BmpFont::getTabWidth() const
{
	return tabWidth;
}

BmpFont::operator bool() const
{
    return (bool)data;
}

u32 BmpFont::height() const
{
    return data->cellHeight;
}
