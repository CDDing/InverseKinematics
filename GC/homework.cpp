
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <chrono>
#include "IK.h"
using namespace std;
GLuint currentProgram;
GLuint programID;
GLuint programID2;
std::chrono::system_clock::time_point starttime; std::chrono::milliseconds mili; std::chrono::system_clock::time_point stoptime;
float kx = 0, ky = 0, kz = 0;
bool isortho = false, isstop = false, rotate1 = true, rotate2 = false, rotate3 = false;
float theta = 100;
float rotatetheta1 = 0;
float rotatetheta2 = 0;
float wheel = 1, dragx = 1, dragy = 1, dragz = 1;
int mouse_prev_x = 0, mouse_prev_y = 0;
int mouse_dx = 0, mouse_dy = 0;
float targetx=0, targety=0, targetz=0;
BoneTree* tree;
void initBoneTree() {
	Bone* Root = new Bone(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
	tree = new BoneTree(Root);
	for (int i = 0; i < 19; i++) {
		Bone* bone = new Bone(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
		tree->pushBone(bone);
	}
}

float GetRadianBetweenVector(glm::vec3 a, glm::vec3 b) {
	float lengthab = glm::length(a) * glm::length(b);

	return glm::acos(glm::dot(a, b) / lengthab);
}

void setVec3(const GLuint ID, const std::string& name, const glm::vec3& value)
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void setMat4(const GLuint ID, const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	os << "glm::vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}
	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open())
	{
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}
	//Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	GLuint positionAttribute = glGetAttribLocation(ProgramID, "vtxPosition");
	GLuint colorAttribute = glGetAttribLocation(ProgramID, "a_Color");
	GLuint NormalAttribute = glGetAttribLocation(ProgramID, "inNormal");
	//GLuint transAttribute = glGetUniformLocation(ProgramID, "trans");
	GLuint Buffers[10];

	GLfloat vertices[] = {
	0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,  -0.5f,-0.5f, 0.5f,   0.5f,-0.5f, 0.5f,  //윗면
						0.5f, 0.5f, 0.5f,   0.5f,-0.5f, 0.5f,   0.5f,-0.5f,-0.5f,   0.5f, 0.5f,-0.5f,//전면  
						0.5f, 0.5f, 0.5f,   0.5f, 0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,  -0.5f, 0.5f, 0.5f,  //오른면
					   -0.5f, 0.5f, 0.5f,  -0.5f, 0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f,-0.5f, 0.5f, //후면
					   -0.5f,-0.5f,-0.5f,   0.5f,-0.5f,-0.5f,   0.5f,-0.5f, 0.5f,  -0.5f,-0.5f, 0.5f,   //왼면
						0.5f,-0.5f,-0.5f,  -0.5f,-0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,   0.5f, 0.5f,-0.5f,//아랫면
						0.0f,0.0f,2.0f //꼭짓점
	};

	GLfloat Normals[] = {
		0,0,1.0f, 0,0,1.0f, 0,0,1.0f, 0,0,1.0f,
		1.0f,0,0, 1.0f,0,0, 1.0f,0,0, 1.0f,0,0,
		0,1.0f,0, 0,1.0f,0, 0,1.0f,0, 0,1.0f,0,
		-1.0f,0,0, -1.0f,0,0, -1.0f,0,0, -1.0f,0,0,
		0,-1.0f,0, 0,-1.0f,0, 0,-1.0f,0, 0,-1.0f,0,
		0,0,-1.0f, 0,0,-1.0f, 0,0,-1.0f, 0,0,-1.0f,
		0,0,1.0f,
	};
	GLfloat colors[] = { 1.0f, 0, 0,   1.0f, 0, 0,1.0f, 0, 0,1.0f, 0, 0,
		1.0f, 0, 0,   1.0f, 0, 0,1.0f, 0, 0,1.0f, 0, 0,
		1.0f, 0, 0,   1.0f, 0, 0,1.0f, 0, 0,1.0f, 0, 0,
		1.0f, 0, 0,   1.0f, 0, 0,1.0f, 0, 0,1.0f, 0, 0,
		1.0f, 0, 0,   1.0f, 0, 0,1.0f, 0, 0,1.0f, 0, 0,
		1.0f, 0, 0,   1.0f, 0, 0,1.0f, 0, 0,1.0f, 0, 0,
		1.0f, 0, 0,
	};

	glGenBuffers(5, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(positionAttribute);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(colorAttribute);
	/*
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Light), Light, GL_STATIC_DRAW);
	glVertexAttribPointer(LightAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(LightAttribute);
	*/
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
	glVertexAttribPointer(NormalAttribute, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
	glEnableVertexAttribArray(NormalAttribute);
	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	}
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
void viewMatrix(GLfloat* m) {
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			printf("%.2f ", m[a * 4 + b]);
		}
		cout << endl << endl;
	}
}

void drawBoneCube(GLuint* cubeIndices, Bone* bone) {
	std::chrono::system_clock::time_point nowtime = std::chrono::system_clock::now();
	if (!isstop) {
		mili = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime - starttime);
		stoptime = std::chrono::system_clock::now();
	}//cout << mili.count()<<endl;
	double milic = mili.count() / 10;
	//glUniform3fv(LightAttribute, 1, Light);
	glm::mat4 transmatrix, rotationmatrix, worldmatrix, viewmatrix, projectionmatrix, worldinvmatrix;

	float rotatex = 0;
	float rotatey = 0;
	float rotatez = -1;
	float zNear = 0.1f;
	float zFar = 100.0f;
	float aspect = 1;
	float fov = theta * 3.141592 / 180;

	viewmatrix = glm::mat4(1.0f);
	if (isortho) {
		glm::mat4 orthoMatrix = glm::ortho(-10.0f * wheel, 10.0f * wheel, -10.0f * wheel, 10.0f * wheel, -10.0f * wheel, 10.0f * wheel);
		projectionmatrix = orthoMatrix;
	}
	else {

		glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(theta), aspect, zNear, zFar);
		projectionmatrix = perspectiveMatrix;
		viewmatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	}
	
	//Bone 위치만큼 이동

	glm::vec3 BoneDirection = glm::normalize(bone->GetEnd() - bone->GetStart());
	
	transmatrix = glm::translate(glm::mat4(1.0f), bone->GetStart() + 0.5f*(bone->GetEnd()-bone->GetStart()));
	transmatrix = glm::rotate(transmatrix, 
		GetRadianBetweenVector(glm::vec3(0,1,0),BoneDirection),
		glm::cross(glm::vec3(0,1,0),BoneDirection));//외적해서 해당 벡터에 대해 수직인 각도로 회전
	transmatrix = glm::scale(transmatrix, glm::vec3(1.0f, bone->length, 1.0f));
	
	rotationmatrix = glm::mat4(1.0f);
	//rotationmatrix = glm::rotate(glm::mat4(1.0f), glm::radians(float(-mouse_dx)), glm::vec3(0.0f, 1.0f, 0.0f));
	//rotationmatrix = glm::rotate(rotationmatrix, glm::radians(float(-mouse_dy)), glm::vec3(1.0f, 0.0f, 0.0f));

	setMat4(currentProgram, "projmat", projectionmatrix);
	setMat4(currentProgram, "viewMat", viewmatrix);
	//glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projectionmatrix[0][0]);
	//glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewmatrix[0][0]);
	if (rotate1) {//회전 중심 큐브 A
		worldmatrix = transmatrix * rotationmatrix;
	}
	else if (rotate2) {//회전 중심 세계좌표계
		worldmatrix = rotationmatrix * transmatrix;

	}
	else if (rotate3) {//회전 중심 카메라좌표계
		worldmatrix = transmatrix;
		glm::mat4 newViewMat = rotationmatrix * viewmatrix;
		setMat4(currentProgram, "viewMat", newViewMat);
		//glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &newViewMat[0][0]);
	}
	setMat4(currentProgram, "worldMat", worldmatrix);
	//glUniformMatrix4fv(worldMatrixLoc, 1, GL_FALSE, &worldmatrix[0][0]);
	worldinvmatrix = glm::inverse(worldmatrix);
	worldinvmatrix = glm::transpose(worldinvmatrix);
	setMat4(currentProgram, "worldinv", worldinvmatrix);
	//glUniformMatrix4fv(worldinvMatrixLoc, 1, GL_FALSE, &worldinvmatrix[0][0]);

	//gluPerspective(fov,aspect,zNear,zFar);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cubeIndices);


}

