/*
 * gGUIGrid.cpp
 *
 *  Created on: 5 Aug 2022
 *      Author: murat
 */

#include "gGUIGrid.h"
//const int gGUIGrid::SELECTEDBOX_X = 0;
//const int gGUIGrid::SELECTEDBOX_Y = 1;
#include "gBaseApp.h"

gGUIGrid::gGUIGrid() {
//	gridsizer.setSize(10,10);
//	gridsizer.enableBorders(true);
	gridx = 0.0f;
	gridy = 0.0f;
	gridboxw = 80.0f;
	gridboxh = 30.0f;
	newgridboxw = gridboxw;
	rownum = 50;
	columnnum = 10;
	gridw = gridboxw * columnnum;
	gridh = gridboxh * rownum;
	selectedbox = 0;
	isselected = false;
	isrowselected = false;
	iscolumnselected = false;
	totalw = columnnum * gridboxw;
	totalh = rownum * gridboxh;
	rowtitle = 1;
	columntitle = 65; // 'A' char in ASCII
	clicktimediff = 250;
	titlediff = 20;
	clicktime = gGetSystemTimeMillis();
	previousclicktime = clicktime - 2 * clicktimediff;
	firstclicktime = previousclicktime - 2 * clicktimediff;
	isdoubleclicked = false;
	selectedtitle = 0;
	shiftpressed = false;
	ctrlpressed = false;
	ctrlvpressed = false;
	ctrlzpressed = false;
	ctrlypressed = false;
	enableScrollbars(true, false);
	Cell tempcell;
	setMargin(tempcell.cellw / 2, tempcell.cellh);
}

gGUIGrid::~gGUIGrid() {

}

void gGUIGrid::set(gBaseApp* root, gBaseGUIObject* topParentGUIObject, gBaseGUIObject* parentGUIObject, int parentSlotLineNo, int parentSlotColumnNo, int x, int y, int w, int h) {
	totalw = columnnum * gridboxw;
	totalh = h;
	totalh = rownum * gridboxh;
	gGUIScrollable::set(root, topParentGUIObject, parentGUIObject, parentSlotLineNo, parentSlotColumnNo, x, y, w, h);
	setDimensions(w, h);
	textbox.set(root, this, this, 0, 0, gridx + (gridboxw / 2) + 1, gridy + gridboxh - 5, gridboxw - 6, gridboxh);
	textbox.setSize(gridboxw - 6, gridboxh - 2);
	textbox.enableBackground(false);
	manager = root->getAppManager()->getGUIManager();

//	gLogi("Grid") << "Textbox:" << textbox.left << " " << textbox.top << " " << textbox.right << " " << textbox.bottom;
}

void gGUIGrid::setGrid(int rowNum, int columnNum) {
	rownum = rowNum;
	columnnum = columnNum;
	gridw = gridboxw * columnnum;
	gridh = gridboxh * rownum;
	createCells();
}

void gGUIGrid::setRowNum(int rowNum) {
	rownum = rowNum;
}

void gGUIGrid::setcolumnNum(int columnNum) {
	columnnum = columnNum;
}

void gGUIGrid::update() {
	textbox.update();
}

void gGUIGrid::createCells() {
	Cell tempcell;
	for(int i = 0; i < rownum; i++) {
		for(int j = 0; j < columnnum; j++) {
			tempcell.cellx = gridx + (gridboxw / 2) + (j * gridboxw);
			tempcell.celly = gridy + gridboxh + (i * gridboxh);
			tempcell.cellrowno = i;
			tempcell.cellcolumnno = j;
			tempcell.cellcontent = "";
			tempcell.fontnum = gGUIManager::FONT_REGULAR;
			tempcell.cellalignment = gBaseGUIObject::TEXTALIGNMENT_LEFT;
			tempcell.textmoveamount = 0;
			tempcell.cellfontcolor = fontcolor;
			tempcell.iscellaligned = false;
			allcells.push_back(tempcell);
		}
	}
}

