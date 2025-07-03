#include <GL/glut.h>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cmath>

// Globals
float carPosX = -1.0f;
float wheelAngle = 0.0f;
float carSpeed = 0.005f;
int direction = 1;

float cloudX = 0.0f;
int lastRainToggleTime = 0;
bool raining = false;

struct Raindrop {
    float x, y;
    float speed;
    float sizeX, sizeY;
    float blueShade;
};

std::vector<Raindrop> raindrops;

void drawOval(float cx, float cy, float rx, float ry, int segments = 20) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + cosf(angle) * rx, cy + sinf(angle) * ry);
    }
    glEnd();
}

void drawCircle(float cx, float cy, float r, int segments = 100) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(cx + cosf(angle) * r, cy + sinf(angle) * r);
    }
    glEnd();
}

void drawCloud(float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(x, y, 0.05f);
    drawCircle(x + 0.05f, y + 0.01f, 0.04f);
    drawCircle(x - 0.05f, y + 0.01f, 0.04f);
    drawCircle(x, y + 0.03f, 0.045f);
}

void drawTree(float x, float y) {
    // Trunk
    glColor3f(0.45f, 0.25f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.02f, y);
    glVertex2f(x + 0.02f, y);
    glVertex2f(x + 0.02f, y + 0.1f);
    glVertex2f(x - 0.02f, y + 0.1f);
    glEnd();

    // Leaves layers - darker bottom, lighter top
    glColor3f(0.0f, 0.4f, 0.0f);
    drawCircle(x, y + 0.13f, 0.05f);
    drawCircle(x + 0.04f, y + 0.15f, 0.045f);
    drawCircle(x - 0.04f, y + 0.15f, 0.045f);

    glColor3f(0.0f, 0.5f, 0.0f);
    drawCircle(x, y + 0.18f, 0.045f);
    drawCircle(x + 0.03f, y + 0.20f, 0.04f);
    drawCircle(x - 0.03f, y + 0.20f, 0.04f);

    glColor3f(0.0f, 0.6f, 0.0f);
    drawCircle(x, y + 0.23f, 0.04f);
    drawCircle(x + 0.02f, y + 0.25f, 0.035f);
    drawCircle(x - 0.02f, y + 0.25f, 0.035f);

    glColor3f(0.2f, 0.8f, 0.2f);
    drawCircle(x, y + 0.28f, 0.03f);
    drawCircle(x + 0.015f, y + 0.30f, 0.025f);
    drawCircle(x - 0.015f, y + 0.30f, 0.025f);

    glColor3f(0.5f, 1.0f, 0.5f);
    drawCircle(x, y + 0.33f, 0.02f);
}


void drawSmallPlant(float x, float y) {
    glColor3f(0.1f, 0.5f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x - 0.01f, y + 0.02f);
    glVertex2f(x + 0.01f, y + 0.02f);
    glEnd();
}

void drawCar() {
    glPushMatrix();
    glTranslatef(carPosX, -0.5f, 0.0f);

    glColor3f(raining ? 0.3f : 0.8f, raining ? 0.5f : 0.0f, raining ? 0.8f : 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.2f, 0.0f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.2f, 0.05f);
    glVertex2f(-0.2f, 0.05f);
    glEnd();

    glColor3f(raining ? 0.4f : 1.0f, raining ? 0.7f : 0.5f, raining ? 0.9f : 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.1f, 0.05f);
    glVertex2f(0.1f, 0.05f);
    glVertex2f(0.06f, 0.1f);
    glVertex2f(-0.06f, 0.1f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-0.12f, 0.0f, 0.0f);
    glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
    drawCircle(0.0f, 0.0f, 0.025f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.12f, 0.0f, 0.0f);
    glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
    drawCircle(0.0f, 0.0f, 0.025f);
    glPopMatrix();

    glPopMatrix();
}

