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

// Memory-mapped timer registers
#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))

volatile int global = 42;
volatile uint32_t controller_status = 0;
volatile uint32_t videoToggle = 0;
volatile uint32_t vidIntCtr = 0;
volatile int sprite_color = 1;  // 1 for green, 2 for red

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile uint32_t *MODE_REGISTER = (volatile uint32_t *)(0x500F6780);
volatile uint32_t *MEDIUM_PALETTE = (volatile uint32_t *)(0x500F2000);
volatile uint32_t *MEDIUM_CONTROL = (volatile uint32_t *)(0x500F5F00);
volatile uint8_t *MEDIUM_DATA = (volatile uint8_t *)(0x500D0000);

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


uint32_t mediumControlSetter(uint8_t palette, int16_t x, int16_t y, int8_t z, uint8_t index) {
    return (((uint32_t)index)<<24) | (((uint32_t)z)<<21) | (((uint32_t)y+32)<<12) | (((uint32_t)x+32)<<2) | (palette & 0x3);
}

void toggleSpriteColor(void) {
    if (sprite_color == 1) { //if green
        MEDIUM_PALETTE[1] = setSpriteColor(255, 255, 0, 0);  // Red color
        sprite_color = 2; // red
    } else { //if red
        MEDIUM_PALETTE[1] = setSpriteColor(255, 0, 255, 0);  // Green color
        sprite_color = 1; // green
    }

    // Set the next timer interrupt
    MTIMECMP_LOW = MTIME_LOW + TIMER_INTERVAL;
    if (MTIMECMP_LOW < TIMER_INTERVAL) {
        // Handle carry
        MTIMECMP_HIGH += 1;
    }
}

uint32_t setSpriteColor(uint16_t alpha, uint16_t red, uint16_t green, uint16_t blue) {
    if (alpha > 255 || red > 255 || green > 255 || blue > 255) {
        return 0;
    } else if (alpha < 0 || red < 0 || green < 0 || blue < 0) {
        return 0;
    }
    return (((uint32_t)alpha)<<24) | (((uint32_t)red)<<16) | (((uint32_t)green)<<8) | ((uint32_t)blue);
}

void setUpMediumSprites() {
    // Set up sprite data for two sprites (this assumes sprite 0 and sprite 1 are the ones we're moving)
    for (int i = 0; i < 1024; i++) {
        MEDIUM_DATA[i] = 1; // Set up the first sprite (index 0)
        MEDIUM_DATA[1024 + i] = 1; // Set up the second sprite (index 1)
    }

    // Set the color green for the sprites
    MEDIUM_PALETTE[1] = setSpriteColor(255, 0, 255, 0);

    // Initialize control registers for two sprites
    MEDIUM_CONTROL[0] = mediumControlSetter(0, 0, 0, 0, 0); // First sprite
    *MODE_REGISTER = 1; // Set graphics mode
}

void beginTheGUI() {
    setUpMediumSprites();
}

int moveBox() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 0;
    int y_pos = 0; // Y position of the sprite, now declared
    int countdown = 1;

    *VIDEO_MEMORY = 1; // VIDEO MODE ON
    beginTheGUI();

    while (1) {
        global = getTicks();
        if(global != last_global){
            controller_status = getControllerStatus();
            if(controller_status){
                // Update sprite position based on controller_status
                if(controller_status & 0x1 && x_pos - SPEED_INCREASE >= 0){
                    x_pos -= SPEED_INCREASE;
                }
                if(controller_status & 0x2 && y_pos - SPEED_INCREASE >= 0){
                    y_pos -= SPEED_INCREASE; // Updated for Y position
                }
                if(controller_status & 0x4 && y_pos + SPEED_INCREASE <= MAX_Y){
                    y_pos += SPEED_INCREASE; // Updated for Y position
                }
                if(controller_status & 0x8 && x_pos + SPEED_INCREASE <= MAX_X){
                    x_pos += SPEED_INCREASE;
                }
                // Update the sprite control registers with the new position
                MEDIUM_CONTROL[0] = mediumControlSetter(0, x_pos, y_pos, 0, 0);
            }
            last_global = global;
        }
        countdown--;
        if(!countdown){
            global++;
            controller_status = (*((volatile uint32_t *)0x40000018));
            countdown = 100000;
        }
        // Check for timer interrupt and toggle sprite color
        if (MTIME_LOW >= MTIMECMP_LOW && MTIME_HIGH >= MTIMECMP_HIGH) {
            toggleSpriteColor();
        }
    }
    return 0;
}

void initGamePlay() {
    *VIDEO_MEMORY = 1;
    beginTheGUI();
    uint8_t bg[DISPLAY_HEIGHT*DISPLAY_WIDTH];
    srand(1234);
    for (uint16_t i = 0; i < DISPLAY_HEIGHT; i++) {
        for (uint16_t j = 0; j < DISPLAY_WIDTH; j++) {
            bg[ i * DISPLAY_WIDTH + j ] = ( rand() % 200 == 0 ) ? VWHITE : VBLACK;
        }
    }
    int16_t bid = addBackground(bg);
    MEDIUM_CONTROL[0] = mediumControlSetter(0, 0, 0, 0, 0);

    moveBox();
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
