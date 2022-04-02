#include <kipr/wombat.h>

void turnleft(int callibration);
void turnright(int callibration);
void moveuntilline();
void forwarduntilbump(int speed);
void move(int speed, int time);
void slowturnleft();
void slowturnright();
void pause();
void waitforlight();
void move_until_black(int threshold);
int calc_threshold();
void slow_servo(int port, int maxmin);
int absol(int n);
void alldown();
void slightturnleft(int speed, int time);
void slightturnright(int speed, int time);
void retract(int callibration);
void extend(int callibration);

int main()
{
    create_connect();
    enable_servos();
   	// waitforlight(); // wait for light
    alldown();
 	// move to botguy head to start
    move(200,200);
    turnleft(0);
    move(300,600);
    turnright(100);
	// move to line
    move_until_black(calc_threshold());
    move(-200,300);
    extend(200);
    msleep(500); // pause after retraction
    retract(200);
    move_until_black(calc_threshold());
 	move(400,1500);
    turnright(300);
   	move(600,1500);
    move_until_black(calc_threshold());
    move(400,400); // if facing left
    turnleft(150);
    move(-200,200);
    //===========
    slow_servo(1, 2000); // open
   	slow_servo(2, 800); // move middle up
    slow_servo(0, 800); // move bottom forward
    //===========
    forwarduntilbump(50);
    move(-100,300);
    slow_servo(1,1000);
   	move(-100,1300); // grabbed already
    
    turnright(0);
    move(250,1200);
    turnright(200);
    move_until_black(calc_threshold());
    move(250,1000);
    forwarduntilbump(100);
    move(-200,500);
    
    turnleft(60);
    move(200,1300);
    slightturnright(100,200);
    slow_servo(0, 100);
    slow_servo(2, 500);
    slightturnright(100,100);
    slow_servo(1, 2000);
    msleep(1000);
    alldown();
   	printf("bruh");
    turnleft(250);
    
    move_until_black(calc_threshold());
    move(400, 1000);
    
    turnleft(200);
    create_drive_direct(0,0);
    move(500, 1000);
    
    turnright(60);
    forwarduntilbump(100);
    move(-100,300);
    turnleft(80);
    
    extend(-300);
    move(-300,1500);
    extend(200);
    
    ao();
    return 0;
}

void turnleft(int callibration)
{
    create_drive_direct(-75,75);
    msleep(1910+callibration);
    pause();
}
void turnright(int callibration)
{
    create_drive_direct(75,-75);
    msleep(1910+callibration);
    pause();
}
void forwarduntilbump(int speed) {
    while (1) {
        if (get_create_lbump() == 1 || get_create_rbump() == 1) {
            break;
        }
        else {
            create_drive_direct(speed, speed);
        }     
    }
    pause();
}
void move(int speed, int time) {
    create_drive_direct(speed,speed);
    msleep(time);
    pause();
}
void slowturnleft() {
    create_drive_direct(-25,25);
    msleep(50);
}
void slowturnright() {
    pause();
    create_drive_direct(25,-25);
    msleep(50);
}
void pause() {
    create_drive_direct(0,0);
    msleep(400);
}
void move_until_black(int threshold) {
    pause();
    create_drive_direct(100,100);
    while(1) {
        int lamt = get_create_lfcliff_amt();
        int ramt = get_create_rfcliff_amt();

        if (lamt < threshold || ramt < threshold) {
            int delta = lamt - ramt;
            printf("black %d %d %d\n", lamt, ramt, delta);

            create_drive_direct(0,0);
            if (delta > 0) {
                // leaning left
                create_drive_direct(50,0);
                msleep(delta/3);
                printf("corrected right\n");
            } else {
                // leaning right
                create_drive_direct(0,50);
                msleep(-delta/3);
                printf("corrected left\n");
            }
            create_drive_direct(0,0);
            break;
        }
        msleep(50);
    }
    pause();
}
int calc_threshold() {
    int lamt0 = get_create_lfcliff_amt();
    int ramt0 = get_create_rfcliff_amt();
    int lamt1 = get_create_lfcliff_amt();
    int ramt1 = get_create_rfcliff_amt();
    int lamt2 = get_create_lfcliff_amt();
    int ramt2 = get_create_rfcliff_amt();
    int avg = (lamt0 + lamt1 + lamt2 + ramt0 + ramt1 + ramt2)/6;   
	int threshold = avg * 85 / 100;
    return threshold;
}
void waitforlight() {
	while (analog(0) > 3500) {
        printf("%d\n", analog(0));
		msleep(500);
    }
}
int absol(int num) {
 	if (num < 0) {
        return num*-1;
    }
    return num;
} 
void slow_servo(int port, int maxmin) {
    int initial = get_servo_position(port);
    int difference = absol(get_servo_position(port)-maxmin);
    if (difference == 0) {
        return;
    }
    int absolute = difference/(get_servo_position(port)-maxmin)*-1;
    int x = 0;
    int interval = 5;
    for (x = 0; x <= difference; x += 2){
        set_servo_position(port, initial+(x*absolute));
        msleep(interval);
    }
}
void alldown() {
   	slow_servo(0, 1725); // move bottom back
    msleep(1000);
    slow_servo(2, 0); // move middle down
    msleep(1000);
    slow_servo(1, 1000); // claw
}
void slightturnleft(int speed, int time) {
    create_drive_direct(speed*-1, speed);
    msleep(time);
    create_drive_direct(0,0);
}
void slightturnright(int speed, int time) {
    create_drive_direct(speed, speed*-1);
    msleep(time);
    create_drive_direct(0,0);
}
void retract(int callibration) {
    mav(0,-1000);
    msleep(500+callibration);   
    mav(0,0);
}
void extend(int callibration) {
    mav(0,1000);
    msleep(500+callibration);
    mav(0,0);
}
