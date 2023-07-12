#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <emscripten.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <utility>
#include <algorithm>
#include <cmath>

struct MGolfObstacle {
  int type;
  int x;
  int y;
  int w;
  int h;
};

struct MGolfLevel {
  int sx, sy;
  int hx, hy;

  std::vector<MGolfObstacle> blocks;
};

struct Minigolf {
  std::string title = "Minigolf";

  int gameStatus = 0;

  int score = 0, hScore = INT_MAX;

  int level = 1;

  int maxlevels = 5;
  
  std::vector<std::pair<int, int>> border;
  std::vector<MGolfLevel> levelmaps;

  int ox, oy;

  double sx, sy;

  int bx, by;

  bool dragging = false;
  bool launching = false;
  long double dir = 0;
  double dirx, diry;

  std::vector<int> spThres {0, 10, 25, 100, 300, 650};
  int power, iPower, speed;

  SDL_Color linecolor = {0, 0, 0, 255};

  int bg, ball, hole, flag, rock1, rock2, rock3, rock4, bounce;
  std::vector<SDL_Texture*> sprites;
};

struct Snake {
  std::string title = "Snake";

  int gameStatus = 0;

  std::vector<std::vector<int>> grid;
  std::vector<std::vector<int>> dirGrid;

  std::vector<std::pair<int, int>> dirs {
    {-1, 0},
    {1, 0},
    {0, 1},
    {0, -1},
  };

  int ax, ay;

  int hx, hy;

  int tx, ty;
  
  int ox, oy;

  int score = 0;

  int dir = 3;
  int odir = 3;

  int length = 3;

  int startDelay = 90;
  bool startDelaying = false;

  std::vector<std::string> sizes {"Small Grid", "Medium Grid", "Large Grid"};
  std::vector<int> mapSize {8, 16, 20};
  int curSize = 1;

  int space1, space2, apple, body, headup, headdown, headright, headleft, start, big, mid, small;
  std::vector<SDL_Texture*> sprites;
};

struct TetrisSpace {
  bool filled = false;
  bool ghost = false;
  int block;
  bool cur;
};

struct Tetris {
  std::string title = "Tetris";

  int gameStatus = 0;

  std::vector<int> nextQueue;

  int holdBlock;
  bool held = false;

  int curBlock;

  std::vector<std::vector<TetrisSpace>> matrix;

  int dropDownTime = 45;
  int dropCool = 90;
  bool dropping = false;
  bool dropCooling = false;

  int hDelay = 2; bool hDelaying = false;

  int ox, oy;

  int gx;

  int cx, cy, cwr, cwl, ch, cr = 0;

  std::vector<std::vector<std::vector<std::pair<int, int>>>> blockMappings {
    {{{0, 0}, {0, -1}, {1, 0}, {1, -1}},{{0, 0}, {0, -1}, {1, 0}, {1, -1}},{{0, 0}, {0, -1}, {1, 0}, {1, -1}},{{0, 0}, {0, -1}, {1, 0}, {1, -1}}},
    {{{-1, -1}, {0, -1}, {1, -1}, {2, -1}}, {{1, 0}, {1, -1}, {1, -2}, {1, -3}}, {{-1, -2}, {0, -2}, {1, -2}, {2, -2}}, {{0, 0}, {0, -1}, {0, -2}, {0, -3}}},
    {{{0, 0}, {0, -1}, {-1, -1}, {1, -1}}, {{0, 0}, {0, -1}, {0, -2}, {1, -1}}, {{0, -1}, {-1, -1}, {1, -1}, {0, -2}}, {{0, 0}, {0, -1}, {0, -2}, {-1, -1}}},
    {{{0, 0}, {1, 0}, {0, -1}, {-1, -1}}, {{0, 0}, {0, -1}, {1, -1}, {1, -2}}, {{0, -1}, {1, -1}, {0, -2}, {-1, -2}}, {{-1, 0}, {0, -1}, {-1, -1}, {0, -2}}},
    {{{0, 0}, {-1, 0}, {0, -1}, {1, -1}}, {{1, 0}, {1, -1}, {0, -1}, {0, -2}}, {{0, -1}, {-1, -1}, {0, -2}, {1, -2}}, {{0, 0}, {0, -1}, {-1, -1}, {-1, -2}}},
    {{{1, 0}, {0, -1}, {1, -1}, {-1, -1}}, {{0, 0}, {0, -1}, {0, -2}, {1, -2}}, {{0, -1}, {1, -1}, {-1, -1}, {-1, -2}}, {{0, 0}, {-1, 0}, {0, -1}, {0, -2}}},
    {{{-1, 0}, {-1, -1}, {0, -1}, {1, -1}}, {{0, 0}, {1, 0}, {0, -1}, {0, -2}}, {{0, -1}, {1, -1}, {-1, -1}, {1, -2}}, {{0, 0}, {0, -1}, {0, -2}, {-1, -2}}}
  };

