/**
 * Simulador de Braço Robótico em OpenGL
 * Dupla: Sérgio Matheus Araujo Silva e Brisa Tielly Almeida da Silva
 * g++ braco_robotico.cpp -o braco_robotico -lGL -lGLU -lglut  (LINUX)
 * g++ -o braco_robotico.exe braco_robotico.cpp -I./include -L./lib/x64 -lfreeglut -lopengl32 -lglu32 (WINDOWS)
 */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Definições para os segmentos
#define BASE 0
#define BRACO 1
#define ANTEBRACO 2
#define PUNHO 3

// Estrutura para representar um segmento do braço robótico
typedef struct {
  float comprimento;
  float largura;
  float altura;
  float anguloX;
  float anguloY;
  float anguloZ;
  float cor[3];
} SegmentoRobo;

// Estrutura para representar a garra
typedef struct {
  float abertura;
  float comprimento;
  float largura;
  float cor[3];
} Garra;

// Variáveis globais
SegmentoRobo base, braco, antebraco, punho;
Garra garra;
int segmentoAtual = BRACO;
float rotX = 20.0, rotY = 0.0, distCamera = 15.0;
float translacaoX = 0.0, translacaoZ = 0.0;
int lastX = 0, lastY = 0;
bool botaoEsquerdoPressionado = false, botaoDireitoPressionado = false;
GLUquadric *quad = nullptr;

// Funções de desenho
void desenharCilindroX(float comprimento, float raio, float cor[3]) {
  glColor3fv(cor);
  glPushMatrix();
  glRotatef(90, 0, 1, 0);
  gluCylinder(quad, raio, raio, comprimento, 20, 5);
  glPopMatrix();
}

void desenharJunta(float raio, float cor[3]) {
  glColor3fv(cor);
  gluSphere(quad, raio, 20, 20);
}

