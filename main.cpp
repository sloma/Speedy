#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_acodec.h>
#include<allegro5/allegro_image.h>
#include<string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "mappy_A5.h"


//Set screen size
const int WIDTH = 720;
const int HEIGHT = 720;
const int squareSize = 80;//will be used as a reference for the square size, the movement and so on
bool keys[] = { false, false, false, false };
bool kickup = false;
bool kickdown = false;
bool kickleft = false;
bool kickright = false;

bool collision(int level, int character[2], double balls[4][2],int dir,int boxes[10][2]){
	bool colli = false;
	if (dir == 0){
		if ((character[1] == balls[level][1] + squareSize) && (character[0] == balls[level][0])) 
			colli = true;
		for (int i=0; i < 10; i++){
			if ((character[1] == boxes[i][1] + squareSize) && (character[0] == boxes[i][0]))
				colli = true;
		}
	}
	if (dir == 1){
		if ((character[1] == balls[level][1] - squareSize) && (character[0] == balls[level][0]))
			colli = true;
		for (int i=0; i < 10; i++){
			if ((character[1] == boxes[i][1] - squareSize) && (character[0] == boxes[i][0]))
				colli = true;
		}
	}
	if (dir == 2){
		if ((character[1] == balls[level][1]) && (character[0] == balls[level][0] + squareSize))
			colli = true;
		for (int i=0; i < 10; i++){
			if ((character[1] == boxes[i][1]) && (character[0] == boxes[i][0] + squareSize))
				colli = true;
		}
	}
	if (dir == 3){
		if ((character[1] == balls[level][1]) && (character[0] == balls[level][0] - squareSize)) 
			colli = true;
		for (int i=0; i < 10; i++){
			if ((character[1] == boxes[i][1]) && (character[0] == boxes[i][0] - squareSize))
				colli = true;
		}	
	}
	return colli;
}

bool ballCollision(int level, double balls[4][2], int ballDir, int boxes[10][2]){
	bool colli = false;
	if (ballDir == 0){
		
		for (int i = 0; i < 10; i++){
			if ((balls[level][1] == boxes[i][1] + squareSize) && (balls[level][0] == boxes[i][0]))
				colli = true;
		}
	}
	if (ballDir == 1){
		
		for (int i = 0; i < 10; i++){
			if ((balls[level][1] == boxes[i][1] - squareSize) && (balls[level][0] == boxes[i][0]))
				colli = true;
		}
	}
	if (ballDir == 2){
	
		for (int i = 0; i < 10; i++){
			if ((balls[level][1] == boxes[i][1]) && (balls[level][0] == boxes[i][0] + squareSize))
				colli = true;
		}

	}
	if (ballDir == 3){
		
		for (int i = 0; i < 10; i++){
			if ((balls[level][1] == boxes[i][1]) && (balls[level][0] == boxes[i][0] - squareSize))
				colli = true;
		}

	}
	if (colli == true){
		kickright = false;
		kickleft = false;
		kickup = false;
		kickdown = false;
	}
	return colli;
}