  std::vector<std::vector<std::vector<std::pair<int, int>>>> wallKickData {
    {{}, {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, {}, {{1, 0}, {1, 1}, {0, -2}, {1, -2}}},
    {{{1, 0}, {1, -1}, {0, 2}, {1, 2}}, {}, {{1, 0}, {1, -1}, {0, 2}, {1, 2}}, {}},
    {{}, {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, {}, {{1, 0}, {1, 1}, {0, -2}, {1, -2}}},
    {{{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}, {}, {{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}, {}}
  };

  std::vector<std::vector<std::vector<std::pair<int, int>>>> wallKickDataI {
    {{}, {{-2, 0}, {1, 0}, {-2, -1}, {1, 2}}, {}, {{-1, 0}, {2, 0}, {-1, 2}, {2, -1}}},
    {{{2, 0}, {-1, 0}, {2, 1}, {-1, -2}}, {}, {{-1, 0}, {2, 0}, {-1, 2}, {2, -1}}, {}},
    {{}, {{1, 0}, {-2, 0}, {1, -2}, {-2, 1}}, {}, {{2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
    {{{-2, 0}, {1, 0}, {-2, -1}, {1, 2}}, {}, {{1, 0}, {-2, 0}, {1, -2}, {-2, 1}}, {}}
  };

  int rotations = 0, movements = 0;

  int lines = 0, timer = 0;

  std::vector<char> blockNames {'O', 'I', 'T', 'S', 'Z', 'L', 'J'};

  std::vector<bool> sevenBag {0,0,0,0,0,0,0};
  int sevenBagCount = 0;

  int bar, next, hold, empty, yellow, cyan, purple, green, red, orange, blue, ghost;
  std::vector<SDL_Texture*> sprites;

  std::vector<SDL_Texture*> blockSprites;
};

struct context {
  std::string title;

  int width, height;

  SDL_Renderer *renderer;

  SDL_Point mouse;
  Uint32 mousestate;

  SDL_Event event;
  const Uint8 *keystates;
  
  SDL_Texture *screen;
  SDL_Rect screensize;
  
  SDL_Color bkg = {90,255,90,255};

  int displayID = 0;
  std::vector<std::string> displayNames {"Main", "Credits", "Tetris"};

  Tetris tetris; int tetrisHighScore; std::vector<int> recentScoresTetris;

  Snake snake; std::pair<int, int> snakeHighScore;

  Minigolf mgolf; int score = 0;

  int frameCount, timerFPS, lastFrame, fps, lastTime;
  int setFPS = 60;

  int cursor, titleimg, background, backbutton, playButton, creditsButton, tetrisStart, snakeStart, miniStart;
  std::vector<SDL_Texture*> spritesMain;

  int gitLogo;
  std::vector<SDL_Texture*> other;

  int fontSize = 24;
  std::vector<TTF_Font*> font;
  SDL_Color fontColor = {0,0,0,255};
};

int loadImg(std::string filename, std::vector<SDL_Texture*>& txt, context *ctx) {
  txt.push_back(IMG_LoadTexture(ctx->renderer, filename.c_str()));
  return txt.size()-1;
}

void drawImage(SDL_Texture* img, SDL_Rect dest, SDL_Rect src, context *ctx) {
  SDL_RenderCopy(ctx->renderer, img, &src, &dest);
}

void writeText(std::string t, int x, int y, int idx, context *ctx) {
  const char *text = t.c_str();
  SDL_Surface *txtSurf = TTF_RenderText_Solid(ctx->font[idx], text, ctx->fontColor);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(ctx->renderer, txtSurf);

  SDL_Rect wrect = {x-(txtSurf->w/2), y, txtSurf->w, txtSurf->h};

  SDL_FreeSurface(txtSurf);
  SDL_RenderCopy(ctx->renderer, texture, NULL, &wrect);
  SDL_DestroyTexture(texture);
}

void updateKeys(context *ctx) {
  ctx->keystates = SDL_GetKeyboardState(NULL);
  ctx->mousestate = SDL_GetMouseState(&ctx->mouse.x, &ctx->mouse.y);
}

void setDrawColor(SDL_Color c, context *ctx) {
  SDL_SetRenderDrawColor(ctx->renderer, c.r, c.g, c.b, c.a); 
}

void beginRender(context *ctx) {
  SDL_SetRenderTarget(ctx->renderer, ctx->screen);
  setDrawColor(ctx->bkg, ctx);
  SDL_RenderPresent(ctx->renderer); 
  ctx->frameCount++;
  ctx->timerFPS = SDL_GetTicks() - ctx->lastFrame;
  if((ctx->timerFPS) < (1000/(ctx->setFPS))) {
    SDL_Delay((1000/(ctx->setFPS))-(ctx->timerFPS));
  }
}

void end_render(context *ctx) {
  SDL_RenderClear(ctx->renderer);
  SDL_SetRenderTarget(ctx->renderer, NULL);
  SDL_RenderCopy(ctx->renderer, ctx->screen, &ctx->screensize, &ctx->screensize);
}

void initImgs(context *ctx) {
  ctx->cursor = loadImg("assets/cursor.png", ctx->spritesMain, ctx);
  ctx->titleimg = loadImg("assets/title.png", ctx->spritesMain, ctx);
  ctx->tetrisStart = loadImg("assets/tetris.png", ctx->spritesMain, ctx);
  ctx->snakeStart = loadImg("assets/snake.png", ctx->spritesMain, ctx);
  ctx->miniStart = loadImg("assets/minigolf.png", ctx->spritesMain, ctx);
  ctx->backbutton = loadImg("assets/backbutton.png", ctx->spritesMain, ctx);
  ctx->playButton = loadImg("assets/playbutton.png", ctx->spritesMain, ctx);
}

void initTexts(context *ctx) {
  TTF_Init();
  ctx->font.push_back(TTF_OpenFont("assets/BrunoAceSC-Regular.ttf", ctx->fontSize));
}

void init(context *ctx) {
  ctx->screensize.x = ctx->screensize.y = 0;
  ctx->screensize.w = ctx->width;
  ctx->screensize.h = ctx->height;
  ctx->screen = SDL_CreateTexture(ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ctx->width, ctx->height);

  initImgs(ctx);
  initTexts(ctx);
}

void quit(context *ctx) {
  SDL_DestroyRenderer(ctx->renderer);
  SDL_Quit();
}

void drawMouse(context *ctx) {
  SDL_ShowCursor(0);
  int size = 32;
  drawImage(ctx->spritesMain[ctx->cursor], {(ctx->mouse.x)-(size/2), (ctx->mouse.y)-(size/2), size, size+1}, {0, 0, size, size+1}, ctx);
}

bool rectRect(std::vector<int> r1, std::vector<int> r2) {
  if(r1[0] < r2[0]+r2[3] && r1[0]+r1[3] > r2[0] && r1[1] < r2[1]+r2[2] && r1[1] + r1[2] > r2[1])
    return true;

  return false;
}

bool pointRect(int x, int y, std::vector<int> r) {
  if(x < r[0] + r[2] && x > r[0] && y < r[1] + r[3] && y > r[1])
    return true;

  return false;
}

bool pointCirc(int x, int y, int rx, int ry, int r) {
  if(((x-rx) * (x-rx)) + ((y-ry) * (y-ry)) <= (r*r)) {
    return true;
  }
  return false;
}

std::pair<bool, bool> rectCircBorder(int rx, int ry, int r, std::vector<int> rect) {
  std::pair<bool, bool> res{0, 0};
  if(rx+r <= rect[0]+rect[2] && rx-r >= rect[0]) {
    res.first = 1;
  }
  if(ry+r <= rect[1]+rect[3] && ry-r >= rect[1]) {
    res.second = 1;
  }
  return res;
}

std::pair<bool, bool> rectCirc(int rx, int ry, int r, std::vector<int> rect) {
  std::pair<bool, bool> res{0, 0};
  int tx = rx, ty = ry;
  if(rx < rect[0]) tx = rect[0];
  else if(rx > rect[0]+rect[2]) tx = rect[0]+rect[2];

  if(ry < rect[1]) ty = rect[1];
  else if(ry > rect[1]+rect[3]) ty = rect[1]+rect[3];

  int dx = rx-tx;
  int dy = ry-ty;

  if(dx*dx + dy*dy <= r*r) {
    if(ty == ry) {
      res = {1, 0};
    }else if(tx == rx) {
      res = {0, 1};
    }else {
      res = {1, 1};
    }
  }
  return res;
}

bool circCirc(int rx, int ry, int r, int tx, int ty, int t) {
  if((rx-tx) * (rx-tx) + (ry-ty) * (ry-ty) <= (r*r) + (t*t)) {
    return true;
  }
  return false;
}

void drawBack(context *ctx) {
  drawImage(ctx->spritesMain[ctx->backbutton], {40, 40, 150, 37}, {0, 0, 150, 37}, ctx);
  writeText("Back", 145, 45, 0, ctx);
}

void drawMain(context *ctx) {
  drawImage(ctx->spritesMain[ctx->titleimg], {((ctx->width)-654)/2, 20, 654, 173}, {0, 0, 654, 173}, ctx);

  if(pointRect(ctx->mouse.x, ctx->mouse.y, {ctx->width/3, 280, 210, 210})) {
    drawImage(ctx->spritesMain[ctx->tetrisStart], {ctx->width/3, 280, 210, 210}, {0, 0, 210, 210}, ctx);
  } else {
    drawImage(ctx->spritesMain[ctx->tetrisStart], {ctx->width/3, 280, 200, 200}, {0, 0, 200, 200}, ctx);
  }

  if(pointRect(ctx->mouse.x, ctx->mouse.y, {(int) ((ctx->width/3)*(1.5)), 280, 210, 210})) {
    drawImage(ctx->spritesMain[ctx->snakeStart], {(int) ((ctx->width/3)*(1.5)), 280, 210, 210}, {0, 0, 210, 210}, ctx);
  } else {
    drawImage(ctx->spritesMain[ctx->snakeStart], {(int) ((ctx->width/3)*(1.5)), 280, 200, 200}, {0, 0, 200, 200}, ctx);
  }

  if(pointRect(ctx->mouse.x, ctx->mouse.y, {(ctx->width-200)/2, 520, 210, 210})){
    drawImage(ctx->spritesMain[ctx->miniStart], {(ctx->width-200)/2, 520, 210, 210}, {0, 0, 210, 210}, ctx);
  }else{
    drawImage(ctx->spritesMain[ctx->miniStart], {(ctx->width-200)/2, 520, 200, 200}, {0, 0, 200, 200}, ctx);
  }
  
}

void initGridMgolf(context* ctx) {
  ctx->mgolf.levelmaps = std::vector<MGolfLevel> (ctx->mgolf.maxlevels+1);
  ctx->mgolf.border = std::vector<std::pair<int, int>> (4);

  ctx->mgolf.ox = (ctx->width-1200)/2;
  ctx->mgolf.oy = 200;
  ctx->mgolf.sx = 400;
  ctx->mgolf.sy = 300;

  ctx->mgolf.bg = loadImg("assets/minigolf/background.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.ball = loadImg("assets/minigolf/ball.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.hole = loadImg("assets/minigolf/hole.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.flag = loadImg("assets/minigolf/flag.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.rock1 = loadImg("assets/minigolf/rock1.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.rock2 = loadImg("assets/minigolf/rock2.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.rock3 = loadImg("assets/minigolf/rock3.png", ctx->mgolf.sprites, ctx);
  ctx->mgolf.bounce = loadImg("assets/minigolf/bounce.png", ctx->mgolf.sprites, ctx);
  
  ctx->mgolf.border[0].first = 47+ctx->mgolf.ox;
  ctx->mgolf.border[0].second = 47+ctx->mgolf.oy;
  ctx->mgolf.border[1].first = ctx->mgolf.ox+1200-47;
  ctx->mgolf.border[1].second = 47+ctx->mgolf.oy;
  ctx->mgolf.border[2].first = ctx->mgolf.ox+1200-47;
  ctx->mgolf.border[2].second = ctx->mgolf.oy+600-47;
  ctx->mgolf.border[3].first = 47+ctx->mgolf.ox;
  ctx->mgolf.border[3].second = ctx->mgolf.oy+600-47;

  ctx->mgolf.levelmaps[1].sx = 400;
  ctx->mgolf.levelmaps[1].sy = 300;
  ctx->mgolf.levelmaps[1].hx = 1000;
  ctx->mgolf.levelmaps[1].hy = 300;
  ctx->mgolf.levelmaps[0].blocks = std::vector<MGolfObstacle>(0);
  
  ctx->mgolf.levelmaps[2].sx = 300;
  ctx->mgolf.levelmaps[2].sy = 300;
  ctx->mgolf.levelmaps[2].hx = 1000;
  ctx->mgolf.levelmaps[2].hy = 300;
  ctx->mgolf.levelmaps[2].blocks = std::vector<MGolfObstacle>(2);
  MGolfObstacle two = {
    ctx->mgolf.rock1,
    500,
    300,
    50,
    280
  };
  ctx->mgolf.levelmaps[2].blocks[0] = two;
  two.x = 750;
  ctx->mgolf.levelmaps[2].blocks[1] = two;

  ctx->mgolf.levelmaps[3].sx = 300;
  ctx->mgolf.levelmaps[3].sy = 300;
  ctx->mgolf.levelmaps[3].hx = 1000;
  ctx->mgolf.levelmaps[3].hy = 300;
  ctx->mgolf.levelmaps[3].blocks = std::vector<MGolfObstacle>(4);
  MGolfObstacle three = {
    ctx->mgolf.bounce,
    920,
    300,
    50,
    50
  };
  ctx->mgolf.levelmaps[3].blocks[0] = three;
  three.x = 450;
  three.y = 275;
  ctx->mgolf.levelmaps[3].blocks[1] = three;
  three.type = ctx->mgolf.rock2;
  three.x = 920;
  three.y = 200;
  three.w = 80;
  three.h = 100;
  ctx->mgolf.levelmaps[3].blocks[2] = three;
  three.x = 920;
  three.y = 400;
  ctx->mgolf.levelmaps[3].blocks[3] = three;

  ctx->mgolf.levelmaps[4].sx = 400;
  ctx->mgolf.levelmaps[4].sy = 300;
  ctx->mgolf.levelmaps[4].hx = 1000;
  ctx->mgolf.levelmaps[4].hy = 500;
  ctx->mgolf.levelmaps[4].blocks = std::vector<MGolfObstacle>(4);
  MGolfObstacle four = {
    ctx->mgolf.bounce,
    1050,
    500,
    50,
    50
  };
  ctx->mgolf.levelmaps[4].blocks[0] = four;
  four.x = 150;
  four.y = 500;
  ctx->mgolf.levelmaps[4].blocks[1] = four;
  four.type = ctx->mgolf.rock3;
  four.x = 1000;
  four.y = 440;
  four.w = 280;
  four.h = 50;
  ctx->mgolf.levelmaps[4].blocks[2] = four;
  four.x = 720;
  four.y = 440;
  ctx->mgolf.levelmaps[4].blocks[3] = four;

  ctx->mgolf.levelmaps[5].sx = 200;
  ctx->mgolf.levelmaps[5].sy = 300;
  ctx->mgolf.levelmaps[5].hx = 800;
  ctx->mgolf.levelmaps[5].hy = 300;
  ctx->mgolf.levelmaps[5].blocks = std::vector<MGolfObstacle>(7);
  MGolfObstacle five = {
    ctx->mgolf.bounce,
    800,
    355,
    50,
    50
  };
  ctx->mgolf.levelmaps[5].blocks[0] = five;
  five.y = 245;
  ctx->mgolf.levelmaps[5].blocks[1] = five;
  five.x = 745;
  five.y = 300;
  ctx->mgolf.levelmaps[5].blocks[2] = five;
  five.x = 125;
  ctx->mgolf.levelmaps[5].blocks[3] = five;
  five.type = ctx->mgolf.rock2;
  five.x = 400;
  five.y = 300;
  five.w = 80;
  five.h = 100;
  ctx->mgolf.levelmaps[5].blocks[4] = five;
  five.x = 550;
  five.y = 100;
  ctx->mgolf.levelmaps[5].blocks[5] = five;
  five.type = ctx->mgolf.rock1;
  five.x = 1000;
  five.y = 300;
  five.w = 50;
  five.h = 280;
  ctx->mgolf.levelmaps[5].blocks[5] = five;
  five.type = ctx->mgolf.rock3;
  five.x = 900;
  five.y = 100;
  five.w = 280;
  five.h = 50;
  ctx->mgolf.levelmaps[5].blocks[6] = five;
}

void initGridSnake(context* ctx) {
  ctx->snake.grid = std::vector<std::vector<int>> (ctx->snake.mapSize[ctx->snake.curSize], std::vector<int>(ctx->snake.mapSize[ctx->snake.curSize], 0));
  ctx->snake.dirGrid = std::vector<std::vector<int>> (ctx->snake.mapSize[ctx->snake.curSize], std::vector<int>(ctx->snake.mapSize[ctx->snake.curSize], 0));
  ctx->snake.space1 = loadImg("assets/snake/space1.png", ctx->snake.sprites, ctx);
  ctx->snake.space2 = loadImg("assets/snake/space2.png", ctx->snake.sprites, ctx);
  ctx->snake.apple = loadImg("assets/snake/apple.png", ctx->snake.sprites, ctx);
  ctx->snake.body = loadImg("assets/snake/body.png", ctx->snake.sprites, ctx);
  ctx->snake.headup = loadImg("assets/snake/headup.png", ctx->snake.sprites, ctx);
  ctx->snake.headdown = loadImg("assets/snake/headdown.png", ctx->snake.sprites, ctx);
  ctx->snake.headright = loadImg("assets/snake/headright.png", ctx->snake.sprites, ctx);
  ctx->snake.headleft = loadImg("assets/snake/headleft.png", ctx->snake.sprites, ctx);
  ctx->snake.start = loadImg("assets/snake/start.png", ctx->snake.sprites, ctx);
  ctx->snake.big = loadImg("assets/snake/big.png", ctx->snake.sprites, ctx);
  ctx->snake.mid = loadImg("assets/snake/mid.png", ctx->snake.sprites, ctx);
  ctx->snake.small = loadImg("assets/snake/small.png", ctx->snake.sprites, ctx);

  ctx->snake.ox = (ctx->width-ctx->snake.mapSize[ctx->snake.curSize] * 30)/2;
  ctx->snake.oy = 240;

  ctx->snake.hx = ctx->snake.mapSize[ctx->snake.curSize]/2;

  if(ctx->snake.curSize == 0) {
    ctx->snake.hy = 2;
    ctx->snake.grid[ctx->snake.hx][ctx->snake.hy] = 4;
    ctx->snake.grid[ctx->snake.hx][ctx->snake.hy-1] = 1;

    ctx->snake.dirGrid[ctx->snake.hx][ctx->snake.hy] = 3;
    ctx->snake.dirGrid[ctx->snake.hx][ctx->snake.hy-1] = 3;

    ctx->snake.tx = ctx->snake.hx;
    ctx->snake.ty = ctx->snake.hy-1;

    ctx->snake.ax = ctx->snake.hx;
    ctx->snake.ay = ctx->snake.hy+2;
    return;
  }

  ctx->snake.hy = 5;

  ctx->snake.grid[ctx->snake.hx][ctx->snake.hy] = 4;
  ctx->snake.grid[ctx->snake.hx][ctx->snake.hy-1] = 1;
  ctx->snake.grid[ctx->snake.hx][ctx->snake.hy-2] = 1;

  ctx->snake.dirGrid[ctx->snake.hx][ctx->snake.hy] = 3;
  ctx->snake.dirGrid[ctx->snake.hx][ctx->snake.hy-1] = 3;
  ctx->snake.dirGrid[ctx->snake.hx][ctx->snake.hy-2] = 3;

  ctx->snake.tx = ctx->snake.hx;
  ctx->snake.ty = ctx->snake.hy-2;

  ctx->snake.ax = ctx->snake.hx;
  ctx->snake.ay = ctx->snake.hy+8;
}

void initGridTetris(context* ctx) {
  ctx->tetris.matrix = std::vector<std::vector<TetrisSpace>>(28, std::vector<TetrisSpace>(10));

  ctx->tetris.ox = ((ctx->width-300)/2);
  ctx->tetris.oy = 180;

  ctx->tetris.bar = loadImg("assets/tetris/bar.png", ctx->tetris.sprites, ctx);
  ctx->tetris.next = loadImg("assets/tetris/next.png", ctx->tetris.sprites, ctx);
  ctx->tetris.hold = loadImg("assets/tetris/hold.png", ctx->tetris.sprites, ctx);
  ctx->tetris.empty = loadImg("assets/tetris/empty.png", ctx->tetris.sprites, ctx);

  ctx->tetris.yellow = loadImg("assets/tetris/cyellow.png", ctx->tetris.sprites, ctx);
  ctx->tetris.cyan = loadImg("assets/tetris/ccyan.png", ctx->tetris.sprites, ctx);
  ctx->tetris.purple = loadImg("assets/tetris/cpurple.png", ctx->tetris.sprites, ctx);
  ctx->tetris.green = loadImg("assets/tetris/cgreen.png", ctx->tetris.sprites, ctx);
  ctx->tetris.red = loadImg("assets/tetris/cred.png", ctx->tetris.sprites, ctx);
  ctx->tetris.orange = loadImg("assets/tetris/corange.png", ctx->tetris.sprites, ctx);
  ctx->tetris.blue = loadImg("assets/tetris/cblue.png", ctx->tetris.sprites, ctx);

  ctx->tetris.ghost = loadImg("assets/tetris/ghost.png", ctx->tetris.sprites, ctx);
  

  loadImg("assets/tetris/block-O.png", ctx->tetris.blockSprites, ctx);
  loadImg("assets/tetris/block-I.png", ctx->tetris.blockSprites, ctx);
  loadImg("assets/tetris/block-T.png", ctx->tetris.blockSprites, ctx);
  loadImg("assets/tetris/block-S.png", ctx->tetris.blockSprites, ctx);
  loadImg("assets/tetris/block-Z.png", ctx->tetris.blockSprites, ctx);
  loadImg("assets/tetris/block-L.png", ctx->tetris.blockSprites, ctx);
  loadImg("assets/tetris/block-J.png", ctx->tetris.blockSprites, ctx);
}

void eventMain(context *ctx) {
  SDL_Event e = ctx->event;

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_MOUSEBUTTONDOWN:
        if(pointRect(ctx->mouse.x, ctx->mouse.y, {ctx->width/3, 280, 210, 210})) {
          //Tetris
          ctx->displayID = 2;
          if(!ctx->tetris.gameStatus) initGridTetris(ctx);
        }else if(pointRect(ctx->mouse.x, ctx->mouse.y, {(int) ((ctx->width/3) * (1.5)), 280, 210, 210})) {
          //Snake
          ctx->displayID = 3;
          if(!ctx->snake.gameStatus) initGridSnake(ctx);
        }else if(pointRect(ctx->mouse.x, ctx->mouse.y, {(ctx->width-200)/2, 520, 210, 210})) {
          //Minigolf
          ctx->displayID = 4;
          if(!ctx->mgolf.gameStatus) initGridMgolf(ctx);
        }
        break;
      default:
        break;
    }
  }
  
}

void drawCredits(context* ctx) {

}

void eventCredits(context* ctx) {

}

/*MINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLFMINIGOLF*/

void newLevelMgolf(context* ctx) {
  ctx->mgolf.level++;
  if(ctx->mgolf.level == ctx->mgolf.maxlevels+1) {
    ctx->mgolf.level = 1;
    ctx->mgolf.hScore = std::min(ctx->mgolf.score, ctx->mgolf.hScore);
    ctx->mgolf.score = 0;
  }
  MGolfLevel level = ctx->mgolf.levelmaps[ctx->mgolf.level];
  ctx->mgolf.sx = level.sx;
  ctx->mgolf.sy = level.sy;
  ctx->mgolf.dragging = false;
  ctx->mgolf.launching = false;
  ctx->mgolf.power = 0;
}

void setSpeedMGolf(context* ctx) {
  std::vector<int>::iterator i = std::lower_bound(ctx->mgolf.spThres.begin(),ctx->mgolf.spThres.end(), ctx->mgolf.power);
  ctx->mgolf.speed = (i-ctx->mgolf.spThres.begin()) * (i-ctx->mgolf.spThres.begin());
}

void calcPowerMGolf(context* ctx) {
  int px, py;
  px = ctx->mouse.x;
  py = ctx->mouse.y;

  int rx = ctx->mgolf.ox + ctx->mgolf.sx, ry = ctx->mgolf.oy + ctx->mgolf.sy;

  ctx->mgolf.power = std::min(400, (int) std::sqrt((rx-px)*(rx-px) + (ry-py) * (ry-py)));
  ctx->mgolf.power *= 2;
}

void launchBall(context* ctx) {
  double sx = ctx->mgolf.sx, sy = ctx->mgolf.sy;
  sx += ctx->mgolf.dirx*ctx->mgolf.speed, sy -= ctx->mgolf.diry*ctx->mgolf.speed;
  int rx = ctx->mgolf.ox + sx, ry = ctx->mgolf.oy + sy, r = 26;
  std::vector<int> border {ctx->mgolf.border[0].first, ctx->mgolf.border[0].second, ctx->mgolf.border[2].first, ctx->mgolf.border[2].second};
  if(rx+r >= border[2]) {
    while(rx+r >= border[2]) {
      rx--;
    }
    sx = rx - ctx->mgolf.ox;
    ctx->mgolf.dirx *= -1;
  }else if(rx-r <= border[0]) {
    while(rx-r <= border[0]) {
      rx++;
    }
    sx = rx - ctx->mgolf.ox;
    ctx->mgolf.dirx *= -1;
  }
  if(ry+r >= border[3]) {
    while(ry+r >= border[3]) {
      ry--;
    }
    sy = ry-ctx->mgolf.oy;
    ctx->mgolf.diry *= -1;
  }else if(ry-r <= border[1]) {
    while(ry-r <= border[1]) {
      ry++;
    }
    sy = ry-ctx->mgolf.oy;
    ctx->mgolf.diry *= -1;
  }

  MGolfLevel level = ctx->mgolf.levelmaps[ctx->mgolf.level];
  if(circCirc(ctx->mgolf.ox+ctx->mgolf.sx, ctx->mgolf.oy+ctx->mgolf.sy, 25, level.hx+ctx->mgolf.ox, level.hy+ctx->mgolf.oy, 22)) {
    newLevelMgolf(ctx);
    return;
  }
  for(int x = 0; x < level.blocks.size(); x++) {
    int bx = level.blocks[x].x - (level.blocks[x].w/2), by = level.blocks[x].y - (level.blocks[x].h/2);
    std::pair<bool, bool> col2 = rectCirc(ctx->mgolf.ox + sx, ctx->mgolf.oy + sy, 26, {ctx->mgolf.ox + bx, ctx->mgolf.oy + by, level.blocks[x].w, level.blocks[x].h});
    if(col2.first) {
      if(sx+r >= bx && sx-r <= bx) {
        while(sx+r >= bx) {
          sx--;
        }
      }else if(sx-r <= bx+level.blocks[x].w && sx+r >= bx) {
        while(sx-r <= bx+level.blocks[x].w) {
          sx++;
        }
      }
      ctx->mgolf.dirx *= -1;
    }
    if(col2.second) {
      if(sy+r >= by && sy-r <= by) {
        while(sy+r >= by) {
          sy--;
        }
      }else if(sy-r <= by+level.blocks[x].h && sy+r >= by) {
        while(sy-r <= by+level.blocks[x].h) {
          sy++;
        }
      }
      ctx->mgolf.diry *= -1;
    }
    if(col2.first || col2.second) {
      if(level.blocks[x].type == ctx->mgolf.bounce) {
        ctx->mgolf.power = 810;
      }
      break;
    }
  }
  ctx->mgolf.sx = sx;
  ctx->mgolf.sy = sy;
  ctx->mgolf.power -= ctx->mgolf.speed;
  setSpeedMGolf(ctx);
  if(ctx->mgolf.power <= 0) {
    ctx->mgolf.launching = false;
    ctx->mgolf.power = 0;
    return;
  }
}

void drawGridMgolf(context* ctx) {
  if(ctx->mgolf.launching) {
    launchBall(ctx);
  }
  drawImage(ctx->mgolf.sprites[ctx->mgolf.bg], {(ctx->width-1200)/2, 200, 1200, 600}, {0, 0, 1200, 600}, ctx);
  writeText("Strokes: " + std::to_string(ctx->mgolf.score), ctx->width/2, 50, 0, ctx);
  if(ctx->mgolf.hScore != INT_MAX){
    writeText("High Score: " + std::to_string(ctx->mgolf.hScore), ctx->width/2, 70, 0, ctx);
  }
  writeText("POWER: " + std::to_string(ctx->mgolf.power), ctx->width/2, 850, 0, ctx);
  MGolfLevel level = ctx->mgolf.levelmaps[ctx->mgolf.level];

  int ox = (ctx->width-1200)/2, oy = 200;
  int sx = (int) ctx->mgolf.sx, sy = (int) ctx->mgolf.sy;
  int hx = ctx->mgolf.levelmaps[ctx->mgolf.level].hx, hy = ctx->mgolf.levelmaps[ctx->mgolf.level].hy;

  drawImage(ctx->mgolf.sprites[ctx->mgolf.ball], {ox+sx-25, oy+sy-25, 50, 50}, {0, 0, 50, 50}, ctx);
  drawImage(ctx->mgolf.sprites[ctx->mgolf.hole], {ox+hx-25, oy+hy-25, 50, 50}, {0, 0, 50, 50}, ctx);
  drawImage(ctx->mgolf.sprites[ctx->mgolf.flag], {ox+hx-25, oy+hy-125, 50, 125}, {0, 0, 50, 125}, ctx);

  if(level.blocks.size()) {
    std::vector<MGolfObstacle> blocks = level.blocks;

    for(int x = 0; x < blocks.size(); x++) {
      drawImage(ctx->mgolf.sprites[blocks[x].type], {ox+blocks[x].x-(blocks[x].w/2), oy+blocks[x].y-(blocks[x].h/2), blocks[x].w, blocks[x].h}, {0, 0, blocks[x].w, blocks[x].h}, ctx);
    }
  }

  if(ctx->mgolf.dragging) {
    calcPowerMGolf(ctx);
    setDrawColor(ctx->mgolf.linecolor, ctx);
    int px, py;
    px = ctx->mouse.x;
    py = ctx->mouse.y;
    
    for(int x = 0; x < 3; x++) {
      for(int y = 0; y < 3; y++) {
        SDL_RenderDrawLine(ctx->renderer, px, py, ox+sx+x, oy+sy+y);
      }
    }
  }
}

void eventMgolf(context* ctx) {
  SDL_Event e = ctx->event;

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_MOUSEBUTTONDOWN:{
        //Backbutton
        if(pointRect(ctx->mouse.x, ctx->mouse.y, {40, 40, 150, 37})) {
          ctx->displayID = 0;
        }else if(pointCirc(ctx->mouse.x, ctx->mouse.y, ctx->mgolf.ox + ctx->mgolf.sx, ctx->mgolf.oy + ctx->mgolf.sy, 50)) {
          if(!ctx->mgolf.launching){
            ctx->mgolf.dragging = true;
            ctx->mgolf.ball = loadImg("assets/minigolf/ball2.png", ctx->mgolf.sprites, ctx);
          }
        }
        
        break;
      }
      case SDL_MOUSEBUTTONUP: {
        if(ctx->mgolf.dragging) {
          ctx->mgolf.score++;
          int px, py;
          px = ctx->mouse.x;
          py = ctx->mouse.y;

          int rx = ctx->mgolf.ox + ctx->mgolf.sx, ry = ctx->mgolf.oy + ctx->mgolf.sy;

          ctx->mgolf.launching = true;
          calcPowerMGolf(ctx);
          ctx->mgolf.iPower = ctx->mgolf.power;
          setSpeedMGolf(ctx);
          double pyy, pxx;
          pyy = ry-py;
          pxx = px-rx;

          ctx->mgolf.dir = (std::atan2(pyy, pxx)) * (180/3.14159265);
          if(ctx->mgolf.dir < 180) {
            ctx->mgolf.dir += 180;
          }else{
            ctx->mgolf.dir -= 180;
          }
          ctx->mgolf.dirx = std::cos(ctx->mgolf.dir * (3.14159265/180));
          ctx->mgolf.diry = std::sin(ctx->mgolf.dir * (3.14159265/180));
          
          ctx->mgolf.ball = loadImg("assets/minigolf/ball.png", ctx->mgolf.sprites, ctx);
          ctx->mgolf.dragging = false;
        }
      }
      default:
        break;
    }
  }
}

void mgolf(context* ctx) {
  drawGridMgolf(ctx);
  drawBack(ctx);
  eventMgolf(ctx);
}

/*SNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKE*/

void gameOverSnake(context* ctx) {
  int size = ctx->snake.curSize;
  ctx->snakeHighScore.first = std::max(ctx->snakeHighScore.first, ctx->snake.score);
  ctx->snakeHighScore.second = ctx->snake.curSize;
  Snake snake;
  ctx->snake = snake;
  ctx->snake.startDelaying = true;
  ctx->snake.curSize = size;
  initGridSnake(ctx);
}

void newAppleSnake(context* ctx) {
  while(ctx->snake.grid[ctx->snake.ax][ctx->snake.ay] != 0){
    ctx->snake.ax = int(rand() % ctx->snake.mapSize[ctx->snake.curSize]);
    ctx->snake.ay = int(rand() % ctx->snake.mapSize[ctx->snake.curSize]);
  }
}

void deleteTailSnake(context* ctx) {
  int tx = ctx->snake.tx, ty = ctx->snake.ty;
  ctx->snake.grid[tx][ty] = 0;
  tx += ctx->snake.dirs[ctx->snake.dirGrid[ctx->snake.tx][ctx->snake.ty]-1].first;
  ty += ctx->snake.dirs[ctx->snake.dirGrid[ctx->snake.tx][ctx->snake.ty]-1].second;
  ctx->snake.dirGrid[ctx->snake.tx][ctx->snake.ty] = 0;
  ctx->snake.tx = tx;
  ctx->snake.ty = ty;
}

void drawCurSnake(context* ctx) {
  int hx = ctx->snake.hx, hy = ctx->snake.hy;
  int ax = ctx->snake.ax, ay = ctx->snake.ay;
  if(ctx->snake.gameStatus == 1) {
    if(ctx->frameCount == 12 || ctx->frameCount == 24 || ctx->frameCount == 36 || ctx->frameCount == 48 || ctx->frameCount == 0) {
      ctx->snake.odir = ctx->snake.dir;
      ctx->snake.grid[hx][hy] = 1;
      ctx->snake.dirGrid[hx][hy] = ctx->snake.dir;
      hx += ctx->snake.dirs[ctx->snake.dir-1].first;
      hy += ctx->snake.dirs[ctx->snake.dir-1].second;
      if(hx >= ctx->snake.mapSize[ctx->snake.curSize] || hy >= ctx->snake.mapSize[ctx->snake.curSize] || hx < 0 || hy < 0 || (ctx->snake.grid[hx][hy] == 1)) {
        gameOverSnake(ctx);
        return;
      }
      ctx->snake.dirGrid[hx][hy] = ctx->snake.dir;
      ctx->snake.grid[hx][hy] = ctx->snake.dir+1;
      if(hx == ax && hy == ay) {
        newAppleSnake(ctx);
        ctx->snake.score++;
        ctx->snake.length++;
      }else{
        deleteTailSnake(ctx);
      }
      ctx->snake.hx = hx;
      ctx->snake.hy = hy;
    }
  }
}

void drawAppleSnake(context* ctx) {
  int ax = ctx->snake.ax, ay = ctx->snake.ay;
  ctx->snake.grid[ax][ay] = 6;
}

void drawGridSnake(context* ctx) {
  writeText("Snake", ctx->width/2, 30, 0, ctx);
  writeText("Score: " + std::to_string(ctx->snake.score), ctx->width/2, 110, 0, ctx);
  writeText(("High Score: " + ((ctx->snakeHighScore.first) ? ((std::to_string(ctx->snakeHighScore.first) + " (" + ctx->snake.sizes[ctx->snakeHighScore.second]) + ")") : ("NONE"))), ctx->width/2, 180, 0, ctx);

  int ox = ctx->snake.ox, oy = ctx->snake.oy;

  int sx = ox, sy = oy;
  int s = ctx->snake.mapSize[ctx->snake.curSize];

  drawCurSnake(ctx);
  drawAppleSnake(ctx);

  for(int x = 0; x < s; x++) {
    sx = ox;
    for(int y = 0; y < s; y++) {
      if(y&1) {
        if(x&1) {
          drawImage(ctx->snake.sprites[ctx->snake.space1], {sx, sy, 30, 30}, {0, 0, 30, 30}, ctx);
        }else {
          drawImage(ctx->snake.sprites[ctx->snake.space2], {sx, sy, 30, 30}, {0, 0, 30, 30}, ctx);
        }
      } else {
        if(x&1) {
          drawImage(ctx->snake.sprites[ctx->snake.space2], {sx, sy, 30, 30}, {0, 0, 30, 30}, ctx);
        } else {
          drawImage(ctx->snake.sprites[ctx->snake.space1], {sx, sy, 30, 30}, {0, 0, 30, 30}, ctx);
        }
      }

      if(ctx->snake.grid[x][y]) {
        if(ctx->snake.grid[x][y] == 1) {
          drawImage(ctx->snake.sprites[ctx->snake.body], {sx+7, sy+7, 15, 15}, {0, 0, 15, 15}, ctx);
        }else if(ctx->snake.grid[x][y] == 2) {
          drawImage(ctx->snake.sprites[ctx->snake.headup], {sx+7, sy+7, 15, 15}, {0, 0, 15, 15}, ctx);
        }else if(ctx->snake.grid[x][y] == 3) {
          drawImage(ctx->snake.sprites[ctx->snake.headdown], {sx+7, sy+7, 15, 15}, {0, 0, 15, 15}, ctx);
        }else if(ctx->snake.grid[x][y] == 4) {
          drawImage(ctx->snake.sprites[ctx->snake.headright], {sx+7, sy+7, 15, 15}, {0, 0, 15, 15}, ctx);
        }else if(ctx->snake.grid[x][y] == 5) {
          drawImage(ctx->snake.sprites[ctx->snake.headleft], {sx+7, sy+7, 15, 15}, {0, 0, 15, 15}, ctx);
        }else if(ctx->snake.grid[x][y] == 6) {
          drawImage(ctx->snake.sprites[ctx->snake.apple], {sx+7, sy+7, 15, 15}, {0, 0, 15, 15}, ctx);
        }
      }
      sx += 30;
    }
    sy += 30;
  }

  if(!ctx->snake.gameStatus) {
    drawImage(ctx->snake.sprites[ctx->snake.start], {ox + ((s*30)-100)/2, oy+200, 100, 100}, {0, 0, 100, 100}, ctx);
    drawImage(ctx->snake.sprites[ctx->snake.big], {65, 350, 100, 40}, {0, 0, 100, 40}, ctx);
    drawImage(ctx->snake.sprites[ctx->snake.mid], {65, 400, 100, 40}, {0, 0, 100, 40}, ctx);
    drawImage(ctx->snake.sprites[ctx->snake.small], {65, 450, 100, 40}, {0, 0, 100, 40}, ctx);
  }
}

void eventSnake(context* ctx) {
  SDL_Event e = ctx->event;

  if(ctx->snake.startDelaying) {
    ctx->snake.startDelay--;
    if(!ctx->snake.startDelay) {
      ctx->snake.startDelaying = false;
      ctx->snake.startDelay = 45;
    }
    return;
  }

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_MOUSEBUTTONDOWN:
        //Backbutton
        if(pointRect(ctx->mouse.x, ctx->mouse.y, {40, 40, 150, 37})) {
          ctx->displayID = 0;
        }
        if(!ctx->snake.gameStatus){
          if(pointRect(ctx->mouse.x, ctx->mouse.y, {65, 350, 100, 40})) {
            ctx->snake.curSize = 2;
            initGridSnake(ctx);
          }else if(pointRect(ctx->mouse.x, ctx->mouse.y, {65, 400, 100, 40})) {
            ctx->snake.curSize = 1;
            initGridSnake(ctx);
          }else if(pointRect(ctx->mouse.x, ctx->mouse.y, {65, 450, 100, 40})) {
            ctx->snake.curSize = 0;
            initGridSnake(ctx);
          }
        }
        break;
      case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
          case SDLK_w:
          case SDLK_UP: {
            if(!ctx->snake.gameStatus) {
              ctx->snake.gameStatus = 1;
            }
            if(ctx->snake.odir != 2)
            ctx->snake.dir = 1;
            break;
          }
          case SDLK_s:
          case SDLK_DOWN: {
            if(!ctx->snake.gameStatus) {
              ctx->snake.gameStatus = 1;
            }
            if(ctx->snake.odir != 1)
            ctx->snake.dir = 2;
            break;
          }
          case SDLK_d:
          case SDLK_RIGHT: {
            if(!ctx->snake.gameStatus) {
              ctx->snake.gameStatus = 1;
            }
            if(ctx->snake.odir != 4)
            ctx->snake.dir = 3;
            break;
          }
          case SDLK_a:
          case SDLK_LEFT: {
            if(!ctx->snake.gameStatus) {
              ctx->snake.gameStatus = 1;
            }
            if(ctx->snake.odir != 3)
            ctx->snake.dir = 4;
            break;
          }
          default:
            break;
        }
      default:
        break;
    }
  }
}

void snake(context* ctx) {
  drawGridSnake(ctx);
  drawBack(ctx);
  eventSnake(ctx);
}

/*TETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRISTETRIS*/

void resetGridTetris(context* ctx) {
  for(int i = 0; i < 28; i++) {
    for(int j = 0; j < 10; j++) {
      ctx->tetris.matrix[i][j].filled = 0;
      ctx->tetris.matrix[i][j].cur = 0;
      ctx->tetris.matrix[i][j].ghost = 0;
    }
  }
  ctx->tetris.nextQueue = {};
  ctx->tetris.sevenBagCount = 0;
  ctx->tetris.sevenBag = {0, 0, 0, 0, 0, 0, 0};
  ctx->tetris.rotations = 0;
  ctx->tetris.movements = 0;
  ctx->tetris.timer = 0;
  ctx->tetris.lines = 0;
  ctx->tetris.holdBlock = 0;
}

void dropDownTetris(context* ctx, int h, int n) {
  for(int x = 0; x < 10; x++) {
    ctx->tetris.matrix[h+1][x].filled = false;
    ctx->tetris.matrix[h+1][x].cur = false;
    ctx->tetris.matrix[h+1][x].ghost = false;
  }
  for(int x = h; x >= 0; x--) {
    for(int y = 0; y < 10; y++) {
      if(!ctx->tetris.matrix[x][y].filled) continue;
      ctx->tetris.matrix[x][y].filled = false;
      ctx->tetris.matrix[x][y].cur = false;
      ctx->tetris.matrix[x][y].ghost = false;
      ctx->tetris.matrix[x+n][y].filled = true;
      ctx->tetris.matrix[x+n][y].cur = false;
      ctx->tetris.matrix[x+n][y].ghost = false;
      ctx->tetris.matrix[x+n][y].block = ctx->tetris.matrix[x][y].block;
    }
  }
}

bool lineCheckTetris(context* ctx, int line) {
  for(int y = 0; y < 10; y++) {
    if(!ctx->tetris.matrix[line][y].filled || ctx->tetris.matrix[line][y].ghost) {
      return false;
    }
  }
  return true;
}

void blockLandTetris(context* ctx) {
  for(int x = ctx->tetris.ch-1; x >= 0; x--) {
    if(lineCheckTetris(ctx, ctx->tetris.cx+x)) {
      ctx->tetris.lines++;
      dropDownTetris(ctx, ctx->tetris.cx+x-1, 1);
      x++;
    }
  }
}

bool pointsTakenTetris(context* ctx, int ox, int oy, std::pair<int, int> changes) {
  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];
  for(int i = 0; i < 4; i++) {
    if(ox-changes.second-dir[i].second >= 28 || oy+changes.first+dir[i].first > 9 || oy+changes.first+dir[i].first < 0 || (ctx->tetris.matrix[ox-changes.second - dir[i].second][oy + changes.first + dir[i].first].filled && !ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].ghost)) {
      return true;
    }
  }
  return false;
}

bool wallKick(context* ctx, int& prevR) {
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy;
  if(ctx->tetris.curBlock == 1) {
    for(int i = 0; i < 4; i++) {
      if(!pointsTakenTetris(ctx, ox, oy, ctx->tetris.wallKickDataI[prevR][ctx->tetris.cr][i])) {
        std::pair<int, int> newVals = ctx->tetris.wallKickDataI[prevR][ctx->tetris.cr][i];
        ctx->tetris.cx = ox-newVals.second;
        ctx->tetris.cy = oy+newVals.first;
        return true;
      }
    }
  }else{
    for(int i = 0; i < 4; i++) {
      if(!pointsTakenTetris(ctx, ox, oy, ctx->tetris.wallKickData[prevR][ctx->tetris.cr][i])) {
        std::pair<int, int> newVals = ctx->tetris.wallKickData[prevR][ctx->tetris.cr][i];
        ctx->tetris.cx = ox-newVals.second;
        ctx->tetris.cy = oy+newVals.first;
        return true;
      }
    }
  }

  return false;
}

void obstructCheck(context* ctx, int& prevR) {
  if(!ctx->tetris.curBlock) return;
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy;

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  bool valid = true;

  //Test 1 (0,0)
  for(int i = 0; i < 4; i++) {
    if(ox-dir[i].second >= 28 || oy+dir[i].first > 9 || oy+dir[i].first < 0 || (ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].filled && !ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].ghost)){
      valid = false;
      break;
    }
  }

  //Tests 2-5
  if(!valid) {
    if(!wallKick(ctx, prevR)) {
      ctx->tetris.cr = prevR;
      return;
    }
  }
}