void desenharTexto(float x, float y, const char *string) {
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glColor3f(1.0, 1.0, 1.0);
  glRasterPos2f(x, y);
  for (const char *c = string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void desenharInstrucoes() {
  char buffer[100];
  int altura = glutGet(GLUT_WINDOW_HEIGHT);
  int y = altura - 20;
  int passo = 15;

  desenharTexto(10, y, "BRACO ROBOTICO - INSTRUCOES:");
  y -= passo * 2;
  desenharTexto(
      10, y,
      "0-3: Selecionar componente (Base, Braco, Antebraco, Punho)");
  y -= passo;
  
  sprintf(buffer, "Componente atual: %d", segmentoAtual);
  desenharTexto(10, y, buffer);
  y -= passo * 2;
  desenharTexto(10, y, "A/D: Rotacao em X");
  y -= passo;
  desenharTexto(10, y, "W/S: Rotacao em Y");
  y -= passo;
  desenharTexto(10, y, "Q/E: Rotacao em Z");
  y -= passo * 2;
  desenharTexto(10, y, "G/H: Abrir/Fechar garra");
  y -= passo * 2;
  desenharTexto(10, y, "Setas: Mover base (translacao X/Z)");
  y -= passo * 2;
  desenharTexto(10, y, "Mouse esquerdo: Rotacionar camera");
  y -= passo;
  desenharTexto(10, y, "Mouse direito: Zoom (cima/baixo)");
  y -= passo;
  desenharTexto(10, y, "R: Resetar posicao da camera");
}

void desenharGarra() {
  glPushMatrix();

  // Garra em tons de branco
  glColor3f(garra.cor[0], garra.cor[1], garra.cor[2]);

  // Base da garra
  glPushMatrix();
  glScalef(0.5, 0.3, 0.5);
  glutSolidCube(1.0);
  glPopMatrix();

  // Dedo superior
  glPushMatrix();
  glTranslatef(0.25, 0.0, 0.0);
  glRotatef(-garra.abertura / 2, 0.0, 0.0, 1.0);
  glTranslatef(garra.comprimento / 2, 0.0, 0.0);
  glScalef(garra.comprimento, garra.largura, garra.largura * 2);
  glutSolidCube(1.0);
  glPopMatrix();

  // Dedo inferior
  glPushMatrix();
  glTranslatef(0.25, 0.0, 0.0);
  glRotatef(garra.abertura / 2, 0.0, 0.0, 1.0);
  glTranslatef(garra.comprimento / 2, 0.0, 0.0);
  glScalef(garra.comprimento, garra.largura, garra.largura * 2);
  glutSolidCube(1.0);
  glPopMatrix();

  glPopMatrix();
}

void desenharBracoRobotico() {
  glPushMatrix();
  glTranslatef(translacaoX, 0.0, translacaoZ);

  // BASE
  glPushMatrix();
  glColor3fv(base.cor);
  glTranslatef(0.0, 0.0, 0.0);
  glRotatef(-90, 1, 0, 0);
  gluCylinder(quad, base.largura / 2, base.largura / 2, base.altura, 30, 5);
  glPopMatrix();

  glTranslatef(0.0, base.altura, 0.0);
  desenharJunta(0.5, braco.cor);

  // BRAÇO
  glPushMatrix();
  glRotatef(braco.anguloX, 1, 0, 0);
  glRotatef(braco.anguloY, 0, 1, 0);
  glRotatef(braco.anguloZ, 0, 0, 1);
  desenharCilindroX(braco.comprimento, braco.largura / 2, braco.cor);
  glTranslatef(braco.comprimento, 0, 0);
  desenharJunta(0.47, antebraco.cor);

  // ANTEBRAÇO
  glPushMatrix();
  glRotatef(antebraco.anguloX, 1, 0, 0);
  glRotatef(antebraco.anguloY, 0, 1, 0);
  glRotatef(antebraco.anguloZ, 0, 0, 1);
  desenharCilindroX(antebraco.comprimento, antebraco.largura / 2,
                    antebraco.cor);
  glTranslatef(antebraco.comprimento, 0, 0);
  desenharJunta(0.36, punho.cor);

  // PUNHO
  glPushMatrix();
  glRotatef(punho.anguloX, 1, 0, 0);
  glRotatef(punho.anguloY, 0, 1, 0);
  glRotatef(punho.anguloZ, 0, 0, 1);
  desenharCilindroX(punho.comprimento, punho.largura / 2, punho.cor);
  glTranslatef(punho.comprimento, 0, 0);
  desenharJunta(0.13, garra.cor);

  // GARRA
  desenharGarra();

  glPopMatrix(); // punho
  glPopMatrix(); // antebraço
  glPopMatrix(); // braço
  glPopMatrix(); // global
}

void inicializarRobo() {
  // Base
  base.comprimento = 2.0;
  base.largura = 2.0;
  base.altura = 0.5;
  base.anguloX = base.anguloY = base.anguloZ = 0.0;
  base.cor[0] = 1.0;
  base.cor[1] = 1.0;
  base.cor[2] = 1.0;

  // Braço
  braco.comprimento = 3.0;
  braco.largura = 1.0;
  braco.altura = 0.5;
  braco.anguloX = braco.anguloY = braco.anguloZ = 0.0;
  braco.cor[0] = 0.2;
  braco.cor[1] = 0.2;
  braco.cor[2] = 0.2;

  // Antebraço
  antebraco.comprimento = 2.5;
  antebraco.largura = 0.8;
  antebraco.altura = 0.4;
  antebraco.anguloX = antebraco.anguloY = antebraco.anguloZ = 0.0;
  antebraco.cor[0] = 0.4;
  antebraco.cor[1] = 0.4;
  antebraco.cor[2] = 0.4;

  // Punho
  punho.comprimento = 1.0;
  punho.largura = 0.6;
  punho.altura = 0.3;
  punho.anguloX = punho.anguloY = punho.anguloZ = 0.0;
  punho.cor[0] = 0.6;
  punho.cor[1] = 0.6;
  punho.cor[2] = 0.6;

  // Garra
  garra.abertura = 30.0;
  garra.comprimento = 1.0;
  garra.largura = 0.2;
  garra.cor[0] = 0.8;
  garra.cor[1] = 0.8;
  garra.cor[2] = 0.8;
}

// Funções de manipulação da interface
void desenhar() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0,
                 (GLfloat)glutGet(GLUT_WINDOW_WIDTH) /
                     (GLfloat)glutGet(GLUT_WINDOW_HEIGHT),
                 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(distCamera * sin(rotY * M_PI / 180) * cos(rotX * M_PI / 180),
            distCamera * sin(rotX * M_PI / 180),
            distCamera * cos(rotY * M_PI / 180) * cos(rotX * M_PI / 180), 0.0,
            1.0, 0.0, 0.0, 1.0, 0.0);

  desenharBracoRobotico();
  desenharInstrucoes();

  glutSwapBuffers();
}

void teclas(unsigned char tecla, int x, int y) {
  float incrementoAngulo = 5.0;

  switch (tecla) {
  // Seleção de segmentos
  case '0':
    segmentoAtual = BASE;
    break;
  case '1':
    segmentoAtual = BRACO;
    break;
  case '2':
    segmentoAtual = ANTEBRACO;
    break;
  case '3':
    segmentoAtual = PUNHO;
    break;

  // Rotação em X
  case 'a':
  case 'A':
    switch (segmentoAtual) {
    case BASE:
      base.anguloX = fmod(base.anguloX - incrementoAngulo, 360.0);
      break;
    case BRACO:
      braco.anguloX = fmod(braco.anguloX - incrementoAngulo, 360.0);
      break;
    case ANTEBRACO:
      antebraco.anguloX = fmod(antebraco.anguloX - incrementoAngulo, 360.0);
      break;
    case PUNHO:
      punho.anguloX = fmod(punho.anguloX - incrementoAngulo, 360.0);
      break;
    }
    break;
  case 'd':
  case 'D':
    switch (segmentoAtual) {
    case BASE:
      base.anguloX = fmod(base.anguloX + incrementoAngulo, 360.0);
      break;
    case BRACO:
      braco.anguloX = fmod(braco.anguloX + incrementoAngulo, 360.0);
      break;
    case ANTEBRACO:
      antebraco.anguloX = fmod(antebraco.anguloX + incrementoAngulo, 360.0);
      break;
    case PUNHO:
      punho.anguloX = fmod(punho.anguloX + incrementoAngulo, 360.0);
      break;
    }
    break;

  // Rotação em Y
  case 'w':
  case 'W':
    switch (segmentoAtual) {
    case BASE:
      base.anguloY = fmod(base.anguloY - incrementoAngulo, 360.0);
      break;
    case BRACO:
      braco.anguloY = fmod(braco.anguloY - incrementoAngulo, 360.0);
      break;
    case ANTEBRACO:
      antebraco.anguloY = fmod(antebraco.anguloY - incrementoAngulo, 360.0);
      break;
    case PUNHO:
      punho.anguloY = fmod(punho.anguloY - incrementoAngulo, 360.0);
      break;
    }
    break;
  case 's':
  case 'S':
    switch (segmentoAtual) {
    case BASE:
      base.anguloY = fmod(base.anguloY + incrementoAngulo, 360.0);
      break;
    case BRACO:
      braco.anguloY = fmod(braco.anguloY + incrementoAngulo, 360.0);
      break;
    case ANTEBRACO:
      antebraco.anguloY = fmod(antebraco.anguloY + incrementoAngulo, 360.0);
      break;
    case PUNHO:
      punho.anguloY = fmod(punho.anguloY + incrementoAngulo, 360.0);
      break;
    }
    break;

  // Rotação em Z
  case 'q':
  case 'Q':
    switch (segmentoAtual) {
    case BASE:
      base.anguloZ = fmod(base.anguloZ - incrementoAngulo, 360.0);
      break;
    case BRACO:
      braco.anguloZ = fmod(braco.anguloZ - incrementoAngulo, 360.0);
      break;
    case ANTEBRACO:
      antebraco.anguloZ = fmod(antebraco.anguloZ - incrementoAngulo, 360.0);
      break;
    case PUNHO:
      punho.anguloZ = fmod(punho.anguloZ - incrementoAngulo, 360.0);
      break;
    }
    break;
  case 'e':
  case 'E':
    switch (segmentoAtual) {
    case BASE:
      base.anguloZ = fmod(base.anguloZ + incrementoAngulo, 360.0);
      break;
    case BRACO:
      braco.anguloZ = fmod(braco.anguloZ + incrementoAngulo, 360.0);
      break;
    case ANTEBRACO:
      antebraco.anguloZ = fmod(antebraco.anguloZ + incrementoAngulo, 360.0);
      break;
    case PUNHO:
      punho.anguloZ = fmod(punho.anguloZ + incrementoAngulo, 360.0);
      break;
    }
    break;

  // Controle da garra
  case 'g':
  case 'G':
    garra.abertura = fmin(garra.abertura + 5.0, 90.0);
    break;
  case 'h':
  case 'H':
    garra.abertura = fmax(garra.abertura - 5.0, 0.0);
    break;

  // Reset da câmera
  case 'r':
  case 'R':
    rotX = 20.0;
    rotY = 0.0;
    distCamera = 15.0;
    break;

  // Sair
  case 27: // ESC
    exit(0);
    break;
  }

  glutPostRedisplay();
}

void teclasEspeciais(int tecla, int x, int y) {
  float incrementoTranslacao = 0.2;

  switch (tecla) {
  case GLUT_KEY_UP:
    translacaoZ -= incrementoTranslacao;
    break;
  case GLUT_KEY_DOWN:
    translacaoZ += incrementoTranslacao;
    break;
  case GLUT_KEY_LEFT:
    translacaoX -= incrementoTranslacao;
    break;
  case GLUT_KEY_RIGHT:
    translacaoX += incrementoTranslacao;
    break;
  }

  glutPostRedisplay();
}

void mouse(int botao, int estado, int x, int y) {
  if (botao == GLUT_LEFT_BUTTON) {
    botaoEsquerdoPressionado = (estado == GLUT_DOWN);
    if (botaoEsquerdoPressionado) {
      lastX = x;
      lastY = y;
    }
  } else if (botao == GLUT_RIGHT_BUTTON) {
    botaoDireitoPressionado = (estado == GLUT_DOWN);
    if (botaoDireitoPressionado)
      lastY = y;
  }
}

void movimentoMouse(int x, int y) {
  int deltaX = x - lastX;
  int deltaY = y - lastY;

  if (botaoEsquerdoPressionado) {
    rotY += 0.5 * deltaX;
    rotX += 0.5 * deltaY;

    // Limita rotação vertical
    if (rotX > 89.0)
      rotX = 89.0;
    if (rotX < -89.0)
      rotX = -89.0;
  } else if (botaoDireitoPressionado) {
    distCamera += 0.1 * deltaY;

    // Limita distância
    if (distCamera < 5.0)
      distCamera = 5.0;
    if (distCamera > 30.0)
      distCamera = 30.0;
  }

  lastX = x;
  lastY = y;
  glutPostRedisplay();
}

void redimensionar(int largura, int altura) {
  altura = (altura == 0) ? 1 : altura;
  glViewport(0, 0, largura, altura);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat)largura / (GLfloat)altura, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void inicializar() {
  // Cor de fundo e profundidade
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

  // Configuração da iluminação
  GLfloat luzAmbiente[4] = {0.2, 0.2, 0.2, 1.0};
  GLfloat luzDifusa[4] = {0.7, 0.7, 0.7, 1.0};
  GLfloat luzEspecular[4] = {1.0, 1.0, 1.0, 1.0};
  GLfloat posicaoLuz[4] = {5.0, 10.0, 5.0, 1.0};

  glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
  glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
  glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  inicializarRobo();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Braco Robotico Articulado");

  quad = gluNewQuadric();
  gluQuadricNormals(quad, GLU_SMOOTH);

  glutDisplayFunc(desenhar);
  glutReshapeFunc(redimensionar);
  glutKeyboardFunc(teclas);
  glutSpecialFunc(teclasEspeciais);
  glutMouseFunc(mouse);
  glutMotionFunc(movimentoMouse);

  inicializar();
  glutMainLoop();

  return 0;
}