void gGUIGrid::createTextBox() {
	//allcells.at(selectedbox).cellx + 1
	//allcells.at(cellindexcounter).cellx + (allcells.at(cellindexcounter).cellw - font->getStringWidth(allcells.at(cellindexcounter).showncontent)) * textbox.getTextMoveAmount() - textbox.getInitX() * textbox.getTextAlignment() - firstx
	int newwamount = font->getStringWidth(allcells.at(selectedbox).cellcontent) / gridboxw + 1;
	newgridboxw = gridboxw * newwamount;
	if(allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_LEFT) {
		while(newgridboxw > gridw - allcells.at(selectedbox).cellx + allcells.at(selectedbox).cellw / 2)
			newgridboxw -= gridboxw;
			//Should go to a new line and the width of the textbox should be "gridw - allcells.at(selectedbox).cellx + allcells.at(selectedbox).cellw / 2"
	}
	else if(allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_RIGHT) {
		while(newgridboxw > allcells.at(selectedbox).cellx - allcells.at(selectedbox).cellw / 2 + gridboxw)
			newgridboxw -= gridboxw;
			//Should go to a new line and the width of the textbox should be "allcells.at(selectedbox).cellx - allcells.at(selectedbox).cellw / 2 + gridboxw"
	}
	textbox.set(root, this, this, 0, 0, allcells.at(selectedbox).cellx + 1, allcells.at(selectedbox).celly - 2, newgridboxw - 10, gridboxh - 2);
	textbox.setTextFont(manager->getFont(allcells.at(selectedbox).fontnum));
	textbox.setTextAlignment(allcells.at(selectedbox).cellalignment, allcells.at(selectedbox).cellw, textbox.getInitX());
	textbox.setTextColor(&allcells.at(selectedbox).cellfontcolor);
	if(allcells.at(selectedbox).cellcontent != "") {
		textbox.setText(allcells.at(selectedbox).cellcontent);
		int length = allcells.at(selectedbox).cellcontent.length();
		if(allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_LEFT || allcells.at(selectedbox).cellalignment == gBaseGUIObject::TEXTALIGNMENT_RIGHT)
			textbox.setCursorPosX(font->getStringWidth(allcells.at(selectedbox).cellcontent), length);
		else {
			std::string mid;
			int middle;
			if(length % 2 == 0) middle = length / 2;
			else middle = length / 2 + 1;
			mid = allcells.at(selectedbox).cellcontent.substr(0, middle);
			textbox.setCursorPosX(font->getStringWidth(mid), middle);
		}
		allcells.at(selectedbox).showncontent = "";
	}
}

void gGUIGrid::showCells() {
	int cellindexcounter = 0;
	for(int i = 0; i < rownum; i++) {
		for(int j = 0; j < columnnum; j++) {
//			gLogi("Grid") << "cellx: "<< allcells.at(cellindexcounter).cellx
//				<< " celly: " << allcells.at(cellindexcounter).celly
//				<< " cellrowno: " << allcells.at(cellindexcounter).cellrowno
//				<< " cellcolumnno: " << allcells.at(cellindexcounter).cellcolumnno
//				<< " cellcontent: " << allcells.at(cellindexcounter).cellcontent
//				<< " celltype:" << allcells.at(cellindexcounter).celltype;
			cellindexcounter++;
		}
	}
}

void gGUIGrid::showCell(int rowNo , int columnNo) {
	int cellindex = columnNo + (rowNo  * columnnum);
//	gLogi("Grid") << "cellindex:" << cellindex;
//	gLogi("Grid") << "cellx: "<< allcells.at(cellindex).cellx
//		<< " celly: " << allcells.at(cellindex).celly
//		<< " cellrowno: " << allcells.at(cellindex).cellrowno
//		<< " cellcolumnno: " << allcells.at(cellindex).cellcolumnno
//		<< " cellcontent: " << allcells.at(cellindex).cellcontent
//		<< " celltype: " << allcells.at(cellindex).celltype;
}

void gGUIGrid::checkCellType(int cellIndex) {
	if(allcells.at(cellIndex).showncontent == "") return;
	bool isnegative = false;
	bool isfractional = false;
	bool doubledot = false;
	if(allcells.at(cellIndex).showncontent.at(0) == '-') isnegative = true;
	for(int i = 1 + isnegative; i < allcells.at(cellIndex).showncontent.length(); i++) {
		if(allcells.at(cellIndex).showncontent.at(i) == '.') {
			for(int j = i + 1; j < allcells.at(cellIndex).showncontent.length(); j++) {
				if(!isdigit(allcells.at(cellIndex).showncontent.at(j))) {
					isfractional = false;
					doubledot = true;
					break;
				}
				else isfractional = true;
			}
			if(doubledot) break;
		}
	}
	for(int i = 0 + isnegative; i < allcells.at(cellIndex).showncontent.length(); i++) {
		if(!isdigit(allcells.at(cellIndex).showncontent.at(i)) && !(isfractional && allcells.at(cellIndex).showncontent.at(i) == '.')) {
			if(allcells.at(cellIndex).celltype == "digit" && !allcells.at(cellIndex).iscellaligned) changeCellAlignment(gBaseGUIObject::TEXTALIGNMENT_LEFT, false);
			allcells.at(cellIndex).celltype = "string";
			break;
		}
		else allcells.at(cellIndex).celltype = "digit";
	}
	if(allcells.at(cellIndex).celltype == "digit" && !allcells.at(cellIndex).iscellaligned) changeCellAlignment(gBaseGUIObject::TEXTALIGNMENT_RIGHT, false);
}

void gGUIGrid::changeCellFont(int fontNum) {
	int index = selectedbox;
	if(!ctrlzpressed && !ctrlypressed && !ctrlvpressed) pushToUndoStack();
	else if(ctrlzpressed) index = undocellnumberstack.top();
	else if(ctrlypressed) index = redocellnumberstack.top();

	allcells.at(index).fontnum = fontNum;
	textbox.setTextFont(manager->getFont(fontNum));
}