void drawRaindrop(const Raindrop& drop) {
    glColor3f(0.2f, 0.2f, drop.blueShade);
    drawOval(drop.x, drop.y, drop.sizeX, drop.sizeY);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Sky
    glColor3f(0.6f, 0.9f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Ground top
    glColor3f(0.5f, 0.35f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(1.0f, -0.4f);
    glVertex2f(-1.0f, -0.4f);
    glEnd();

    // Road
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, -0.6f);
    glVertex2f(1.0f, -0.6f);
    glVertex2f(1.0f, -0.4f);
    glVertex2f(-1.0f, -0.4f);
    glEnd();

    // Ground bottom
    glColor3f(0.5f, 0.35f, 0.2f);
    glBegin(GL_POLYGON);
    glVertex2f(-1.0f, -0.6f);
    glVertex2f(1.0f, -0.6f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Road white lines
    glColor3f(1.0f, 1.0f, 1.0f);
    for (float x = -1.0f; x < 1.0f; x += 0.2f) {
        glBegin(GL_POLYGON);
        glVertex2f(x, -0.505f);
        glVertex2f(x + 0.1f, -0.505f);
        glVertex2f(x + 0.1f, -0.495f);
        glVertex2f(x, -0.495f);
        glEnd();
    }

    // Clouds
    drawCloud(cloudX - 0.5f, 0.8f);
    drawCloud(cloudX + 0.2f, 0.85f);
    drawCloud(cloudX - 1.0f, 0.75f);

    // Trees
    for (float x = -0.9f; x <= 0.9f; x += 0.4f)
        drawTree(x, -0.2f);

    // Small plants
    for (float x = -1.0f; x <= 1.0f; x += 0.05f) {
        drawSmallPlant(x, -0.4f);
        drawSmallPlant(x, -0.6f);
    }

    drawCar();

    if (raining) {
        for (const auto& drop : raindrops)
            drawRaindrop(drop);
    }

    glutSwapBuffers();
}

void timer(int) {
    carPosX += direction * carSpeed;
    wheelAngle -= direction * (carSpeed * 1000);

    if (carPosX > 1.2f) carPosX = -1.2f;
    if (carPosX < -1.2f) carPosX = 1.2f;

    cloudX += 0.0005f;
    if (cloudX > 2.0f) cloudX = -1.0f;

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (currentTime - lastRainToggleTime >= 3000) {
        raining = !raining;
        lastRainToggleTime = currentTime;

        if (raining) {
            raindrops.clear();
            for (int i = 0; i < 100; i++) {
                Raindrop drop;
                drop.x = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
                drop.y = 1.0f + static_cast<float>(rand()) / (RAND_MAX / 0.5f);
                drop.speed = 0.01f + static_cast<float>(rand()) / (RAND_MAX / 0.01f);
                drop.sizeX = 0.002f + static_cast<float>(rand()) / (RAND_MAX / 0.004f);
                drop.sizeY = 0.008f + static_cast<float>(rand()) / (RAND_MAX / 0.012f);
                drop.blueShade = 0.5f + static_cast<float>(rand()) / (RAND_MAX / 0.5f);
                raindrops.push_back(drop);
            }
        } else {
            raindrops.clear();
        }
    }

    if (raining) {
        for (auto& drop : raindrops) {
            drop.y -= drop.speed;
            if (drop.y < -1.0f) {
                drop.y = 1.0f;
                drop.x = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case '+':
        case '=':
            carSpeed += 0.001f;
            if (carSpeed > 0.05f) carSpeed = 0.05f;
            break;
        case '-':
            carSpeed -= 0.001f;
            if (carSpeed < 0.001f) carSpeed = 0.001f;
            break;
        case 'r':
        case 'R':
            direction = -1;
            break;
        case 'm':
        case 'M':
            direction = 1;
            break;
        case 27:
            exit(0);
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("Car Scene with Rain, Trees, Clouds, and Plants");

    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