glm::vec3 lerpmouse(int mouse_dx, int mouse_dy) {
	float offset = 0.075f;


	targetx = offset * mouse_dx;
	targety = offset * -mouse_dy;
	return offset * glm::vec3(mouse_dx, -mouse_dy, 0);
}
void drawTargetCube(GLuint* cubeIndices) {

	std::chrono::system_clock::time_point nowtime = std::chrono::system_clock::now();
	if (!isstop) {
		mili = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime - starttime);
		stoptime = std::chrono::system_clock::now();
	}//cout << mili.count()<<endl;
	double milic = mili.count() / 10;
	//glUniform3fv(LightAttribute, 1, Light);
	glm::mat4 transmatrix, rotationmatrix, worldmatrix, viewmatrix, projectionmatrix, worldinvmatrix;

	float rotatex = 0;
	float rotatey = 0;
	float rotatez = -1;
	float zNear = 0.1f;
	float zFar = 100.0f;
	float aspect = 1;
	float fov = theta * 3.141592 / 180;

	viewmatrix = glm::mat4(1.0f);
	if (isortho) {
		glm::mat4 orthoMatrix = glm::ortho(-10.0f * wheel, 10.0f * wheel, -10.0f * wheel, 10.0f * wheel, -10.0f * wheel, 10.0f * wheel);
		projectionmatrix = orthoMatrix;
	}
	else {

		glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(theta), aspect, zNear, zFar);
		projectionmatrix = perspectiveMatrix;
		viewmatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	}
	//viewMatrix(glm::value_ptr(viewmatrix));

	transmatrix = glm::translate(glm::mat4(1.0f), lerpmouse(mouse_dx,mouse_dy));
	transmatrix = glm::rotate(transmatrix, glm::radians(float(milic)), glm::vec3(1.0f, 1.0f, 1.0f));
	transmatrix = glm::scale(transmatrix, glm::vec3(1.0f, 1.0f, 1.0f));
	rotationmatrix = glm::rotate(glm::mat4(1.0f), glm::radians(float(-mouse_dx)), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationmatrix = glm::rotate(rotationmatrix, glm::radians(float(-mouse_dy)), glm::vec3(1.0f, 0.0f, 0.0f));

	setMat4(currentProgram, "projmat", projectionmatrix);
	setMat4(currentProgram, "viewMat", viewmatrix);
	//glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projectionmatrix[0][0]);
	//glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewmatrix[0][0]);
	if (rotate1) {//회전 중심 큐브 A
		worldmatrix = transmatrix * rotationmatrix;
	}
	else if (rotate2) {//회전 중심 세계좌표계
		worldmatrix = rotationmatrix * transmatrix;
	}
	else if (rotate3) {//회전 중심 카메라좌표계
		worldmatrix = transmatrix;
		glm::mat4 newViewMat = rotationmatrix * viewmatrix;
		setMat4(currentProgram, "viewMat", newViewMat);
		//glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &newViewMat[0][0]);
	}
	setMat4(currentProgram, "worldMat", worldmatrix);
	//glUniformMatrix4fv(worldMatrixLoc, 1, GL_FALSE, &worldmatrix[0][0]);
	worldinvmatrix = glm::inverse(worldmatrix);
	worldinvmatrix = glm::transpose(worldinvmatrix);
	setMat4(currentProgram, "worldinv", worldinvmatrix);
	//glUniformMatrix4fv(worldinvMatrixLoc, 1, GL_FALSE, &worldinvmatrix[0][0]);

	//gluPerspective(fov,aspect,zNear,zFar);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, cubeIndices);


}
void renderScene(void)
{


	//Clear all pixels
	glClearDepth(1.0);
	//glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//Let's draw something here
	GLuint cubeTargetIndices[] = {
	0, 1, 2,   2, 3, 0,
	 4, 5, 6,   6, 7, 4,
	8, 9,10,  10,11, 8,
	12,13,14,  14,15,12,
	16,17,18,  18,19,16,
	20,21,22,  22,23,20
	};

	GLuint BoneIndices[] = {
	20, 21, 22, 20, 22, 23, // Using the original indices
	20, 21, 24, 21, 22, 24, // Including the new vertex
	22, 23, 24, 23, 20, 24  // Including the new vertex
	};
	drawTargetCube(cubeTargetIndices);
	Bone* tmp = tree->GetRoot();
	IK ik(tree);
	ik.target = glm::vec3(targetx, targety, targetz);
	ik.solve();
	while(tmp){
		drawBoneCube(cubeTargetIndices,tmp);
		tmp = tmp->GetChild();
	}
	setVec3(currentProgram, "DLightDir", glm::vec3(sin(double(rotatetheta1)), 3.0f, cos(double(rotatetheta1))));
	setVec3(currentProgram, "PLightPos", glm::vec3(5 * sin(double(rotatetheta2)), 10.5f, 5 * cos(double(rotatetheta2))));

	//Double buffer
	glutSwapBuffers();
}