void gGUIGrid::changeCellAlignment(int cellAlignment, bool clicked) {
	int index = selectedbox;
	if(clicked && !ctrlzpressed && !ctrlypressed && !ctrlvpressed) pushToUndoStack();
	else if(ctrlzpressed) index = undocellnumberstack.top();
	else if(ctrlypressed) index = redocellnumberstack.top();
	else if(ctrlvpressed) index = selectedbox;
	if(clicked && !allcells.at(index).iscellaligned) allcells.at(index).iscellaligned = true;
	else if(clicked && allcells.at(index).iscellaligned && allcells.at(index).celltype == "digit") {
		allcells.at(index).iscellaligned = false;
		cellAlignment = gBaseGUIObject::TEXTALIGNMENT_RIGHT;
	}
	else if(clicked && allcells.at(index).cellalignment == cellAlignment) {
		allcells.at(index).iscellaligned = false;
		cellAlignment = gBaseGUIObject::TEXTALIGNMENT_LEFT;
	}
	allcells.at(index).cellalignment = cellAlignment;
	allcells.at(index).textmoveamount = 0.5f * cellAlignment;
	textbox.setTextAlignment(cellAlignment, allcells.at(index).cellw, textbox.getInitX());
}

void gGUIGrid::changeCellFontColor(gColor *fontColor) {
	int index = selectedbox;
	if(!ctrlzpressed && !ctrlypressed && !ctrlvpressed) pushToUndoStack();
	else if(ctrlzpressed) index = undocellnumberstack.top();
	else if(ctrlypressed) index = redocellnumberstack.top();
	allcells.at(index).cellfontcolor = fontColor;
	textbox.setTextColor(fontColor);
}

void gGUIGrid::pushToUndoStack() {
	int index = selectedbox;
	if(ctrlypressed) index = redocellnumberstack.top();
	undostringstack.push(allcells.at(index).cellcontent);
	undofontstack.push(allcells.at(index).fontnum);
	undoalignmentstack.push(allcells.at(index).cellalignment);
	undofontcolorstack.push(allcells.at(index).cellfontcolor);
	undocopiednostack.push(allcells.at(index).copiedno);
	undocellnumberstack.push(index);
}

void gGUIGrid::pushToRedoStack() {
	redostringstack.push(allcells.at(undocellnumberstack.top()).cellcontent);
	redofontstack.push(allcells.at(undocellnumberstack.top()).fontnum);
	redoalignmentstack.push(allcells.at(undocellnumberstack.top()).cellalignment);
	redofontcolorstack.push(allcells.at(undocellnumberstack.top()).cellfontcolor);
	redocopiednostack.push(allcells.at(undocellnumberstack.top()).copiedno);
	redocellnumberstack.push(undocellnumberstack.top());
}

std::string gGUIGrid::fixTextFunction(std::string text) {
	if(text == "") return text;
	std::string tempstr = text;
	if(int(tempstr[0]) == 39) {
		tempstr.erase(0, 1);
		allcells.at(selectedbox).hasfunction = false;
	}
	else {
		bool function = (tempstr[0] == '=');
		bool isnegative = false;
		allcells.at(selectedbox).hasfunction = function;
		tempstr.erase(0, function);
		if(function) std::transform(tempstr.begin(), tempstr.end(), tempstr.begin(), ::toupper);
		if(function) {
			tempstr = fixNumeric(tempstr);
			isnegative = (tempstr[0] == '-');
			tempstr.erase(0, isnegative);
		}
		bool hasdigit = isNumeric(tempstr);

		if(function && !isdigit(tempstr[0])) {
			int parentheses1 = tempstr.find('(');
			int parentheses2 = tempstr.find(')');
			if(parentheses1 != std::string::npos && parentheses2 != std::string::npos) {
				std::string functionstr = tempstr.substr(0, parentheses1);
				std::string parenthesesstr = tempstr.substr(parentheses1 + 1, parentheses2 - parentheses1 - 1);
				std::transform(functionstr.begin(), functionstr.end(), functionstr.begin(), ::toupper);
				std::transform(parenthesesstr.begin(), parenthesesstr.end(), parenthesesstr.begin(), ::toupper);
				parenthesesstr = fixNumeric(parenthesesstr);
				bool isparenthesesnegative = (parenthesesstr[0] == '-');
				if(isparenthesesnegative) parenthesesstr.erase(0, 1);
				if(functionstr == "SUM") {
					int doubledat = parenthesesstr.find(':');
					if(doubledat == std::string::npos) {
						if(isNumeric(parenthesesstr)) allcells.at(selectedbox).copiedno = -1;
						else {
							int copiedindex = getCellIndex(parenthesesstr);
							tempstr = allcells.at(copiedindex).showncontent;
							allcells.at(selectedbox).copiedno = copiedindex;
						}
					}
					else {
						std::string str1 = parenthesesstr.substr(0, doubledat);
						std::string str2 = parenthesesstr.substr(str1.size() + 1, parenthesesstr.size());
						int columnno1 = int(str1[0]) % 65;
						int columnno2 = int(str2[0]) % 65;
						int rowno1 = std::stoi(str1.substr(1, str1.size())) - 1;
						int rowno2 = std::stoi(str2.substr(1, str2.size())) - 1;

						tempstr = std::to_string(makeSum(columnno1, rowno1, columnno2, rowno2));
					}
					if((isnegative && !isparenthesesnegative) || (!isnegative && isparenthesesnegative)) tempstr = "-" + tempstr;
				}
			}
			else if(function && int(tempstr[0]) >= 65 && int(tempstr[0]) < 65 + columnnum) {
				if(tempstr.size() > 1) {
					std::string tstr = tempstr.substr(1, tempstr.size());
					int found = tstr.find_first_not_of("0123456789");
					if(found == std::string::npos) {
						int copiedindex = (std::stoi(tstr) - 1) * columnnum + (int(tempstr[0]) % 65);
						allcells.at(selectedbox).copiedno = copiedindex;
						tempstr = allcells.at(copiedindex).showncontent;
						if(isnegative) tempstr = "-" + tempstr;
					}
					else if(allcells.at(selectedbox).copiedno != -1) allcells.at(selectedbox).copiedno = -1;
				}
			}
		}
	}

	return tempstr;
}

