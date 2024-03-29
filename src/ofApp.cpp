#include <ctime>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include "ofApp.h"
using namespace std;

//화면 해상도에 따라 출력 위치를 조정하기 위한 offset 변수
int offset_x = 0;
int offset_y = 0;

int** maze_graph;							//생성되는 미로 정보를 bit-form(0/1)으로 저장하는 2차원 배열
queue<pair<int, int>> q;					//BFS 탐색을 수행하는 queue
string gen_maze[41];						//Eller's algorithm으로 생성한 미로 정보를 담는 배열
int bfs_dis[50][50] = { 0 };				//미로의 각 좌표별로 BFS_distance를 담는 2차원 배열
pair<int, pair<int, int>> max_heap[2000];	//first에는 BFS_distance, second에는 미로의 좌표를 담는 Max heap
pair<int, pair<int, int>> gems[2000];		//Heap sort의 결과를 받아, 각 Gem의 BFS_Distance를 내림차순으로 저장
int elementCnt = 0;							//Heap sort에 사용되는 원소의 개수 저장

// Animation 관련 변수
float dotScale = 15;				//현재 한 점의 크기
bool dotDwindleFlag = false;		//점 크기를 감소시키기 위한 flag
bool growRapid = false;				//점 크기를 급증시키기 위한 flag
int bounceCnt = 0;					//점이 몇 번 반짝였는지
int startCnt = 0;					//애니메이션이 시작하고 흐른 시간

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Blooming Maze Flower");  // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(57, 62, 70);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isGem = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2,
		(ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	// myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see
	// keyPressed)
	ofSetEscapeQuitsApp(false);

	// Create a menu using ofxWinMenu

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist.
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	// Create a "File" popup menu
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	// Add popup items to the File menu

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	// View popup menu
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bTopmost = false;     // app is topmost
	bFullscreen = false;  // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false);  // Not checked and not auto-check

	menu->SetWindowMenu(); // Set the menu to the window

}  // end Setup

// Menu function
void ofApp::appMenuFunction(string title, bool bChecked) {
	ofFileDialogResult result;
	string filePath;

	// File menu
	if (title == "Exit") {
		ofExit();  // Quit the application
	}

	// Window menu
	if (title == "Full screen") {
		bFullscreen = !bFullscreen;  // Not auto-checked and also used in the
									 // keyPressed function
		doFullScreen(bFullscreen);   // But als take action immediately
	}

}  // end appMenuFunction

