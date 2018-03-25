
import processing.serial.*;
Serial ser;


int max_x, min_x, max_y, min_y;
int center_x, center_y;
int speed_x,speed_y;


int[] ma_x;
int[] ma_y;
int ma_index;

int dir = 10;

void get_m_ave(){
  long sum_x = 0;
  long sum_y = 0;
  
  for(int i = 0;i<30;i++){
    sum_x += ma_x[i];
    sum_y += ma_y[i];
  }
  center_x = int(sum_x / 30);
  center_y = int(sum_y / 30);
}

void get_s_ave(){
  long sum_x = 0;
  long sum_y = 0;
  
  for(int i = 1;i<29;i++){
    sum_x += abs(ma_x[i] - ma_x[i+1]);
    sum_y += abs(ma_y[i] - ma_y[i+1]);
  }
  speed_x = int(sum_x)*2;
  speed_y = int(sum_y)*2;
}
    
  

void setup() {

  max_x = -300;
  min_x = 300;

  max_y = -300;
  min_y = 300;
  
  ma_index = 0;
  ma_x = new int[30];
  ma_y = new int[30];
  
  
  for(int i = 0;i<30;i++){
    ma_x[i] = 0;
    ma_y[i] = 0;
  }

  size(880, 700);
  background(0, 0, 0);
  println(Serial.list());
  String xdk = Serial.list()[2];       
  ser = new Serial(this, xdk, 9600);
}

void draw() {
  if ( ser.available() > 0) {
    background(0, 0, 0); 
    String data = ser.readString(); 
    String[] s = splitTokens(data, ",");  
    
    //text(data, 20, 20); 

    int da = int(s[1]);
    int db = int(s[2]);
    int dc = int(s[3]);
    
    ma_x[ma_index] = int(s[1]);
    ma_y[ma_index] = int(s[2]);
    
    ma_index++;
    if(ma_index == 30) ma_index = 0;
    
    get_m_ave();
    get_s_ave();
    
    int vx = (da - center_x) * dir;
    int vy = (db - center_y) * dir;
    
    
 background(0);
 
 fill(0);
 stroke(255);
 rect(200, 200, 400, 400);
 ellipse(400, 400,400,400);
 
 
 
fill(255, 255, 255);
  ellipse(400 + vx, 400 + vy,speed_x+10,speed_y+10);
  


    //text(s[1], 600, 20);
    //text(s[2], 600, 40);
    //text(s[3], 600, 60);
     
    //textSize(16);      
    //text(center_x, 180, 420);
    //text(center_y, 180, 480);
    
    //text(speed_x, 180, 440);
    //text(speed_y, 180, 500);

    //text(vx, 620, 420);
    //text(vy, 620, 440);
    //text(s[2], 620, 460);
    
    textSize(40);

    int light = int(s[4]) / 1000;
    text(light, 620, 440);

    int tempture = int(s[5]) /100;
    float v_tempture = float(tempture) / 10;
    text(nf(v_tempture,2,1) + "ºC", 100, 680);

    int moisture = int(s[6]);
    text(moisture + "%", 350, 680);

    float pressure = float(s[7]) /100;
    text(nf(pressure,4,0) + "hPa", 500, 680);
    
    textSize(100);
     text("Coffee Dojo!", 80, 120);
  }
}
