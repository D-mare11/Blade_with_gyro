#define COLOR_NUM 12

typedef struct {
  unsigned short int red[COLOR_NUM] ={
    255,//オレンジ
    0,//水色
    255,//ピンク
    0,//黄緑
    0,//青
    255,//黄色
    255,//赤
    207,//steelblue
    255,//ショッキングピンク
    0,//うすい緑
    138,//紫
    255//白
  };

  unsigned short int green[COLOR_NUM] ={
    69,//オレンジ
    191,//水色
    105,//ピンク
    255,//黄緑
    0,//青
    255,//黄色
    0,//赤
    130,//steelblue
    0,//ショッキングピンク
    255,//うすい緑
    43,//紫
    255//白
  };

  unsigned short int blue[COLOR_NUM] ={
    0,//オレンジ
    255,//水色
    180,//ピンク
    127,//黄緑
    255,//青
    0,//黄色
    0,//赤
    180,//steelblue
    255,//ショッキングピンク
    255,//うすい緑
    226,//紫
    255//白
  };
}COLORS;

typedef struct
{
    double h, s, v;
}HSV;

typedef struct
{
    unsigned short int r, g, b;
}RGB;