void clearSevenBag(context* ctx) {
  for(int i = 0; i < 7; i++) {
    ctx->tetris.sevenBag[i] = 0;
  }
}

int randomBlockTetris(context* ctx) {
  if(ctx->tetris.sevenBagCount == 7) {
    ctx->tetris.sevenBagCount = 0;
    clearSevenBag(ctx);
  }
  int res = int(rand() % 7);
  while(ctx->tetris.sevenBag[res]) {
    res = int(rand() % 7);
  }
  ctx->tetris.sevenBag[res] = true;
  ctx->tetris.sevenBagCount++;
  return res;
}

bool unmarkCurTetris(context* ctx) {
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy;

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  bool valid = false;

  for(int i = 0; i < 4; i++) {
    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].cur = 0;
    if(ox-dir[i].second > 7) {
      valid = true;
    }
  }

  if(!valid) {
    resetGridTetris(ctx);
    ctx->tetris.gameStatus = 0;
  }
  return valid;
}

void deleteCurTetris(context* ctx) {
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy, gx = ctx->tetris.gx;

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  for(int i = 0; i < 4; i++) {
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].filled = 0;
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].cur = 0;
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].ghost = 0;

    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].filled = 0;
    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].ghost = 0;
    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].cur = 0;
  }
}

int ghostCollide(context* ctx) {
  int ox = ctx->tetris.cx-1, oy = ctx->tetris.cy;

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  bool valid = false;

  while(!valid){
    ox++;
    for(int i = 0; i < 4; i++) {
      if(ox >= 28-ctx->tetris.ch || (ctx->tetris.matrix[ox - dir[i].second + 1][oy + dir[i].first].filled && !ctx->tetris.matrix[ox - dir[i].second + 1][oy + dir[i].first].cur && !ctx->tetris.matrix[ox - dir[i].second + 1][oy + dir[i].first].ghost)) {
        valid = true; break;
      };
    }
  }
  return ox;
}

