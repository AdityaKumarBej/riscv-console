#include <stdint.h>
#include <stddef.h>
#include "video.h"
#include "control.h"
#include "sprite.h"
#include "malloc.h"

#define CONTROLLER_LEFT  0x1
#define CONTROLLER_UP    0x2
#define CONTROLLER_RIGHT 0x4
#define CONTROLLER_DOWN  0x8
#define CONTROLLER_U     0x10

__attribute__((always_inline)) inline void csr_enable_interrupts(void){
  asm volatile ("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void){
  asm volatile ("csrci mstatus, 0x8");
}

typedef enum {
    game_start,
    game_play,
    game_pause,
    game_over
} gameState_t;

typedef struct sprite_t {
    uint16_t id;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    uint32_t dead;
} sprite_t;

sprite_t shipBullet;
mutex_t mutex;
gameState_t globalState = game_start;
int16_t shipVelocity = 0;
#define ALIENS_SIZE 36
sprite_t aliens[ALIENS_SIZE];
uint32_t alienCounter = 0;
int32_t alienDirection = 1;
uint32_t numLivingAliens = ALIENS_SIZE;
sprite_t ship;
#define TOT_SHIP_LIVES 3
uint32_t numShipLives = TOT_SHIP_LIVES;
sprite_t shipLives[TOT_SHIP_LIVES];
#define ALIEN_BULLETS_SIZE 5
sprite_t alienBullets[ALIEN_BULLETS_SIZE];
uint32_t alienBulletCounters[ALIEN_BULLETS_SIZE];

void initGamePlay() {
    uint8_t bg[DISPLAY_HEIGHT*DISPLAY_WIDTH];
    srand(1234);
    for (uint16_t i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint16_t j = 0; j < DISPLAY_WIDTH; j++) {
            bg[ i * DISPLAY_WIDTH + j ] = ( rand() % 200 == 0 ) ? VWHITE : VBLACK;
        }
    }
    
    int16_t bid = addBackground(bg);

    uint32_t x = 0, y = 0;
    for (uint32_t i = 0; i < ALIENS_SIZE; i++) {
        if (i % 12 == 0) {
            x = 64;
            y = i * 2 + 64;
        } else {
            x += 32;
        }
        aliens[i].id = drawSprite(alien_s, ALIEN_W, ALIEN_H, x, y, 0);
        aliens[i].x = x;
        aliens[i].y = y;
        aliens[i].w = ALIEN_W;
        aliens[i].h = ALIEN_H;
        aliens[i].dead = 0;
    }

    ship.x = 0, ship.y = DISPLAY_HEIGHT * 7 / 8;
    ship.w = SHIP_W;
    ship.h = SHIP_H;
    ship.id = drawSprite(ship_s, SHIP_W, SHIP_H, ship.x, ship.y, 0);
    ship.dead = 0;

    for (int i = 0; i < numShipLives; i++) {
        shipLives[i].w = SHIP_W;
        shipLives[i].h = SHIP_H;
        shipLives[i].x = DISPLAY_WIDTH - (i+1) * (shipLives[i].w + 10);
        shipLives[i].y = 0;
        shipLives[i].id = drawSprite(ship_s, SHIP_W, SHIP_H, shipLives[i].x, shipLives[i].y, 0);
    }

    for (int i = 0; i < ALIEN_BULLETS_SIZE; i++) {
        alienBulletCounters[i] = rand() % 150 + 50;
        alienBullets[i].x = DISPLAY_WIDTH;
        alienBullets[i].y = DISPLAY_HEIGHT;
        alienBullets[i].w = BULLET_W;
        alienBullets[i].h = BULLET_H;
        alienBullets[i].id = drawSprite(alienBullet_s, BULLET_W, BULLET_H, alienBullets[i].x, alienBullets[i].y, 1);
    }
    shipBullet.x = DISPLAY_WIDTH;
    shipBullet.y = DISPLAY_HEIGHT;
    shipBullet.w = BULLET_W;
    shipBullet.h = BULLET_H;
    shipBullet.id = drawSprite(shipBullet_s, BULLET_W, BULLET_H, shipBullet.x, shipBullet.y, 1);
}

int16_t hitDetected(sprite_t bullet, sprite_t shipOrAlien) {
    if (shipOrAlien.dead) return 0;
    uint32_t bulletCenterX = bullet.x + 1;
    return ((bulletCenterX > shipOrAlien.x) && (bulletCenterX < shipOrAlien.x + shipOrAlien.w)) &&
           (((bullet.y > shipOrAlien.y) && (bullet.y < shipOrAlien.y + shipOrAlien.h)) ||
            ((bullet.y + bullet.h > shipOrAlien.y) && (bullet.y + bullet.h < shipOrAlien.y + shipOrAlien.h)));
}

