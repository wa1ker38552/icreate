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
void print(char text);

int main()
{
    create_connect();
    enable_servos();
    wait_for_light(0); // wait for light
    shut_down_in(115);
    //alldown();
    /* uncomment if going bottom right
 	// move to botguy head to start
    move(200,200);
    turnleft(20);
    move(300,600);
    turnright(100);
	// move to line
    */
    move_until_black(calc_threshold());

    move(500,750);
    turnright(200); // calibrate
    move(500,500);
    extend(300); msleep(500);
    retract(300);
    turnleft(100);

    pause();
    forwarduntilbump(800);
    move(-200,600);
    turnright(300);
    move(600,1100);
    move_until_black(calc_threshold());
    move(400,350); // if facing left
    turnleft(200);
    move(-100,400);
    //===========
   	// open
    slow_servo(2, 850); // move middle up
    slow_servo(3, 1200); // move bottom forward
    //===========
    forwarduntilbump(50);
    slow_servo(1,810);
    msleep(1000*16); // time to wait
    move(-100,1300); // grabbed already

    turnright(0);
    move(200,400);
    turnright(280);

    move_until_black(calc_threshold());
    move(250,1700);
    forwarduntilbump(100);
    move(-200,500);

    turnleft(60);
    move(600,525);
    move(250,1100);
    
    turnright(100);
    forwarduntilbump(100);
    move(-100,600);
    turnleft(0);
    
    //slightturnright(100,200);
    slow_servo(3, 300);
    slow_servo(2, 200);
    slightturnright(50,200);
    move(100,600);
    msleep(1000);

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
    int previous = analog(0);
    int threshold = absol(25*(previous-analog(0)));
    previous = analog(0);
    msleep(333);
    threshold =+ absol(25*(previous-analog(0)));
    previous = analog(0);
    msleep(333);
    threshold =+ absol(25*(previous-analog(0)));
    threshold = threshold/3;
    printf("%i", threshold);
    while (1) {
        msleep(500);
        if ((previous-analog(0)) > threshold) {
            printf("Previous %i Analog %i",previous,analog(0));
            break;
        }
        previous = analog(0);
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
    slow_servo(3, 1725); // move bottom back
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
    mav(0,1000);
    msleep(500+callibration);   
    mav(0,0);
}
void extend(int callibration) {
    mav(0,-1000);
    msleep(500+callibration);
    mav(0,0);
}
void print(char text) {
    printf("%d", text);
}
