#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include "Elf2D.h"
#include "ElfMath.h"
#include "ElfTime.h"
#include "ElfObject.h"

#define WIDTH 60
#define HEIGHT 25

void Initialize(GameObject_Line* obj, int objNum)
{
    // 삼각형의 중심 좌표 설정
    obj[0].Position.x = WIDTH / 2;
    obj[0].Position.y = HEIGHT / 2;

    obj[0].Rotation = 0; // 초기 회전값 설정

    // 정삼각형 꼭짓점 세 변 설정
    obj[0].Line[0].x = 0;
    obj[0].Line[0].y = -5; // 꼭짓점 A (중앙 위)
    obj[0].Line[1].x = -5;
    obj[0].Line[1].y = 4; // 꼭짓점 B (왼쪽 아래)

    obj[1].Line[0].x = -5;
    obj[1].Line[0].y = 4; // 꼭짓점 B (왼쪽 아래)
    obj[1].Line[1].x = 5;
    obj[1].Line[1].y = 4; // 꼭짓점 C (오른쪽 아래)

    obj[2].Line[0].x = 5;
    obj[2].Line[0].y = 4; // 꼭짓점 C (오른쪽 아래)
    obj[2].Line[1].x = 0;
    obj[2].Line[1].y = -5; // 꼭짓점 A (중앙 위)

    // 삼각형의 크기 초기화
    for (int i = 0; i < objNum; i++) {
        obj[i].Scale.x = 1;
        obj[i].Scale.y = 1;
    }
}

int Input()
{
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { // ESC 키
        return 99;
    }
    else if (GetAsyncKeyState('A') & 0x8000) { // A 키
        return 1; // 왼쪽 회전
    }
    else if (GetAsyncKeyState('D') & 0x8000) { // D 키
        return 2; // 오른쪽 회전
    }
    else if (GetAsyncKeyState('W') & 0x8000) { // W 키
        return 3; // 전진
    }
    else if (GetAsyncKeyState('S') & 0x8000) { // S 키
        return 4; // 후진
    }
    return 0;
}


void Update(GameObject_Line* obj, int objNum, int e)
{
    float speed = 0.1f; // 이동 속도 (프레임당 0.1px)

    for (int i = 0; i < objNum; i++) {
        if (e == 1) { // 왼쪽 회전
            obj[i].Rotation -= 1;
            if (obj[i].Rotation < 0) {
                obj[i].Rotation += 360;
            }
        }
        else if (e == 2) { // 오른쪽 회전
            obj[i].Rotation += 1;
            if (obj[i].Rotation >= 360) {
                obj[i].Rotation -= 360;
            }
        }
        else if (e == 3) { // 전진
            float radians = obj[i].Rotation * (3.14159265f / 180.0f); // 각도를 라디안으로 변환
            obj[i].Position.x += cos(radians) * speed;
            obj[i].Position.y -= sin(radians) * speed;
        }
        else if (e == 4) { // 후진
            float radians = obj[i].Rotation * (3.14159265f / 180.0f); // 각도를 라디안으로 변환
            obj[i].Position.x -= cos(radians) * speed;
            obj[i].Position.y += sin(radians) * speed;
        }
    }
}



void DrawPoint(Vector3 point, char* Buf, int width, int height)
{
    int x = (int)point.x;
    int y = (int)point.y;
    if (x >= 0 && x < width && y >= 0 && y < height) {
        Buf[y * (width + 1) + x] = '+';
        if (x + 1 < width) {
            Buf[y * (width + 1) + (x + 1)] = '+'; // "++" 기호 생성
        }
    }
}