void getDimensionsTetris(context* ctx) {
  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  int newH = 0, newWR = 0, newWL = 0;

  for(int i = 0; i < 4; i++) {
    newH = std::max(newH, -dir[i].second);
    newWR = std::max(newWR, dir[i].first);
    newWL = std::min(newWL, dir[i].first);
  }
  ctx->tetris.ch = newH+1;
  ctx->tetris.cwr = newWR;
  ctx->tetris.cwl = -newWL;
  if(ctx->tetris.curBlock == 1 && ctx->tetris.cr == 1) {
    ctx->tetris.cwl = -1;
  }
}

void drawCurTetris(context* ctx) {
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy;
  getDimensionsTetris(ctx);

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  int gx = ghostCollide(ctx);
  ctx->tetris.gx = gx;

  for(int i = 0; i < 4; i++) {
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].filled = true;
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].cur = false;
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].ghost = true;
    ctx->tetris.matrix[gx - dir[i].second][oy + dir[i].first].block = 7;

    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].filled = true;
    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].cur = true;
    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].ghost = false;
    ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first].block = ctx->tetris.curBlock;
  }
}

void newCurBlockTetris(context* ctx) {
  if(!unmarkCurTetris(ctx)) {
    return;
  }

  ctx->tetris.held = false;

  ctx->tetris.curBlock = ctx->tetris.nextQueue[0];
  for(int i = 0; i < 3; i++) {
    ctx->tetris.nextQueue[i] = ctx->tetris.nextQueue[i+1];
  }
  ctx->tetris.nextQueue[3] = randomBlockTetris(ctx);
  ctx->tetris.cr = 0;
  ctx->tetris.cx = 6;
  ctx->tetris.cy = 4;

  drawCurTetris(ctx);
}

