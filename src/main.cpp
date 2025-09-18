// OŰ �ٷ� ���� (���߿� ��������)
// PŰ ���� (���߿� ���� ����)
// �ٷ� ������ ���� ���� �����ϴ°Ŵ� ���ΰ���.

// ����Ű A, S ,D, W, C(�ɱ�), X(��������), SpaceBar(�ö󰡱�)


#include <GL/glut.h>
#include <GL/glu.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<int> frameIndices; // �� ����� ���� �������� ����

struct CameraState {
    float cameraX, cameraY, cameraZ;
    float lookX, lookY, lookZ;
};

std::vector<CameraState> lastCameraPositions; // �� ����� ������ ��ġ�� ������ ����
std::vector<std::vector<CameraState>> cameraPaths;
int currentPathIndex = 0;
int currentFrame = 0;

// �׳� ���� ��ư
bool startButtonPressed = false;

// ���� ���� ������
bool isEarthquake = false;         // ���� ȿ���� Ȱ��ȭ�Ǿ����� ����
float shakeIntensity = 0.1f;       // ������ ���� (���� Ŀ���� ������ ������)
float shakeDuration = 0.0f;       // ������ ���� �ð�
float shakeTime = 6.0f;           // ������ ���� �ð� (��)
bool earthquakeFinished = false;   // ���� ȿ���� �������� ����

// 1��Ī ���� Ȱ��ȭ ����
bool isFirstPersonActive = false; // 1��Ī ������ Ȱ��ȭ �Ǿ����� ����

// ī�޶� ����
float cameraX = -5.0f, cameraY = 1.1f, cameraZ = -5.3f;
float yaw = 0.0f, pitch = 0.0f;
float moveSpeed = 0.05f, mouseSensitivity = 0.2f;
float lookX = 0.0f, lookY = 0.0f, lookZ = 1.0f; // �ʱ�ȭ�� lookZ
float rightX = 1.0f, rightZ = 0.0f; // �ʱ�ȭ�� rightX

// �� �� �߷� ���� ����
const float groundHeight = 0.8f;
float cameraVelocityY = 0.0f;
bool isFalling = false;

// ������ ��ũ���� ���� ����
bool isJumping = false;
bool isCrouching = false;
const float jumpHeight = 1.0f;
const float crouchHeight = 0.6f;

// ������ ũ��
int windowWidth = 1280, windowHeight = 720;
bool isWarping = false; // ���콺 ���� �̵� ���� �÷���

// Ű ���� �迭
bool keys[256] = { false };

// ���콺 ������ ��ġ
int lastX = windowWidth / 2, lastY = windowHeight / 2;

// ī�޶� ���� ���� �Լ�
void saveCameraPath(float cameraX, float cameraY, float cameraZ,
    float lookX, float lookY, float lookZ) {
    std::ofstream outFile("camera_path.txt", std::ios::app); // append ���� ����
    if (outFile.is_open()) {
        // ī�޶� ��ġ�� ������ �� �ٿ� ���
        outFile << cameraX << " " << cameraY << " " << cameraZ << " "
            << lookX << " " << lookY << " " << lookZ << std::endl;
        outFile.close();
    }
    else {
        std::cout << "���� ���� ����!" << std::endl;
    }
}

// ī�޶� ��ġ�� ������ �ǽð����� ����ϴ� �Լ�
void recordCameraPath() {
    saveCameraPath(cameraX, cameraY, cameraZ, lookX, lookY, lookZ); // ī�޶� ���� ����
}

// ī�޶� ��θ� ���󰡴� �Լ�
void followCameraPath(const std::vector<CameraState>& cameraPath) {
    if (currentFrame < cameraPath.size()) {
        CameraState state = cameraPath[currentFrame];
        cameraX = state.cameraX;
        cameraY = state.cameraY;
        cameraZ = state.cameraZ;
        lookX = state.lookX;
        lookY = state.lookY;
        lookZ = state.lookZ;
        currentFrame++;
    }
    else {
        currentFrame = cameraPath.size();  // ��� ���� �����ϸ� �� �̻� �̵����� ����
    }
}


// ī�޶� ���� ������Ʈ
void updateCameraDirection() {
    lookX = cosf(yaw) * cosf(pitch);
    lookY = sinf(pitch);
    lookZ = sinf(yaw) * cosf(pitch);
    rightX = cosf(yaw + 3.14159f / 2.0f);
    rightZ = sinf(yaw + 3.14159f / 2.0f);
}

float defaultCameraY = 1.1f; // �⺻ ī�޶� ����
float cameraYOriginal = defaultCameraY;  // �ʱ� ī�޶� Y ���� ���� (�⺻��)

// X Ű�� ī�޶� �̵� (�Ʒ��� ��������)
void processInput() {
    float forwardX = lookX * moveSpeed;
    float forwardZ = lookZ * moveSpeed;

    if (keys['w']) { cameraX += forwardX; cameraZ += forwardZ; }
    if (keys['s']) { cameraX -= forwardX; cameraZ -= forwardZ; }
    if (keys['a']) { cameraX -= rightX * moveSpeed; cameraZ -= rightZ * moveSpeed; }
    if (keys['d']) { cameraX += rightX * moveSpeed; cameraZ += rightZ * moveSpeed; }

    if (keys[' ']) { // �����̽��� (���� �ö󰡱�)
        cameraY += moveSpeed; // ī�޶� ���� �̵�
    }

    if (keys['x']) { // X Ű (�Ʒ��� ��������)
        cameraY -= moveSpeed; // ī�޶� �Ʒ��� �̵�
    }

    if (keys['c']) { // Crouch (���帮��)
        if (!isJumping && !isCrouching) {
            isCrouching = true;
            cameraYOriginal = cameraY; // ���� Y���� �������� ����
            cameraY -= (defaultCameraY - crouchHeight); // ���� Y������ ���̸� ���� (���帮��)
        }
    }
    else if (isCrouching) { // Stand up (�Ͼ)
        isCrouching = false;
        cameraY = cameraYOriginal; // ���帮�� ���� �ִ� Y������ ���ư���
    }
}

