#include "mbed.h"
#include "bbcar.h"


Ticker servo_ticker;
PwmOut pin9(D9), pin8(D8);
DigitalInOut pin10(D10);
BBCar car(pin8, pin9, servo_ticker);
Serial pc(USBTX,USBRX); //tx,rx
Serial uart(D1,D0); //tx,rx
Serial xbee(D12, D11);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
Thread t;
EventQueue queue(32 * EVENTS_EVENT_SIZE);
parallax_ping  ping1(pin10);

void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
void recieve_thread(void);
void send_thread(void);

int main() {
    
    pc.baud(9600);
    t.start(recieve_thread);

    // turn off the LED
    led1 = 1;
    led2 = 1;
    led3 = 1;

    // into the mission 1
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<25){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Turn Left\r\n");
    car.turn(100,0.3);
    wait(1.9);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);

    // parking
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<48){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Parking\r\n");
    car.turn(-100, 0.1);
    wait(1.5);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Backward!!!\r\n");
    car.goStraight(-100);
    while(1){
        if((float)ping1>45){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<25){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Turn Left\r\n");
    car.turn(100,0.3);
    wait(1.9);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);


    // take a shot
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<5){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Backward Turn Left\r\n");
    car.turn(-100,0.1);
    wait(1.5);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Backward!!!\r\n");
    car.goStraight(-100);
    while(1){
        if((float)ping1>30){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Take a Shot!!!\r\n");
    send_uart();
    wait(1);

    // go to next mission
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<25){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Turn Right\r\n");
    car.turn(100, -0.3);
    wait(2);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);

    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    wait(3);
    xbee.printf("Turn Right\r\n");
    car.turn(100,-0.2);
    wait(1.5);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<25){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Turn Right\r\n");
    car.turn(100,-0.3);
    wait(2);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<50){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Turn Right\r\n");
    car.turn(100,-0.1);
    wait(1.5);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);

    // scan object
    xbee.printf("Scan Object\r\n");
    if(float(ping1) > 25)
        xbee.printf("Double triangle\r\n");
    else if(float(ping1) > 20)
        xbee.printf("Triangle\r\n");
    else if(float(ping1) > 15)
        xbee.printf("Right Triangle\r\n");
    else
        xbee.printf("Square\r\n");
        
    // leave
    xbee.printf("Backward Turn Right\r\n");
    car.turn(-100,-0.1);
    wait(1.5);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    while(1){
        if((float)ping1<25){
            car.stop();
            break;
        }
        wait(.01);
    }
    xbee.printf("Turn Right\r\n");
    car.turn(100, -0.3);
    wait(2);
    xbee.printf("Stop\r\n");
    car.stop();
    wait(1);
    xbee.printf("Forward!!!\r\n");
    car.goStraight(100);
    wait(10);




}

void xbee_rx(void)
{
  static int i = 0;
  static char buf[100] = {0};
  while(xbee.readable()){
    char c = xbee.getc();
    if(c!='\r' && c!='\n'){
      buf[i] = c;
      i++;
      buf[i] = '\0';
    }else{
      i = 0;
      pc.printf("Get: %s\r\n", buf);
      xbee.printf("%s", buf);
    }
  }
  wait(0.1);
  xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
   pc.printf("%s\r\n", messange);
   xbee_reply[0] = '\0';
   xbee_reply[1] = '\0';
   xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();
  pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}
void recieve_thread(){
   while(1) {
      if(uart.readable()){
            char recv = uart.getc();
            swiitch(recv):
                case 0:
                    led1 = 0;
                    led2 = 1;
                    led3 = 1;
                    break;
                case 1:
                    led1 = 1;
                    led2 = 0;
                    led3 = 1;
                    break;
                case 2:
                    led1 = 1;
                    led2 = 1;
                    led3 = 0;
                    break;
                default:
                    led1 = 0;
                    led2 = 1;
                    led3 = 1;
                    break;
            xbee.printf("%c\r\n, recv");
      }
   }
}
void send_thread(){
    char s[21];
    sprintf(s,"image_classification");
    uart.puts(s);
    wait(0.5);
    }
}