bool gGUIGrid::isNumeric(std::string text) {
	bool hasdigit = true;
	for(int i = 0; i < text.size(); i++) {
		if(!isdigit(text[i])) {
			hasdigit = false;
			break;
		}
	}
	return hasdigit;
}

std::string gGUIGrid::fixNumeric(std::string text) {
	std::string tempstr = text;
	std::stack<int> unnecessaryindexes;

	if(tempstr[0] == '+') {
		unnecessaryindexes.push(0);
		for(int i = 1; i < tempstr.size(); i++) {
			if(tempstr[i] != '+' && tempstr[i] != '-') break;
			if(tempstr[i] == '+') unnecessaryindexes.push(i);
			else if(tempstr[i] != '-') continue;
		}
		while(!unnecessaryindexes.empty()) {
			tempstr.erase(unnecessaryindexes.top(), 1);
			unnecessaryindexes.pop();
		}
	}
	if(tempstr[0] == '-') {
		int minuscount = 1;
		for(int i = 1; i < tempstr.size(); i++) {
			if(tempstr[i] == '-') minuscount++;
			else if(tempstr[i] == '+') unnecessaryindexes.push(i);
			else break;
		}
		while(!unnecessaryindexes.empty()) {
			tempstr.erase(unnecessaryindexes.top(), 1);
			unnecessaryindexes.pop();
		}
		if(minuscount % 2 == 1) tempstr.erase(0, minuscount - 1);
		else tempstr.erase(0, minuscount);
	}

	return tempstr;
}

std::string gGUIGrid::getTextColumn(std::string text) {
    int found = text.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (found != std::string::npos && std::isalpha(text[0])) {
        return text.substr(0, found);
    }
    return text;
}

int gGUIGrid::getCellIndex(std::string text) {
	int result;
	std::string letterstr = getTextColumn(text);
	std::string numberstr = text.substr(letterstr.size(), text.size());
	try {
		result = (int(letterstr[0]) % 65) + (std::stoi(numberstr) - 1) * columnnum;
	}
	catch(const std::exception& e) {
		result = selectedbox;
	}
	return result;
}

gGUIGrid::Cell gGUIGrid::getCopiedCell(int cellIndex) {
	int index = cellIndex;
	while(allcells.at(index).copiedno != -1) index = allcells.at(index).copiedno;
	Cell tmpcell = allcells.at(index);
	return tmpcell;
}

int gGUIGrid::makeSum(int c1, int r1, int c2, int r2) {
	if(c1 > c2) {
		int flag = c1;
		c1 = c2;
		c2 = flag;
	}
	if(r1 > r2) {
		int flag = r1;
		r1 = r2;
		r2 = flag;
	}

	int result = 0;
	int currentindex = r1 * columnnum + c1;
	for(int row = r1; row <= r2; row++) {
		for(int column = c1; column <= c2; column++) {
			if(allcells.at(currentindex).celltype == "digit") result += std::stoi(allcells.at(currentindex).showncontent);
			currentindex++;
		}
		currentindex -= (c2 - c1);
		currentindex += columnnum - 1;
	}
	return result;
}

