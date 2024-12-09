#ifndef ELF2D_H  // ELFMATH_H�� ���ǵǾ� ���� ������
#define ELF2D_H  // ELFMATH_H�� �����ϰ� �Ʒ� �ڵ带 ������

void Elf2DClearScreen(char* Buffer, int width, int height);

void Elf2DDrawBuffer(char* Buffer);

void Elf2DDrawLine(int x1, int y1, int x2, int y2, char* Buffer, int width, int height);

void Elf2DDrawLine2(float x1, float y1, float x2, float y2, char* Buffer, int width, int height);

void Elf2DInitScreen();

void Elf2DSleep(int ms);

#endif