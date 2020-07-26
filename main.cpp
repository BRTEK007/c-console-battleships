#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

const int MAP_SIZE = 15;

bool gameOver;

const int SHIP = 0, SHOT_SHIP = 1, DEAD_SHIP = 2, MISSED = 3;
char CHARS[] = {'O', 'X', '$','*'};
int SHIPS[] = {5, 4, 4, 3, 3, 3, 2, 2};//sizes and count

int xpos = 0;
int ypos = 0;

int x_offset = 2;
int y_offset = 1;

int my_layout[MAP_SIZE * MAP_SIZE];
int pc_layout[MAP_SIZE * MAP_SIZE];

int my_points = 0;
int pc_points = 0;

int last_dir = 0;
int last_pos = 0;
int org_ship = 0;

int turns = 0;

string current_message = "turns: ";

string player = "";

class Ship{

     vector<int> old_cords;

 public:
     vector<int> cords;
     int *layout;

     Ship(vector<int> c, int *lay){
      cords = c;
      old_cords = c;
      layout = lay;
     }

     int was_shot(int c){

     //check if cord c is in cords
     if(  std::find(cords.begin(), cords.end(), c) != cords.end()  ){

     std::remove(cords.begin(), cords.end(), c);
     cords.resize(cords.size() - 1);

     if(cords.size() == 0){
        for(int i = 0; i < old_cords.size(); i++){
            layout[old_cords[i]] = DEAD_SHIP;
        }
        return 2;
     }

     return 1;
     }

     return 0;

     }///
     void flip_cords(){

      for(int i = 0; i < cords.size(); i++){
       int a = cords[i] % MAP_SIZE;
       int b = (cords[i] - a) / MAP_SIZE;
       cords[i] = b - MAP_SIZE * a + MAP_SIZE * MAP_SIZE - MAP_SIZE;
       old_cords[i] = cords[i];
      }

     }

};

vector<Ship> my_ships;
vector<Ship> pc_ships;

void flip(int *layout) {

	int temp[MAP_SIZE * MAP_SIZE];

	copy(layout, layout + MAP_SIZE*MAP_SIZE, temp);

	for (int j = 0; j < MAP_SIZE; j++) {

		for (int i = 0; i < MAP_SIZE; i++) {
			layout[i + j * MAP_SIZE] = temp[j - MAP_SIZE * i + MAP_SIZE * MAP_SIZE - MAP_SIZE];
		}

	}
}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void createShip(int length) {

	//get empty slots

	vector <int> empty_slots;

	for (int i = MAP_SIZE; i < MAP_SIZE * MAP_SIZE; i++) {
		if (pc_layout[i] != SHIP  && pc_layout[i + 1] != SHIP && pc_layout[i - 1] != SHIP && pc_layout[i + MAP_SIZE] != SHIP && pc_layout[i - MAP_SIZE] != SHIP) {
			 empty_slots.push_back(i);
		}
	}

	//get possible slots

	vector <int> possible_slots;

	for (int i = 0; i < (empty_slots.size() + 1 - length); i++) {
		if (empty_slots[i] + length - 1 == empty_slots[i + length - 1] && empty_slots[i] % MAP_SIZE < MAP_SIZE + 1 - length) {
			possible_slots.push_back(empty_slots[i]);
		}
	}

	//spawn ship at random possible slot

	int pos = possible_slots[rand() % possible_slots.size()];
	vector<int> new_ship_cords;

	for (int i = 0; i < length; i++) {
		pc_layout[pos + i] = SHIP;
		new_ship_cords.push_back(pos + i);
	}

	Ship new_ship = Ship(new_ship_cords, pc_layout);//create new ship

    pc_ships.push_back(new_ship);

}

void setup() {
	ShowConsoleCursor(false);

	gameOver = false;

	for(int i = 0; i < MAP_SIZE * MAP_SIZE; i++){
        my_layout[i] = -1;
        pc_layout[i] = -1;
	}

	for (int i = 0; i < sizeof(SHIPS) / sizeof(int); i++) {
		createShip(SHIPS[i]);
		flip(pc_layout);//must be done n % 4 == 0 times

		for(int i = 0; i < pc_ships.size(); i++){
        pc_ships[i].flip_cords();
        pc_ships[i].flip_cords();
        pc_ships[i].flip_cords();
	    }
	}
}