int main(void)
{	//init
	#pragma region
	//init the variables
	bool done = false;//the game will run as long as done is true
	bool render = false;//the screen will refresh when render is true
	int menuStatus = 1;//if the status is 0 then the game is on, 1 is the menu
	int menuY = 0;//determin where the arrow is in the menu
	enum Direction{ UP, DOWN, LEFT, RIGHT };//list of available movements TO DO ADD PLACE BOX AND KICK
	int dir = DOWN;
	int ballDir = NULL;

	//screen focus coordinate, this will determin where the screen focuses on the map
	int xOff = squareSize * 9;
	int yOff = squareSize * 9;
	// the xmax will increase as the player passes the levels
	int xmax[5] = { squareSize * 17, squareSize * 26, squareSize * 35, squareSize * 44, squareSize * 44 };
	int xmin = squareSize * 9;
	int level = 0;
	int maxbox = 4;
	int boxcount = 0;
	int ymin = 9 * squareSize;
	int ymax = 17 * squareSize;
	float time = 0.00;
	bool reset = false;



	//target coordinates
	//mark 1 : 14	9
	//mark 2 : 18	17
	//mark 3 : 32	9
	//mark 4 : 39	17
	int target[4][2] = { { 14 * squareSize, 9 * squareSize },
	{ 18 * squareSize, 17 * squareSize },
	{ 32 * squareSize, 9 * squareSize },
	{ 39 * squareSize, 17 * squareSize } };
	//balls coordinates
	double	balls[4][2] = { { 14 * squareSize, 13 * squareSize },
	{ 23 * squareSize, 13 * squareSize },
	{ 32 * squareSize, 13 * squareSize },
	{ 41 * squareSize, 13 * squareSize } };
	//boxs coordinates
	//box  1 : 15	12
	//box  2 : 18	10
	//box  3 : 23	17
	//box  4 : 28	12
	//box  5 : 32   10
	int boxes[10][2] = { { 15 * squareSize, 12 * squareSize },
	{ 18 * squareSize, 10 * squareSize },
	{ 23 * squareSize, 17 * squareSize },
	{ 28 * squareSize, 12 * squareSize },
	{ 32 * squareSize, 10 * squareSize }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

	//char coordinates
	int character[2] = { 13 * squareSize, 13 * squareSize };
	



	//allegro variable
	ALLEGRO_DISPLAY *display = NULL;


	//program init
	if (!al_init())
		al_show_native_message_box(NULL, NULL, NULL, "failed to initialize allegro!", NULL, NULL);


	display = al_create_display(WIDTH, HEIGHT);			//create our display object

	if (!display)
		al_show_native_message_box(display, NULL, NULL, "failed to create display!\n", NULL, NULL);
	al_set_window_position(display, 100, 0);
	//addon init
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_install_audio();
	al_init_acodec_addon();

	//Load the map using Mappy
	if (MapLoad("test3.fmp", 1))
		return -5;

	//init objects
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer;
	ALLEGRO_BITMAP *ball = NULL;
	ALLEGRO_BITMAP *box = NULL;
	ALLEGRO_BITMAP *characterFace = NULL;
	ALLEGRO_BITMAP *characterBack = NULL;
	ALLEGRO_BITMAP *characterRight = NULL;
	ALLEGRO_BITMAP *characterLeft = NULL;
	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 16, 0);
	ALLEGRO_FONT *font22 = al_load_font("arial.ttf", 22, 0);
	ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);

	//Load the music and sound
	ALLEGRO_SAMPLE *song = al_load_sample("gameMusic.wav");
	ALLEGRO_SAMPLE *soundEffect = al_load_sample("select.wav");

	al_reserve_samples(2);

	ALLEGRO_SAMPLE_INSTANCE *songInstance = al_create_sample_instance(song);
	al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);
	al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());
	al_play_sample_instance(songInstance);
	
	//Load bitmaps
	box = al_load_bitmap("box.bmp");
	ball = al_load_bitmap("ball.bmp");
	al_convert_mask_to_alpha(ball, al_map_rgb(255, 255, 255));

	characterFace = al_load_bitmap("characterFace.bmp");
	al_convert_mask_to_alpha(characterFace, al_map_rgb(255, 255, 255));

	characterBack = al_load_bitmap("characterBack.bmp");
	al_convert_mask_to_alpha(characterBack, al_map_rgb(255, 255, 255));

	characterLeft = al_load_bitmap("characterLeft.bmp");
	al_convert_mask_to_alpha(characterLeft, al_map_rgb(255, 255, 255));

	characterRight = al_load_bitmap("characterRight.bmp");
	al_convert_mask_to_alpha(characterRight, al_map_rgb(255, 255, 255));


	//init the event queue and timer
	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 9.0);
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_start_timer(timer);
	#pragma endregion
	//main loop
	while (!done)
	{		

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		while (level == 4){
			al_draw_textf(font22, green, WIDTH / 2-250, HEIGHT / 2-50, 0, "Congratulation! you finished the game in %.2f seconds!!",time);
			al_draw_textf(font18, green, WIDTH / 2-250, HEIGHT / 2 , 0, "press the escape key to restart the game and try to beat your previous score!");
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				//movement  
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
				{// reset all variables
			
				
					kickup = false;
					kickdown = false;
					kickleft = false;
					kickright = false;
					render = false;//the screen will refresh when render is true
					menuY = 0;//determin where the arrow is in the menu
					dir = DOWN;
					ballDir = NULL;

					//resenter the screen
					xOff = squareSize * 9;
					yOff = squareSize * 9;
					//xmax depends on the level, so reset is not necessary
					level = 0;
					boxcount = 0;
					time = 0.00;
				

					balls[0][0] = 14 * squareSize;
					balls[0][1] = 13 * squareSize;
					balls[1][0] = 23 * squareSize;
					balls[1][1] = 13 * squareSize;
					balls[2][0] = 32 * squareSize;
					balls[2][1] = 13 * squareSize;
					balls[3][0] = 41 * squareSize;
					balls[3][1] = 13 * squareSize;
				
					boxes[5][0] = 0;
					boxes[5][1] = 0;
					boxes[6][0] = 0;
					boxes[6][1] = 0;
					boxes[7][0] = 0;
					boxes[7][1] = 0;
					boxes[8][0] = 0;
					boxes[8][1] = 0;
					boxes[9][0] = 0;
					boxes[9][1] = 0;
					//char coordinates
					character[0] = 13 * squareSize;
					character[1] = 13 * squareSize;
					level = 0;
					reset = true;
					menuStatus = 1;
				};
				break;


				}

			}
		}
		//menu loop
		if (menuStatus == 1)
		{	//arrow pointer
			al_draw_triangle(250, 220 + menuY, 250, 240 + menuY, 285, 230 + menuY, green, 2.0);
			//menu option
			al_draw_textf(font18, green, 295, 220, 0, "Start/Resume");
			al_draw_textf(font18, green, 295, 300, 0, "Restart");
			al_draw_textf(font18, green, 295, 380, 0, "Exit");

			if (reset){
				al_draw_textf(font18, green, 100, 100, 0, "game reset!");
			}
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));

			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN){
				//movement  
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_DOWN:
				{if (menuY < 160)
				{
					menuY += squareSize;
					al_play_sample(soundEffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
				}};
				break;
				case ALLEGRO_KEY_UP:
				{if (menuY > 0)
				{
					menuY -= squareSize;
					al_play_sample(soundEffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
				}};
				break;
				//select
				case ALLEGRO_KEY_ENTER:
				{
					switch (menuY) {
					case 0:menuStatus = 0; break;//Start,resume
					case 80://reset
					{
						
						kickup = false;
						kickdown = false;
						kickleft = false;
						kickright = false;
						render = false;//the screen will refresh when render is true
						menuY = 0;//determin where the arrow is in the menu
						dir = DOWN;
						ballDir = NULL;

						//resenter the screen
						xOff = squareSize * 9;
						yOff = squareSize * 9;
						//xmax depends on the level, so reset is not necessary
						level = 0;
						boxcount = 0;
						time = 0.00;


						balls[0][0] = 14 * squareSize;
						balls[0][1] = 13 * squareSize;
						balls[1][0] = 23 * squareSize;
						balls[1][1] = 13 * squareSize;
						balls[2][0] = 32 * squareSize;
						balls[2][1] = 13 * squareSize;
						balls[3][0] = 41 * squareSize;
						balls[3][1] = 13 * squareSize;

						boxes[5][0] = 0;
						boxes[5][1] = 0;
						boxes[6][0] = 0;
						boxes[6][1] = 0;
						boxes[7][0] = 0;
						boxes[7][1] = 0;
						boxes[8][0] = 0;
						boxes[8][1] = 0;
						boxes[9][0] = 0;
						boxes[9][1] = 0;
						//char coordinates
						character[0] = 13 * squareSize;
						character[1] = 13 * squareSize;
						level = 0;
						reset = true;
						menuStatus = 1;
					}; break;
					case 160:done = true; break;//End game
					}
				};
				break;
				}
			
			}
		}
		//game looop
		else{
			reset = false;
			//al_wait_for_event(event_queue, &ev);
			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				done=true;
			}
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					menuStatus=1;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					dir=LEFT;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					dir = RIGHT;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					dir = UP;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					dir = DOWN;
					break;
					//kick the ball
				case ALLEGRO_KEY_Q:
				{
					if (dir == UP && character[0] == balls[level][0] && character[1] == balls[level][1] + squareSize) {
						kickup = true;
						ballDir = UP;
					}
					//if the character is under the ball, x coordinate are the same, character is one square less for y, so to be equal balls Y needs - square
					if (dir == DOWN && character[0] == balls[level][0] && character[1] == balls[level][1] - squareSize){
						kickdown = true;
					ballDir = DOWN;
					}
					if (dir == LEFT && character[0] - squareSize == balls[level][0] && character[1] == balls[level][1]){
						kickleft = true;
						ballDir = LEFT;
					}
					if (dir == RIGHT && character[0] + squareSize == balls[level][0] && character[1] == balls[level][1]){
						kickright = true;
						ballDir = RIGHT;
			}
				}
					break;
					//place a box;
				case ALLEGRO_KEY_W:
					if (boxcount < 5 && !collision(level, character, balls, dir, boxes)){
						boxcount++;
						if (dir == UP){
							boxes[4 + boxcount][0] = character[0];
							boxes[4 + boxcount][1] = character[1] - squareSize;
						}
						if (dir == DOWN){
							boxes[4 + boxcount][0] = character[0];
							boxes[4 + boxcount][1] = character[1] + squareSize;
						}
						if (dir == LEFT){
							boxes[4 + boxcount][0] = character[0] - squareSize;
							boxes[4 + boxcount][1] = character[1];
						}
						if (dir == RIGHT){
							boxes[4 + boxcount][0] = character[0] + squareSize;
							boxes[4 + boxcount][1] = character[1];
						}
						
						
					}

				}
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_UP)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					menuStatus = 1;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;

				}
			}
			//Add the movement int the timer so you can keep pressing and moving
			else if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				time += 1.00 / 9.00;

				//to make the movement of the ball smooth, we add it to the timer
				if (kickup == true && !ballCollision(level, balls, ballDir, boxes)){
					if (balls[level][1] > ymin)
						balls[level][1] -= 40;
					else kickup = false;
				}
				if (kickdown == true && !ballCollision(level, balls, ballDir, boxes)){
					if (balls[level][1] < ymax)
						balls[level][1] += 40;
					else kickdown = false;
				}

				if (kickleft == true && !ballCollision(level, balls, ballDir, boxes)){
					if (balls[level][0] > xmin)
						balls[level][0] -= 40;
					else kickleft = false;
				}

				if (kickright == true && !ballCollision(level, balls, ballDir, boxes)){
					if (balls[level][0] < xmax[level])
						balls[level][0] += 40	;
					else kickright = false;
				}

				if (keys[UP] && yOff > 5 * squareSize && !collision( level,  character, balls,dir,boxes)) {
					yOff -= squareSize;
					character[1] -= squareSize;
				}

				if (keys[DOWN] && yOff<squareSize * 13 && !collision(level, character, balls,dir,boxes)) {
					yOff += squareSize;
					character[1] += squareSize;
				}

				if (keys[LEFT] && xOff>5 * squareSize && !collision(level, character, balls,dir,boxes)) {
					xOff -= squareSize;
					character[0] -= squareSize;
				}

				if (keys[RIGHT] && xOff<xmax[level] - 4 * squareSize  && !collision(level, character, balls,dir,boxes)) {
					xOff += squareSize;
					character[0] += squareSize;
				}
				
		
				
				render = true;
			}
			


			//draw map and bitmaps
			if (render)
			{
				MapDrawBG(xOff, yOff, 0, 0, WIDTH, HEIGHT);
				//display level gates and boxes
				if (level > 0){

					al_draw_filled_rectangle(target[0][0] - xOff, target[0][1] - yOff, target[0][0] + squareSize - xOff, target[0][1] + squareSize - yOff, green);
					al_draw_filled_rectangle(xmax[0] + 65 - xOff, ymin - yOff, xmax[0] + 80 - xOff, ymax + 80 - yOff, green);
				}
				if (level > 1){

					al_draw_filled_rectangle(target[1][0] - xOff, target[1][1] - yOff, target[1][0] + squareSize - xOff, target[1][1] + squareSize - yOff, green);
					al_draw_filled_rectangle(xmax[1] + 65 - xOff, ymin - yOff, xmax[1] + 80 - xOff, ymax + 80 - yOff, green);
				}
				if (level > 2){

					al_draw_filled_rectangle(target[2][0] - xOff, target[2][1] - yOff, target[2][0] + squareSize - xOff, target[2][1] + squareSize - yOff, green);
					al_draw_filled_rectangle(xmax[2] + 65 - xOff, ymin - yOff, xmax[2] + 80 - xOff, ymax + 80 - yOff, green);
				}
				if (level >3){

					al_draw_filled_rectangle(target[3][0] - xOff, target[3][1] - yOff, target[3][0] + squareSize - xOff, target[3][1] + squareSize - yOff, green);
					al_draw_filled_rectangle(xmax[3] + 65 - xOff, ymin - yOff, xmax[3] + 80 - xOff, ymax + 80 - yOff, green);
				}
				
				if (dir==DOWN)
					al_draw_scaled_bitmap(characterFace, 0, 0, al_get_bitmap_width(characterFace), al_get_bitmap_height(characterFace), WIDTH / 2 - squareSize / 2, HEIGHT / 2 - 70, squareSize, squareSize, 0);
				if (dir == UP)
					al_draw_scaled_bitmap(characterBack, 0, 0, al_get_bitmap_width(characterBack), al_get_bitmap_height(characterBack), WIDTH / 2 - squareSize / 2, HEIGHT / 2 - 70, squareSize, squareSize, 0);
				if (dir == RIGHT)
					al_draw_scaled_bitmap(characterRight, 0, 0, al_get_bitmap_width(characterRight), al_get_bitmap_height(characterLeft), WIDTH / 2 - squareSize / 2, HEIGHT / 2 - 70, squareSize, squareSize, 0);
				if (dir == LEFT)
					al_draw_scaled_bitmap(characterLeft, 0, 0, al_get_bitmap_width(characterLeft), al_get_bitmap_height(characterRight), WIDTH / 2 - squareSize / 2, HEIGHT / 2 - 70, squareSize, squareSize, 0);
				
				//display ball
				al_draw_scaled_bitmap(ball, 0, 0, al_get_bitmap_width(ball), al_get_bitmap_width(ball), balls[level][0]+10-xOff, balls[level][1]+10-yOff, 60, 60, 0);
				//display boxes
				if(boxcount>0)
				al_draw_scaled_bitmap(box, 0, 0, al_get_bitmap_width(box), al_get_bitmap_width(box), boxes[5][0]  - xOff, boxes[5][1]  - yOff, 80, 80, 0);
				if (boxcount>1)
				al_draw_scaled_bitmap(box, 0, 0, al_get_bitmap_width(box), al_get_bitmap_width(box), boxes[6][0]  - xOff, boxes[6][1]  - yOff, 80, 80, 0);
				if (boxcount>2)
				al_draw_scaled_bitmap(box, 0, 0, al_get_bitmap_width(box), al_get_bitmap_width(box), boxes[7][0]  - xOff, boxes[7][1]  - yOff, 80, 80, 0);
				if (boxcount>3)
				al_draw_scaled_bitmap(box, 0, 0, al_get_bitmap_width(box), al_get_bitmap_width(box), boxes[8][0] - xOff, boxes[8][1]  - yOff, 80, 80, 0);
				if (boxcount>4)
				al_draw_scaled_bitmap(box, 0, 0, al_get_bitmap_width(box), al_get_bitmap_width(box), boxes[9][0] - xOff, boxes[9][1] - yOff, 80, 80, 0);
				
				al_draw_textf(font22, green, WIDTH/2+100, HEIGHT/2+240, 0, "time:%.2f  ,  level:%d",time,level+1);

			

				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));

				render = false;
			


			}
		
			if (balls[level][0] == target[level][0] && balls[level][1] == target[level][1]){
				level++;
				kickright = false;
				kickleft = false;
				kickup = false;
				kickdown = false;
			}
		}
	}

	//destroy the pointers to avoid memory elaks
	al_destroy_bitmap(ball);
	al_destroy_bitmap(box);
	al_destroy_bitmap(characterFace);
	al_destroy_bitmap(characterBack);
	al_destroy_bitmap(characterRight);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);				

	return 0;



}