// Ű���� �Է� ó��
void keyboardDown(unsigned char key, int x, int y) {
    if (key == 27) exit(0);  // EscapeŰ ����
    if (key == 'p' && !earthquakeFinished) {  // pŰ�� ������ �� ���� ����
        isEarthquake = true; // ���� ����
        shakeDuration = shakeTime; // ������ ���� �ð� ����
    }
    if (key == 'o') {  // oŰ�� ������ �� ���� ��ư
        startButtonPressed = true; // ���� ��ư Ȱ��ȭ
    }
    keys[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

// ���콺 �Է� ó��
void mouseMotion(int x, int y) {
    if (isWarping) return;

    int deltaX = x - lastX;
    int deltaY = y - lastY;

    yaw += deltaX * mouseSensitivity * 0.01f;
    pitch -= deltaY * mouseSensitivity * 0.01f;

    if (pitch > 1.5f) pitch = 1.5f;
    if (pitch < -1.5f) pitch = -1.5f;

    if (yaw > 3.14159f * 2) yaw -= 3.14159f * 2;
    if (yaw < -3.14159f * 2) yaw += 3.14159f * 2;

    updateCameraDirection();

    isWarping = true;
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
    lastX = windowWidth / 2;
    lastY = windowHeight / 2;
    isWarping = false;
}

// ���� ȿ�� ���� �Լ�
void applyEarthquakeEffect() {
    if (isEarthquake) {
        // ������ ��ġ �̵� (x, y, z ����)
        float randomOffsetX = (rand() % 1000 - 500) / 1000.0f * shakeIntensity;
        float randomOffsetY = (rand() % 1000 - 500) / 1000.0f * (shakeIntensity / 2); // Y�� ���� ����
        float randomOffsetZ = (rand() % 1000 - 500) / 1000.0f * shakeIntensity;

        // ���� ī�޶� ��ġ�� ���� �߰�
        cameraX += randomOffsetX;
        cameraY += randomOffsetY;
        cameraZ += randomOffsetZ;

        // ���� ���� �ð� ����
        shakeDuration -= 0.01f; // ���� �ð��� ���ҽ�Ŵ
        if (shakeDuration <= 0) {
            isEarthquake = false;      // ���� ȿ�� ����
            earthquakeFinished = true; // ���� ����
        }
    }
}


// ũ�ν���� �׸���
void drawCrosshair() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    glVertex2f((GLfloat)windowWidth / 2 - 10, (GLfloat)windowHeight / 2);
    glVertex2f((GLfloat)windowWidth / 2 + 10, (GLfloat)windowHeight / 2);
    glVertex2f((GLfloat)windowWidth / 2, (GLfloat)windowHeight / 2 - 10);
    glVertex2f((GLfloat)windowWidth / 2, (GLfloat)windowHeight / 2 + 10);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ���� ������Ʈ �� ȭ�� ����
void idle() {
    processInput();
    glutPostRedisplay();
}

// ������ ũ�� ���� ó��
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// ��� �� �׸��� (��ü ��ȯ ����)
void drawPerson(float x, float y, float z, float rx, float ry, float rz) {
    glPushMatrix();

    // ��ü �ι� ��ġ �� ȸ��
    glTranslatef(x, y, z);
    glRotatef(rx, 1.0f, 0.0f, 0.0f);
    glRotatef(ry, 0.0f, 1.0f, 0.0f);
    glRotatef(rz, 0.0f, 0.0f, 1.0f);

    // �Ӹ�
    glPushMatrix();
    glColor3f(1.0f, 0.8f, 0.6f); // �Ǻλ�
    glTranslatef(0.0f, 0.8f, 0.0f); // �Ӹ� ��ġ
    glutSolidSphere(0.2f, 20, 20); // �Ӹ� ũ��
    glPopMatrix();

    // ����
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f); // �Ķ��� ����
    glTranslatef(0.0f, 0.5f, 0.0f); // ���� ��ġ
    glScalef(0.3f, 0.5f, 0.2f); // ���� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // �ٸ� (����)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f); // ������ ����
    glTranslatef(-0.1f, 0.1f, 0.0f); // ���� �ٸ� ��ġ
    glScalef(0.1f, 0.4f, 0.1f); // ���� �ٸ� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // �ٸ� (������)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.1f, 0.1f, 0.0f); // ������ �ٸ� ��ġ
    glScalef(0.1f, 0.4f, 0.1f); // ������ �ٸ� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // �� (����)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f); // �Ķ��� ��
    glTranslatef(-0.25f, 0.6f, 0.0f); // ���� �� ��ġ
    glScalef(0.1f, 0.3f, 0.1f); // ���� �� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // �� (������)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(0.25f, 0.6f, 0.0f); // ������ �� ��ġ
    glScalef(0.1f, 0.3f, 0.1f); // ������ �� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}


void loadCameraPaths() {
    std::vector<std::string> filenames = {
        "camera_path_1.txt",
        "camera_path_2.txt",
        "camera_path_3.txt",
        "camera_admin.txt"
    };

    for (const auto& filename : filenames) {
        std::ifstream file(filename);
        std::vector<CameraState> path;
        float x, y, z, lx, ly, lz;
        while (file >> x >> y >> z >> lx >> ly >> lz) {
            path.push_back({ x, y, z, lx, ly, lz });
        }

        if (!path.empty()) {
            // �����: ù ������ ���
            const auto& firstFrame = path.front();
            std::cout << "File: " << filename
                << " First Frame - Position: ("
                << firstFrame.cameraX << ", " << firstFrame.cameraY << ", " << firstFrame.cameraZ << ")"
                << " LookAt: (" << firstFrame.lookX << ", " << firstFrame.lookY << ", " << firstFrame.lookZ << ")"
                << std::endl;

            cameraPaths.push_back(path);
            frameIndices.push_back(0);  // �ʱ� ������ ����

            // ��ΰ� ���� �� ������ �������� ��ġ�� ���
            CameraState lastFrame = path.back();  // ����� ������ ������
            lastCameraPositions.push_back(lastFrame); // ������ ��ġ�� ����
        }
        else {
            std::cerr << "��� ������ �ҷ����� ���߽��ϴ�: " << filename << std::endl;
        }
        file.close();
    }
}


//===================================================          =====================================================//
//=================================================== ���ǰ�� =====================================================//
//===================================================          =====================================================//

void drawArrow2D(float baseX, float baseY, float length, float width) {
    float headLength = length * 0.3f; // ȭ��ǥ �Ӹ� ����
    float bodyLength = length - headLength; // ȭ��ǥ ��ü ����
    float halfWidth = width / 2.0f; // ��ü ���� ����

    // ȭ��ǥ ��ü (���簢��)
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f); // �ʷϻ�
    glVertex3f(baseX, baseY - halfWidth, 0.0f);                // ���� �Ʒ�
    glVertex3f(baseX + bodyLength, baseY - halfWidth, 0.0f);   // ������ �Ʒ�
    glVertex3f(baseX + bodyLength, baseY + halfWidth, 0.0f);   // ������ ��
    glVertex3f(baseX, baseY + halfWidth, 0.0f);                // ���� ��
    glEnd();

    // ȭ��ǥ �Ӹ� (�ﰢ��)
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.8f, 0.0f); // ��ο� �ʷϻ�

    // �Ʒ��� �ﰢ�� ��
    glVertex3f(baseX + bodyLength, baseY - width, 0.0f);       // �Ʒ��� ��
    glVertex3f(baseX + bodyLength + headLength, baseY, 0.0f);  // ȭ��ǥ ��
    glVertex3f(baseX + bodyLength, baseY + width, 0.0f);       // ���� ��

    // ���� �ﰢ�� �� (�ݴ�� ����� �߰�)
    glVertex3f(baseX + bodyLength, baseY + width, 0.0f);       // ���� ��
    glVertex3f(baseX + bodyLength + headLength, baseY, 0.0f);  // ȭ��ǥ ��
    glVertex3f(baseX + bodyLength, baseY - width, 0.0f);       // �Ʒ��� ��

    glEnd();
}


void drawBoldRedX(float centerX, float centerY, float size, float lineWidth) {
    float halfSize = size / 2.0f; // X ǥ���� �� ũ��

    glColor3f(1.0f, 0.0f, 0.0f); // ������
    glLineWidth(lineWidth); // ���� �β� ����

    glBegin(GL_LINES);
    // �밢�� 1
    glVertex3f(centerX - halfSize, centerY - halfSize, 0.0f); // ���� �Ʒ�
    glVertex3f(centerX + halfSize, centerY + halfSize, 0.0f); // ������ ��

    // �밢�� 2
    glVertex3f(centerX - halfSize, centerY + halfSize, 0.0f); // ���� ��
    glVertex3f(centerX + halfSize, centerY - halfSize, 0.0f); // ������ �Ʒ�
    glEnd();

    glLineWidth(1.0f); // �⺻ �� �β��� ����
}



//=====================================================      =====================================================//
//===================================================== ���� =====================================================//
//=====================================================      =====================================================//