void draw() {
	system("cls");

	cout<<">>>   "<<current_message<<turns<<"   <<<"<<endl;

    for(int i = 0; i <= MAP_SIZE * 2 + 2; i++){
            if(i == xpos + MAP_SIZE + 2)
                cout<<'v';
            else
                cout<<' ';
    }
    cout<<endl;

	//map top
	for(int i = 0; i <= MAP_SIZE * 2 + 2; i++)
        cout << "#";
    cout << endl;
    //layouts
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x <= MAP_SIZE * 2 + 2; x++) {

		 //map left, center, right
          if(x == 0 || x == MAP_SIZE + 1 || x == MAP_SIZE * 2 + 2)
            cout << '#';
         //left layout
          else if(x < MAP_SIZE + 1){
            int id = (x - 1) + y * MAP_SIZE;

            if(my_layout[id] >= 0)
                cout<<CHARS[my_layout[id]];
            else
                cout<<" ";
          }
          //right layout
          else if(x > MAP_SIZE + 1){
            int id = (x - MAP_SIZE - 2) + y * MAP_SIZE;

            if(pc_layout[id] > 0)
                cout<<CHARS[pc_layout[id]];
            else if(x == xpos + MAP_SIZE + 2 && y == ypos)
                cout<<'@';
            else
                cout<<" ";
          }

		}
		if(y == ypos)
            cout<<'<';
		cout << endl;
	}
    //map bottom
	for(int i = 0; i <= MAP_SIZE * 2 + 2; i++)
        cout << "#";
    //text
     cout<<endl;
     cout<<"     "<<player<<"-"<<my_points<<"         CPU-"<<pc_points;

}

void check_win(){

if( my_points == sizeof(SHIPS) / sizeof(SHIPS[0]) ){
    current_message = "You have won, bravo ! in: ";
    gameOver = true;
}

else if( pc_points == sizeof(SHIPS) / sizeof(SHIPS[0]) ){
    current_message = "You have been defeated by CPU, lmao ! in: ";
    gameOver = true;
}

}

bool shoot(int pos){

  if(pc_layout[pos] == -1 || pc_layout[pos] == SHIP){

    for(int i=0; i< 8; i++){

    switch(pc_ships[i].was_shot(pos)){
    case 0://MISSED
     pc_layout[pos] = MISSED;
     break;
    case 1://HIT
     pc_layout[pos] = SHOT_SHIP;
     return true;
    case 2://DESTROY
     my_points++;
     check_win();
     return true;
    }

    }
       return true;
  }
  else
    return false;
}

bool legit_shoot(int pos, int d){//legit if found ship

  switch(d){
   case 1:
    if(pos - MAP_SIZE < 0 )
       return false;
    if(my_layout[pos - MAP_SIZE * 2] == SHOT_SHIP)
        return false;
    if(my_layout[pos - MAP_SIZE] == MISSED)
        return false;
    break;
   case 2:
    if(pos % MAP_SIZE == MAP_SIZE - 1)
        return false;
    if(pos % MAP_SIZE == MAP_SIZE - 2 && my_layout[pos + 1] == MISSED)
       return false;
    if(pos % MAP_SIZE == MAP_SIZE - 3 && my_layout[pos + 1] == SHOT_SHIP)
       return false;
    break;
   case 3:
    if(pos + MAP_SIZE > MAP_SIZE * MAP_SIZE)
       return false;
    if(my_layout[pos + MAP_SIZE * 2] == SHOT_SHIP)
        return false;
    if(my_layout[pos + MAP_SIZE] == MISSED)
        return false;
    break;
  }

  return true;

}

bool legit_random(int pos){//legit for random

  if(my_layout[pos] > SHIP)
    return false;

  if(pos - MAP_SIZE >= 0 && my_layout[pos - MAP_SIZE] == DEAD_SHIP)//dead ship na gorze
    return false;

  if(pos % MAP_SIZE != MAP_SIZE - 1 && my_layout[pos + 1] == DEAD_SHIP)//na prawo dead ship
    return false;

  if(pos % MAP_SIZE != 0 && my_layout[pos - 1] == DEAD_SHIP)//na lewo dead ship
    return false;

  if(pos + MAP_SIZE <= MAP_SIZE * MAP_SIZE && my_layout[pos + MAP_SIZE] == DEAD_SHIP)//na dole dead ship
    return false;

  if(my_layout[pos - MAP_SIZE] >= DEAD_SHIP && my_layout[pos + 1] >= DEAD_SHIP && my_layout[pos - 1] >= DEAD_SHIP && my_layout[pos + MAP_SIZE] >= DEAD_SHIP)
    return false;

  return true;

}

int change_dir(int d){

  last_pos = org_ship;

   switch(last_dir){//MISSED
 case 0://PREV MISSED
    return d;
 case 1://UP
    return  3;//DOWN
 case 3://DOWN
    return  2;//RIGHT
 case 2://RIGHT
    return  4;
    }

}

