/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#define _CRT_SECURE_NO_WARNINGS

#include "ofApp.h"
#include <iostream>
using namespace std;

int dirx[4] = { 0, 1, 0 ,-1 };
int diry[4] = { 1, 0, -1 ,0 };
int row, col;

void shuffle(int* shuf) {

	for (int i = 0; i < 4; i++) {
		int random = rand() % 4;
		int temp = shuf[random];
		shuf[random] = shuf[i];
		shuf[i] = temp;

	}

}

void backtracking(char** maze, int** kan, int x, int y) {

	int shuf[4] = { 0, 1, 2, 3 };
	kan[y][x] = 1;
	shuffle(shuf);
	for (int i = 0; i < 4; i++) {
		int nextx = x + dirx[shuf[i]], nexty = y + diry[shuf[i]];

		if (nextx >= 0 && nextx < row && nexty >= 0 && nexty < col && kan[nexty][nextx] != 1) {
			maze[((2 * y + 1) + (2 * nexty + 1)) / 2][((1 + 2 * x) + (1 + 2 * nextx)) / 2] = ' ';
			backtracking(maze, kan, nextx, nexty);
		}
		// 0, 0에서 0,1 로 가면 1,2 의 벽이 허물어짐
		// 1 1      1 3 평균이네
		// 1,2 에서 2,2로 가면 4,5벽
		// 3 5      5 5
		// n,m의 maze 좌표는 2n+1, 2m+1임
	}



}





