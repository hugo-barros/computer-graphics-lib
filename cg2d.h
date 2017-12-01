
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


