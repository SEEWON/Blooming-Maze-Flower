#include "ofApp.h"
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <ctime>
#include <string>
using namespace std;

int offset_x = 0; int offset_y = 0;

int** maze_graph;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
stack<pair<int, int>> s; //DFS 하는 stack
vector<pair<int, int>> exact_dfs_path; //정확한 경로를 저장하는 vector
vector<pair<pair<int, int>, pair<int, int>>> tried_dfs_path; //시도한 모든 경로를 저장하는 vector

bool isbfs;
queue<pair<int, int>> q; //BFS 하는 queue
vector<pair<int, int>> exact_bfs_path; //정확한 경로를 저장하는 vector
vector<pair<pair<int, int>, pair<int, int>>> tried_bfs_path; //시도한 모든 경로를 저장하는 vector

bool gameStart = false;
vector<string> gen_maze(41);

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(57, 62, 70);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = isbfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
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

	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


// Menu function
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	// File menu
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	// Window menu
	if (title == "Show DFS") {
		isbfs = 0;
		if (isOpen)
			DFS();
		else
			cout << "you must Generate maze first" << endl;

	}

	if (title == "Show BFS") {
		isdfs = 0;
		if (isOpen)
			BFS();
		else
			cout << "you must Generate maze first" << endl;
	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	
	int i, j;

	//저장된 자료구조를 이용해 미로를 그린다.
	offset_x = (1792 - WIDTH * 30) / 2;
	offset_y = (1344 - HEIGHT * 30) / 2 - 30;

	if (!isOpen) {
		ofSetColor(236, 155, 59);
		ofPushMatrix();
		ofScale(3, 3);
		myFont.drawString("Press 'S' to start game", 180, 215);
		ofPopMatrix();
	}

	//미로 틀 출력
	ofSetColor(0, 173, 181);
	ofSetLineWidth(5);
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (input[i][j] == '|')
				ofDrawLine(offset_x + j * 30, offset_y + (i - 1) * 30, offset_x + j * 30, offset_y + (i + 1) * 30);
			else if (input[i][j] == '-')
				ofDrawLine(offset_x + (j - 1) * 30, offset_y + i * 30, offset_x + (j + 1) * 30, offset_y + i * 30);
		}
	}

	//DFS 탐색 경로 출력
	if (isdfs)
	{
		ofSetColor(238, 238, 238);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "you must Generate maze first" << endl;
	}

	//BFS 탐색 경로 출력
	if (isbfs)
	{
		ofSetColor(238, 238, 238);
		ofSetLineWidth(5);
		if (isOpen)
			bfsdraw();
		else
			cout << "you must Generate maze first" << endl;
	}

	ofSetColor(247, 215, 22);
	sprintf(str, "Made by SEEWON, GitHub @SEEWON");
	myFont.drawString(str, 15, ofGetHeight() - 20);

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
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
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
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
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


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
		gameStart = true;
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

bool ofApp::genMaze() 
{
	int i, j;
	int N, M;
	int areaCnt = 0;
	srand(time(NULL));

	//N*M, 미로의 크기 설정
	N = 20; M = 20;

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
	//형식에 맞게 출력
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

	//DEBUG CODE
	cout << "Generated a new maze :" << endl;
	for (int it = 0; it < gen_maze.size();it++) {
		cout << gen_maze[it] << endl;
	}

	setMaze();
	return true;
}

//생성한 미로를 적절한 자료구조로 저장하는 함수
bool ofApp::setMaze()
{
	isOpen = 1;
	//새로운 미로 생성 시 기존 경로 초기화
	isdfs = 0; isbfs = 0;
	exact_dfs_path.clear();	tried_dfs_path.clear();
	exact_bfs_path.clear();	tried_bfs_path.clear();
	while (!s.empty()) s.pop();
	while (!q.empty()) q.pop();

	HEIGHT = WIDTH = 0;
	input = (char**)malloc(sizeof(char*));		//미로 그리기 위한 정보 저장
	maze_graph = (int**)malloc(sizeof(int*));	//BFS-DFS 위한 미로 정보 저장. 벽은 1, 공간은 0으로 나타냄
	
	cout << "Set a new maze as bit-form:" << endl;
	for (int it = 0; it< gen_maze.size(); it++)
	{
		string line = gen_maze[it];
		//첫 번째 loop에서 WIDTH 정보 저장
		if (it ==0 ) WIDTH = line.length();

		//새 줄 입력받을 때마다 미로 정보 저장 array size 재할당
		input = (char**)realloc(input, sizeof(char*) * (HEIGHT + 1));
		maze_graph = (int**)realloc(maze_graph, sizeof(int*) * (HEIGHT + 1));
		input[HEIGHT] = (char*)malloc(sizeof(char)*WIDTH);
		maze_graph[HEIGHT] = (int*)malloc(sizeof(int)*WIDTH);
		//하나의 행 읽어서 저장
		for (int i = 0;i < WIDTH;i++) {
			input[HEIGHT][i] = line[i];
			if (line[i] == ' ') maze_graph[HEIGHT][i] = 0;
			else maze_graph[HEIGHT][i] = 1;
		}

		//maze_graph이 잘 저장되는지 출력하는 Debug code
		for (int i = 0;i < WIDTH;i++) {
			cout << maze_graph[HEIGHT][i];
		}
		cout << endl;
		HEIGHT++;
	}

	//미로 다 읽은 후 HEIGHT*WIDTH만큼 visited 메모리 할당
	visited = (int**)malloc(sizeof(int*)*HEIGHT);
	for (int i = 0;i < HEIGHT;i++) visited[i] = (int*)malloc(sizeof(int)*WIDTH);
	return true;
}

