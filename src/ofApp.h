#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h"

//�ػ� ����

//2560*1600 ���÷��� ����
#define XS 30
#define resolution_x 1792
#define resolution_y 1344

//#define XS 20
//#define resolution_x 1024
//#define resolution_y 768

class ofApp : public ofBaseApp {

public:

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
	bool genMaze();
	bool setMaze();
	void freeMemory();
	void insertHeap(pair<int, pair<int, int>> insertElement);
	void placeGem();
	void drawGem();
	bool BFS();
	void bfsdraw();

	int HEIGHT;//�̷��� ����
	int WIDTH;//�̷��� �ʺ�
	char** input;//�ؽ�Ʈ ������ ��� ������ ��� ������ �迭�̴�.
	int** visited;//�湮���θ� ������ ����
	int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
	int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
	int k;
	int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
	int isDFS;//DFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
	int isBFS;//BFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
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
	int isbfs;
	bool isGem;
	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

};
