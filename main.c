#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define EventMask (KeyPressMask | ExposureMask)

#define PI 3.1415926535

/* Exemplo de estruturas de dados para a
   biblioteca de funções gráficas em C, considerando
   o plano como referência para o mundo */

float XWMax, XWMin, YWMax, YWMin;

struct Window {
  float xmin,
        xmax,
        ymin,
        ymax;
        };

typedef struct Window window;

struct ViewPort {
  int xmin,
      xmax,
      ymin,
      ymax;
      };

typedef struct Viewport viewport;

struct Point2D {
  float x,
        y;
  int   color;
  };

typedef struct Point2D point;

struct Object2D {
  int numbers_of_points;
  point * points;
  };

typedef struct Object2D object;

struct Color {
  float red,
        green,
        blue;
        };

typedef struct Color ColorValues;

struct Palette {
  int numbers_of_colors;
  ColorValues * colors;
  };

typedef struct Palette palette;

struct Buffer {
  int MaxX,
      MaxY;
  int * buffer;
  };

typedef struct Buffer bufferdevice;

struct HPoint2D {
  float x,
        y,
        w;
        };

typedef struct HPoint2D hpoint;

struct HMatrix2D {
  float a11, a12, a13,
        a21, a22, a23,
        a31, a32, a33;
        };

typedef struct HMatrix2D hmatrix;


  /* estabelece os limites do mundo */
void SetWorld(float, float, float, float);

/* cria pontos e objetos no mundo */
point * SetPoint(float, float, int);
object * CreateObject(int);
int SetObject(point *, object *);

/* sistemas de referências */
window * CreateWindow(float, float, float, float);
point * Sru2Srn(point *, window *);
bufferdevice * CreateBuffer(int, int);
point * Srn2Srd(point *, bufferdevice *);

/* funções para criar e gerenciar uma
   paleta e cores */
palette * CreatePalette(int);
int SetColor(float, float, float, palette *);
ColorValues * GetColor(int, palette *);
object * ChangeColor(object *, int);

/* funções para conversão matricial
   e preenchimento de objetos */
int DrawLine(point *, point *, window *, bufferdevice *, int);
int DrawObject(object *, window *, bufferdevice *);
int Fill(object *, window *, bufferdevice *, int);

/* operações com objetos no mundo */
object * Rotate(object *, float);
object * Translate(object *, float, float);
object * Scale(object *, float, float);
hpoint * LinearTransf(hmatrix *, hpoint *);
hmatrix * ComposeMatrix(hmatrix *, hmatrix *);
hmatrix * SetRotMatrix(float);
hmatrix * SetSclMatrix(float, float);
hmatrix * SetSftMatrix(float, float);

/* visualiza o buffer (SRD) no monitor virtual */
int Dump2X(bufferdevice *, palette *);



/* estabelece os limites do mundo */
void SetWorld(float xmin, float xmax, float ymin, float ymax){
  XWMin = xmin;
  XWMax = xmax;
  YWMin = ymin;
  YWMax = ymax;
}

/* cria pontos e objetos no mundo */
point * SetPoint(float x, float y, int color){
  point * p;
  p = (point *) malloc(sizeof(point));
  p->x = x;
  p->y = y;
  p->color = color;

  return p;
}

object * CreateObject(int n){
  object * o;
  o = (object *) malloc(sizeof(object));
  o->numbers_of_points = 0;
  o->points = (point *) malloc (n * sizeof(point));
  return o;
}


int SetObject(point * p, object * obj){
  obj->points[obj->numbers_of_points] = *p;
  obj->numbers_of_points++;
  return obj->numbers_of_points; // (???????)
}

/* sistemas de referências */
window * CreateWindow(float xmin, float xmax, float ymin, float ymax) {
  window * w;
  w = (window *) malloc(sizeof(window));
  w->xmin = xmin;
  w->xmax = xmax;
  w->ymin = ymin;
  w->ymax = ymax;

  return w;
}


point * Sru2Srn(point * p, window * w){

  double ncx, ncy;
  ncx = (p->x - w->xmin) / (w->xmax - w->xmin);
  ncy = (p->y - w->ymin) / (w->ymax - w->ymin);

  return SetPoint(ncx, ncy, p->color);
}


bufferdevice * CreateBuffer(int x, int y){
  bufferdevice * bd;
  bd = (bufferdevice *) malloc(2 * sizeof(int) + x*y*sizeof(int));

  bd->MaxX = x;
  bd->MaxY = y;
  bd->buffer = (int *) malloc (x*y*sizeof(int));
  return bd;

}