void ofApp::freeMemory() {
	// malloc한 memory를 free해주는 함수
	for (int i = 0;i < HEIGHT;i++) {
		free(input[i]);
		free(maze_graph[i]);
	}
	free(input);
	free(maze_graph);
	if (visited) {
		for (int i = 0;i < HEIGHT;i++) free(visited[i]);
		free(visited);
	}
}

//DFS탐색을 하는 함수
bool ofApp::DFS()
{
	//DFS탐색을 하는 함수 ( 3주차)
	cout << "DFS start" << endl;

	int R_col[4] = { 0, -1, 0, 1 };
	int C_col[4] = { -1, 0, 1, 0 };

	for (int i = 0;i < HEIGHT;i++) {
		for (int j = 0;j < WIDTH;j++) {
			visited[i][j] = 0;
		}
	}

	pair<int, int> target = make_pair(HEIGHT - 2, WIDTH - 2);
	visited[1][1] = 1;
	s.push(make_pair(1, 1));
	while (!s.empty()) {
		int curr_R = s.top().first;
		int curr_C = s.top().second;

		//target 도달 시 break
		if (curr_R == target.first && curr_C == target.second) break;

		int flag = 0;
		//네 가지 방향에 대해 방문하지 않았다면 stack과 tried_dfs_path에 push
		for (int i = 0;i < 4;i++) {
			int next_R = curr_R + R_col[i];
			int next_C = curr_C + C_col[i];
			if (next_R < 0 || next_R >= HEIGHT || next_C < 0 || next_C >= WIDTH) continue;	//경계값 체크, 범위 벗어나면 pass

			if (!visited[next_R][next_C] && !maze_graph[next_R][next_C]) {
				flag = 1;
				visited[next_R][next_C] = 1;
				s.push(make_pair(next_R, next_C));
				//next 좌표가 경계값이 아닌, 칸인 경우에만 경로에 추가함. (벽까지만 가고 칸까지는 가지 않는 경우 제외)
				if (next_R % 2 == 1 && next_C % 2 == 1) {
					tried_dfs_path.push_back(make_pair(make_pair(curr_R, curr_C), make_pair(next_R, next_C)));
				}
			}
		}
		if (!flag) s.pop();
	}

	//Stack에 들어있는 경로 vector에 저장
	while (!s.empty()) {
		int r = s.top().first;
		int c = s.top().second;
		if (r % 2 == 1 && c % 2 == 1) exact_dfs_path.push_back(make_pair(r, c));
		s.pop();
	}
	isdfs = 1;
	return true;
}

//DFS 수행 결과를 그리는 함수
void ofApp::dfsdraw()
{
	//시도한 모든 경로 그리기
	for (int i = 0;i < tried_dfs_path.size();i++) {
		int start_R = tried_dfs_path[i].first.first;
		int start_C = tried_dfs_path[i].first.second;
		int end_R = tried_dfs_path[i].second.first;
		int end_C = tried_dfs_path[i].second.second;

		//현재 경로 상에는 (경계선 -> 도착칸)만 경로로 저장되어 있다.
		//따라서 출발 지점이 경계선이 아닌 (시작칸 -> 도착칸)이 될 수 있도록 시작점을 늘린다.
		if (start_R == end_R) start_C -= end_C - start_C;
		else start_R -= end_R - start_R;

		ofDrawLine(offset_x + start_C * 30, offset_y + start_R * 30, offset_x + end_C * 30, offset_y + end_R * 30);
	}

	//도착 경로 그리기
	ofSetColor(247, 215, 22);
	for (int i = 0;i < exact_dfs_path.size() - 1;i++) {
		int start_R = exact_dfs_path[i].first;
		int start_C = exact_dfs_path[i].second;
		int end_R = exact_dfs_path[i + 1].first;
		int end_C = exact_dfs_path[i + 1].second;
		//Show DFS -> Show BFS -> Show DFS 순으로 메뉴를 선택하는 경우, exact_dfs_path에 시작점-목표점을 한 번에 잇는 경로 생성 버그 예외처리
		if (start_R != end_R && start_C != end_C) continue;

		ofDrawLine(offset_x + start_C * 30, offset_y + start_R * 30, offset_x + end_C * 30, offset_y + end_R * 30);
	}
}

