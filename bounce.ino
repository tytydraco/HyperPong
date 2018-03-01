#include <Arduboy2.h>
#define EEP_OFFSET 100
#define HIGHSCORE_ADDR EEPROM_STORAGE_SPACE_START + EEP_OFFSET
#define EEP_CHECK_ADDR EEPROM_STORAGE_SPACE_START + EEP_OFFSET - 1
#define EEP_CHECK_VALUE 1
#define PADDLE_THICKNESS 4
#define FPS_INCREASE 2

Arduboy2 arduboy;

int framerate = 60;

int highscore = 0;
int score = 0;

uint8_t ball_size = 2;
uint8_t bx;
uint8_t by;
bool br;
bool bu;

int player_paddle_x = 16;
int player_paddle_y = 16;
int enemy_paddle_x = WIDTH - 16;
int enemy_paddle_y = 16;
uint8_t player_paddle_size = 24;
uint8_t enemy_paddle_size = 24;

uint8_t reset_highscore_hold = 0;

void setup() {
	arduboy.begin();
	arduboy.setFrameRate(framerate);
	read_highscore();
	arduboy.initRandomSeed();
	reset_ball();
}

void loop() {
	if (!(arduboy.nextFrame())) return;
	arduboy.setFrameRate(framerate);
	arduboy.pollButtons();
	arduboy.clear();
	
	if (arduboy.everyXFrames(framerate) && arduboy.pressed(B_BUTTON)) {
		reset_highscore_hold++;
		if (reset_highscore_hold == 3) {
			EEPROM.put(HIGHSCORE_ADDR, 0);
			read_highscore();
			reset_highscore_hold = 0;
		}
		
	}
	
	draw_ball();
	draw_player_paddle();
	draw_enemy_paddle();
	
	arduboy.setTextSize(1);
	arduboy.println(score);
	arduboy.println(highscore);
	
	//if (score % 20 == 0) arduboy.invert((score % 40 == 0 ? false : true));
		
	arduboy.display();
}

void draw_player_paddle() {
	arduboy.fillRect(player_paddle_x, player_paddle_y, PADDLE_THICKNESS, player_paddle_size, WHITE);
	if (arduboy.pressed(UP_BUTTON) && player_paddle_y > 0) {
		player_paddle_y--;
	}
	
	if (arduboy.pressed(DOWN_BUTTON) && player_paddle_y + player_paddle_size < HEIGHT) {
		player_paddle_y++;
	}
}

void draw_enemy_paddle() {
	arduboy.fillRect(enemy_paddle_x, enemy_paddle_y, PADDLE_THICKNESS, enemy_paddle_size, WHITE);
	int target_y = by - enemy_paddle_size / 2;
	if (enemy_paddle_y > target_y) enemy_paddle_y--;
	if (enemy_paddle_y < target_y) enemy_paddle_y++;
}

void reset_ball() {
	bx = WIDTH / 2 + ball_size / 2;
	by = HEIGHT / 2 - ball_size / 2;
	bu = random(0,2) ? true : false;
	br = random(0,2) ? true : false;
	framerate = 60;
	write_highscore();
	read_highscore();
	score = 0;
}

void draw_ball() {
	arduboy.fillCircle(bx, by, ball_size, WHITE);
	
	// go right
	if (br) {
		bx++;
	} else {
		bx--;
	}
	
	// go up
	if (bu) {
		by--;
	} else {
		by++;
	}
	
	// reset if min x
	if (bx == ball_size) {
		reset_ball();
	}
	
	// reset if max x
	if (bx == WIDTH - ball_size) {
		reset_ball();
	}
	
	// flip vertical dir on y max / min hit
	if (by == ball_size || by == HEIGHT - ball_size) {
		bu = !bu;
	}
	
	// if hitting a paddle
	if((bx == player_paddle_x + PADDLE_THICKNESS + ball_size && player_paddle_y < by + ball_size && player_paddle_y + player_paddle_size > by) ||
		(bx + ball_size == enemy_paddle_x && enemy_paddle_y < by + ball_size && enemy_paddle_x + enemy_paddle_size > by)) {
		br = !br;
		framerate += FPS_INCREASE;
		score++;
	}
}

void read_highscore() {
	// if our special addr matches, get the highscore
	if (EEPROM.read(EEP_CHECK_ADDR) != EEP_CHECK_VALUE) {
		EEPROM.update(EEP_CHECK_ADDR, EEP_CHECK_VALUE);
		EEPROM.put(HIGHSCORE_ADDR, 0);	
	}
	EEPROM.get(HIGHSCORE_ADDR, highscore);
}

void write_highscore() {
	read_highscore();
	// if faster, or if never set
	if (score > highscore || highscore == 0) {
		EEPROM.put(HIGHSCORE_ADDR, score);
	}
}