void gGUIGrid::fillCell(int rowNo, int columnNo, std::string tempstr) { //when rowNo = 1, columnNO = 4; tempstr = "happyyyy";
	if(rowNo > rownum - 1 || columnNo > columnnum - 1) return;
	int cellindex = columnNo + (rowNo * columnnum);
	allcells.at(cellindex).cellcontent = tempstr;
	allcells.at(cellindex).showncontent = fixTextFunction(tempstr);
	bool isempty = (tempstr == "");
//	gLogi("Grid") << "isempty:" << isempty;

	checkCellType(cellindex);
	// Check left cell & update that cell's shown content if necessary
	int leftcolumnlimit = rowNo * columnnum;
	int leftcolumnindex = cellindex - 1;

	 // Check if input is empty and update left cells according to that
	if(isempty) {
		leftcolumnindex = cellindex;
		cellindex++;
	}

	while(leftcolumnindex >= leftcolumnlimit) {
		if(allcells.at(leftcolumnindex).cellcontent.empty()) {
			leftcolumnindex--;
			continue;
		}
		else {
			if(font->getStringWidth(allcells.at(leftcolumnindex).cellcontent) > (cellindex - leftcolumnindex) * gridboxw) {
				std::string tempstr;
				int index = 0;
				while (font->getStringWidth(tempstr) < (cellindex - leftcolumnindex - 1) * gridboxw + gridboxw * (0.9f)) {
					tempstr += allcells.at(leftcolumnindex).cellcontent[index];
					index++;
				}
				tempstr.pop_back();
				allcells.at(leftcolumnindex).showncontent = fixTextFunction(tempstr);
				break;
			} break;
		}
	}

	// Check right cell for shown content
	if(font->getStringWidth(tempstr) >= gridboxw){
		int rightcolumnlimit = (int)(font->getStringWidth(tempstr) / gridboxw) + cellindex;
		if(rightcolumnlimit >= (rowNo + 1) * columnnum) rightcolumnlimit = (rowNo + 1) * columnnum - 1;
		if(cellindex == rightcolumnlimit) {
			std::string tempstr;
			int index = 0;
			while (font->getStringWidth(tempstr) < gridboxw * (0.9f)) {
				tempstr += allcells.at(cellindex).cellcontent[index];
				index++;
			}
			tempstr.pop_back();
			allcells.at(cellindex).showncontent = fixTextFunction(tempstr);
			return;
		}
		int rightcolumnindex = cellindex + 1;

		while(rightcolumnindex <= rightcolumnlimit) {
			if(allcells.at(rightcolumnindex).cellcontent.empty()) {
				if(rightcolumnindex == rightcolumnlimit) {
					std::string tempstr;
					int index = 0;
					while (font->getStringWidth(tempstr) < (rightcolumnindex - cellindex) * gridboxw + gridboxw * (0.9f)) {
						if(index > allcells.at(cellindex).cellcontent.length()) break;
						tempstr += allcells.at(cellindex).cellcontent[index];
						index++;
					}
					tempstr.pop_back();
					allcells.at(cellindex).showncontent = fixTextFunction(tempstr);
					break;
				}
				rightcolumnindex++;
				continue;
			}
			else {
				std::string tempstr;
				int index = 0;
				while (font->getStringWidth(tempstr) < (rightcolumnindex - cellindex - 1) * gridboxw + gridboxw * (0.9f)) {
					if(index > allcells.at(cellindex).cellcontent.length()) break;
					tempstr += allcells.at(cellindex).cellcontent[index];
					index++;
				}
				tempstr.pop_back();
				allcells.at(cellindex).showncontent = fixTextFunction(tempstr);
				break;
			} break;

		}

		if(allcells.at(cellindex).showncontent == "") {
//			gLogi("Grid");
		}
	}
}

void gGUIGrid::changeCell() {
	std::string tmpstr;
	if(!ctrlvpressed && !ctrlzpressed && !ctrlypressed) {
		tmpstr = textbox.getText();
		textbox.cleanText();
		istextboxactive = false;
		fillCell((allcells.at(selectedbox).celly - gridboxh) / gridboxh, (allcells.at(selectedbox).cellx - (gridboxw / 2)) / gridboxw, tmpstr);
	}
	else if(ctrlvpressed) {
		if(ctrlcpressed) tmpstr = "=" + std::string(1, char(allcells.at(copiedno).cellcolumnno + 65)) + std::to_string(allcells.at(copiedno).cellrowno + 1);
		else tmpstr = appmanager->getClipboardString();
		std::string tmpcellstr = allcells.at(selectedbox).cellcontent;
		int tmpfont = allcells.at(selectedbox).fontnum;
		int tmpalignment = allcells.at(selectedbox).cellalignment;
		gColor tmpcolor = allcells.at(selectedbox).cellfontcolor;
		int tmpcopiedno = allcells.at(selectedbox).copiedno;
		changeCellFont(copiedfont);
		changeCellAlignment(copiedalignment, false);
		changeCellFontColor(&copiedfontcolor);
		allcells.at(selectedbox).copiedno = -1;
		fillCell((allcells.at(selectedbox).celly - gridboxh) / gridboxh, (allcells.at(selectedbox).cellx - (gridboxw / 2)) / gridboxw, tmpstr);
		ctrlvpressed = false;

		undostringstack.push(tmpcellstr);
		undofontstack.push(tmpfont);
		undoalignmentstack.push(tmpalignment);
		undofontcolorstack.push(tmpcolor);
		undocopiednostack.push(tmpcopiedno);
		undocellnumberstack.push(selectedbox);
	}
	else if(ctrlzpressed) {
		tmpstr = undostringstack.top();
		changeCellFont(undofontstack.top());
		changeCellAlignment(undoalignmentstack.top(), false);
		changeCellFontColor(&undofontcolorstack.top());
		allcells.at(undocellnumberstack.top()).copiedno = undocopiednostack.top();
		fillCell((allcells.at(undocellnumberstack.top()).celly - gridboxh) / gridboxh, (allcells.at(undocellnumberstack.top()).cellx - (gridboxw / 2)) / gridboxw, tmpstr);
		ctrlzpressed = false;
	}
	else if(ctrlypressed) {
		tmpstr = redostringstack.top();
		changeCellFont(redofontstack.top());
		changeCellAlignment(redoalignmentstack.top(), false);
		changeCellFontColor(&redofontcolorstack.top());
		allcells.at(redocellnumberstack.top()).copiedno = redocopiednostack.top();
		fillCell((allcells.at(redocellnumberstack.top()).celly - gridboxh) / gridboxh, (allcells.at(redocellnumberstack.top()).cellx - (gridboxw / 2)) / gridboxw, tmpstr);
	}
}