void DrawLineWithDoubleAt(Vector3 start, Vector3 end, char* Buf, int width, int height)
{
    int x1 = (int)start.x;
    int y1 = (int)start.y;
    int x2 = (int)end.x;
    int y2 = (int)end.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
            Buf[y1 * (width + 1) + x1] = '@';
            if (x1 + 1 < width) {
                Buf[y1 * (width + 1) + (x1 + 1)] = '@';
            }
        }
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void FillTriangle(Vector3 v0, Vector3 v1, Vector3 v2, char* Buf, int width, int height)
{
    // 삼각형의 경계 박스를 계산
    int minX = (int)fmin(fmin(v0.x, v1.x), v2.x);
    int maxX = (int)fmax(fmax(v0.x, v1.x), v2.x);
    int minY = (int)fmin(fmin(v0.y, v1.y), v2.y);
    int maxY = (int)fmax(fmax(v0.y, v1.y), v2.y);

    // 화면 경계로 제한
    minX = fmax(minX, 0);
    maxX = fmin(maxX, width - 1);
    minY = fmax(minY, 0);
    maxY = fmin(maxY, height - 1);

    // 삼각형 내부 점 채우기
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // 점 (x, y)가 삼각형 내부에 있는지 확인 (바리센트릭 좌표 계산)
            float w0 = (v1.x - v0.x) * (y - v0.y) - (v1.y - v0.y) * (x - v0.x);
            float w1 = (v2.x - v1.x) * (y - v1.y) - (v2.y - v1.y) * (x - v1.x);
            float w2 = (v0.x - v2.x) * (y - v2.y) - (v0.y - v2.y) * (x - v2.x);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                Buf[y * (width + 1) + x] = '@';
                if (x + 1 < width) {
                    Buf[y * (width + 1) + (x + 1)] = '@';
                }
            }
        }
    }
}

void Render(GameObject_Line* obj, int objNum, char* Buf, int width, int height)
{
    Elf2DClearScreen(Buf, width, height); // 화면 초기화

    Vector3 transformed[3];

    // 삼각형의 각 꼭짓점을 변환
    for (int i = 0; i < 3; i++) {
        Matrix3x3 world = identity_matrix();
        Matrix3x3 scale = scale_matrix(obj[0].Scale.x, obj[0].Scale.y);
        Matrix3x3 rotation = rotation_matrix(obj[0].Rotation);
        Matrix3x3 translation = translation_matrix(obj[0].Position.x, obj[0].Position.y);

        world = multiply_matrices(scale, world);
        world = multiply_matrices(rotation, world);
        world = multiply_matrices(translation, world);

        transformed[i] = multiply_matrix_vector(world, (Vector3) { obj[i].Line[0].x, obj[i].Line[0].y, 1 });
    }

    // 삼각형 내부 채우기
    FillTriangle(transformed[0], transformed[1], transformed[2], Buf, width, height);

    // 삼각형의 변 그리기
    for (int i = 0; i < objNum; i++) {
        Vector3 lineA, lineB;
        lineA = transformed[i % 3];
        lineB = transformed[(i + 1) % 3];

        if ((int)lineA.x >= 0 && (int)lineA.x < width && (int)lineA.y >= 0 && (int)lineA.y < height &&
            (int)lineB.x >= 0 && (int)lineB.x < width && (int)lineB.y >= 0 && (int)lineB.y < height) {
            DrawLineWithDoubleAt(lineA, lineB, Buf, width, height);
        }
    }

    // 삼각형 꼭짓점에 점 추가
    DrawPoint(transformed[0], Buf, width, height);
}

int main()
{
    int fps = 30; // 프레임 속도 설정
    double frameTime = 1000.0 / fps;

    char screenBuffer[(WIDTH + 1) * HEIGHT];
    int screenWidth = WIDTH;
    int screenHeight = HEIGHT;

    GameObject_Line LineObj[3];

    Initialize(LineObj, 3);

    Elf2DInitScreen();
    Elf2DClearScreen(screenBuffer, screenWidth, screenHeight);
    Elf2DDrawBuffer(screenBuffer);

    ElfTimeInitialize();

    int isGameRunning = 1;
    while (isGameRunning) {
        int gameEvent = Input();

        if (gameEvent == 99) {
            isGameRunning = 0;
        }

        Update(LineObj, 3, gameEvent);
        Render(LineObj, 3, screenBuffer, screenWidth, screenHeight);
        Elf2DDrawBuffer(screenBuffer);

        ElfTimeCalculateDeltaTime();
        Elf2DSleep((int)(frameTime - ElfTimeGetDeltaTime()));
    }

    return 0;
}
