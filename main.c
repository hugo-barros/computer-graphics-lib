#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cg2d.h"

#define PI 3.1415926535

int main(int argc, char **argv){
  bufferdevice * meu_bd;
  window * w1;
  palette * palette1;
  point * p1, * p2, * p3, * p4, * p5, * p6;
  object * o1, * o2, * o3, * o4;
 // printf("Hello World\n");

  SetWorld(0, 1024, 0, 1024);
  printf("SetWorld\n");
  meu_bd = CreateBuffer(512,512);
  printf("Atribuicao buffer meu bd\n");
  w1 = CreateWindow(0, 400, 0, 400);
  printf("create window\n");
  palette1 = CreatePalette(2);
  printf("Atribuicao palleta\n");
  SetColor(0.0, 0.0, 0.0, palette1);
  printf("setando cor1\n");
  SetColor(0.1, 0.1, 0.8, palette1);
  printf("setando cor2\n");
  SetColor(1.0, 1.0, 1.0, palette1);
  printf("setando cor3\n");

  o1 = CreateObject(6);

  p1 = SetPoint(20, 20, 2);
  printf("atribui ponto1\n");
  p2 = SetPoint(60, 60, 2);
  printf("eeatribuindo ponto2\n");
  p3 = SetPoint(100, 10, 2);
  p4 = SetPoint(150, 150, 2);
  p5 = SetPoint(20, 100, 2);
  p6 = SetPoint(20, 21, 2);

  SetObject(p1, o1);
  SetObject(p2, o1);
  SetObject(p3, o1);
  SetObject(p4, o1);
  SetObject(p5, o1);
  SetObject(p6, o1);

  //DrawLine(p1, p2, w1, meu_bd, 1);
  //DrawLine(p2, p3, w1, meu_bd, 2);

  printf("\n\ntadebrimks");
  o2 = Rotate(o1, 90*PI/180);
  o3 = Scale(o2, 0.2, 0.2);
  o4 = Translate(o1, 50, 100);
  ChangeColor(o4, 2);
  ChangeColor(o3, 1);
  DrawObject(o4, w1, meu_bd);
  //Fill(o1, w1, meu_bd, 2);


  Dump2X(meu_bd, palette1);
  printf("Chamando dumpx\n");
  return 0;

}