void drawHoldTetris(context* ctx) {
  int ox = ctx->tetris.ox, oy = ctx->tetris.oy;
  drawImage(ctx->tetris.blockSprites[ctx->tetris.holdBlock-1], {ox-160, oy+80, 120, 120}, {0, 0, 120, 120}, ctx);
}

void holdTetris(context* ctx) {
  if(ctx->tetris.held) return;
  deleteCurTetris(ctx);
  if(!ctx->tetris.holdBlock) {
    ctx->tetris.holdBlock = ctx->tetris.curBlock+1;
    ctx->tetris.cx = 10; //Make sure does not reset on unmark
    newCurBlockTetris(ctx);
  }else {
    int temp = ctx->tetris.curBlock;
    ctx->tetris.curBlock = ctx->tetris.holdBlock-1;
    ctx->tetris.holdBlock = temp+1;
  }
  ctx->tetris.held = true;
  ctx->tetris.cx = 6;
  ctx->tetris.cy = 4;
  
  drawCurTetris(ctx);
}

void drawNextTetris(context* ctx) {
  int ox = ctx->tetris.ox, oy = ctx->tetris.oy;
  ox += 350; oy += 70;
  for(int i = 0; i < 4; i++) {
    drawImage(ctx->tetris.blockSprites[ctx->tetris.nextQueue[i]], {ox, oy, 120, 120}, {0, 0, 120, 120}, ctx);
    oy += 120;
  }
}

