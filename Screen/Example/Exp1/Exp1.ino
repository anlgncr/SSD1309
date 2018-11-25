#include <Scene.h>
#include <Sprite.h>
#include "image.h"
#include <Buttons.h>

#define LEFT_ARROW    1
#define UP_ARROW      0
#define RIGHT_ARROW   3
#define DOWN_ARROW    2

#define LEFT_ARROW2    5
#define UP_ARROW2      4
#define RIGHT_ARROW2   7
#define DOWN_ARROW2    6

Buttons my_buttons(8);
Buttons::Button *up, *down, *left, *right;

Scene myScene(10);
Sprite my_sprite(1);
Sprite my_sprite2(1);

void buttonListener(Buttons::Button* my_button)
{ 
	if(my_buttons.buttonUp(my_button)){
		switch(my_buttons.getPin(my_button)){
		  case LEFT_ARROW2   :  myScene.setX(myScene.getX()-1); break;
		  case UP_ARROW2     :  my_sprite.addChild(&my_sprite2); break;
		  case RIGHT_ARROW2  :  myScene.setX(myScene.getX()+1); break;
		  case DOWN_ARROW2   :  my_sprite.removeChild(&my_sprite2);break;
		}
	}
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  left = my_buttons.create(LEFT_ARROW, buttonListener);
  up = my_buttons.create(UP_ARROW, buttonListener);
  right = my_buttons.create(RIGHT_ARROW, buttonListener);
  down = my_buttons.create(DOWN_ARROW, buttonListener);
  
  my_buttons.create(LEFT_ARROW2, buttonListener);
  my_buttons.create(UP_ARROW2, buttonListener);
  my_buttons.create(RIGHT_ARROW2, buttonListener);
  my_buttons.create(DOWN_ARROW2, buttonListener);
  

 myScene.setName("Sahne");
 my_sprite.setImage((uint8_t*)box);
 my_sprite2.setImage((uint8_t*)box2);
 
 myScene.addChild(&my_sprite);
  myScene.addChild(&my_sprite2);

  my_sprite2.setX(64);
}

void loop() {
  // put your main code here, to run repeatedly:
 myScene.update();
 my_buttons.update();
 
 if(my_buttons.isDown(up))
	 my_sprite.setY(my_sprite.getY()-1);
 if(my_buttons.isDown(down))
	 my_sprite.setY(my_sprite.getY()+1); 
 if(my_buttons.isDown(left))
	 my_sprite.setX(my_sprite.getX()-1);
 if(my_buttons.isDown(right))
	  my_sprite.setX(my_sprite.getX()+1);
 
}