int random_shoot(){

 vector <int> slots;

 for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++) {
            if(legit_random(i) && my_layout[i + 1] != MISSED && my_layout[i - 1] != MISSED && my_layout[i + MAP_SIZE] != MISSED && my_layout[i - MAP_SIZE] != MISSED  )
                slots.push_back(i);
 }

 if(slots.size() ==  0){

 for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++) {
            if(legit_random(i))
                slots.push_back(i);
 }

 }

 return slots[rand() % slots.size()];

}

void pc_shoot(){


    while(!legit_shoot(last_pos,last_dir)){
        last_dir = change_dir(last_dir);
    }

    int pos = 0;

    switch(last_dir){
    case 0://PREV MISSED
     pos = random_shoot();//random shot
     break;
    case 1://HIT UP
     pos = last_pos - MAP_SIZE;
     break;
    case 3://HIT DOWN
     pos = last_pos + MAP_SIZE;
     break;
    case 2://HIT RIGHT
     pos = last_pos + 1;
     break;
    case 4:
     pos = last_pos - 1;
    }

    last_pos = pos;


    for(int i=0; i< 8; i++){

    int r = my_ships[i].was_shot(pos);

    if(r == 1){//HIT
     my_layout[pos] = SHOT_SHIP;
     if(last_dir == 0){
        last_dir = 1;
        org_ship = pos;
     }
     return;
    }

    else if(r == 2){//DESTROY
      last_dir = 0;
      pc_points++;
      check_win();
      return;
     }

    }


    my_layout[pos] = MISSED;
    last_dir = change_dir(last_dir);// MISSED;
}

void logic() {

	if (GetKeyState(VK_UP) & 0x8000 && ypos > 0 /*&& pc_layout[pos - MAP_SIZE] < SHOT_SHIP*/)
	{
		ypos--;
		draw();
	}
	else if (GetKeyState(VK_DOWN) & 0x8000 && ypos < MAP_SIZE - 1)
	{
		ypos++;
		draw();
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000 && xpos < MAP_SIZE - 1)
	{
		xpos++;
		draw();
	}
	else if (GetKeyState(VK_LEFT) & 0x8000 && xpos > 0)
	{
		xpos--;
		draw();
	}
	else if (GetKeyState(VK_RETURN) & 0x8000)
    {
            int pos = ((xpos + x_offset) - MAP_SIZE - 2) + (ypos + y_offset) * MAP_SIZE;

            if(shoot(pos)){
             //Sleep(500);
             pc_shoot();
             turns ++;
             draw();
            }
            /*while(!gameOver){
            pc_shoot();
            draw();
            turns++;
            }*/
	}
}

bool place_ship(int pos,int s, int vertical){

 vector<int> new_ship_cords;

if(vertical){

 for(int i = 0; i < s; i++){
  if(my_layout[pos + i * MAP_SIZE] >= 0)
    return false;
  if(pos >= MAP_SIZE && my_layout[pos - MAP_SIZE] >= 0)//gora
    return false;
  if(pos < MAP_SIZE * MAP_SIZE - MAP_SIZE * s &&  my_layout[pos + s * MAP_SIZE] >= 0)//dol
    return false;
  if(pos % MAP_SIZE < MAP_SIZE - 1 && my_layout[pos + i * MAP_SIZE + 1] >= 0)//prawo
    return false;
  if(pos % MAP_SIZE > 0 && my_layout[pos + i * MAP_SIZE - 1] >= 0)//lewo
    return false;

 }


 for(int i = 0; i < s; i++){
    my_layout[pos + i * MAP_SIZE] = SHIP;
    new_ship_cords.push_back(pos + i * MAP_SIZE);
 }

}
else{

 for(int i = 0; i < s; i++){
  if(my_layout[pos + i] >= 0)
    return false;
  if(pos >= MAP_SIZE && my_layout[pos + i - MAP_SIZE] >= 0)//gora
    return false;
  if(pos < MAP_SIZE * MAP_SIZE - MAP_SIZE && my_layout[pos + i + MAP_SIZE] >= 0)//dol
    return false;
  if(pos + s % MAP_SIZE < MAP_SIZE - 1 && my_layout[pos + s + 1] >= 0)
    return false;
  if(pos % MAP_SIZE > 0 && my_layout[pos - 1] >= 0)
    return false;
 }

 for(int i = 0; i < s; i++){
    my_layout[pos + i] = SHIP;
    new_ship_cords.push_back(pos + i);
 }

}

 Ship new_ship = Ship(new_ship_cords, my_layout);//create new ship
 my_ships.push_back(new_ship);
 return true;
}