void gGUIGrid::drawContent() {
	gColor oldcolor = renderer->getColor();
	drawCellBackground();
	if(isselected) drawSelectedBox();
	else if(isrowselected) drawSelectedRow();
	else if(iscolumnselected) drawSelectedColumn();
	drawCellContents();
	textbox.setFirstX(firstx);
	textbox.setFirstY(firsty);
	if(istextboxactive)textbox.draw();
	drawTitleRowBackground();
	drawRowContents();
	drawTitleColumnBackground();
	drawColumnContents();
	drawTitleLines();
	renderer->setColor(oldcolor);
}

void gGUIGrid::drawCellBackground() {
	renderer->setColor(*textbackgroundcolor);
	gDrawRectangle(gridx + (gridboxw / 2) - firstx, gridy + gridboxh - firsty, gridw, gridh, true);
}

void gGUIGrid::drawSelectedBox() {
	renderer->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	gDrawRectangle(allcells.at(selectedbox).cellx + 1 - firstx, (allcells.at(selectedbox).celly + 1) - firsty, gridboxw - 2, gridboxh - 2, false);
	gDrawRectangle(allcells.at(selectedbox).cellx + (gridboxw - 2) - 6 - firstx, allcells.at(selectedbox).celly + (gridboxh - 2) - 4 - firsty, 6, 6, true); // FLAG
}

void gGUIGrid::drawSelectedRow() {
	renderer->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	gDrawRectangle(gridx + (gridboxw / 2) + 1 - firstx, gridy + gridboxh * selectedtitle + 1 - firsty, gridw - 2, gridboxh - 2, false);
	gDrawRectangle(gridx + (gridboxw / 2) + gridw - 2 - 6 - firstx, gridy + gridboxh * selectedtitle + (gridboxh - 2) - 4 - firsty, 6, 6, true); // FLAG
}

void gGUIGrid::drawSelectedColumn() {
	renderer->setColor(0.0f, 1.0f, 0.0f, 1.0f);
	gDrawRectangle(gridx + (gridboxw / 2) + 1 + (gridboxw * (selectedtitle - 1)) - firstx, gridy + gridboxh + 1 - firsty, gridboxw - 2, gridh - 2, false);
	gDrawRectangle(gridx + (gridboxw / 2) + (gridboxw * (selectedtitle - 1)) + gridboxw - 2 - 6 - firstx, gridy + gridboxh + gridh - 2 - 4 - firsty, 6, 6, true); // FLAG
}

void gGUIGrid::drawTitleRowBackground() {
	renderer->setColor(*buttoncolor);
	gDrawRectangle(gridx, gridy - firsty, gridboxw / 2, gridh + gridboxh, true);
}

void gGUIGrid::drawTitleColumnBackground() {
	renderer->setColor(*buttoncolor);
	gDrawRectangle(gridx - firstx, gridy, gridw + (gridboxw / 2), gridboxh , true);
}

void gGUIGrid::drawRowContents() {
	int temprow = rowtitle;
	renderer->setColor(*pressedbuttoncolor);
	for(int i = 1; i <= rownum; i++) {
		std::string rowtitlestring = std::to_string(temprow);
		renderer->setColor(*fontcolor);
		font->drawText(rowtitlestring, gridx + (gridboxw / 4) - (font->getStringWidth(rowtitlestring) / 2), gridy + (gridboxh / 2) + (i * gridboxh)  + (font->getStringHeight(rowtitlestring) / 2) - firsty);
		temprow++;
		renderer->setColor(*pressedbuttoncolor);
		gDrawLine(gridx - firstx, gridy + ((i + 1) * gridboxh) - firsty, gridx + gridw + (gridboxw / 2) - firstx, gridy + ((i + 1) * gridboxh) - firsty);
	}
}

void gGUIGrid::drawColumnContents() {
	int tempcol = columntitle;
	for(int i = 1; i <= columnnum; i++) {
	    std::string columntitlestring(1, (char) tempcol);
	    renderer->setColor(*fontcolor);
		font->drawText(columntitlestring, gridx + (i * gridboxw) - (font->getStringWidth(columntitlestring) / 2) - firstx, gridy + (gridboxh / 2) + (font->getStringHeight(columntitlestring) / 2));
		tempcol++;
		renderer->setColor(*pressedbuttoncolor);
		gDrawLine(gridx - (gridboxw / 2) + ((i + 1) * gridboxw) - firstx, gridy - firsty, gridx - (gridboxw / 2) + ((i + 1) * gridboxw) - firstx, gridy + gridboxh +gridh - firsty);
		gDrawRectangle(gridx, gridy, allcells.at(0).cellw / 2, allcells.at(0).cellh, true);
	}
}