bool rotateTetris(context* ctx, int dir) {
  if(ctx->tetris.dropping) {
    if(ctx->tetris.rotations >= 20 && ctx->tetris.cx == 28-ctx->tetris.ch) {
      ctx->tetris.dropDownTime = 0;
      return false;
    }
    ctx->tetris.dropDownTime = 45;
    ctx->tetris.rotations++;
  }
  ctx->tetris.cr += dir;
  if(ctx->tetris.cr == 4) {
    ctx->tetris.cr = 0;
  }else if(ctx->tetris.cr == -1) {
    ctx->tetris.cr = 3;
  }
  return true;
}

void generateBlocksTetris(context* ctx) {
  while(ctx->tetris.nextQueue.size() < 4) {
    ctx->tetris.nextQueue.push_back(randomBlockTetris(ctx));
  }
  if(!ctx->tetris.curBlock) {
    ctx->tetris.curBlock = randomBlockTetris(ctx);
    ctx->tetris.cx = 6;
    ctx->tetris.cy = 4;

    drawCurTetris(ctx);
  }
}

bool collideFallTetris(context* ctx) {
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy;

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  for(int i = 0; i < 4; i++) {
    if(ox-dir[i].second+1 >= 28) { continue; }
    if(ctx->tetris.matrix[ox - dir[i].second + 1][oy + dir[i].first].filled && !ctx->tetris.matrix[ox - dir[i].second + 1][oy + dir[i].first].cur && !ctx->tetris.matrix[ox - dir[i].second + 1][oy + dir[i].first].ghost) {
      return true;
    };
  }
  return false;
}