point * Srn2Srd(point * p, bufferdevice * bd){
  int dcx, dcy;
  dcx = round(p->x * (bd->MaxX - 1));
  dcy = round(p->y * (bd->MaxY - 1));

  return SetPoint(dcx, dcy, p->color);
}

/* funções para criar e gerenciar uma
   paleta e cores */
palette * CreatePalette(int n){
  palette * pal;
  pal->numbers_of_colors = 0;

  pal->colors = (ColorValues *) malloc(n * sizeof(ColorValues));
  return pal;

}

int SetColor(float r, float g, float b, palette * pal){
  ColorValues * cv;
  cv = (ColorValues *) malloc(sizeof(ColorValues));
  cv->red = r;
  cv->green = g;
  cv->blue = b;

  pal->numbers_of_colors++;
  //pal->colors  = realloc(pal->colors, pal->numbers_of_colors * sizeof(ColorValues));
  pal->colors[pal->numbers_of_colors-1] = *cv;
  return pal->numbers_of_colors - 1; // retornando posição da nova cor dentro do vetor em pallete->colors

}

ColorValues * GetColor(int n, palette * pal){
  ColorValues * cv = &(pal->colors[n]);

  return cv;
}

object * ChangeColor(object * obj, int n){
  int i;
  for (i = 0; i < obj->numbers_of_points; i++)
    obj->points[i].color = n;

}

/* funções para conversão matricial
   e preenchimento de objetos */
int DrawLine(point * point1, point * point2, window * w, bufferdevice * bd, int color){
  int x1;
  int x2;
  int y1;
  int y2;
  int dx, dy, p, p2, xy2, x, y, xf;

  printf("wxmax: %f ", w->xmax);
  printf("wxmin: %f ", w->xmin);
  printf("wymax: %f ", w->ymax);
  printf("wymin: %f ", w->ymin);

  printf("point1x: %f ", point1->x);
  printf("point1y: %f ", point1->y);

  printf("drawline 1\n");
  // criar funcao para caso os pontos estejam fora da janela.
  point * p1_normalizado = Sru2Srn(point1, w);
  printf("drawline 2\n");
  point * p2_normalizado = Sru2Srn(point2, w);
  printf("drawline 3\n");
  point * p1_d = Srn2Srd(p1_normalizado, bd);
  printf("drawline 4\n");
  point * p2_d = Srn2Srd(p2_normalizado, bd);
  printf("drawline 5\n");
  free(p1_normalizado);
  free(p2_normalizado);

  x1 = p1_d->x;
  x2 = p2_d->x;
  y1 = p1_d->y;
  y2 = p2_d->y;

  dx = x2-x1; // absoluto?
  dy = y2-y1;
  p = 2 * dy - dx;
  p2 = 2 * dy;
  xy2 = 2 * (dy-dx);

  if (x1>x2){
    x = x2;
    y = y2;
    xf = x1;
  } else {
    x = x1;
    y = y1;
    xf = x2;
  }
  printf("drawline 5\n");
  bd->buffer[bd->MaxY * y + x] = color; // colocando o ponto x,y pra ser desenhado mais tarde
  printf("drawline 6\n");
  while (x < xf){
    x++;
    if (p < 0)
      p = p + p2;
    else{
      y++;
      p = p + xy2;
    }
    bd->buffer[bd->MaxY * y + x] = color; // colocando o ponto x,y pra ser desenhado mais tarde
  }
  return 0;
}

int DrawObject(object * obj, window * w, bufferdevice * bd){
  int i;

  if (obj->numbers_of_points > 1)
    for (i=1; i < obj->numbers_of_points; i++){
      DrawLine(&(obj->points[i-1]), &(obj->points[i]), w, bd, obj->points[i-1].color);
    }

}

//int Fill(object *, window *, bufferdevice *, int);

/* operações com objetos no mundo */
object * Rotate(object * obj, float tetha){
  hpoint * hp;
  object * new_obj;
  int n, i;
  hmatrix * rot_matrix;

  hp = (hpoint *) malloc(sizeof (hpoint));
  new_obj = (object *) malloc(sizeof (object));

  rot_matrix = SetRotMatrix(tetha);
  n = obj->numbers_of_points;

