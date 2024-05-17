// Original library and code base - https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <bits/stdc++.h> 
using namespace std;

#include <esp_random.h>
#include <EEPROM.h>

#include "matrices.h"
#include "panels.h"
#include "chess.h"
#include "genetic.h"

vector<vector<vector<bool>>> animation{grid0, grid1, grid2, grid3, grid4, grid5, grid6, grid7, grid8};

// EEPROM

#define EEPROM_SIZE 1
uint8_t flashbyte = 0;

void eeprom_setup() {
  EEPROM.begin(EEPROM_SIZE);
  flashbyte = EEPROM.read(0);
  EEPROM.write(0, (flashbyte != 0) ? 0 : 127);
  EEPROM.commit();
}

// Two AIs trained.

ChessAI ai1(0.123330, 0.265831, 0.897112, -0.111321, -0.363924, -1.067507, 1.903415, 1000.000000, -0.050465);
ChessAI ai2(0.123330, 0.265831, 0.897112, -0.111321, -0.363924, -1.067507, 1.903415, 1000.000000, -0.050465);
// ChessAI ai2(0.109726, -0.780763, 1.211751, 0.962076, 0.007741, 0.326344, 0.019265, 1000.000000, -0.007614);

ChessGame game;

// SETUP

void setup() {
  game.reset();
  eeprom_setup();
  matrixsetup();
  disp(grid0);
}

int blinkdelay = 0;
int blinkthreshold = 0;

// ANIMATION

void anim_loop(uint16_t color) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m * chain; j++) col[i][j] = color;
  }
  delay(100);
  // Serial.println(rand() % 1024);
  disp(grid0);

  blinkdelay++;
  if (blinkdelay >= blinkthreshold) {
    blinkdelay = 0;
    blinkthreshold = (int)(runif(16, 64));
    playAnimation(animation, 20, false);
    playAnimation(animation, 20, true);
    dispstats();
    Serial.printf("BLINK THRESHOLD %ld\n", blinkthreshold);
  }
}

// LOOP

std::pair<int, int> src = {0, 0};
std::pair<int, int> vec = {0, 0};
ChessPiece movingpiece;
int status = -100;

void loop() {
  if (flashbyte == 0) {
    anim_loop(matrixpanel->color565(255, 255, 255));
    return;
  }

  cout << "MOVING..." << game.sidetomove << "\n";

  dispstats();

  // Game logic

  render();

  auto move = (game.sidetomove) ? ai1.pick(game) : ai2.pick(game);
  render();
  dispstats();
  src = move.first;
  vec = move.second;
  movingpiece = game.board[move.first.first][move.first.second];

  game.execute(move.first, move.second);
  game.sidetomove = !game.sidetomove;

  status = -100;
  
  if (game.checkmate()) status = (game.sidetomove) ? 1 : -1;
  if (game.stalemate()) status = 0;

  render();

  if (game.checkmate() || game.stalemate()) {
    game.reset();
  }

  delay(100);
}


void render() {
  matrixpanel->clearScreen();
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) matrixpanel->drawRect((i<<2), 28 - (j<<2), 5, 5, hex565(0x808080));
  }

  std::pair<int, int> des = {src.first + vec.first, src.second + vec.second};

  matrixpanel->drawPixel((src.first<<2), 28 - (src.second<<2), hex565(0xFFFFFF));
  matrixpanel->drawPixel((src.first<<2) + 4, 28 - (src.second<<2), hex565(0xFFFFFF));
  matrixpanel->drawPixel((src.first<<2), 28 - (src.second<<2) + 4, hex565(0xFFFFFF));
  matrixpanel->drawPixel((src.first<<2) + 4, 28 - (src.second<<2) + 4, hex565(0xFFFFFF));

  matrixpanel->drawRect(((des.first)<<2), 28 - (des.second<<2), 5, 5, hex565(0xFFFFFF));

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      ChessPiece piece = game.board[x][y];
      uint16_t color = piece.getColor() ? hex565(0xFF0000) : hex565(0x00FFFF);
      if (piece.isEmpty()) continue;

      if (piece.isPawn()) matrixpanel->drawPixel((x<<2) + 2, 28 - (y<<2) + 2, color);
      if (piece.isKnight()) {
        matrixpanel->drawPixel((x<<2) + 1, 28 - (y<<2) + 1, color);
        matrixpanel->drawPixel((x<<2) + 2, 28 - (y<<2) + 1, color);
        matrixpanel->drawPixel((x<<2) + 3, 28 - (y<<2) + 1, color);
        matrixpanel->drawPixel((x<<2) + 3, 28 - (y<<2) + 2, color);
        matrixpanel->drawPixel((x<<2) + 3, 28 - (y<<2) + 3, color);
      }

      if (piece.isBishop()) {
        matrixpanel->drawPixel((x<<2) + 1, 28 - (y<<2) + 1, color);
        matrixpanel->drawPixel((x<<2) + 1, 28 - (y<<2) + 3, color);
        matrixpanel->drawPixel((x<<2) + 2, 28 - (y<<2) + 2, color);
        matrixpanel->drawPixel((x<<2) + 3, 28 - (y<<2) + 1, color);
        matrixpanel->drawPixel((x<<2) + 3, 28 - (y<<2) + 3, color);
      }

      if (piece.isRook()) {
        matrixpanel->drawPixel((x<<2) + 2, 28 - (y<<2) + 1, color);
        matrixpanel->drawPixel((x<<2) + 2, 28 - (y<<2) + 3, color);
        matrixpanel->drawPixel((x<<2) + 1, 28 - (y<<2) + 2, color);
        matrixpanel->drawPixel((x<<2) + 3, 28 - (y<<2) + 2, color);
        matrixpanel->drawPixel((x<<2) + 2, 28 - (y<<2) + 2, color);
      }

      if (piece.isQueen()) {
        matrixpanel->fillRect((x<<2) + 1, 28 - (y<<2) + 1, 3, 3, color);
      }

      if (piece.isKing()) {
        matrixpanel->drawRect((x<<2) + 1, 28 - (y<<2) + 1, 3, 3, color);
      }
    }
  }
}