bool collideHorizontalTetris(context* ctx, int d) {
  int ox = ctx->tetris.cx, oy = ctx->tetris.cy;

  std::vector<std::pair<int, int>> dir = ctx->tetris.blockMappings[ctx->tetris.curBlock][ctx->tetris.cr];

  for(int i = 0; i < 4; i++) {
    if((d == 1 && oy+dir[i].first+1 > 9) || (d == -1 && oy+dir[i].first-1 < 0)) {
      return false;
    }
    if(ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first+d].filled && !ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first+d].cur && !ctx->tetris.matrix[ox - dir[i].second][oy + dir[i].first+d].ghost) {
      return true;
    };
  }
  return false;
}

void drawGridTetris(context* ctx) {
  writeText("40 Lines", ctx->width/2, 30, 0, ctx);

  int ox = ctx->tetris.ox, oy = ctx->tetris.oy;
  int sx = ox, sy = oy-240;
  
  if(ctx->tetris.dropDownTime <= 0) {
    blockLandTetris(ctx);
    newCurBlockTetris(ctx);
    ctx->tetris.dropping = false;
    ctx->tetris.dropDownTime = 45;
    ctx->tetris.rotations = 0;
    ctx->tetris.movements = 0;
  }

  if(ctx->tetris.dropCool <= 0) {
    ctx->tetris.dropping = false;
    ctx->tetris.dropDownTime = 45;
    ctx->tetris.rotations = 0;
    ctx->tetris.movements = 0;
    ctx->tetris.dropCooling = false;
    ctx->tetris.dropCool = 90;
  }

  drawImage(ctx->tetris.sprites[ctx->tetris.bar], {ox+300, oy, 35, 600}, {0, 0, 35, 600}, ctx);
  drawImage(ctx->tetris.sprites[ctx->tetris.next], {ox+335, oy, 150, 600}, {0, 0, 150, 600}, ctx);
  drawImage(ctx->tetris.sprites[ctx->tetris.hold], {ox-200, oy, 200, 300}, {0, 0, 200, 300}, ctx);
  writeText("NEXT", ox+410, oy+15, 0, ctx);
  writeText("HOLD", ox-100, oy+15, 0, ctx);

  writeText(std::to_string(ctx->tetris.lines) + "/40 Lines", ox-120, 500, 0, ctx);
  writeText("Completed", ox-120, 520, 0, ctx);
  writeText(std::to_string((ctx->tetris.timer/60/60)) + ":" + std::to_string(ctx->tetris.timer/60 % 60), ox-110, 560, 0, ctx);

  if(ctx->tetrisHighScore) writeText("Personal Best: " + std::to_string((ctx->tetrisHighScore/60/60)) + ":" + std::to_string(ctx->tetrisHighScore/60 % 60), ox+650, 300, 0, ctx);
  writeText("Recent Scores: ", ox+650, 400, 0, ctx);
  for(int x = 0; x < ctx->recentScoresTetris.size(); x++) {
    writeText(std::to_string(ctx->recentScoresTetris[x]/60/60) + ":" + std::to_string(ctx->recentScoresTetris[x]/60 % 60), ox+650, 400+((x+1)*40), 0, ctx);
  }

  if(ctx->tetris.gameStatus == 1) {
    bool collide = collideFallTetris(ctx);
    drawNextTetris(ctx);
    if(ctx->tetris.holdBlock) drawHoldTetris(ctx);
    if(ctx->frameCount == 1 && ctx->tetris.cx < 28-ctx->tetris.ch && !collide) {
      deleteCurTetris(ctx);
      ctx->tetris.cx++;
      drawCurTetris(ctx);
    }else if(ctx->tetris.cx == 28-ctx->tetris.ch) {
      ctx->tetris.dropping = true;
    }else if(collide) {
      ctx->tetris.dropping = true;
    }
    if(ctx->tetris.dropping && (!collide && ctx->tetris.cx < 28-ctx->tetris.ch)) {
      ctx->tetris.dropping = false;
      ctx->tetris.dropDownTime = 45;
    }
  }

  //Width of grid: 300px
  for(int x = 0; x < 28; x++) {
    sx = ox;
    for(int y = 0; y < 10; y++) {
      if(!ctx->tetris.matrix[x][y].filled) {
        if(x >= 8)
        drawImage(ctx->tetris.sprites[ctx->tetris.empty], {sx, sy, 30, 30}, {0, 0, 30, 30}, ctx);
      } else {
        drawImage(ctx->tetris.sprites[ctx->tetris.matrix[x][y].block+4], {sx, sy, 30, 30}, {0, 0, 30, 30}, ctx);
      }
      sx+=30;
    }
    sy+=30;
  }

  if(!ctx->tetris.gameStatus) {
    drawImage(ctx->spritesMain[ctx->playButton], {ox+25, oy+200, 250, 60}, {0, 0, 250, 60}, ctx);
    writeText("START", ox+150, oy+215, 0, ctx);
  }
}