void drawPodium() {
    // ��Ź �⺻ ��ü
    glPushMatrix();
    glColor3f(0.6f, 0.4f, 0.2f); // ��Ź ���� (���� ����)
    glTranslatef(-2.5f, -0.3f, 4.6f); // ���ʿ� ��ġ��Ŵ
    glScalef(0.8f, 1.0f, 0.5f); // ��Ź ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();

    // ��Ź ��� (å ���� �κ�)
    glPushMatrix();
    glColor3f(0.5f, 0.3f, 0.1f); // �� £�� ����
    glTranslatef(-2.5f, 0.3f, 4.6f);
    glScalef(0.9f, 0.1f, 0.6f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawChairs() {
    // ���� ���� ���� (�Ķ���)
    for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
        // ���� ��Ʈ �κ�
        glPushMatrix();
        glColor3f(0.0f, 0.4f, 0.8f);
        glTranslatef(x, -0.3f, -0.5f); // ���� ��ġ ���� (å�� ������ ��ġ)
        glScalef(0.4f, 0.05f, 0.4f); // ��Ʈ ũ�� ����
        glutSolidCube(1.0);
        glPopMatrix();

        // ���� ����� �κ�
        glPushMatrix();
        glColor3f(0.0f, 0.4f, 0.8f);
        glTranslatef(x, -0.1f, -0.68f); // ����� ��ġ ����
        glScalef(0.4f, 0.4f, 0.05f); // ����� ũ�� ����
        glutSolidCube(1.0);
        glPopMatrix();

        // ���� �ٸ�
        glColor3f(0.5f, 0.5f, 0.5f); // �ٸ� ���� (ȸ��)
        for (float dx = -0.15f; dx <= 0.15f; dx += 0.3f) {
            for (float dz = -0.15f; dz <= 0.15f; dz += 0.3f) {
                glPushMatrix();
                glTranslatef(x + dx, -0.55f, -0.5f + dz); // �ٸ� ��ġ ���� (��Ʈ�� �ٵ���)
                glScalef(0.05f, 0.5f, 0.05f); // �ٸ� ũ�� ����
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }
}


void drawGlassWindow(float posX, float posY, float posZ, float width, float height) {
    glEnable(GL_BLEND);                      // ������ ȿ�� Ȱ��ȭ
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ���� �Լ� ����

    glDisable(GL_CULL_FACE);                 // ��� ������ Ȱ��ȭ
    glDepthMask(GL_FALSE);                   // ���� ���� ���� ��Ȱ��ȭ (���� ó��)


    glColor4f(0.6f, 0.8f, 1.0f, 0.6f);       // ���İ�(0.5)���� ���� ����

    // ������ �� �׸���
    glPushMatrix();
    glTranslatef(posX, posY, posZ);          // ��ġ ����
    glScalef(width, height, 0.01f);          // ������ ũ�� ���� (��� ����)
    glutSolidCube(1.0f);                     // ����â
    glPopMatrix();
    glDepthMask(GL_TRUE);                    // ���� ���� ���� ��Ȱ��ȭ
    glEnable(GL_CULL_FACE);                  // �⺻ �������� ����
    glDisable(GL_BLEND);                     // ������ ȿ�� ��Ȱ��ȭ
}

void drawClassRoomGlassWindow(float posX, float posY, float posZ, float totalWidth, float height, float frameThickness) {
    float paneWidth = totalWidth / 3.0f; // �� â���� �ʺ� (�� �ʺ� 3���)

    glEnable(GL_BLEND); // ������ ȿ�� Ȱ��ȭ
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ������ ���� (ȸ��)
    glColor3f(0.4f, 0.4f, 0.4f);

    // ��� ������
    glPushMatrix();
    glTranslatef(posX, posY + height / 2.0f, posZ);
    glScalef(totalWidth, frameThickness, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �ϴ� ������
    glPushMatrix();
    glTranslatef(posX, posY - height / 2.0f, posZ);
    glScalef(totalWidth, frameThickness, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� ������
    glPushMatrix();
    glTranslatef(posX - totalWidth / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� ������
    glPushMatrix();
    glTranslatef(posX + totalWidth / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �߰� ������ 1
    glPushMatrix();
    glTranslatef(posX - paneWidth / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �߰� ������ 2
    glPushMatrix();
    glTranslatef(posX + paneWidth / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ����â (�� ���� ���� �г�)
    glDisable(GL_CULL_FACE); // ��� ������ Ȱ��ȭ
    glDepthMask(GL_FALSE);  // ���� ���� ���� ��Ȱ��ȭ

    glColor4f(0.6f, 0.8f, 1.0f, 0.5f); // ���� ���� ���� (����)

    for (int i = 0; i < 3; i++) { // 3���� �г� ����
        glPushMatrix();
        glTranslatef(posX - totalWidth / 2.0f + paneWidth / 2.0f + i * paneWidth, posY, posZ);
        // ��ġ: ���� ���������� `paneWidth / 2.0f`��ŭ �̵� ��, �� �г��� ��ġ�� �̵�
        glScalef(paneWidth - frameThickness, height - frameThickness, frameThickness);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE); // ���� ���� ���� Ȱ��ȭ
    glEnable(GL_CULL_FACE); // �⺻ �������� ����
    glDisable(GL_BLEND); // ������ ȿ�� ��Ȱ��ȭ
}






void drawBlackboard() {
    glColor3f(0.1f, 0.3f, 0.1f); // £�� ���
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 4.95f); // ĥ�� ��ġ ����
    glScalef(5.0f, 1.0f, 0.05f); // ĥ�� ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();
}



void drawFloor() {
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // �ٴ� ���� �� £��
    glTranslatef(0.0f, -0.7f, 0.0f);
    glScalef(10.0f, 0.05f, 10.0f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawWalls() {
    glPushMatrix();
    glColor3f(0.75f, 0.75f, 0.75f); // ���� ��
    glTranslatef(-5.0f, 0.75f, 0.0f);
    glScalef(0.05f, 5.0f, 10.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.75f, 0.75f, 0.75f); // ������ ��
    glTranslatef(5.0f, 0.75f, 0.0f);
    glScalef(0.05f, 5.0f, 10.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // �պ�
    glTranslatef(0.0f, 0.25f, 5.0f);
    glScalef(10.0f, 5.0f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    drawBlackboard();

    // �޺�: â�� ���κа� �Ʒ��κ��� ����
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);
    glTranslatef(0.0f, 2.5f, -5.0f); // â�� ���� ��
    glScalef(10.0f, 1.5f, 0.05f); // â�� ���� �� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -1.5f, -5.0f); // â�� �Ʒ��� ��
    glScalef(10.0f, 3.0f, 0.05f); // â�� �Ʒ��� �� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    drawClassRoomGlassWindow(0.0f, 1.0f, -5.0f, 10.0f, 1.75f, 0.1f); // â�� �׸���
}

void drawProjector() {
    glColor3f(0.5f, 0.5f, 0.5f); // £�� ȸ��
    glPushMatrix();
    glTranslatef(0.0f, 1.35f, 3.0f);
    glScalef(0.5f, 0.2f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.4f); // ���� ����
    glTranslatef(0.0f, 1.35f, 2.75f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.1f, 0.1f, 0.25f, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();

    glColor4f(1.0f, 1.0f, 0.8f, 0.3f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 1.3f, 2.75f);
    glVertex3f(-2.5f, 0.5f, 4.95f);
    glVertex3f(2.5f, 0.5f, 4.95f);
    glEnd();
    glPopMatrix();

    glDisable(GL_BLEND);
}

void drawMonitors() {
    // ����� ȭ�� ���� (ȸ��)
    glColor3f(0.3f, 0.3f, 0.3f);
    for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
        glPushMatrix();
        glTranslatef(x, 0.2f, 0.25f); // å�� ���� �߾� ������ �� ������ ������ ����
        glScalef(0.5f, 0.3f, 0.05f); // ����� ũ�� ����
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // ����� ��ħ�� (������)
    glColor3f(0.0f, 0.0f, 0.0f);
    for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
        glPushMatrix();
        glTranslatef(x, 0.05f, 0.25f); // ��ħ�� ��ġ ����
        glScalef(0.1f, 0.05f, 0.1f); // ��ħ�� ũ�� ����
        glutSolidCube(1.0);
        glPopMatrix();
    }
}

void drawDoor(float x, float z, bool open = false) {
    glColor3f(0.7f, 0.7f, 0.7f); // �� ����
    glPushMatrix();

    // ���� ���� ������ ���⿡ �°� ȸ��
    if (open) {
        glTranslatef(x, -0.3f, z);
        if (x > 0) { // ������ ���� ��
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // ������ �� ����
            glTranslatef(-0.35f, 0.0f, -0.35f); // ȸ�� �� ��ġ ����
        }
        else { // ���� ���� ��
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f); // ���� �� ����
            glTranslatef(0.35f, 0.0f, -0.35f); // ȸ�� �� ��ġ ����
        }
    }
    else {
        glTranslatef(x, -0.3f, z); // ���� ���� ��� �⺻ ��ġ
    }

    glScalef(0.7f, 1.5f, 0.01f); // �� ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();

    // ������ �߰� (����� ���)
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f); // ������ ����
    glTranslatef(x + 0.25f * (x > 0 ? -1 : 1), -0.3f, z + 0.01f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.05f, 0.05f, 0.1f, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();
}

void drawDesk() {
    // å�� ���� �׸���
    glPushMatrix();
    glColor3f(0.76f, 0.49f, 0.27f); // ���� ���� ���� ���� (RGB)
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(3.0f, 0.1f, 0.6f); // ���� ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();

    // å�� �߾� ĭ���� �׸���
    glPushMatrix();
    glColor3f(0.6f, 0.4f, 0.3f); // �߾� ĭ���� ���� ����
    glTranslatef(0.0f, -0.4f, 0.25f); // �߾� ĭ���� ��ġ ���� (���� �ٷ� �Ʒ�)
    glScalef(2.9f, 0.7f, 0.1f); // �߾� ĭ���� ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();

    // å�� ���� ĭ���� �׸���
    glPushMatrix();
    glColor3f(0.6f, 0.4f, 0.3f); // ���� ĭ���� ���� ����
    glTranslatef(-1.4f, -0.4f, 0.0f); // ���� ĭ���� ��ġ ����
    glScalef(0.1f, 0.7f, 0.6f); // ���� ĭ���� ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();

    // å�� ������ ĭ���� �׸���
    glPushMatrix();
    glColor3f(0.6f, 0.4f, 0.3f); // ������ ĭ���� ���� ����
    glTranslatef(1.4f, -0.4f, 0.0f); // ������ ĭ���� ��ġ ����
    glScalef(0.1f, 0.7f, 0.6f); // ������ ĭ���� ũ�� ����
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawClassroomSetupWithChairs() {
    // ������ �߾� å��, �����, ���� (��� ����)
    drawDesk();
    drawMonitors();
    drawChairs(); // �߾ӿ��� ��� ������

    // ������ å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(2.8f, 0.0f, 0.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ���� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(-2.8f, 0.0f, 0.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ������ �� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, -2.2f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ���� �� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -2.2f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ���� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -4.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // �밢�� �� ���� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -4.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // �밢�� �� ������ å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, -4.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();
}

//=====================================================      =====================================================//
//=====================================================  2F  =====================================================//
//=====================================================      =====================================================//
void drawElevatorButtons(float baseX, float baseY, float baseZ, float width, float height, float depth) {
    float buttonPanelWidth = 0.2f;  // ��ư �г��� ��
    float buttonPanelHeight = 0.5f; // ��ư �г��� ����
    float buttonSize = 0.1f;        // ��ư�� ũ��
    float buttonSpacing = 0.15f;    // ��ư �� ����

    // ��ư �г� (���������� ��ü�� ����, ���� ��ġ�� ����)
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.4f); // ��ư �г� ����
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(baseX + width / 2.0f + 0.05f, baseY, baseZ - depth / 2.0f + 0.1f); // �г� ��ġ (������ ���� �𼭸��� ����)
    glScalef(buttonPanelWidth, buttonPanelHeight, 0.05f); // �г� ũ��
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��ư 1 (���� ��ư)
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f); // ��ư ���� (������)
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(baseX + width / 2.0f + 0.1f, baseY + buttonSpacing / 2.0f, baseZ - depth / 2.0f + 0.15f);
    glScalef(buttonSize, buttonSize, buttonSize);
    glutSolidSphere(1.0f, 20, 20); // ��ư ��� (��)
    glPopMatrix();

    // ��ư 2 (�Ʒ��� ��ư)
    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f); // ��ư ���� (���)
    glRotatef(270.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(baseX + width / 2.0f + 0.1f, baseY - buttonSpacing / 2.0f, baseZ - depth / 2.0f + 0.15f);
    glScalef(buttonSize, buttonSize, buttonSize);
    glutSolidSphere(1.0f, 20, 20); // ��ư ��� (��)
    glPopMatrix();
}


void drawElevatorBox(float width, float height, float depth) {
    // ���������� ��ü
    glPushMatrix();
    glColor3f(0.7f, 0.7f, 0.7f); // �ݼ� ȸ��
    glScalef(width, height, depth);
    glutSolidCube(1.0);
    glPopMatrix();

    // ������
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f); // �� ��ο� ������
    glScalef(width + 0.1f, height + 0.1f, depth + 0.05f); // ������ ũ��
    glutWireCube(1.0);
    glPopMatrix();
}

void drawElevatorDoors(float width, float height) {
    float doorWidth = width / 2.0f;

    // ���� �� (���������� ��ġ)
    glPushMatrix();
    glColor3f(0.6f, 0.6f, 0.6f); // ���� ȸ��
    glTranslatef(doorWidth / 2.0f, 0.0f, 0.51f); // ���� ������ �� ��ġ
    glScalef(doorWidth, height, 0.05f); // �� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // ������ �� (�������� ��ġ)
    glPushMatrix();
    glColor3f(0.6f, 0.6f, 0.6f); // ���� ȸ��
    glTranslatef(-doorWidth / 2.0f, 0.0f, 0.51f); // ���� ���� �� ��ġ
    glScalef(doorWidth, height, 0.05f); // �� ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // �� �� ��輱 (���� �� ��)
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.4f); // �� ��ο� ��輱
    glTranslatef(doorWidth, 0.0f, 0.50f); // ���� �� ���� ��輱 ��ġ
    glScalef(0.02f, height, 0.05f); // ��輱 ũ��
    glutSolidCube(1.0);
    glPopMatrix();

    // �� �� ��輱 (������ �� ��)
    glPushMatrix();
    glColor3f(0.4f, 0.4f, 0.4f); // �� ��ο� ��輱
    glTranslatef(-doorWidth, 0.0f, 0.50f); // ������ �� ���� ��輱 ��ġ
    glScalef(0.02f, height, 0.05f); // ��輱 ũ��
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawElevator(float x, float y, float z, float width, float height, float depth) {
    glPushMatrix();
    glTranslatef(x, y, z); // ���������� ��ġ ����
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    // ���������� ���� ���
    drawElevatorBox(width, height, depth);
    drawElevatorDoors(width, height);
    drawElevatorButtons(-4.5f, -0.5f, 5.0f, width, height, depth);

    glPopMatrix();
}

void draw2F() {
    glColor3f(0.765f, 0.608f, 0.467f);
    glPushMatrix();
    glTranslatef(-5.0f, -1.700, 5.0f);
    glScalef(20.0f, 0.05f, 20.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    //��� �ֺ�
    glColor3f(0.765f, 0.608f, 0.467f);
    glPushMatrix();
    glTranslatef(-21.0f, -1.7f, 0.0f);
    glScalef(12.0f, 0.05f, 10.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.65f, 0.608f, 0.467f);
    glPushMatrix();//��ܿ� ����� ��
    glTranslatef(-21.0f, -3.8f, 5.0f);
    glScalef(12.0f, 4.3f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.65f, 0.608f, 0.467f);
    glPushMatrix();//��ܿ� �ǹ����� ��
    glTranslatef(-21.0f, -3.8f, 10.0f);
    glScalef(12.0f, 4.3f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.65f, 0.608f, 0.467f);
    glPushMatrix();//��ܼ���º�
    glTranslatef(-15.0f, -3.8f, 7.5f);
    glScalef(0.4f, 4.3f, 5.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.765f, 0.608f, 0.467f);
    glPushMatrix();
    glTranslatef(-31.0f, -1.7f, 5.0f);
    glScalef(8.0f, 0.0f, 20.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.765f, 0.608f, 0.467f);
    glPushMatrix();
    glTranslatef(-21.0f, -1.7f, 12.5f);
    glScalef(12.0f, 0.0f, 5.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    //��� �� ��
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(-27.0f, 0.5f, 7.5f);
    glScalef(0.5f, 4.5f, 5.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    //�Ա� �� ����
    glColor3f(0.8f, 0.8f, 0.7f);
    glPushMatrix();
    glTranslatef(-31.0f, -1.8f, -6.0f);
    glScalef(8.0f, 0.5f, 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();

}

void draw2FWalls() {
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8); // �պ�
    glTranslatef(-15.0f, 0.0f, 10.0f);
    glScalef(40.0f, 3.5f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8); //��������
    glTranslatef(5.0f, -2.0f, 10.0f);
    glScalef(0.0f, 8.0f, 10.0f);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawStairs() {
    glPushMatrix();
    glColor3f(0.6f, 0.6f, 0.6f); // ��� ����
    glTranslatef(-22.0f, -4.50f, 7.5f); // ��� ��ġ ����
    glRotatef(20.0f, .0f, 0.0f, 1.0f); // ����� �ణ ��￩ ����ó�� ���̰� ����
    glScalef(15.0f, 0.5f, 5.0f); // ��� ũ�� ���� (�а� ������ ���簢��)
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawPillar(float x, float y, float z) {
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0); // ��� ����
    glTranslatef(x, y, z); // ��� ��ġ
    glScalef(1.0, 10.0, 1.0); // ��� ũ��
    glutSolidCube(1.0f);
    glPopMatrix();
}
void drawPillarInside(float x, float y, float z) {
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0); // ��� ����
    glTranslatef(x, y, z); // ��� ��ġ
    glScalef(0.6, 10.0, 0.6); // ��� ũ��
    glutSolidCube(1.0f);
    glPopMatrix();
}
void drawToiletWall(float x, float y, float z, float width, float height, float depth, float r, float g, float b) {
    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(x, y, z);
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}
void drawBathroomLayout(float x, float y, float z) {
    glPushMatrix();
    float wallHeight = 3.0f;
    float wallThickness = 0.1f;
    float roomWidth = 10.0f;
    float roomDepth = 5.0f;
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    // �ܺ�
    drawToiletWall(x - roomWidth / 2.0f, y, z, wallThickness, wallHeight, roomDepth, 0.8f, 0.8f, 0.8f); // ���� ��
    drawToiletWall(x + roomWidth / 2.0f, y, z, wallThickness, wallHeight, roomDepth, 0.8f, 0.8f, 0.8f); // ������ ��
    drawToiletWall(x, y, z - roomDepth / 2.0f, roomWidth, wallHeight, wallThickness, 0.8f, 0.8f, 0.8f); // ���� ��

    // ���� ���� ��
    drawToiletWall(x, y, z, wallThickness, wallHeight, roomDepth, 0.6f, 0.6f, 0.6f); // �߾� ��

    // ���� ȭ���
    drawToiletWall(x - 2.5f, y, z + 10.0 / 4.0f, 5.0f, wallHeight, wallThickness, 0.6f, 0.6f, 0.8f);
    drawToiletWall(x - 3.5f, y, z + 2.6f, 1.0f, wallHeight, wallThickness, 0.9f, 0.9f, 0.9f);

    // ���� ȭ���
    drawToiletWall(x + 2.5f, y, z + 10.0 / 4.0f, 5.0f, wallHeight, wallThickness, 0.8f, 0.6f, 0.6f);
    drawToiletWall(x + 3.5f, y, z + 2.6f, 1.0f, wallHeight, wallThickness, 0.9f, 0.9f, 0.9f);


    glPopMatrix();

}

//=====================================================      =====================================================//
//=====================================================  1F  =====================================================//
//=====================================================      =====================================================//

void drawGlassDoorLeft(float posX, float posY, float posZ, float width, float height, float frameThickness) {
    glEnable(GL_BLEND); // ������ ȿ�� Ȱ��ȭ
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ��Ʋ ���� (��ο� ȸ��)
    glColor3f(0.4f, 0.4f, 0.4f);

    // ���� ��Ʋ
    glPushMatrix();
    glTranslatef(posX - width / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ������ ��Ʋ
    glPushMatrix();
    glTranslatef(posX + width / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��� ��Ʋ
    glPushMatrix();
    glTranslatef(posX, posY + height / 2.0f, posZ);
    glScalef(width, frameThickness, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �ϴ� ��Ʋ
    glPushMatrix();
    glTranslatef(posX, posY - height / 2.0f, posZ);
    glScalef(width, frameThickness, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� �κ�
    glDisable(GL_CULL_FACE); // ��� ������ Ȱ��ȭ
    glDepthMask(GL_FALSE);  // ���� ���� ���� ��Ȱ��ȭ

    glColor4f(0.6f, 0.8f, 1.0f, 0.5f); // ���� ���� ���� (����)

    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    glScalef(width - frameThickness * 2, height - frameThickness * 2, frameThickness * 0.5f); // ���� ũ��
    glutSolidCube(1.0f);
    glPopMatrix();

    glDepthMask(GL_TRUE); // ���� ���� ���� Ȱ��ȭ
    glEnable(GL_CULL_FACE); // �⺻ �������� ����
    glDisable(GL_BLEND); // ������ ȿ�� ��Ȱ��ȭ

    glColor3f(0.5f, 0.5f, 0.5f); // ������ ���� (ȸ��)
    GLUquadric* quad = gluNewQuadric(); // �����̿� ����� ����

    // �� ���� ������
    glPushMatrix();
    glTranslatef(posX + 2.0 - 4 + width / 4.0f, posY, posZ + frameThickness * 2.0f); // ������ ��ġ (x-4 ����)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // ����� ȸ�� (���� ����)
    gluCylinder(quad, 0.05f, 0.05f, 0.3f, 20, 20); // ������ ũ�� ����
    glPopMatrix();

    // �� ���� ������
    glPushMatrix();
    glTranslatef(posX + 2.0 - 4 + width / 4.0f, posY, posZ - frameThickness * 2.0f); // �ݴ��� ������ ��ġ (x-4 ����)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // ����� ȸ�� (���� ����)
    gluCylinder(quad, 0.05f, 0.05f, 0.3f, 20, 20); // ������ ũ�� ����
    glPopMatrix();

    gluDeleteQuadric(quad);

}
void drawGlassDoorRight(float posX, float posY, float posZ, float width, float height, float frameThickness) {
    glEnable(GL_BLEND); // ������ ȿ�� Ȱ��ȭ
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ��Ʋ ���� (��ο� ȸ��)
    glColor3f(0.4f, 0.4f, 0.4f);

    // ���� ��Ʋ
    glPushMatrix();
    glTranslatef(posX - width / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ������ ��Ʋ
    glPushMatrix();
    glTranslatef(posX + width / 2.0f, posY, posZ);
    glScalef(frameThickness, height, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��� ��Ʋ
    glPushMatrix();
    glTranslatef(posX, posY + height / 2.0f, posZ);
    glScalef(width, frameThickness, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �ϴ� ��Ʋ
    glPushMatrix();
    glTranslatef(posX, posY - height / 2.0f, posZ);
    glScalef(width, frameThickness, frameThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� �κ�
    glDisable(GL_CULL_FACE); // ��� ������ Ȱ��ȭ
    glDepthMask(GL_FALSE);  // ���� ���� ���� ��Ȱ��ȭ

    glColor4f(0.6f, 0.8f, 1.0f, 0.5f); // ���� ���� ���� (����)

    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    glScalef(width - frameThickness * 2, height - frameThickness * 2, frameThickness * 0.5f); // ���� ũ��
    glutSolidCube(1.0f);
    glPopMatrix();

    glDepthMask(GL_TRUE); // ���� ���� ���� Ȱ��ȭ
    glEnable(GL_CULL_FACE); // �⺻ �������� ����
    glDisable(GL_BLEND); // ������ ȿ�� ��Ȱ��ȭ

    glColor3f(0.5f, 0.5f, 0.5f); // ������ ���� (ȸ��)
    GLUquadric* quad = gluNewQuadric(); // �����̿� ����� ����

    // �� ���� ������
    glPushMatrix();
    glTranslatef(posX + width / 4.0f, posY, posZ + frameThickness * 2.0f); // ������ ��ġ
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // ����� ȸ�� (���� ����)
    gluCylinder(quad, 0.05f, 0.05f, 0.3f, 20, 20); // ������ ũ�� ����
    glPopMatrix();

    // �� ���� ������
    glPushMatrix();
    glTranslatef(posX + width / 4.0f, posY, posZ - frameThickness * 2.0f); // �ݴ��� ������ ��ġ
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // ����� ȸ�� (���� ����)
    gluCylinder(quad, 0.05f, 0.05f, 0.3f, 20, 20); // ������ ũ�� ����
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void draw1FWalls() {
    // ���� ��
    glPushMatrix();
    glColor3f(0.7f, 0.7f, 0.7f); // �� ����
    glTranslatef(-27.0f, -1.0f, 0.0f); // ���� �� ��ġ ����
    glScalef(0.5f, 10.0f, 10.0f); // �� ũ�� ���� (��� �� ��)
    glutSolidCube(1.0);
    glPopMatrix();

    // ������ ��
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // �� ����
    glTranslatef(-35.0f, -1.0f, 5.0f); // ������ �� ��ġ ����
    glScalef(0.5f, 10.0f, 20.0f); // �� ũ�� ���� (��� �� ��)
    glutSolidCube(1.0);
    glPopMatrix();


    //�ǹ� ū���ʺ�
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // �� ����
    glTranslatef(-15.0f, -1.0f, 15.0f); // ������ �� ��ġ ����
    glScalef(40.0f, 10.0f, 0.5f); // �� ũ�� ���� (��� �� ��)
    glutSolidCube(1.0);
    glPopMatrix();

    //�ǹ� �պ�
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // �� ����
    glTranslatef(-16.0f, -1.0f, -5.0f); // ������ �� ��ġ ����
    glScalef(22.0f, 10.0f, 0.5f); // �� ũ�� ���� (��� �� ��)
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawChairs1F() {
    // ���� ���� ���� (�Ķ���)
    for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
        // ���� ��Ʈ �κ�
        glPushMatrix();
        glColor3f(0.0f, 0.4f, 0.8f);
        glTranslatef(x, -0.3f, -0.5f); // ���� ��ġ ���� (å�� ������ ��ġ)
        glScalef(0.4f, 0.05f, 0.4f); // ��Ʈ ũ�� ����
        glutSolidCube(1.0);
        glPopMatrix();

        // ���� ����� �κ�
        glPushMatrix();
        glColor3f(0.0f, 0.4f, 0.8f);
        glTranslatef(x, -0.1f, -0.68f); // ����� ��ġ ����
        glScalef(0.4f, 0.4f, 0.05f); // ����� ũ�� ����
        glutSolidCube(1.0);
        glPopMatrix();

        // ���� �ٸ�
        glColor3f(0.5f, 0.5f, 0.5f); // �ٸ� ���� (ȸ��)
        for (float dx = -0.15f; dx <= 0.15f; dx += 0.3f) {
            for (float dz = -0.15f; dz <= 0.15f; dz += 0.3f) {
                glPushMatrix();
                glTranslatef(x + dx, -0.55f, -0.5f + dz); // �ٸ� ��ġ ���� (��Ʈ�� �ٵ���)
                glScalef(0.05f, 0.5f, 0.05f); // �ٸ� ũ�� ����
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }


    }
}

void drawClassroomSetupWithChairs1F() {
    // ������ �߾� å��, �����, ���� (��� ����)
    drawDesk();
    drawMonitors();
    drawChairs1F(); // �߾ӿ��� ��� ������

    // ������ å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(2.8f, 0.0f, 0.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ���� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(-2.8f, 0.0f, 0.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ������ �� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, -2.2f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ���� �� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -2.2f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // ���� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -4.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // �밢�� �� ���� å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -4.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();

    // �밢�� �� ������ å��� ����� (��� ����)
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, -4.0f);
    drawDesk();
    drawMonitors();
    glPopMatrix();
}

void drawClassroom1F(float x, float y, float z) {

    glPushMatrix();
    glTranslatef(x, y, z);  // ���� ��ġ ����

    // ���� ���� ��� �׸���
    drawFloor();
    drawWalls();
    drawDoor(-4.0f, 4.95f, true);  // ���� �� ����
    drawDoor(4.0f, 4.95f);         // ������ �� ����
    drawBlackboard();
    drawProjector();
    drawDesk();
    drawMonitors();
    drawChairs1F();
    drawPodium();
    drawClassroomSetupWithChairs1F();

    glPopMatrix();

}

void drawGround() {
    glPushMatrix();
    glColor3f(0.765f, 0.608f, 0.467f);
    glTranslatef(-5.0f, -6.0f, 5.0f);
    glScalef(20.0f, 0.05f, 20.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.765f, 0.608f, 0.467f);
    glTranslatef(-25.0f, -6.0f, 5.0f);
    glScalef(20.0f, 0.05f, 20.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    
    //�ǹ� �ܺ��Ѳ�
    glColor3f(1.0f, 1.0f, 0.941f);
    glPushMatrix();
    glTranslatef(-15.0f, 2.0f, 5.0f);
    glScalef(40.0f, 0.5f, 20.0f);
    glutSolidCube(1.0);
    glPopMatrix();
     

    glPushMatrix();
    glColor3f(0.34f, 0.41f, 0.11f);
    glTranslatef(-15.0f, -6.0f, -7.0f);
    glScalef(40.0f, 0.05f, 4.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.34f, 0.41f, 0.11f);
    glTranslatef(-40.0f, -6.0f, 3.0f);
    glScalef(10.0f, 0.05f, 24.0f);
    glutSolidCube(1.0);
    glPopMatrix();

}

void draw1Foutside() {
    glPushMatrix();
    glColor3f(0.34f, 0.41f, 0.11f);;   // (�ʷϻ�)
    glTranslatef(-20.00f, -6.00f, -19.00f);
    glScalef(50.0f, 0.05f, 20.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // ��� ��輱 (Ʈ��) ���� (�ʷϻ�)
    glPushMatrix();
    glColor3f(0.2f, 0.8f, 0.2f);
    glTranslatef(-18.0f, -5.92f, -18.9f);
    glScalef(-30.00f, 0.1f, 10.00f);
    glutSolidCube(1.0);
    glPopMatrix();
}




//=====================================================            ===============================================//
//=====================================================   ���   ===============================================//
//=====================================================            ===============================================//


// �ʱ�ȭ �Լ�
void init() {
    glEnable(GL_DEPTH_TEST); // ���� �׽�Ʈ Ȱ��ȭ
    glClearColor(0.3f, 0.7f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 200.0); // ���� ���� ����
    std::srand(std::time(0)); // ���� �ʱ�ȭ
}

// ��� �� ���� ������ �׸���
void drawFieldLines() {
    glColor3f(1.0f, 1.0f, 1.0f); // ���

    // �߾� ��Ŭ�� �߽� ��ǥ
    float centerX = -18.5f;
    float centerY = -5.9f; // �ʷϻ� ��� ���� ����
    float centerZ = -18.9f;

    // �߾Ӽ� (�߾� ��Ŭ �߽��� �������� �¿�� �׸���)
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex3f(centerX, centerY, centerZ + 5.3f); // ���� ��
    glVertex3f(centerX, centerY, centerZ - 5.3f); // ������ ��
    glEnd();

    // �߾� ��Ŭ (Y�� 90�� ȸ��)
    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ); // �߾� ��Ŭ�� �߽� ��ġ
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex3f(2.0f * cos(angle), 0.0f, 2.0f * sin(angle)); // ������ 2
    }
    glEnd();
    glPopMatrix();
}

void drawTrack(float x, float y, float z, float rotationAngle) {
    glPushMatrix();
    // Ʈ���� �߽� ��ġ ����
    glTranslatef(x, y, z);
    // Ʈ�� ȸ�� ���� (Z�� ���� ȸ��)
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    glColor3f(0.8f, 0.1f, 0.1f); // Ʈ�� ���� (������)

    int numLanes = 4;          // Ʈ�� ���� ��
    float laneWidth = 1.0f;    // �� ������ ��
    float trackLength = 25.0f; // Ʈ���� ����
    float trackWidth = 20.0f;  // Ʈ���� �ʺ�
    float laneHeight = 0.10f;   // ��� ������ ���� ����
    float Height = 0.15f;
    float gap = 0.2f;          // ���� ���� ����

    for (int i = 0; i < numLanes; i++) {
        float innerRadius = (trackWidth / 2.0f) - (i * (laneWidth + gap)); // ���� ������
        float outerRadius = innerRadius - laneWidth;                      // �ܺ� ������

        // Ʈ���� ���κ� (�ݿ�)
        glPushMatrix();
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= 180; j += 10) { // 0������ 180������
            float angle = j * 3.14159f / 180.0f;
            glVertex3f(innerRadius * cos(angle), laneHeight, trackLength / 2.0f + innerRadius * sin(angle));
            glVertex3f(outerRadius * cos(angle), laneHeight, trackLength / 2.0f + outerRadius * sin(angle));
        }
        glEnd();
        glPopMatrix();

        // Ʈ���� �Ʒ��κ� (�ݿ�)
        glPushMatrix();
        glBegin(GL_QUAD_STRIP);
        for (int j = 180; j <= 360; j += 10) { // 180������ 360������
            float angle = j * 3.14159f / 180.0f;
            glVertex3f(innerRadius * cos(angle), laneHeight, -trackLength / 2.0f + innerRadius * sin(angle));
            glVertex3f(outerRadius * cos(angle), laneHeight, -trackLength / 2.0f + outerRadius * sin(angle));
        }
        glEnd();
        glPopMatrix();

        // Ʈ���� ���� �κ� (�¿�)
        glPushMatrix();
        glBegin(GL_QUADS);

        // ���� ����
        glVertex3f(-innerRadius, laneHeight, -trackLength / 2.0f);
        glVertex3f(-innerRadius, laneHeight, trackLength / 2.0f);
        glVertex3f(-outerRadius, laneHeight, trackLength / 2.0f);
        glVertex3f(-outerRadius, laneHeight, -trackLength / 2.0f);


        // ������ ����
        glVertex3f(innerRadius, laneHeight, -trackLength / 2.0f);
        glVertex3f(outerRadius, laneHeight, -trackLength / 2.0f);
        glVertex3f(outerRadius, laneHeight, trackLength / 2.0f);
        glVertex3f(innerRadius, laneHeight, trackLength / 2.0f);

        glEnd();
        glPopMatrix();
    }

    // ��¼� �߰�
    glColor3f(1.0f, 1.0f, 1.0f); // ���
    glPushMatrix();
    glLineWidth(10.0f);
    glBegin(GL_LINES);

    // ��¼��� Ʈ���� ���̿� �ʺ� �°� ����
    for (int i = 0; i < numLanes; i++) {
        float innerRadius = (trackWidth / 2.0f) - (i * (laneWidth + gap)); // ���� ������
        float outerRadius = innerRadius - laneWidth;                      // �ܺ� ������

        // ��¼��� ���۰� �� ���� Ʈ�� ���� ������ ����
        glVertex3f(-innerRadius, Height, trackLength / 2.0f);
        glVertex3f(-outerRadius, Height, trackLength / 2.0f);

        glVertex3f(innerRadius, Height, trackLength / 2.0f);
        glVertex3f(outerRadius, Height, trackLength / 2.0f);
    }

    glEnd();
    glPopMatrix();


    glPopMatrix(); // Ʈ�� ��ȯ �� ȸ�� ����
}

void drawGoal(float xPosition, float zPosition, float rotationAngle) {
    glPushMatrix();
    glTranslatef(xPosition, -5.95f, zPosition); // ��� ��ġ
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Y�� ���� ȸ��

    // ��� ��� (4��)
    glColor3f(0.5f, 0.5f, 0.5f); // �߰� ȸ�� (�� ��ο� ����)

    float poleHeight = 2.0f;
    float poleThickness = 0.1f;

    // ���� ���� ���
    glPushMatrix();
    glTranslatef(-2.0f, poleHeight / 2.0f, 0.0f);
    glScalef(poleThickness, poleHeight, poleThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� ������ ���
    glPushMatrix();
    glTranslatef(2.0f, poleHeight / 2.0f, 0.0f);
    glScalef(poleThickness, poleHeight, poleThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� ���� ���
    glPushMatrix();
    glTranslatef(-2.0f, poleHeight / 2.0f, -2.0f);
    glScalef(poleThickness, poleHeight, poleThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ���� ������ ���
    glPushMatrix();
    glTranslatef(2.0f, poleHeight / 2.0f, -2.0f);
    glScalef(poleThickness, poleHeight, poleThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��� ���� ���� (����)
    glPushMatrix();
    glTranslatef(0.0f, poleHeight, 0.0f);
    glScalef(4.0f, poleThickness, poleThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��� ���� ���� (����)
    glPushMatrix();
    glTranslatef(0.0f, poleHeight, -2.0f);
    glScalef(4.0f, poleThickness, poleThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��� ���� ���� (����)
    glPushMatrix();
    glTranslatef(-2.0f, poleHeight, -1.0f);
    glScalef(poleThickness, poleThickness, 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // ��� ���� ���� (������)
    glPushMatrix();
    glTranslatef(2.0f, poleHeight, -1.0f);
    glScalef(poleThickness, poleThickness, 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �׹� (����)
    glColor3f(0.7f, 0.7f, 0.7f); // �ణ ��ο� ȸ��
    for (float x = -1.8f; x <= 1.8f; x += 0.6f) {
        glPushMatrix();
        glTranslatef(x, 1.0f, -2.0f); // ���� �� ��ġ
        glScalef(0.05f, 2.0f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    for (float y = 0.0f; y <= 2.0f; y += 0.5f) {
        glPushMatrix();
        glTranslatef(0.0f, y, -2.0f); // ���� �� ��ġ
        glScalef(4.0f, 0.05f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // �׹� (����)
    for (float z = -0.2f; z >= -2.0f; z -= 0.4f) {
        glPushMatrix();
        glTranslatef(-2.0f, 1.0f, z); // ���� �� ��ġ
        glScalef(0.05f, 2.0f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    for (float y = 0.0f; y <= 2.0f; y += 0.5f) {
        glPushMatrix();
        glTranslatef(-2.0f, y, -1.0f); // ���� �� ��ġ
        glScalef(0.05f, 0.05f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // �׹� (������)
    for (float z = -0.2f; z >= -2.0f; z -= 0.4f) {
        glPushMatrix();
        glTranslatef(2.0f, 1.0f, z); // ���� �� ��ġ
        glScalef(0.05f, 2.0f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    for (float y = 0.0f; y <= 2.0f; y += 0.5f) {
        glPushMatrix();
        glTranslatef(2.0f, y, -1.0f); // ���� �� ��ġ
        glScalef(0.05f, 0.05f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // �׹� (����)
    for (float x = -1.8f; x <= 1.8f; x += 0.6f) {
        glPushMatrix();
        glTranslatef(x, 2.0f, -1.0f); // ���� �� ��ġ
        glScalef(0.05f, 0.05f, 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    for (float z = -0.2f; z >= -2.0f; z -= 0.4f) {
        glPushMatrix();
        glTranslatef(0.0f, 2.0f, z); // ���� �� ��ġ
        glScalef(4.0f, 0.05f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    glPopMatrix(); // ��ȯ ����
}

void drawStands(float xPosition, float yPosition, float zPosition, bool rotate, float rotationAngle, int rows, int blocksPerRow, float heights[], float depths[], float blockWidths[], float blockHeights[], float blockDepths[]) {
    glPushMatrix();
    // ���߼� ��ġ ����
    glTranslatef(xPosition, yPosition, zPosition);

    // ���߼� ȸ�� ���� Ȯ��
    if (rotate) {
        glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Y�� �������� ȸ��
    }

    // ���߼� ��� �� ���� ����
    float chairOffset = 0.25f; // ���� ��Ͽ����� ���� ������
    int halfBlocks = blocksPerRow / 2; // ���߼��� ���� ��� ��

    for (int row = 0; row < rows; row++) {
        float height = heights[row];  // �� ���� ����
        float depth = depths[row] - (row * 0.5f); // �� ���� ���� (���� ����)
        float blockWidth = blockWidths[row];   // �� ���� ��� �ʺ�
        float blockHeight = blockHeights[row]; // �� ���� ��� ����
        float blockDepth = blockDepths[row];   // �� ���� ��� ����

        for (int block = 0; block < blocksPerRow; block++) {
            // �¿� ��Ī ��ġ ���
            float offset = (block < halfBlocks) ? -1 : 1; // ����(-1) �Ǵ� ������(1)
            float blockX = offset * (7.5f - (block % halfBlocks) * blockWidth - row * 0.3f);

            // ��� �׸���
            glPushMatrix();
            glColor3f(0.3f, 0.3f, 0.3f); // ���߼� ��� ���� (���� ȸ��)
            glTranslatef(blockX + blockWidth / 2.0f, height / 2.0f, depth);
            glScalef(blockWidth, blockHeight, blockDepth);
            glutSolidCube(1.0f);
            glPopMatrix();

            // ���� �߰� (���߼��� ���̱�)
            float chairX = blockX + blockWidth / 2.0f;
            glPushMatrix();
            glColor3f(0.6f, 0.6f, 0.6f); // ���� ���� (�߰� ȸ��)
            glTranslatef(chairX, height + chairOffset, depth); // ���� ��ġ (���߼��� ������ Z��)
            glScalef(0.8f, 0.2f, 1.0f); // ���� ũ��
            glutSolidCube(1.0f); // ���� �¼�
            glPopMatrix();

            // ����� �߰�
            glPushMatrix();
            glColor3f(0.5f, 0.5f, 0.5f); // ����� ���� (��ο� ȸ��)
            glTranslatef(chairX, height + chairOffset + 0.15f, depth - 0.5f); // ����� ��ġ
            glScalef(0.8f, 0.6f, 0.1f); // ����� ũ��
            glutSolidCube(1.0f); // ���� �����
            glPopMatrix();
        }
    }
    glPopMatrix();
}

// ���⿡ ����

//=====================================================            ===============================================//
//=====================================================  ���θ޴�  ===============================================//
//=====================================================            ===============================================//



// �ؽ�Ʈ�� 2D ȭ�鿡 ����ϴ� �Լ� (��Ʈ �߰�)
void drawText(const std::string& text, float x, float y, void* font) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// 2D �������� ����
void setupOrtho() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 800.0, 0.0, 600.0);  // ȭ�� ��ǥ�� ���� (������ ũ�� ����)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ���÷��� �ݹ� �Լ�
void display() {
    if (cameraPaths.empty()) {
        std::cerr << "ī�޶� ��ΰ� �����ϴ�. ���÷��̸� �ߴ��մϴ�." << std::endl;
        return;
    }

    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    recordCameraPath(); // ���� �� ����

    // ���� ȿ�� ����
    applyEarthquakeEffect(); // ī�޶� ���� ȿ�� ����

    // ī�޶� ��ġ ���
    std::ostringstream oss;
    oss << "Camera Position: (" << cameraX << ", " << cameraY << ", " << cameraZ << ")";
    std::string cameraPositionText = oss.str();

    // 2D �ؽ�Ʈ�� 3D ������ ���
    glPushMatrix();  // ���� ��ȯ ���� ����

    // 2D �ؽ�Ʈ�� �׸��� ���� ����
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();  // �������� ��Ʈ���� ����
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);  // 2D ��ǥ �ý��� ����

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();  // �𵨺� ��� �ʱ�ȭ

    // �ؽ�Ʈ ���
    drawText(cameraPositionText, 10, 580, GLUT_BITMAP_HELVETICA_18);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();  // �������� ��Ʈ���� ����

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();  // �𵨺� ��� ����


    // 2�� ��� (1��Ī ����) ó��
    if (startButtonPressed) {
        // ���� ���� �� �Ǵ� ���� ��ư�� ���� ��� 1��Ī ���� ����
        isFirstPersonActive = true; // 1��Ī ���� Ȱ��ȭ
    }

    if (isFirstPersonActive) {  // 1��Ī ���� Ȱ��ȭ �� ī�޶� �̵�
        if (currentPathIndex == 3) {  // 2�� ���� = 1, ������ ���� = 3
            if (frameIndices[currentPathIndex] < cameraPaths[currentPathIndex].size()) {
                followCameraPath(cameraPaths[currentPathIndex]);

                // ��� �� ��ġ (���⼭ �Ӹ� ��ġ �������� ī�޶� �̵�)
                float headX = cameraX;  // �Ӹ� X ��ġ
                float headY = cameraY + 0.8f;  // �Ӹ� Y ��ġ (������� ����)
                float headZ = cameraZ;  // �Ӹ� Z ��ġ

                // ī�޶� �Ӹ� ��ġ���� �ణ �Ʒ��� ����
                float cameraHeightOffset = -0.2f;  // ī�޶� �Ӹ� �Ʒ��� ������ �Ÿ� (1��Ī ������ ����)

                // �Ӹ����� �ణ �Ʒ��� �������� ī�޶� ��ġ ����
                gluLookAt(headX, headY + cameraHeightOffset, headZ,  // ī�޶� ��ġ
                    headX + lookX, headY + lookY, headZ + lookZ,  // �ٶ󺸴� ����
                    0.0f, 1.0f, 0.0f);  // ���� ����
            }
            else {
                // 2�� ��θ� ������ ��������, ī�޶� ���߰� ����� �׸�����
                currentPathIndex = -1;  // ��� ������ ���߰� �� �̻� �̵����� �ʵ���
            }
        }
    }
    else {
        // �ٸ� ��ο����� �Ϲ� ī�޶� ��ġ�� ���󰡰�, ����� �𵨸�
        gluLookAt(cameraX, cameraY, cameraZ,
            cameraX + lookX, cameraY + lookY, cameraZ + lookZ,
            0.0f, 1.0f, 0.0f);
    }


    // ���� ���� ��� �׸���
    glPushMatrix();
    glTranslatef(-5.0f, 0.7f, -5.0f);  // ����� �� �߽����� �̵�
    drawFloor();
    drawWalls();

    drawDoor(-4.0f, 4.95f, true);  // ���� �� ����
    drawDoor(4.0f, 4.95f);         // ������ �� ����
    drawBlackboard();
    drawProjector();
    drawDesk();
    drawMonitors();
    drawChairs();
    drawPodium();
    draw1FWalls();
    drawStairs();
    drawGround();
    drawClassroomSetupWithChairs();
    drawElevator(-7.75, 0.75f, -2.5f, 5.0f, 5.0f, 5.0f);
    drawElevator(-7.75, -4.2f, -2.5f, 5.0f, 5.0f, 5.0f);
    draw2F();
    draw2FWalls();
    drawClassroom1F(0.0, -4.3, 0.0);
    drawGlassWindow(-30.5f, 0.5f, -5.0f, 9.0f, 5.0f);
    drawGlassDoorLeft(-29.0f, -4.0f, -5.0f, 4.0f, 4.0f, 0.1f);
    drawGlassDoorRight(-33.0f, -4.0f, -5.0f, 4.0f, 4.0f, 0.1f);
    drawClassRoomGlassWindow(0.0f, 0.9f, -5.0f, 10.0f, 1.75f, 0.1f);
    // OUTSIDE
    // OUTSIDE
    drawPillar(5.0, -1.0, 15.0);
    drawPillar(-35.0, -1.0, 15.0);
    drawPillarInside(-27.0, -1.0, 9.9);
    drawPillarInside(-27.0, -1.0, 4.9);
    drawPillarInside(5.0, -1.0, 10.0);
    drawBathroomLayout(0.0f, -0.3f, -24.0f);
    drawBathroomLayout(0.0f, -4.5f, -24.0f);
    draw1Foutside();


    //���� ���� ����
    drawTrack(-18.0f, -6.04f, -18.9f, 90.0f);
    drawFieldLines();
    drawGoal(-33.0f, -19.0f, 90.0f); // ���� ���
    drawGoal(-3.0f, -19.0f, -90.0f);  // ������ ���

    // ���߼� ����
    int rows = 6; // �� ��
    int blocksPerRow = 30; // �� ���� ��� ��

    // ���� ���� ��
    float heights[6] = { 1.0f, 1.5f, 2.0f, 2.5f, 2.7f,2.9f };  // ���� ����
    float depths[6] = { -5.0f, -6.5f, -8.0f, -9.5f, -11.0f,-12.5f }; // ���� ����
    float blockWidths[6] = { 1.5f, 1.6f, 1.7f, 1.8f, 1.9f ,2.0f };  // ���� ��� �ʺ�
    float blockHeights[6] = { 1.0f, 1.5f, 2.0f, 2.5f, 3.0f ,3.5f }; // ���� ��� ����
    float blockDepths[6] = { 1.5f, 1.6f, 1.7f, 1.8f, 1.9f,2.0f };  // ���� ��� ����

    // ���� ���߼� �׸���
    drawStands(-21.0f, -6.0f, -24.7f, false, 0.0f, rows, 42, heights, depths, blockWidths, blockHeights, blockDepths);

    // ���� ���߼�: ȸ�� Ȱ��ȭ
    drawStands(-40.0f, -6.0f, -15.6f, true, 90.0f, rows, blocksPerRow, heights, depths, blockWidths, blockHeights, blockDepths);

    //������ ���߼�: ȸ�� Ȱ��ȭ
    drawStands(0.0f, -6.0f, -20.0f, true, -90.0f, rows, blocksPerRow, heights, depths, blockWidths, blockHeights, blockDepths);



      
    ///// ȭ��ǥ  ///// 
    ///// ȭ��ǥ  ///// 

    ///// 2F���� �� �տ�
    glPushMatrix();
    glTranslatef(-4.0f, -0.6f, 4.0f); // �߽� �̵�
    glRotatef(-90, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(-90, 1.0f, 0.0f, 0.0f); // x�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();
    /////

    ///// 2F���� ��
    glPushMatrix();
    glTranslatef(-4.0f, 0.10f, 9.9f); // �߽� �̵�
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f); // x�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();
    /////

    ///// 2F ��� ���ۺκ�
    glPushMatrix();
    glTranslatef(-13.0f, 0.10f, 9.9f); // �߽� �̵�
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(0.0f, 1.0f, 0.0f, 0.0f); // x�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-25.0f, -3.3f, 9.9f); // �߽� �̵�
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(-17.0f, 0.0f, 0.0f, 1.0f); // z�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-25.0f, -3.3f, 5.1f); // �߽� �̵�
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(-160.0f, 0.0f, 0.0f, 1.0f); // z�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();
    /////


    ///// 1F ����
    glPushMatrix();
    glTranslatef(-34.7f, -4.3f, 7.6f); // �߽� �̵�
    glRotatef(-270.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(0.0f, 0.0f, 0.0f, 1.0f); // x�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-34.7f, -4.3f, 0.9f); // �߽� �̵�
    glRotatef(-270.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    glRotatef(0.0f, 0.0f, 0.0f, 1.0f); // x�� ���� ȸ��
    drawArrow2D(0.0f, 0.0f, 1.0f, 0.3f);
    glPopMatrix();
    /////



    // ���������� ��
    glPushMatrix();
    glTranslatef(-7.7f, 0.10f, 0.9f); // �߽� �̵�
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // y�� ���� ȸ��
    drawBoldRedX(0.0f, 0.0f, 2.0f, 14.0f); // �߽� (0, 0), ũ�� 2.0, �β� 5.0
    glPopMatrix();


    glPopMatrix();

    // ��κ� ��� �׸��� (���� ���� �ִ� ����� ����)
    for (size_t i = 0; i < cameraPaths.size(); ++i) {
        if (i != currentPathIndex && currentPathIndex != -1) {  // ���� ���� �ִ� ����� �����ϰ� �׸���
            if (frameIndices[i] < cameraPaths[i].size()) {
                const auto& state = cameraPaths[i][frameIndices[i]];

                // ��� �� �׸���
                glPushMatrix();
                glTranslatef(state.cameraX, state.cameraY, state.cameraZ);
                drawPerson(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                glPopMatrix();

                // ������ ������Ʈ
                frameIndices[i]++;
            }
            else {
                // ������ ��ǥ���� ��� �׸���
                const auto& lastState = lastCameraPositions[i];

                glPushMatrix();
                glTranslatef(lastState.cameraX, lastState.cameraY, lastState.cameraZ);
                drawPerson(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
                glPopMatrix();
            }
        }
    }

    glutSwapBuffers();
}

// Ÿ�̸� �ݹ�
void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(100, update, 0);
}

// ���� �Լ�
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Moving Person");
    glEnable(GL_DEPTH_TEST);

    loadCameraPaths();


    // 2�� ��η� ī�޶� �ʱ�ȭ
    currentPathIndex = 3; // 2�� ��θ� ��� (1��Ī ����) ��� ī�޶� ���� �ϰ� ������ �ּ� ó���ϼ�.
    // 2�� ���� = 1, ������ ���� = 3


    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(idle);
    glutTimerFunc(100, update, 0);
    glutMainLoop();
    return 0;
}