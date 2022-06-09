#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h"

//해상도 관련

//2560*1600 디스플레이 기준
#define XS 30
#define resolution_x 1792
#define resolution_y 1344

//#define XS 20
//#define resolution_x 1024
//#define resolution_y 768

class ofApp : public ofBaseApp {

public:

	//openFrameworks의 base 함수들이다.
	void setup();
	void update();
	void draw();

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool genMaze(); //Eller's algorithm을 적용해 mage를 generate하는 함수이다.
	bool setMaze(); //생성한 미로를 적절한 자료구조에 저장하는 함수이다.
	void freeMemory(); //사용한 메모리를 해제하는 함수이다.
	void insertHeap(pair<int, pair<int, int>> insertElement); //BFS distance와 미로의 좌표를 삽입해 Heap을 구성하는 함수이다.
	void placeGem(); //Heap sort를 수행해, 내림차순으로 정렬된 결과를 gems 변수에 저장하는 함수이다.
	void drawGem(); //화면에 Gem을 직접 출력하는 함수이다. BFS distance에 따라 10개 색으로 나누어 출력한다.
	bool BFS(); // BFS탐색을 수행해 결과를 자료구조에 저장하는 함수이다.
	void assignBFSDtoHeap(); //BFS 탐색 결과를 Heap에 삽입하는 함수
	void mazeRegen(); //다시 새로운 미로를 생성하기 위한 초기화 및 미로 재생성 함수

	int HEIGHT;//미로의 높이
	int WIDTH;//미로의 너비
	char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
	int** visited;//방문여부를 저장할 포인
	int maze_col;//미로칸의 열의 인덱스를 가리킨다.
	int maze_row;//미로칸의 행의 인덱스를 가리킨다.
	int k;
	int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
	int isDFS;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
	int isBFS;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
	// Menu
	ofxWinMenu * menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	// Used by example app
	ofTrueTypeFont myFont;
	ofImage myImage;
	float windowWidth, windowHeight;
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;
	bool bTopmost;
	bool isbfs;
	bool isGem;
	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

};