void eventTetris(context* ctx) {
  updateKeys(ctx);
  SDL_Event e = ctx->event;

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_MOUSEBUTTONDOWN:
        //Backbutton
        if(pointRect(ctx->mouse.x, ctx->mouse.y, {40, 40, 150, 37})) {
          ctx->displayID = 0;
        }

        if(!ctx->tetris.gameStatus && pointRect(ctx->mouse.x, ctx->mouse.y, {ctx->tetris.ox+25, ctx->tetris.oy+200, 250, 60})) {
          ctx->tetris.gameStatus = 1;
          generateBlocksTetris(ctx);
        }
      case SDL_KEYDOWN:
        if(ctx->tetris.gameStatus != 1 || e.key.repeat != 0) break;
        switch(e.key.keysym.sym) {
          case SDLK_SPACE: {
            deleteCurTetris(ctx);
            while(ctx->tetris.cx < 28-ctx->tetris.ch && !collideFallTetris(ctx)) {
              ctx->tetris.cx++;
            }
            drawCurTetris(ctx);
            blockLandTetris(ctx);
            newCurBlockTetris(ctx);
            break;
          }
          case SDLK_UP: {
            deleteCurTetris(ctx);
            int prevR = ctx->tetris.cr;
            if(rotateTetris(ctx, 1)) obstructCheck(ctx, prevR);
            drawCurTetris(ctx);
            break;
          }
          case SDLK_z: {
            deleteCurTetris(ctx);
            int prevR = ctx->tetris.cr;
            if(rotateTetris(ctx, -1)) obstructCheck(ctx, prevR);
            drawCurTetris(ctx);
            break;
          }
          case SDLK_c: {
            holdTetris(ctx);
            break;
          }
          case SDLK_LEFT: {
            if(ctx->tetris.cy-ctx->tetris.cwl > 0 && !collideHorizontalTetris(ctx, -1)) {
              deleteCurTetris(ctx);
              ctx->tetris.cy--;
              drawCurTetris(ctx);
            }
            ctx->tetris.hDelaying = 1;
            break;
          }
          case SDLK_RIGHT: {
            if(ctx->tetris.cy < (9-ctx->tetris.cwr) && !collideHorizontalTetris(ctx, 1)){
              deleteCurTetris(ctx);
              ctx->tetris.cy++;
              drawCurTetris(ctx);
            }
            ctx->tetris.hDelaying = 1;
            break;
          }
          default:
            break;
        }
      case SDL_KEYUP:
        break;
      default:
        break;
    }
  }

  if(ctx->tetris.gameStatus == 1){
    if(ctx->frameCount % 3 == 0) {
      if(ctx->keystates[SDL_SCANCODE_DOWN]) {
        if(ctx->tetris.cx < 28-ctx->tetris.ch && !collideFallTetris(ctx)) {
          deleteCurTetris(ctx);
          ctx->tetris.cx++;
          drawCurTetris(ctx);
        }
      }
    }else if(ctx->frameCount % 4 == 0){
      if(ctx->tetris.hDelaying) {
        ctx->tetris.hDelay--;
        if(ctx->tetris.hDelay <= 0) {
          ctx->tetris.hDelaying = false;
          ctx->tetris.hDelay = 2;
        }
      }else{
        if(ctx->keystates[SDL_SCANCODE_RIGHT]) {
          if(ctx->tetris.cy < (9-ctx->tetris.cwr) && !collideHorizontalTetris(ctx, 1)){
            deleteCurTetris(ctx);
            ctx->tetris.cy++;
            drawCurTetris(ctx);
          }
        }
        if(ctx->keystates[SDL_SCANCODE_LEFT]) {
          if(ctx->tetris.cy-ctx->tetris.cwl > 0 && !collideHorizontalTetris(ctx, -1)) {
            deleteCurTetris(ctx);
            ctx->tetris.cy--;
            drawCurTetris(ctx);
          }
        }
      }
    }
  }
}

void gameFinishTetris(context* ctx) {
  if(ctx->tetrisHighScore) ctx->tetrisHighScore = std::min(ctx->tetris.timer, ctx->tetrisHighScore);
  else ctx->tetrisHighScore = ctx->tetris.timer;

  if(ctx->recentScoresTetris.size() == 3) {
    for(int x = 1; x < 3; x++) {
      ctx->recentScoresTetris[x] = ctx->recentScoresTetris[x-1];
    }
    ctx->recentScoresTetris[2] = ctx->tetris.timer;
  }else {
    ctx->recentScoresTetris.push_back(ctx->tetris.timer);
  }
  
  resetGridTetris(ctx);
  ctx->tetris.gameStatus = 0;
}

void tetris(context* ctx) {
  if(ctx->tetris.dropping) ctx->tetris.dropDownTime--;
  if(ctx->tetris.dropCooling) ctx->tetris.dropCool--;
  if(ctx->tetris.gameStatus == 1) {
    if(ctx->tetris.lines < 40) {
      ctx->tetris.timer++;
    }else {
      gameFinishTetris(ctx);
    }
  }
  drawGridTetris(ctx);
  drawBack(ctx);
  eventTetris(ctx); 
}

void loop(context *ctx) {
  ctx->lastFrame = SDL_GetTicks();
  end_render(ctx);
  if(ctx->lastFrame >= (ctx->lastTime+1000)) {
    ctx->lastTime = ctx->lastFrame;
    ctx->fps = ctx->frameCount;
    ctx->frameCount = 0;
  }

  updateKeys(ctx);

  switch(ctx->displayID) {
    case 0:
      drawMain(ctx);
      eventMain(ctx);
      break;
    case 1:
      drawCredits(ctx);
      eventCredits(ctx);
      break;
    case 2:
      tetris(ctx);
      break;
    case 3:
      snake(ctx);
      break;
    case 4:
      mgolf(ctx);
      break;
    default:
      break;
  }

  writeText("FPS: " + std::to_string(ctx->fps), ctx->width-70, 8, 0, ctx);

  drawMouse(ctx);
  beginRender(ctx);
}

void mainloop(void *arg) {
  context *ctx = static_cast<context*>(arg);
  loop(ctx);
}

int main() {
  srand(time(NULL));

  context ctx;

  SDL_InitSubSystem(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_DisplayMode DM;

  SDL_GetCurrentDisplayMode(0, &DM);
  ctx.width = DM.w; ctx.height = DM.h;
  ctx.title = "Four Minigames";

  SDL_SetWindowTitle(window, ctx.title.c_str());

  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer(ctx.width, ctx.height, 0, &window, &renderer);
  ctx.renderer = renderer;

  init(&ctx);

  emscripten_set_main_loop_arg(mainloop, &ctx, -1, 1);

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  quit(&ctx);
  return 0;
}