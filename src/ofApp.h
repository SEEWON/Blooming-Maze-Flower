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

	//openFrameworks�� base �Լ����̴�.
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
	bool genMaze(); //Eller's algorithm�� ������ mage�� generate�ϴ� �Լ��̴�.
	bool setMaze(); //������ �̷θ� ������ �ڷᱸ���� �����ϴ� �Լ��̴�.
	void freeMemory(); //����� �޸𸮸� �����ϴ� �Լ��̴�.
	void insertHeap(pair<int, pair<int, int>> insertElement); //BFS distance�� �̷��� ��ǥ�� ������ Heap�� �����ϴ� �Լ��̴�.
	void placeGem(); //Heap sort�� ������, ������������ ���ĵ� ����� gems ������ �����ϴ� �Լ��̴�.
	void drawGem(); //ȭ�鿡 Gem�� ���� ����ϴ� �Լ��̴�. BFS distance�� ���� 10�� ������ ������ ����Ѵ�.
	bool BFS(); // BFSŽ���� ������ ����� �ڷᱸ���� �����ϴ� �Լ��̴�.
	void assignBFSDtoHeap(); //BFS Ž�� ����� Heap�� �����ϴ� �Լ�
	void mazeRegen(); //�ٽ� ���ο� �̷θ� �����ϱ� ���� �ʱ�ȭ �� �̷� ����� �Լ�

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
	bool isbfs;
	bool isGem;
	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

};