//--------------------------------------------------------------
void ofApp::update() {
	if (isGem) {
		startCnt++;
		if (10 < startCnt) {		//점들이 반짝일 시간 간격
			if (bounceCnt > 2) {
				growRapid = true;
			}
			// Bloom 효과
			if (growRapid) dotScale += 3;
			if (dotScale > 70) dotScale += 6;
			if (dotScale > 130) {
				mazeRegen();
			}
			//숨쉬기 효과
			dotScale += 0.4;       // always
			if (dotDwindleFlag) {  // if flag
				dotScale -= 0.81;
			}
			if (dotScale >= 18.2) {  //한번 반짝일 때 얼마만큼 커지는지
				dotDwindleFlag = 1;
			}
			if (dotScale < 15) {
				dotDwindleFlag = 0;
				bounceCnt++;
				startCnt = 0;
				cout << "Bounce count is: " << bounceCnt << endl;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	int i, j;

	//저장된 자료구조를 이용해 미로를 그린다.
	offset_x = (resolution_x - WIDTH * XS) / 2;
	offset_y = (resolution_y - HEIGHT * XS) / 2 - XS;

	if (!isOpen) {
		ofSetColor(236, 155, 59);
		ofPushMatrix();
		ofScale(3, 3);
		myFont.drawString("Press 'S' to start Blooming", 180, 215);
		ofPopMatrix();
	}

	//미로 틀 출력
	ofSetColor(42, 179, 146);
	ofSetLineWidth(5);
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (input[i][j] == '|')
				ofDrawLine(offset_x + j * 30, offset_y + (i - 1) * 30,
					offset_x + j * 30, offset_y + (i + 1) * 30);
			else if (input[i][j] == '-')
				ofDrawLine(offset_x + (j - 1) * 30, offset_y + i * 30,
					offset_x + (j + 1) * 30, offset_y + i * 30);
		}
	}

	//점 출력
	if (isGem) drawGem();

	ofSetColor(247, 215, 22);
	myFont.drawString("Made by SEEWON, GitHub @SEEWON", 15, ofGetHeight() - 20);

}  // end Draw

void ofApp::doFullScreen(bool bFull) {
	// Enter full screen
	if (bFull) {
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
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2,
			(ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

}  // end doFullScreen

void ofApp::doTopmost(bool bTop) {
	if (bTop) {
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
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE);
	}
}  // end doTopmost

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

	if (key == 's' || key == 'S') {
		genMaze();
	}

}  // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

//Eller's algorithm을 적용해 mage를 generate하는 함수이다.
bool ofApp::genMaze() {
	int i, j;
	int N, M;
	int areaCnt = 0;
	srand(time(NULL));

	// N*M, 미로의 크기 설정
	N = 20;
	M = 20;

	//영역, 경계에 관한 2차원 배열 2개 동적 할당
	int** area = (int**)malloc(sizeof(int*) * M);
	for (i = 0; i < M; i++) {
		area[i] = (int*)malloc(sizeof(int) * N);
	}
	int** H_border = (int**)malloc(sizeof(int*) * (M + 1));
	for (i = 0; i < M + 1; i++) {
		H_border[i] = (int*)malloc(sizeof(int) * N);
	}
	int** V_border = (int**)malloc(sizeof(int*) * M);
	for (i = 0; i < M; i++) {
		V_border[i] = (int*)malloc(sizeof(int) * (N + 1));
	}

	//미로의 첫 번째 줄 초기화
	for (j = 0; j < N + 1; j++) {
		V_border[0][j] = 1;
		if (j < N) {
			H_border[0][j] = 1;
			area[0][j] = ++areaCnt;
		}
	}

	//첫 번째 줄 벽뚫기
	for (j = 0; j < N + 1; j++) {
		if (j == 0 || j == N)
			V_border[0][j] = 1;
		else {
			//첫 번째 줄은 무조건 오른쪽 칸과 다름
			int spread = rand() % 2;
			if (spread) {
				V_border[0][j] = 0;
				area[0][j] = area[0][j - 1];
			}
			else
				V_border[0][j] = 1;
		}
	}

	for (i = 0; i < M - 1; i++) {
		int flag = 0;
		for (j = 0; j < N + 1; j++) {
			// H_border 관련 연산(마지막 열 제외)
			if (j < N) {
				int spread;

				//새로운 영역 왔을 땐 flag 초기화
				if (j > 0) {
					if (area[i][j - 1] != area[i][j]) flag = 0;
				}

				//이번이 마지막이라서 무조건 spread해야하는 경우
				if (!flag && j < N - 1) {
					if (area[i][j] != area[i][j + 1])
						spread = 1;
					else
						spread = rand() % 2;
				}
				else if (!flag && j == N - 1)
					spread = 1;
				//아닌 경우 임의로 spread하도록
				else
					spread = rand() % 2;
				if (spread) {
					H_border[i + 1][j] = 0;
					// area[i + 1][j] = area[i][j];
					//이전 값을 갖는 모든 영역 값 바꿔주기
					for (int m = 0; m < M; m++) {
						for (int n = 0; n < N; n++) {
							if (area[m][n] == area[i + 1][j]) area[m][n] = area[i][j];
						}
					}
					flag = 1;
				}
				else {
					H_border[i + 1][j] = 1;
					area[i + 1][j] = ++areaCnt;
				}
			}

			// V_border 관련 연산(마지막 행 제외)
			if (i < M - 2) {
				if (j == 0 || j == N)
					V_border[i + 1][j] = 1;
				else {
					//왼쪽 영역과 다른 경우 임의로 결정
					if (area[i + 1][j - 1] != area[i + 1][j]) {
						int spread = rand() % 2;
						if (spread) {
							V_border[i + 1][j] = 0;
							// area[i + 1][j] = area[i + 1][j - 1];
							//이전 값을 갖는 모든 영역 값 바꿔주기
							for (int m = 0; m < M; m++) {
								for (int n = 0; n < N; n++) {
									if (area[m][n] == area[i + 1][j])
										area[m][n] = area[i + 1][j - 1];
								}
							}
						}
						else
							V_border[i + 1][j] = 1;
					}
					//좌우 인접 영역과 같은 경우 제거X
					else {
						V_border[i + 1][j] = 1;
					}
				}
			}
		}
	}

	//마지막 행의 V_border 관련 연산
	for (j = 0; j < N + 1; j++) {
		if (j == 0 || j == N)
			V_border[M - 1][j] = 1;
		else {
			//왼쪽 영역과 다른 무조건 벽 뚫음
			if (area[M - 1][j - 1] != area[M - 1][j]) {
				V_border[M - 1][j] = 0;
				// area[M - 1][j] = area[M - 1][j - 1];
				//이전 값을 갖는 모든 영역 값 바꿔주기
				for (int m = 0; m < M; m++) {
					for (int n = 0; n < N; n++) {
						if (area[m][n] == area[M - 1][j]) area[m][n] = area[M - 1][j - 1];
					}
				}
			}
			//좌우 인접 영역과 같은 경우 제거X
			else {
				V_border[M - 1][j] = 1;
			}
		}
	}
	//마지막 열의 H_border 관련 연산
	for (j = 0; j < N; j++) {
		H_border[M][j] = 1;
	}

	int vectorCnt = 0;
	//형식에 맞게 저장
	for (i = 0; i < M; i++) {
		for (j = 0; j < N; j++) {
			gen_maze[vectorCnt] += "+";
			if (H_border[i][j])
				gen_maze[vectorCnt] += "-";
			else
				gen_maze[vectorCnt] += " ";
		}
		gen_maze[vectorCnt] += "+";
		vectorCnt++;
		for (j = 0; j < N + 1; j++) {
			if (V_border[i][j])
				gen_maze[vectorCnt] += "|";
			else
				gen_maze[vectorCnt] += " ";
			if (j != N) gen_maze[vectorCnt] += " ";
		}
		vectorCnt++;
	}
	for (j = 0; j < N; j++) {
		gen_maze[vectorCnt] += "+";
		if (H_border[i][j])
			gen_maze[vectorCnt] += "-";
		else
			gen_maze[vectorCnt] += " ";
	}
	gen_maze[vectorCnt] += "+";

	//해당 함수에서 사용한 동적 할당 메모리 해제
	for (i = 0; i < M; i++) {
		free(area[i]);
	}
	free(area);
	for (i = 0; i < M + 1; i++) {
		free(H_border[i]);
	}
	free(H_border);
	for (i = 0; i < M; i++) {
		free(V_border[i]);
	}
	free(V_border);

	// DEBUG CODE
	cout << "Generated a new maze :" << endl;
	for (int it = 0; it < 41; it++) {
		cout << gen_maze[it] << endl;
	}

	setMaze();
	return true;
}

//생성한 미로를 적절한 자료구조로 저장하는 함수
bool ofApp::setMaze() {
	isOpen = 1;
	//새로운 미로 생성 시 기존 큐 초기화
	while (!q.empty()) q.pop();

	HEIGHT = WIDTH = 0;
	input = (char**)malloc(sizeof(char*));  //미로 그리기 위한 정보 저장
	maze_graph = (int**)malloc(sizeof(
		int*));  // BFS 탐색을 위한 미로 정보 저장. 벽은 1, 공간은 0으로 나타냄

	int gen_maze_size = 41;
	cout << "Set a new maze as bit-form:" << endl;
	for (int it = 0; it < gen_maze_size; it++) {
		string line = gen_maze[it];
		//첫 번째 loop에서 WIDTH 정보 저장
		if (it == 0) WIDTH = line.length();

		//새 줄 입력받을 때마다 미로 정보 저장 array size 재할당
		input = (char**)realloc(input, sizeof(char*) * (HEIGHT + 1));
		maze_graph = (int**)realloc(maze_graph, sizeof(int*) * (HEIGHT + 1));
		input[HEIGHT] = (char*)malloc(sizeof(char) * WIDTH);
		maze_graph[HEIGHT] = (int*)malloc(sizeof(int) * WIDTH);
		//하나의 행 읽어서 저장
		for (int i = 0; i < WIDTH; i++) {
			input[HEIGHT][i] = line[i];
			if (line[i] == ' ')
				maze_graph[HEIGHT][i] = 0;
			else
				maze_graph[HEIGHT][i] = 1;
		}

		// maze_graph이 잘 저장되는지 출력하는 Debug code
		for (int i = 0; i < WIDTH; i++) {
			cout << maze_graph[HEIGHT][i];
		}
		cout << endl;
		HEIGHT++;
	}

	//미로 다 읽은 후 HEIGHT*WIDTH만큼 visited 메모리 할당
	visited = (int**)malloc(sizeof(int*) * HEIGHT);
	for (int i = 0; i < HEIGHT; i++)
		visited[i] = (int*)malloc(sizeof(int) * WIDTH);

	//미로 생성 후 좌표별로 BFS_distance를 게산하기 위해 BFS() 호출
	BFS();
	return true;
}

//사용한 메모리를 해제하는 함수
void ofApp::freeMemory() {
	for (int i = 0; i < HEIGHT; i++) {
		free(input[i]);
		free(maze_graph[i]);
	}
	free(input);
	free(maze_graph);
	if (visited) {
		for (int i = 0; i < HEIGHT; i++) free(visited[i]);
		free(visited);
	}
}

//BFS distance와 미로의 좌표를 삽입해 Heap을 구성하는 함수
void ofApp::insertHeap(pair<int, pair<int, int>> insertElement) {
	elementCnt++;
	int idx = elementCnt;

	// max_heap에 Element 삽입
	while (insertElement.first > max_heap[idx / 2].first && idx != 1) {
		max_heap[idx] = max_heap[idx / 2];
		idx /= 2;
	}
	max_heap[idx] = insertElement;
}

//Heap sort를 수행해, 내림차순으로 정렬된 결과를 gems 변수에 저장하는 함수
void ofApp::placeGem() {
	int t = elementCnt;
	for (int i = 0; i < elementCnt; i++) {
		// Configure Gem data
		int bfsD = max_heap[1].first;
		int row = max_heap[1].second.first;
		int col = max_heap[1].second.second;

		gems[i] = make_pair(bfsD, make_pair(row, col));

		pair<int, pair<int, int>> temp = max_heap[t];
		max_heap[t--] = make_pair(0, make_pair(0, 0));

		int parent = 1;
		int child = 2;
		while (child <= t) {
			if ((child < t) && (max_heap[child].first < max_heap[child + 1].first))
				child++;
			if (max_heap[child].first <= temp.first) break;

			max_heap[parent] = max_heap[child];
			parent = child;
			child *= 2;
		}
		max_heap[parent] = temp;
	}
	isGem = 1;
}

//화면에 Gem을 직접 출력하는 함수, BFS distance에 따라 10개 색으로 나누어 출력
void ofApp::drawGem() {
	float D1 = gems[1].first * 0.1;
	float D2 = gems[1].first * 0.2;
	float D3 = gems[1].first * 0.3;
	float D4 = gems[1].first * 0.4;
	float D5 = gems[1].first * 0.5;
	float D6 = gems[1].first * 0.6;
	float D7 = gems[1].first * 0.7;
	float D8 = gems[1].first * 0.8;
	float D9 = gems[1].first * 0.9;
	for (int i = 0; i < elementCnt; i++) {
		int bfsD = gems[i].first;
		int row = gems[i].second.first;
		int col = gems[i].second.second;

		if (bfsD > D9) {
			ofSetColor(179, 57, 57);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D8) {
			ofSetColor(255, 82, 82);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D7) {
			ofSetColor(205, 97, 51);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D6) {
			ofSetColor(255, 121, 63);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D5) {
			ofSetColor(204, 142, 53);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D4) {
			ofSetColor(255, 177, 66);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D3) {
			ofSetColor(204, 174, 98);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D2) {
			ofSetColor(255, 218, 121);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else if (bfsD > D1) {
			ofSetColor(170, 166, 157);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
		else {
			ofSetColor(247, 241, 227);
			ofDrawCircle(offset_x + col * XS, offset_y + row * XS, dotScale);
		}
	}
}

// BFS탐색을 수행해 결과를 자료구조에 저장하는 함수
bool ofApp::BFS() {
	int R_col[4] = { 0, 1, 0, -1 };
	int C_col[4] = { 1, 0, -1, 0 };

	//미로의 네 꼭짓점에 대해 반복
	pair<int, int> start[4] = { make_pair(1, 1), make_pair(1, WIDTH - 2), make_pair(HEIGHT - 2, WIDTH - 2), make_pair(HEIGHT - 2, 1) };
	pair<int, int> target[4] = { make_pair(HEIGHT - 2, WIDTH - 2), make_pair(HEIGHT - 2, 1), make_pair(1, 1), make_pair(1, WIDTH - 2) };

	for (int it = 0; it < 4; it++) {
		//탐색 전 Queue, 방문 여부 초기화
		while (!q.empty()) q.pop();
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				visited[i][j] = 0;
			}
		}

		//BFS 탐색 시작점에 대한 처리
		visited[start[it].first][start[it].second] = 1;
		q.push(make_pair(start[it].first, start[it].second));

		int distance = 1;
		bfs_dis[start[it].first][start[it].second] += distance;
		while (!q.empty()) {
			int curr_R = q.front().first;
			int curr_C = q.front().second;
			distance++;
			q.pop();

			//네 가지 방향에 대해 방문하지 않았다면 queue에 push
			for (int i = 0; i < 4; i++) {
				int next_R = curr_R + R_col[i];
				int next_C = curr_C + C_col[i];
				if (next_R < 0 || next_R >= HEIGHT || next_C < 0 || next_C >= WIDTH)
					continue;  //경계값 체크, 범위 벗어나면 pass

				  //아직 방문하지 않았고, 이동할 수 있는 미로인 경우
				if (!visited[next_R][next_C] && !maze_graph[next_R][next_C]) {
					visited[next_R][next_C] = 1;        //방문 표시
					q.push(make_pair(next_R, next_C));  // enqueue
					// next 좌표가 경계값이 아닌, 칸인 경우에만 경로에 추가함. (벽까지만
					// 가고 칸까지는 가지 않는 경우 제외)
					if (next_R % 2 == 1 && next_C % 2 == 1) {
						bfs_dis[next_R][next_C] += distance;
					}
				}
			}
		}
	}

	//탐색한 결과를 Heap에 삽입하기 위한 함수 호출
	assignBFSDtoHeap();
	return true;
}

//BFS 탐색 결과를 Heap에 삽입하는 함수
void ofApp::assignBFSDtoHeap()
{
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (i % 2 == 1 && j % 2 == 1) {
				insertHeap(make_pair(bfs_dis[i][j], make_pair(i, j)));
			}
		}
	}

	// Gem 배치 함수 호출
	placeGem();
}

//다시 새로운 미로를 생성하기 위한 초기화 및 미로 재생성 함수
void ofApp::mazeRegen() {
	//한 번의 cycle에서 사용한 메모리 초기화
	freeMemory();
	for (int i = 0; i < 41; i++) {
		gen_maze[i] = "";
	}
	for (int i = 0; i < 2000; i++) {
		max_heap[i] = make_pair(0, make_pair(0, 0));
		gems[i] = make_pair(0, make_pair(0, 0));
	}
	memset(bfs_dis, 0, sizeof(bfs_dis));

	//전역 변수 초기화
	elementCnt = 0;
	dotScale = 15;
	dotDwindleFlag = 0;
	startCnt = 0;
	bounceCnt = 0;
	growRapid = false;

	//미로 재생성
	genMaze();
}