void gGUIGrid::drawTitleLines() {
	renderer->setColor(*backgroundcolor);
	gDrawLine(gridx + (gridboxw / 2) + 1 - firstx, gridy - firsty, gridx + (gridboxw / 2) + 1 - firstx, gridy + gridboxh + gridh - firsty);
	gDrawLine(gridx - firstx, gridy + (gridboxh) - firsty, gridx + gridboxw / 2 + gridw - firstx, gridy + (gridboxh) - firsty);
}

void gGUIGrid::drawCellContents() {
	int cellindexcounter = 0;
	for(int i = 0; i < rownum; i++) {
		for(int j = 0; j < columnnum; j++) {
			renderer->setColor(allcells.at(cellindexcounter).cellfontcolor);
			if(allcells.at(cellindexcounter).hasfunction) allcells.at(cellindexcounter).showncontent = fixTextFunction(allcells.at(cellindexcounter).cellcontent);
			manager->getFont(allcells.at(cellindexcounter).fontnum)->drawText(allcells.at(cellindexcounter).showncontent, allcells.at(cellindexcounter).cellx + (allcells.at(cellindexcounter).cellw - manager->getFont(allcells.at(cellindexcounter).fontnum)->getStringWidth(allcells.at(cellindexcounter).showncontent)) * allcells.at(cellindexcounter).textmoveamount - textbox.getInitX() * allcells.at(cellindexcounter).cellalignment - firstx, allcells.at(cellindexcounter).celly + (gridboxh / 2) + (manager->getFont(allcells.at(cellindexcounter).fontnum)->getStringHeight(allcells.at(cellindexcounter).showncontent) / 2) - firsty);
			cellindexcounter++;
		}
	}
}

void gGUIGrid::mousePressed(int x, int y, int button) {
	gGUIScrollable::mousePressed(x, y, button);
	int pressedx = x - left - firstx;
	int pressedy = y - top - firsty - titledy;
	if(!(pressedy < gridy + gridboxh - firsty && pressedx < gridx + (gridboxw / 2) - firstx) && pressedx >= gridx - firstx && pressedx <= gridx + (gridboxw / 2) + gridw - firstx && pressedy >= gridy - firsty && pressedy <= gridy + gridboxh + gridh - firsty) {
		if(pressedx >= gridx + (gridboxw / 2) - firstx && pressedx <= gridx + (gridboxw / 2) + gridw - firstx && pressedy >= gridy + gridboxh - firsty && pressedy <= gridy + gridboxh + gridh - firsty) {
			isselected = true;
			isrowselected = false;
			iscolumnselected = false;
			int newcellindex = ((int)((x + firstx - left - (gridboxw / 2)) / gridboxw)) + ((int)((y + firsty - top - gridboxh - titletopmargin + ((font->getSize() * 1.8f) * !istitleon)) / gridboxh))  * columnnum ; // * gridboxw + (gridboxw / 2);
			if(newcellindex != selectedbox) {
				if(istextboxactive) changeCell();
				if(newcellindex != selectedbox) {
					allcells.at(selectedbox).iscellselected = false;
					allcells.at(newcellindex).iscellselected = true;
				}
				selectedbox = newcellindex;
			}
		}
		else if(pressedx >= gridx - firstx && pressedx < gridx + (gridboxw / 2) - firstx && pressedy >= gridy + gridboxh - firsty && pressedy <= gridy + gridboxh + gridh - firsty) {
			selectedtitle = ceil((pressedy + (firsty * 2)) / gridboxh) - 1;
			selectedbox = columnnum * (selectedtitle - 1);
			isselected = false;
			isrowselected = true;
			iscolumnselected = false;
		}
		else if(pressedx >= gridx + (gridboxw / 2) - firstx && pressedx < gridx + (gridboxw / 2) + gridw - firstx && pressedy >= gridy - firsty && pressedy <= gridy + gridboxh - firsty) {
			selectedtitle = ceil((pressedx - gridboxw / 2 + (firstx * 2)) / gridboxw);
			selectedbox = selectedtitle - 1;
			isselected = false;
			isrowselected = false;
			iscolumnselected = true;
		}

		previousclicktime = clicktime;
		clicktime = gGetSystemTimeMillis();
		if(clicktime - previousclicktime <= clicktimediff) {
			isdoubleclicked = true;
//			gLogi("Grid") << "doubleclicked.";
		} else isdoubleclicked = false;
		if(isdoubleclicked) {
			pushToUndoStack();
			textbox.cleanText();
			createTextBox();
			textbox.mousePressed(pressedx, pressedy, button);
			istextboxactive = true;
		} else istextboxactive = false;
	}

}

void gGUIGrid::mouseReleased(int x, int y, int button) {
	if(istextboxactive) textbox.mouseReleased((x - left), (y - top - firsty - titledy), button);
	gGUIScrollable::mouseReleased(x, y, button);
}

void gGUIGrid::mouseDragged(int x, int y, int button) {
//	gLogi("Grid") << x << " " << y;
	if(istextboxactive) textbox.mouseDragged((x - left), (y - top - firsty), button);
	gGUIScrollable::mouseDragged(x, y, button);
}

