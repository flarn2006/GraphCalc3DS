/*
The MIT License (MIT)

Copyright (c) 2016 Michael Norton <flarn2006@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sf2d.h>

enum TextAlignment { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

class BmpFont
{
private:
    struct FontData
    {
        sf2d_texture *texture;
        u32 imgWidth, imgHeight;
        u32 cellWidth, cellHeight;
        unsigned char baseChar;
        u8 charWidths[256];
        ~FontData();
    };
    
    std::shared_ptr<FontData> data;
    TextAlignment alignment;
    int clipLeft, clipTop, clipRight, clipBottom;
	u32 tabWidth = 50;
    
    static constexpr u32 WHITE = RGBA8(0xFF, 0xFF, 0xFF, 0xFF);
    
    void splitToLines(const std::string &str, std::vector<std::string> &lines, int wrapWidth) const;
    u32 drawStrInternal(const std::string &str, int x, int y, u32 color) const;
    u32 getLineWidth(const std::string &line) const;
	u32 nextTabPos(u32 currentPos) const;

public:
    BmpFont();
    BmpFont(const char *filename);
    
    bool load(const char *filename);
    u8 drawChar(char ch, int x, int y, u32 color = WHITE) const;
    u32 drawStr(const std::string &str, int x, int y, u32 color = WHITE) const;
    u32 drawStrWrap(const std::string &str, int x, int y, int wrapWidth, u32 color = WHITE) const;
    void getTextDims(const std::string &str, u32 &width, u32 &height, int wrapWidth = 0) const;
    u32 getTextWidth(const std::string &str, int wrapWidth = 0) const;
    u32 getTextHeight(const std::string &str, int wrapWidth = 0) const;
    void free();
    
    BmpFont &align(TextAlignment alignment);
    BmpFont align(TextAlignment alignment) const;
    
    BmpFont &clip(int left, int top, int right, int bottom);
    BmpFont clip(int left, int top, int right, int bottom) const;
    BmpFont &unclip();
    BmpFont unclip() const;
    bool isClipped() const;

	BmpFont &setTabWidth(u32 width);
	BmpFont setTabWidth(u32 width) const;
	u32 getTabWidth() const;
    
    operator bool() const;
    u32 height() const;
};