//BFS탐색을 하는 함수
bool ofApp::BFS()
{
	cout << "BFS start" << endl;

	int R_col[4] = { 0, 1, 0, -1 };
	int C_col[4] = { 1, 0, -1, 0 };
	pair<int, int> **parent = (pair<int, int>**)malloc(sizeof(pair<int, int>*)*HEIGHT);
	for (int i = 0;i < HEIGHT;i++) {
		parent[i] = (pair<int, int>*)malloc(sizeof(pair<int, int>)*WIDTH);
	}

	for (int i = 0;i < HEIGHT;i++) {
		for (int j = 0;j < WIDTH;j++) {
			visited[i][j] = 0;
			parent[i][j] = make_pair(0, 0);
		}
	}
	parent[1][1] = make_pair(-1, -1);

	pair<int, int> target = make_pair(HEIGHT - 2, WIDTH - 2);
	visited[1][1] = 1;
	q.push(make_pair(1, 1));
	while (!q.empty()) {
		int curr_R = q.front().first;
		int curr_C = q.front().second;

		//target 도달 시 break
		if (curr_R == target.first && curr_C == target.second) break;
		q.pop();

		//네 가지 방향에 대해 방문하지 않았다면 queue과 tried_bfs_path에 push
		for (int i = 0;i < 4;i++) {
			int next_R = curr_R + R_col[i];
			int next_C = curr_C + C_col[i];
			if (next_R < 0 || next_R >= HEIGHT || next_C < 0 || next_C >= WIDTH) continue;	//경계값 체크, 범위 벗어나면 pass

			if (!visited[next_R][next_C] && !maze_graph[next_R][next_C]) {
				parent[next_R][next_C] = make_pair(curr_R, curr_C);
				visited[next_R][next_C] = 1;
				q.push(make_pair(next_R, next_C));
				//next 좌표가 경계값이 아닌, 칸인 경우에만 경로에 추가함. (벽까지만 가고 칸까지는 가지 않는 경우 제외)
				if (next_R % 2 == 1 && next_C % 2 == 1) {
					tried_bfs_path.push_back(make_pair(make_pair(curr_R, curr_C), make_pair(next_R, next_C)));
				}
			}
		}
	}
	int r = target.first;
	int c = target.second;
	while (r>0 && c>0) {
		exact_bfs_path.push_back(make_pair(r, c));

		if (r % 2 == 1 && c % 2 == 1) {
		}
		int temp_r = parent[r][c].first;
		int temp_c = parent[r][c].second;
		r = temp_r; c = temp_c;
	}

	isbfs = 1;

	//해당 함수에서 사용한 메모리 해제
	for (int i = 0;i < HEIGHT;i++) {
		free(parent[i]);
	}
	free(parent);
	return true;
}

//BFS 수행 결과를 그리는 함수
void ofApp::bfsdraw()
{
	//시도한 모든 경로 그리기
	for (int i = 0;i < tried_bfs_path.size();i++) {
		int start_R = tried_bfs_path[i].first.first;
		int start_C = tried_bfs_path[i].first.second;
		int end_R = tried_bfs_path[i].second.first;
		int end_C = tried_bfs_path[i].second.second;

		//현재 경로 상에는 (경계선 -> 도착칸)만 경로로 저장되어 있다.
		//따라서 출발 지점이 경계선이 아닌 (시작칸 -> 도착칸)이 될 수 있도록 시작점을 늘린다.
		if (start_R == end_R) start_C -= end_C - start_C;
		else start_R -= end_R - start_R;

		ofDrawLine(offset_x + start_C * 30, offset_y + start_R * 30, offset_x + end_C * 30, offset_y + end_R * 30);
	}

	//도착 경로 그리기
	ofSetColor(247, 215, 22);
	for (int i = 0;i < exact_bfs_path.size() - 1;i++) {
		int start_R = exact_bfs_path[i].first;
		int start_C = exact_bfs_path[i].second;
		int end_R = exact_bfs_path[i + 1].first;
		int end_C = exact_bfs_path[i + 1].second;
		//여러 메뉴 번갈아서 선택 시, exact_bfs_path에 시작점-목표점을 한 번에 잇는 경로 생성 버그 예외처리
		if (start_R != end_R && start_C != end_C) continue;

		ofDrawLine(offset_x + start_C * 30, offset_y + start_R * 30, offset_x + end_C * 30, offset_y + end_R * 30);
	}
}