void pre_draw(int ship_count, int pos, int ship_size, bool vertical){

system("cls");

	cout<<">>>   "<<"Place your ships "<<player<<" remains: "<<ship_count<<"   <<<"<<endl;
	cout<<">>>   "<<"Use <>^v to move, r/t to rotate, enter to confirm and space to reset. "<<"   <<<"<<endl;

	//map top
	for(int i = 0; i <= MAP_SIZE + 1; i++)
        cout << "#";
    cout << endl;
    //layouts
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x <= MAP_SIZE + 1; x++) {

		 //map left, center, right
          if(x == 0 || x == MAP_SIZE + 1 || x == MAP_SIZE * 2 + 2)
            cout << '#';
         //left layout
          else{
            int id = (x - 1) + y * MAP_SIZE;
            int xx = pos % MAP_SIZE;
            int yy = pos - pos % MAP_SIZE;

            if(my_layout[id] == SHIP)
                cout<<CHARS[SHIP];
            else if(!vertical && id >= pos && id < pos + ship_size){
                cout<<'@';
            }
            else if (vertical && x == xx + 1 && id >= pos && id < pos + ship_size * MAP_SIZE){
                cout<<'@';
            }
            else
                cout<<" ";
          }

		}
		cout << endl;
	}
    //map bottom
	for(int i = 0; i <= MAP_SIZE + 1; i++)
        cout << "#";

}

void set_layout(){

 int ship_id = 0;
 int pos = 0;
 int total = sizeof(SHIPS) / sizeof(SHIPS[0]);
 bool vertical = false;
 int x_add = SHIPS[ship_id];//ship = not vertical
 int y_add = 0;//ship * MAP_SIZE = vertical

 pre_draw(total - ship_id, pos, SHIPS[ship_id], vertical);

 while( ship_id < total ){

  int ship = SHIPS[ship_id];

  if (GetKeyState(VK_UP) & 0x8000 && pos >= MAP_SIZE)
	{
		pos -= MAP_SIZE;
		pre_draw(total - ship_id, pos, ship, vertical);
	}
	else if (GetKeyState(VK_DOWN) & 0x8000 && pos + y_add * MAP_SIZE < MAP_SIZE * MAP_SIZE - MAP_SIZE)
	{
		pos += MAP_SIZE;
		pre_draw(total - ship_id, pos, ship, vertical);
	}
	else if (GetKeyState(VK_RIGHT) & 0x8000 && pos % MAP_SIZE < MAP_SIZE - x_add)
	{
		pos++;
		pre_draw(total - ship_id, pos, ship, vertical);
	}
	else if (GetKeyState(VK_LEFT) & 0x8000 && pos % MAP_SIZE > 0)
	{
		pos--;
		pre_draw(total - ship_id, pos, ship, vertical);
	}
	else if (GetKeyState('R') & 0x8000)
	{
		if(vertical && pos % MAP_SIZE < MAP_SIZE - ship){
          vertical = !vertical;
          x_add = ship;
          y_add = 0;
          pre_draw(total - ship_id, pos, ship, vertical);
		}
	}

	else if (GetKeyState('T') & 0x8000)
	{
		if(!vertical && pos + (ship - 1)*MAP_SIZE <= MAP_SIZE * MAP_SIZE - 1){
          vertical = !vertical;
          x_add = 1;
          y_add = ship - 1;
          pre_draw(total - ship_id, pos, ship, vertical);
		}
	}

	else if (GetKeyState(VK_RETURN) & 0x8000)
    {
      if( place_ship(pos, ship, vertical) )
             ship_id ++;

             if(vertical){
                x_add = 1;
                y_add = ship - 1;
             }else{
                x_add = ship;
                y_add = 0;
             }

      pre_draw(total - ship_id, pos, ship, vertical);

	}

	else if (GetKeyState(VK_SPACE) & 0x8000)
    {
             ship_id = 0;
             for(int i = 0; i < MAP_SIZE * MAP_SIZE; i++){
                my_layout[i] = -1;
             }
             pre_draw(total - ship_id, pos, ship, vertical);
	}

 }

}

int main()
{
    cout<<"Your name sir ? ";
    cin>>player;
    Sleep(250);

	srand(time(NULL));
	setup();
    set_layout();
	draw();
	Sleep(250);

	while (!gameOver) {
		logic();
		Sleep(30);
	}
	draw();
	while (gameOver) {

	}

    return 0;
}