void make() {

	printf("세로 : ");
	scanf("%d", &row);
	printf("가로 : ");
	scanf("%d", &col);

	char** maze;
	int** kan;


	maze = (char**)malloc((2 * row + 1) * sizeof(char*));
	for (int i = 0; i < 2 * row + 1; i++) maze[i] = (char*)malloc((2 * col + 1) * sizeof(char));

	for (int i = 0; i < 2 * row + 1; i++) {
		for (int j = 0; j < 2 * col + 1; j++) {
			if (i % 2 == 0) {
				if (j % 2 == 0) maze[i][j] = '+';
				else maze[i][j] = '-';
			}
			else {
				if (j % 2 == 0) maze[i][j] = '|';
				else maze[i][j] = ' ';
			}
		}
	}


	//+-+-+-+  3 x 3 
	//l l l l			+ 짝짝
	//+-+-+-+			- 짝홀
	//l l l l			' ' 홀홀
	//+-+-+-+			l 홀짝
	//l l l l
	//+-+-+-+


	kan = (int**)malloc(row * sizeof(int*));
	for (int i = 0; i < row; i++) kan[i] = (int*)calloc(col, sizeof(int));

	backtracking(maze, kan, 0, 0);

	FILE* file;
	file = fopen("maze.maz", "w");

	if (file == NULL) {
		return;
	}

	for (int i = 0; i < 2 * row + 1; i++) {
		for (int j = 0; j < 2 * col + 1; j++) {
			fprintf(file, "%c ", maze[i][j]);
		}
		fprintf(file, "\n");
	}




	fclose(file);

	return;
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Make_maze", false, false); // Not checked and not auto-checked
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Make_maze") {
		make();
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			isdfs = true;
			DFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
		
	}

	if(title == "Show BFS") {
		doTopmost(bChecked); // Use the checked value directly

	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	if (isOpen && !isdfs) {
		for (int i = 0; i < 2 * HEIGHT + 1; i++) {
			for (int j = 0; j < 2 * WIDTH + 1; j++) {
				switch (input[i][j]) {
				case '+':
				case '|':
				case '-':
					ofSetColor(0);
					break;
				case ' ':
					ofSetColor(255);
					break;


				}
				ofDrawRectangle(j * 15, i * 15, 15, 15);
			}
		}
	}

	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) {
			dfsdraw();
		}
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName.c_str());
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;
			
			
			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

			//int HEIGHT;//미로의 높이
			//int WIDTH;//미로의 너비
			//char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
			//int** visited;//방문여부를 저장할 포인

			HEIGHT = 0;
			input = (char**)malloc(sizeof(char*));
			for (auto line : buffer.getLines()) {
				WIDTH = strlen(line.c_str());
				HEIGHT++;
				input = (char**)realloc(input, HEIGHT * sizeof(char*));
				input[HEIGHT - 1] = (char*)malloc((WIDTH+1) * sizeof(char));
				strcpy(input[HEIGHT - 1], line.c_str());
			}
			HEIGHT = (HEIGHT - 1) / 2;
			WIDTH = (WIDTH - 1) / 2;
			visited = (int**)malloc(HEIGHT * sizeof(int*));
			for (int i = 0; i < HEIGHT; i++) visited[i] = (int*)calloc(WIDTH , sizeof(int));

			for (int i = 0; i < 2*HEIGHT+1; i++) {
				cout << input[i] << "\n";
			}



		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수
	for (int i = 0; i < HEIGHT; i++) {
		free(input[i]);
	}
	free(input);

	for (int i = 0; i < HEIGHT; i++) free(visited[i]);
	free(visited);
	HEIGHT = 0;
	WIDTH = 0;

}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	printf("DFS\n");
	
	//dir
	isfound = 0;
	int offset[4][2] = {
		{1,0}, 
		{0, 1}, 
		{-1,0}, 
		{0, -1} //순서대로 오른 아래 왼 위
	};



	rooftop = (node*)malloc(sizeof(struct node));
	rooftop->row = -1; rooftop->col = -1; rooftop->dir = -1; rooftop->next = NULL;
	failtop = (node*)malloc(sizeof(struct node));
	failtop->row = -1; failtop->col = -1; failtop->dir = -1; failtop->next = NULL;

	node start;
	start.row = 0; start.col = 0; start.dir = 0;
	visited[0][0] = 1;
	push(rooftop, start);

	int row, col, dir, nextx, nexty;
	bool found = false;

	node pos;

	while (rooftop->next != NULL && !found) {
		pos = pop(rooftop);
		push(failtop, pos);
		row = pos.row; col = pos.col; dir = pos.dir;

		while (dir < 4 && !found) {
			nextx = col + offset[dir][0];
			nexty = row + offset[dir][1];
			if (nextx >= WIDTH || nexty >= HEIGHT || nextx < 0 || nexty < 0) {
				dir++;
				continue;
			}

			if (nextx == WIDTH-1 && nexty == HEIGHT-1) { //도착
				found = true;
				pos.row = row; pos.col = col; pos.dir = ++dir;
				push(rooftop, pos);
			}
			else if (!visited[nexty][nextx] && input[row + nexty + 1][col+ nextx + 1] == ' ') { //0,0 -> 0,1
				visited[nexty][nextx] = 1;
				pos.row = row; pos.col = col; pos.dir = ++dir;
				push(rooftop, pos);
				dfsdraw();

				row = nexty; col = nextx; dir = 0;
			}
			else dir++;

		}


	}

	if (found) {
		isfound = 1;
		dfsdraw();
	}
	node* top = rooftop->next;
	while (top != NULL) {
		printf("%d %d %d\n", top->row, top->col, top->dir);
		top = top->next;
	}

	return 0;
}
void ofApp::push(node* top, node elem) {
	node* newnode = (node*)malloc(sizeof(node));
	newnode->row = elem.row; newnode->col = elem.col; newnode->dir = elem.dir; newnode->next = NULL;

	while (top->next != NULL) top = top->next;
	top->next = newnode;


}
node ofApp::pop(node* top) {
	if (top->next == NULL) return *top;

	node* curr = top;
	node* next = top->next;

	while (next->next != NULL) {
		curr = next;
		next = next->next;
	}
	curr->next = NULL;
	node ret;
	ret.row = next->row; ret.col = next->col; ret.dir = next->dir;
	free(next);

	return ret;
}
void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)

	// l ㅡ ㄴ ㄱ +

	for (int i = 0; i < 2 * HEIGHT + 1; i++) {
		for (int j = 0; j < 2 * WIDTH + 1; j++) {
			switch (input[i][j]) {
			case '+':
			case '|':
			case '-':
				ofSetColor(0);
				break;
			case ' ':
				ofSetColor(255);
				break;


			}
			ofDrawRectangle(j * 15, i * 15, 15, 15);
		}
	}

	ofSetColor(255, 0, 0);
	node* top = failtop;
	while (top != NULL) {
		int row, col, dir;
		row = 1 + 2 * (top->row); col = 1 + 2 * top->col; dir = top->dir;


		switch (dir) {
		case 1: //오른쪽
			ofDrawRectangle(col * 15 + 6, row * 15 + 6, 30, 3);
			break;
		case 2: //아래
			ofDrawRectangle(col * 15 + 6, row * 15 + 6, 3, 30);
			break;
		case 3: //왼
			ofDrawRectangle((col - 2) * 15 + 9, row * 15 + 6, 30, 3);
			break;
		case 4: //위
			ofDrawRectangle(col * 15 + 6, (row - 2) * 15 + 9, 3, 30);
			break;
		}



		top = top->next;
	}


	top = rooftop->next;
	ofSetColor(0, 255, 0);

	while (top != NULL) {
		int row, col, dir;
		row = 1+2*(top->row); col = 1 + 2 * top->col; dir = top->dir;


		switch (dir) {
		case 1: //오른쪽
			ofDrawRectangle(col * 15 + 6, row * 15 + 6, 30, 3);
			break;
		case 2: //아래
			ofDrawRectangle(col * 15 + 6, row * 15 + 6, 3, 30);
			break;
		case 3: //왼
			ofDrawRectangle((col - 2) * 15 + 9, row * 15 + 6, 30, 3);
			break;
		case 4: //위
			ofDrawRectangle(col * 15 + 6, (row-2) * 15 + 9, 3, 30);
			break;
		}



		top = top->next;
	}


}