  for (i=0; i<n; i++){
    //obj->points[i].x;
    //obj->points[i].y;
    // new_obj->

    // terminar isso, fazendo as multiplicações necessárias
  }
  return new_obj;
}

//object * Translate(object *, float, float);

//object * Scale(object *, float, float);

//hpoint * LinearTransf(hmatrix *, hpoint *);

//hmatrix * ComposeMatrix(hmatrix *, hmatrix *);

hmatrix * SetRotMatrix(float tetha){
  hmatrix * new_m;
  new_m = (hmatrix *) malloc(9 * sizeof(float));

  new_m->a11 = 1;
  new_m->a12 = 0;
  new_m->a13 = 0;

  new_m->a21 = 0;
  new_m->a22 = cos(tetha);
  new_m->a23 = -sin(tetha);

  new_m->a31 = 0;
  new_m->a32 = sin(tetha);
  new_m->a33 = cos(tetha);

  return new_m;
}

hmatrix * SetSclMatrix(float a, float b){
  hmatrix * new_m;
  new_m = (hmatrix *) malloc(9 * sizeof(float));

  new_m->a11 = a;
  new_m->a12 = 0;
  new_m->a13 = 0;

  new_m->a21 = 0;
  new_m->a22 = b;
  new_m->a23 = 0;

  new_m->a31 = 0;
  new_m->a32 = 0;
  new_m->a33 = 1;

  return new_m;

}

//hmatrix * SetSftMatrix(float, float);

int Dump2X(bufferdevice * dev, palette * pal) {
  Display               * display;
  XImage                * ximage;
  Window                window;
  XEvent                an_event;
  GC                    gc;
  Visual                * visual;
  XGCValues             values;
  int                   m, n, screen, dplanes;
  int                   width, height, ret = 1;
  ColorValues           * cor;

  width = dev->MaxX;
  height = dev->MaxY;

  if ((display = XOpenDisplay(NULL)) == NULL) ret = 0;
  else {
    screen = DefaultScreen(display);
    dplanes = DisplayPlanes(display,screen);
    visual = XDefaultVisual(display,screen);

    if (!(window=XCreateSimpleWindow(display,RootWindow(display,screen),0,0,width,height,1,BlackPixel(display,screen),WhitePixel(display,screen)))) ret = 0;
    else {
      XSelectInput(display, window, EventMask);
      XStoreName(display, window, "Monitor Virtual");
      gc = XCreateGC(display, window, 0, &values);

      XMapWindow(display,window);
      XSync(display,False);

      ximage = XCreateImage(display,visual,dplanes,ZPixmap,0,malloc(width*height*sizeof(int)),width,height,8,0);

      for(m=0;m<height;m++) {
        for(n=0;n<width;n++) {
          cor = GetColor(dev->buffer[m*width+n],pal);
          ximage -> data[(m*4)*width+n*4] = (char) round((cor->blue)*255);
          ximage -> data[(m*4)*width+n*4+1] = (char) round((cor->green)*255);
          ximage -> data[(m*4)*width+n*4+2] = (char) round((cor->red)*255);
          ximage -> data[(m*4)*width+n*4+3] = (char) 0;
          }
        }

      XPutImage(display,window,gc,ximage,0,0,0,0,width,height);

      /* Trata os eventos */
      while(1) {
      XNextEvent(display, &an_event);
      switch(an_event.type) {
  case Expose:
              XPutImage(display,window,gc,ximage,0,0,0,0,width,height);
                break;
  /* outros eventos ... */
           }
        }

      }
    }

  return ret;
  }





int main(int argc, char **argv){
  bufferdevice * meu_bd;
  window * w1;
  palette * palette1;
  point * p1, * p2, * p3;
  object * o1;
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

  o1 = CreateObject(3);

  p1 = SetPoint(2, 200, 2);
  printf("atribui ponto1\n");
  p2 = SetPoint(400, 100, 2);
  printf("eeatribuindo ponto2\n");
  p3 = SetPoint(150, 10, 1);

  SetObject(p1, o1);
  SetObject(p2, o1);
  SetObject(p3, o1);

  //DrawLine(p1, p2, w1, meu_bd, 1);
  //DrawLine(p2, p3, w1, meu_bd, 2);

  DrawObject(o1, w1, meu_bd);
  //printf("Chamando drawLine\n");

  Dump2X(meu_bd, palette1);
  printf("Chamando dumpx\n");
  return 0;

}
