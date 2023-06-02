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

struct Snake {
  std::string title = "Snake";

  int gameStatus = 0;

  std::vector<std::vector<int>> grid;

  int ax, ay;

  int hx, hy;

  int score = 0;

  std::vector<std::string> sizes {"Small Grid", "Medium Grid", "Large Grid"};
  int curSize = 1;

  int space1, space2, apple, body, head;
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

  Snake snake; std::pair<int, int> snakeHighScore; std::vector<std::pair<int, int>> snakeRecentScores;

  int frameCount, timerFPS, lastFrame, fps, lastTime;
  int setFPS = 60;

  int cursor, titleimg, background, backbutton, playButton, creditsButton, tetrisStart;
  std::vector<SDL_Texture*> spritesMain;

  std::vector<SDL_Texture*> spritesGolf;

  std::vector<SDL_Texture*> spritesSnake;

  std::vector<SDL_Texture*> spritesAim;

  std::vector<SDL_Texture*> spritesUp;

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
}

void initGridSnake(context* ctx) {

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
/*SNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKESNAKE*/

void snake(context* ctx) {

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

void drawGrid(context* ctx) {
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
  drawGrid(ctx);
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
    default:
      break;
  }

  writeText("FPS: " + std::to_string(ctx->fps), ctx->width-70, 8, 0, ctx);
  writeText("DISPLAYID: " + std::to_string(ctx->displayID), ctx->width-120, 60, 0, ctx);

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