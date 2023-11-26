#include <stdint.h>
#include <stddef.h>
#include "video.h"
#include "control.h"
#include "sprite.h"
#include "malloc.h"
#include "main.h"

#define CONTROLLER_LEFT  0x1
#define CONTROLLER_UP    0x2
#define CONTROLLER_RIGHT 0x4
#define CONTROLLER_DOWN  0x8
#define CONTROLLER_U     0x10

// Timer constants
#define TIMER_INTERVAL 3  

uint32_t rand(void);

// Memory-mapped timer registers
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))

volatile int sprite_color = 1;  // 1 for green, 2 for red

// volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
// volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);
// volatile uint32_t *MEDIUM_PALETTE = (volatile uint32_t *)(0x500F2000);
// volatile uint32_t *MEDIUM_CONTROL = (volatile uint32_t *)(0x500F5F00);
// volatile uint8_t *MEDIUM_DATA = (volatile uint8_t *)(0x500D0000);
// volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile uint32_t *INT_ENABLE_REG = (volatile uint32_t *)(0x40000000);
volatile int global = 42;
volatile int videoInt = 0;
volatile int last_videoInt;
volatile uint32_t controller_status = 0;

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
    for (uint16_t i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint16_t j = 0; j < DISPLAY_WIDTH; j++) {
            bg[ i * DISPLAY_WIDTH + j ] = ( rand() % 200 == 0 ) ? VWHITE : VBLACK;
        }
    }
    
    int16_t bid = addBackground(bg);  //sys call implementation

    uint32_t x = 0, y = 0;
    ship.x = 0, ship.y = DISPLAY_HEIGHT * 7 / 8;
    ship.w = SHIP_W;
    ship.h = SHIP_H;
    ship.id = drawSprite(ship_s, SHIP_W, SHIP_H, ship.x, ship.y, 0);
    ship.dead = 0;

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
    pthread_mutex_unlock(&mutex);
    csr_enable_interrupts();
}


int32_t *alien_tid, *ship_tid, *bullet_tid;

int16_t gamePlay() {
    *ship_tid = thread(&shipUpdate);
    join(*ship_tid);

    // if (ship.dead) {
    //     ship.dead = 0;
    //     numShipLives--;
    //     removeSmallSprite(shipLives[numShipLives].id);
    // }
    // if (numShipLives == 0) {
    //     return 1;
    // } else if (numLivingAliens == 0) {
    //     return 2;
    // }
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
    addText("STOP", 5, 30, 18);
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
    ship_tid = t_malloc(sizeof(int32_t));

    int last_global = global;
    int last_videoInt = videoInt;
    int mode;

    // -------Starting Page--------------
    addText("Move Box", 15, 25, 15);        //sys call implementation
    addText("Press CMD to start...", 21, 22, 18);
    setCmdListener(&cmdButtonListener);
    setOnClickListener(&buttonClickedListener);
    setOnReleasedListener(&buttonReleasedListener);
    while (1) {
            wait(50);
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
    t_free(ship_tid);
}
