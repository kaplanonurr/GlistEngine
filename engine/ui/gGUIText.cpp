/*
 * gGUIText.cpp
 *
 *  Created on: Sep 1, 2021
 *      Author: noyan
 */

#include "gGUIText.h"


gGUIText::gGUIText() {
    textalignment = TEXTALIGNMENT_LEFT;
    text = "";
    fontsize = font->getSize();
    lineh = 14 * fontsize / 10;
    width = 0;
    linenum = 0;
}

gGUIText::~gGUIText() {
}

void gGUIText::set(gBaseApp* root, gBaseGUIObject* topParentGUIObject, gBaseGUIObject* parentGUIObject, int parentSlotLineNo, int parentSlotColumnNo, int x, int y, int w, int h) {
    gGUIControl::set(root, topParentGUIObject, parentGUIObject, parentSlotLineNo, parentSlotColumnNo, x, y, w, h);
    resetText();
}

void gGUIText::setText(std::string text) {
    this->text = text;
    resetText();
}

void gGUIText::setTextAlignment(int textAligment) {
    textalignment = textAligment;
    resetAlignment();
}

void gGUIText::draw() {
    gColor oldcolor = *renderer->getColor();
    renderer->setColor(fontcolor);
    for (int i = 0; i < linenum; i++) font->drawText(line[i], left + linefirstx[i], top + fontsize + (i * lineh));
    renderer->setColor(&oldcolor);
}

void gGUIText::resetText() {
    if(width <= 0) return;
    linenum = 0; // Reset the number of lines to zero.
	line.clear(); // Clear the content of the line vector.
	std::vector<std::string> lines = splitString(text, font, width); // Split the text into lines based on the specified width.
	linenum = lines.size(); // Update the number of lines.
	line = lines; // Copy the split lines to the line vector.
    resetAlignment();
}

void gGUIText::resetAlignment() {
    linefirstx.clear();
    int spacewidth = font->getStringWidth(" ");
    for (int i = 0; i < linenum; i++) {
        int linetextwidth = font->getStringWidth(line[i]);
        int lineindent = 0;

        if (textalignment == TEXTALIGNMENT_CENTER) {
            lineindent = (width - linetextwidth) / 2;
        } else if (textalignment == TEXTALIGNMENT_RIGHT) {
            lineindent = std::max(width - linetextwidth, 0);
        } else if (textalignment == TEXTALIGNMENT_JUSTIFY) {
        	 int numspaces = std::count(line[i].begin(), line[i].end(), ' ');
				if (numspaces > 0) {
					int totalspacing = width - linetextwidth;
				    int extraspacing = (totalspacing % spacewidth) - font->getStringWidth(" ");
					totalspacing /= spacewidth;
					int spacing = totalspacing / numspaces * 2;
					if(spacing == 0) spacing = 1;
					std::string justifiedline;
					size_t pos = 0;
					size_t lastpos = 0;
					while ((pos = line[i].find(' ', pos)) != std::string::npos) {
						justifiedline += line[i].substr(lastpos, pos - lastpos);
						int spacestoadd = spacing;
						justifiedline.append(spacestoadd, ' ');
						if(extraspacing > 0){
							justifiedline.append(extraspacing, ' ');
							extraspacing--;
						}
						lastpos = ++pos;
					}
					justifiedline += line[i].substr(lastpos);
				    line[i] = justifiedline;
				}
	    }
        linefirstx.push_back(lineindent);
    }
    if (textalignment == TEXTALIGNMENT_RIGHT) {
        int margin = 5;
        for (int i = 0; i < linenum; i++) {
            int linetextwidth = font->getStringWidth(line[i]);
            linefirstx[i] -= margin;
        }
    }
    if (textalignment == TEXTALIGNMENT_JUSTIFY) {
            int margin = 3;
            for (int i = 0; i < linenum; i++) {
                int linetextwidth = font->getStringWidth(line[i]);
                linefirstx[i] -= margin;
            }
        }
}

std::vector<std::string> gGUIText::splitString(const std::string& textToSplit, gFont* font, int lineWidth) {
    std::vector<std::string> tokens;
    if (textToSplit.empty() || lineWidth == 0) return tokens;

    std::istringstream iss(textToSplit); // Create an std::istringstream object to read the text.
    std::string word;
    std::string line;
    while (iss >> word) { //if word is not empty enter the loop
        if (font->getStringWidth(line + " " + word) <= lineWidth) { // Check if adding the word to the existing line, with a space in between, does not exceed the line width.
            if (!line.empty()) line += " "; // If the line is not empty, add a space before adding the word.
            line += word; // Add the word to the line.
        } else { // If adding the current word exceeds the line width,
            tokens.push_back(line); // Add the current line to the vector of parsed lines.
            line = word; // Create a new line and add the word to it.
        }
    }
    if (!line.empty()) {
        tokens.push_back(line);
    }
    return tokens;
}