void init()
{
	//initilize the glew and check the errors.
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
	}

	//select the background color
	glClearColor(1.0, 1.0, 1.0, 1.0);

}

void doTimer(int i) {
	glutPostRedisplay();
	glutTimerFunc(10, doTimer, 1);
}
void mySpecialKeyboard(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		theta -= 1.0f;
	}
	else if (key == GLUT_KEY_RIGHT) {
		theta += 1.0f;
	}
}
void myMouseDrag(int x, int y) {
	mouse_dx += x - mouse_prev_x;
	mouse_dy += y - mouse_prev_y;
	

	mouse_prev_x = x;
	mouse_prev_y = y;
}
void myMouseWheel(int button, int dir, int x, int y) {
	if (button == 3) {//아래
		wheel += 0.1f;
		cout << "배율" << wheel << endl;
	}
	else if (button == 4) {//위
		wheel -= 0.1f;
		cout << "배율" << wheel << endl;
	}
	else if (button == GLUT_LEFT_BUTTON) {
		if (dir == GLUT_UP) {
			cout << "마우스 뗌" << endl;
			mouse_dx = 0;
			mouse_dy = 0;
		}
	}
}
void myKeyboard(unsigned char i, int x, int y) {
	cout << i << endl;
	if (i == 'q') {
		rotate1 = true;
		rotate2 = false;
		rotate3 = false;
	}
	else if (i == 'w') {
		rotate1 = false;
		rotate2 = true;
		rotate3 = false;
	}
	else if (i == 'e') {
		rotate1 = false;
		rotate2 = false;
		rotate3 = true;
	}
	else if (i == 's') {
		ky += 0.1;
	}
	else if (i == 'x') {
		ky -= 0.1;
	}
	else if (i == 'z') {
		kx -= 0.1;
	}
	else if (i == 'c') {
		kx += 0.1;
	}
	else if (i == 'o') {
		isortho = true;
	}
	else if (i == 'p') {
		isortho = false;
	}
	else if (i == 'p') {
		isortho = false;
	}
	else if (i == 'm') {
		rotatetheta1 += 0.1;
	}
	else if (i == 'n') {
		rotatetheta1 -= 0.1;
	}
	else if (i == 'k') {
		rotatetheta2 += 0.1;
		cout << 5 * sin(double(rotatetheta2)) << 10.5f << 5 * cos(double(rotatetheta2));
	}
	else if (i == 'j') {
		rotatetheta2 -= 0.1;
		cout << 5 * sin(double(rotatetheta2)) << 10.5f << 5 * cos(double(rotatetheta2));
	}
	else if (i == '1') {
		currentProgram = programID;
	}
	else if (i == '2') {
		currentProgram = programID2;
	}
	else if (i == ' ') {
		isstop = !isstop;
		if (!isstop) {
			starttime += std::chrono::system_clock::now() - stoptime;
		}
	}
	glUseProgram(currentProgram);
}
int main(int argc, char** argv)
{
	starttime = std::chrono::system_clock::now();
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//These two functions are used to define the position and size of the window.
	glutInitWindowPosition(1000, 1000);
	glutInitWindowSize(1000, 1000);
	//This is used to define the name of the window.
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	initBoneTree();
	//glutMouseFunc(myMouse);
	//3.
	programID = LoadShaders("VertexShader_Gouraud.txt", "FragmentShader_Gouraud.txt");
	programID2 = LoadShaders("VertexShader_Phong.txt", "FragmentShader_Phong.txt");
	currentProgram = programID2;
	glUseProgram(currentProgram);

	glutDisplayFunc(renderScene);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecialKeyboard);
	glutMotionFunc(myMouseDrag);
	glutMouseFunc(myMouseWheel);
	glutTimerFunc(10, doTimer, 1);
	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}