void alienUpdate() {
    while(!pthread_mutex_lock(&mutex)) {
      yield();
    }
    alienCounter++;
    if (alienCounter == 64) {
        alienCounter = 0;
        if (((alienDirection == 1) && (aliens[ALIENS_SIZE - 1].x + ALIEN_W + 10 >= DISPLAY_WIDTH)) ||
            ((alienDirection == -1) && (aliens[0].x - 10 >= DISPLAY_WIDTH))) {
            alienDirection *= -1;
            for (uint32_t i = 0; i < ALIENS_SIZE; i++) {
                aliens[i].y += 5;
                moveSprite(aliens[i].id, ALIEN_W, ALIEN_H, aliens[i].x, aliens[i].y, 0);
            }
        } else {
            for (uint32_t i = 0; i < ALIENS_SIZE; i++) {
                aliens[i].x += 5 * alienDirection;
                moveSprite(aliens[i].id, ALIEN_W, ALIEN_H, aliens[i].x, aliens[i].y, 0);
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void shipUpdate() {
    csr_disable_interrupts();
    while(!pthread_mutex_lock(&mutex)) {
      yield();
    }
    if (shipVelocity) {
        ship.x += shipVelocity;
        moveSprite(ship.id, SHIP_W, SHIP_H, ship.x, ship.y, 0);
    }
    if (shipBullet.x != DISPLAY_WIDTH) {
        shipBullet.y -= 1;
        int16_t ret = moveSprite(shipBullet.id, BULLET_W, BULLET_H, shipBullet.x, shipBullet.y, 1);
        int16_t hit = 0;
        for (int i = 0; i < ALIENS_SIZE; i++) {
            hit = hitDetected(shipBullet, aliens[i]);
            if (hit) {
                removeSmallSprite(aliens[i].id);
                aliens[i].dead = 1;
                numLivingAliens--;
                break;
            }
        }
        if (ret || hit) {
            shipBullet.x = DISPLAY_WIDTH;
            shipBullet.y = DISPLAY_HEIGHT;
            moveSprite(shipBullet.id, BULLET_W, BULLET_H, shipBullet.x, shipBullet.y, 1);
        }
    }
    pthread_mutex_unlock(&mutex);
    csr_enable_interrupts();
}

void alienBulletUpdate() {
    csr_disable_interrupts();
    while(!pthread_mutex_lock(&mutex)) {
        yield();
    }
    for (int i = 0; i < ALIEN_BULLETS_SIZE; i++) {
        if (alienBullets[i].x == DISPLAY_WIDTH) {
            alienBulletCounters[i] -= 1;
            if (alienBulletCounters[i] == 0) {
                sprite_t alien = aliens[rand() % ALIENS_SIZE];
                alienBullets[i].x = alien.x + ALIEN_W / 2;
                alienBullets[i].y = alien.y + ALIEN_H;
                moveSprite(alienBullets[i].id, BULLET_W, BULLET_W, alienBullets[i].x, alienBullets[i].y, 1);
            }
        } else {
            alienBullets[i].y += 1;
            int16_t ret = moveSprite(alienBullets[i].id, BULLET_W, BULLET_W, alienBullets[i].x, alienBullets[i].y, 1);
            int16_t hit = hitDetected(alienBullets[i], ship);
            if (hit) {
                ship.dead = 1;
            }
            if (ret || hit) {
                alienBullets[i].x = DISPLAY_WIDTH;
                alienBullets[i].y = DISPLAY_HEIGHT;
                moveSprite(alienBullets[i].id, BULLET_W, BULLET_W, alienBullets[i].x, alienBullets[i].y, 1);
                alienBulletCounters[i] = rand() % 150 + 50;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    csr_enable_interrupts();
}

int32_t *alien_tid, *ship_tid, *bullet_tid;

int16_t gamePlay() {
    *alien_tid = thread(&alienUpdate);
    *ship_tid = thread(&shipUpdate);
    *bullet_tid = thread(&alienBulletUpdate);
    join(*alien_tid);
    join(*ship_tid);
    join(*bullet_tid);


    if (ship.dead) {
        ship.dead = 0;
        numShipLives--;
        removeSmallSprite(shipLives[numShipLives].id);
    }
    if (numShipLives == 0) {
        return 1;
    } else if (numLivingAliens == 0) {
        return 2;
    }
    return 0;
}

void buttonClickedListener(uint32_t ctrl) {
    if (ctrl & 0x1) {
        shipVelocity = -1;
    }
    if (ctrl & 0x8) {
        shipVelocity = 1;
    }
    if ((ctrl & 0x10) && shipBullet.x == DISPLAY_WIDTH) {
        shipBullet.x = ship.x + ship.w / 2;
        shipBullet.y = ship.y;
        moveSprite(shipBullet.id, BULLET_W, BULLET_H, shipBullet.x, shipBullet.y, 1);
    }
}

void buttonReleasedListener(uint32_t ctrl) {
    shipVelocity = 0;
}

void cmdButtonListener() {
  switch (globalState) {
  case game_start: {
    clearScreen();
    globalState = game_play;
    initGamePlay();
    break;
  }
  case game_play: {
    globalState = game_pause;
    addText("PAUSE", 5, 30, 18);
    break;
  }
  case game_pause: {
    globalState = game_play;
    break;
  }
  }
}

int main() {
    pthread_mutex_init(&mutex);
    alien_tid = t_malloc(sizeof(int32_t));
    ship_tid = t_malloc(sizeof(int32_t));
    bullet_tid = t_malloc(sizeof(int32_t));

    addText("Space Invaders!", 15, 25, 15);
    addText("Press CMD to start...", 21, 22, 18);
    setCmdListener(&cmdButtonListener);
    setOnClickListener(&buttonClickedListener);
    setOnReleasedListener(&buttonReleasedListener);
    while (1) {
            wait(100);
            switch (globalState)
            {
            case game_play:
            {
                int16_t gameDone = gamePlay();
                if (gameDone) {
                    globalState = game_over;
                    addText((gameDone == 2) ? "YOU WIN!" : "GAME OVER", 9, 27, 18);
                }
                break;
            }
            case game_over:
            {
                break;
            }
            default:
                break;
            }
    }
    t_free(alien_tid);
    t_free(ship_tid);
    t_free(bullet_tid);
}