void gGUIGrid::keyPressed(int key){
	if(istextboxactive) textbox.keyPressed(key);
	else if(key == G_KEY_C && ctrlpressed) {
		ctrlcpressed = true;
		copiedfont = allcells.at(selectedbox).fontnum;
		copiedalignment = allcells.at(selectedbox).cellalignment;
		copiedfontcolor = allcells.at(selectedbox).cellfontcolor;
		copiedno = allcells.at(selectedbox).cellrowno * columnnum + allcells.at(selectedbox).cellcolumnno;
	}
	else if(key == G_KEY_V && ctrlpressed) {
		ctrlvpressed = true;
		changeCell();
	}
	else if(key == G_KEY_X && ctrlpressed) {
		ctrlcpressed = false;
		appmanager->setClipboardString(allcells.at(selectedbox).cellcontent);
		copiedfont = allcells.at(selectedbox).fontnum;
		copiedalignment = allcells.at(selectedbox).cellalignment;
		copiedfontcolor = allcells.at(selectedbox).cellfontcolor;
		copiedno = allcells.at(selectedbox).copiedno;
		pushToUndoStack();
		textbox.cleanText();
		allcells.at(selectedbox).cellcontent = "";
		allcells.at(selectedbox).showncontent = "";
		allcells.at(selectedbox).fontnum = gGUIManager::FONT_REGULAR;
		allcells.at(selectedbox).cellalignment = gBaseGUIObject::TEXTALIGNMENT_LEFT;
		allcells.at(selectedbox).cellfontcolor = fontcolor;
		allcells.at(selectedbox).copiedno = -1;
	}
	else if(key == G_KEY_Z && ctrlpressed) {
		if(undocellnumberstack.empty()) return;
		ctrlzpressed = true;
		pushToRedoStack();
		changeCell();
		undostringstack.pop();
		undofontstack.pop();
		undoalignmentstack.pop();
		undofontcolorstack.pop();
		undocopiednostack.pop();
		undocellnumberstack.pop();
	}
	else if(key == G_KEY_Y && ctrlpressed) {
		if(redocellnumberstack.empty()) return;
		ctrlypressed = true;
		pushToUndoStack();
		changeCell();
		redostringstack.pop();
		redofontstack.pop();
		redoalignmentstack.pop();
		redofontcolorstack.pop();
		redocopiednostack.pop();
		redocellnumberstack.pop();
		ctrlypressed = false;
	}
	else if((isselected || isrowselected || iscolumnselected) && (key == G_KEY_LEFT_CONTROL || key == G_KEY_RIGHT_CONTROL)) ctrlpressed = true;
	else if((isselected || isrowselected || iscolumnselected) && (key == G_KEY_LEFT_SHIFT || key == G_KEY_RIGHT_SHIFT)) shiftpressed = true;
	else if((isselected || isrowselected || iscolumnselected) && key != G_KEY_ENTER && key != G_KEY_UP && key != G_KEY_DOWN && key != G_KEY_RIGHT && key != G_KEY_LEFT && key != G_KEY_ESC && key != G_KEY_LEFT_SHIFT && key != G_KEY_RIGHT_SHIFT && key != G_KEY_F2) {
		pushToUndoStack();
		textbox.cleanText();
		allcells.at(selectedbox).cellcontent = "";
		allcells.at(selectedbox).showncontent = "";
		createTextBox();
		textbox.mousePressed(allcells.at(selectedbox).cellx + textbox.getInitX(), allcells.at(selectedbox).celly + textbox.getInitX(), 0);
		textbox.keyPressed(key);
		istextboxactive = true;
	}
}

void gGUIGrid::keyReleased(int key) {
	if(istextboxactive) textbox.keyReleased(key);
	if((key == G_KEY_ENTER && istextboxactive)) {
		changeCell();
		istextboxactive = false;
		textbox.setEditable(false);
	}
	else if ((key == G_KEY_ENTER || key == G_KEY_DOWN) && !istextboxactive) {
		if(selectedbox + columnnum < rownum * columnnum) selectedbox += columnnum;
		if(isrowselected) {
			isrowselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_UP && !istextboxactive) {
		if(selectedbox - columnnum >= 0) selectedbox -= columnnum;
		if(isrowselected) {
			isrowselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_RIGHT && !istextboxactive) {
		if(selectedbox % columnnum != (columnnum - 1)) selectedbox++;
		if(iscolumnselected) {
			iscolumnselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_LEFT && !istextboxactive) {
		if(selectedbox % columnnum != 0) selectedbox--;
		if(iscolumnselected) {
			iscolumnselected = false;
			isselected = true;
		}
	}
	else if(key == G_KEY_F2) {
		textbox.cleanText();
		createTextBox();
		istextboxactive = true;
		istextboxactive = true;
		textbox.setEditable(true);
	}
	else if(key == G_KEY_LEFT_CONTROL || key == G_KEY_RIGHT_CONTROL) ctrlpressed = false;
	else if(key == G_KEY_LEFT_SHIFT || key == G_KEY_RIGHT_SHIFT) shiftpressed = false;
}

void gGUIGrid::charPressed(unsigned int codepoint) {
	if(istextboxactive) textbox.charPressed(codepoint);
}

void gGUIGrid::mouseScrolled(int x, int y) {
	gGUIScrollable::mouseScrolled(x, y);
}
