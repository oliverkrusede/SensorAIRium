//VERSION 1.0
#include <Adafruit_NeoPixel.h>
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 24 // Popular NeoPixel ring size

#define sideLED 0.75  

#define BREATH_FADE_TIME_P 0.125    // 1/8
#define BREATH_BREATH_TIME_P 0.125  // 1/8

#define BREATH_LOW 0.1
#define BREATH_CYC 0.5
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int iByte = 0;
byte a = 99;
bool green = true;

long startTime[NUMPIXELS];
int changeTime[NUMPIXELS];


int targetColor[NUMPIXELS][3];
int oldColor[NUMPIXELS][3];
int currentColor[NUMPIXELS][3];

bool ownColor[NUMPIXELS];

int breathColor[3]={0,150,0};
int groundColor[3]={0,0,0};
int breathTime=0;
long breathStart=0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pixels.begin();
  //pixels.fill(pixels.Color(0,115,0), 0, 24);
  pixels.clear();
  pixels.show();

  //uint8_t a = 70;
  //uint8_t type = a >> 6;
  //uint8_t id = a & 63;
  
  //Serial.print("TYPE : ");
  //Serial.println(type);
  //Serial.print("LED ID: ");
  //Serial.println(id);


  //test
  /*for(int cTime=0;cTime<50; cTime++){
    Serial.print("\nTime: ");
    Serial.println(cTime);
    if(breathTime<1){ 
      Serial.println("KILL");
      return;
    }
    if(cTime-breathStart<=breathTime*BREATH_FADE_TIME_P){
      Serial.println("INIT FADE UP");
      //Serial.println("fadeUP");
      float param = min(1,((1-BREATH_LOW)/(float) (breathTime*BREATH_FADE_TIME_P))*(cTime-breathStart)+BREATH_LOW);
      Serial.println(param);
      groundColor[0] = breathColor[0]*param;
      groundColor[1] = breathColor[1]*param;
      groundColor[2] = breathColor[2]*param;
      Serial.print(groundColor[0]);
      Serial.print(" ");
      Serial.print(groundColor[1]);
      Serial.print(" ");
      Serial.println(groundColor[2]);
    }else if(breathTime-cTime-breathStart<breathTime*BREATH_FADE_TIME_P){
      //fade Out
      //Serial.println("DOWN");
    }else{
      float step = ((1-BREATH_CYC)/(breathTime*BREATH_BREATH_TIME_P));
      Serial.print("step:");
      Serial.println(step);
      int delta=cTime-breathStart-breathTime*BREATH_FADE_TIME_P-1;
      Serial.print("delta:");
      Serial.println(delta);
      bool goingDown =round(floor(delta/round(breathTime*BREATH_BREATH_TIME_P)))%2==0;
      //breath
      //int value = (cTime-breathStart) % round(BREATH_BREATH_TIME_P*breathTime+1);
      float value = ((delta)%round(breathTime*BREATH_BREATH_TIME_P))+1;
      Serial.print("value: ");
      Serial.println(value);
      if(goingDown){
        //going up
        Serial.println("Going down");
        //float param = min(1,((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
        //float param = min(1,((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
        
        float param = (1-step*value);
        Serial.println(param);
        
      }else{
        Serial.println("Going up");
        //float param = max(0,((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
        //float param = max(0,-((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
        float param = (BREATH_CYC+step*value);
        Serial.println(param);
        
      }

      
  
    }  
    delay(10);
  }*/
  breathTime=20000;
  breathStart=millis();
  



  
 

}





void fade(long currentTime, int index, int tRed, int tGreen, int tBlue, int chgTime){
  if(changeTime[index]!=0){
    oldColor[index][0] = currentColor[index][0];
    oldColor[index][1] = currentColor[index][1];
    oldColor[index][2] = currentColor[index][2];
  }
  /*Serial.print("index: ");
  Serial.println(index);

  Serial.print("time: ");
  Serial.println(chgTime);

  Serial.print("rgb: ");
  Serial.print(tRed);
  Serial.print(",");
  Serial.print(tGreen);
  Serial.print(",");
  Serial.println(tBlue);*/
  ownColor[index]=true;
  startTime[index] = currentTime;
  changeTime[index] = chgTime;
  targetColor[index][0] = tRed;
  targetColor[index][1] = tGreen;
  targetColor[index][2] = tBlue;
}
void fadeBack(long currentTime, int index, int chgTime){
  if(changeTime[index]!=0){
    oldColor[index][0] = currentColor[index][0];
    oldColor[index][1] = currentColor[index][1];
    oldColor[index][2] = currentColor[index][2];
  }
  /*Serial.print("index: ");
  Serial.println(index);

  Serial.print("time: ");
  Serial.println(chgTime);*/
  startTime[index] = currentTime;
  changeTime[index] = chgTime;
  
  //if fading this was the reason - color was overridden
  targetColor[index][0] = -1;
  targetColor[index][1] = -1;
  targetColor[index][2] = -1;
}
int myModLED(int number){
  return (number+NUMPIXELS)%NUMPIXELS;
}
void handleFade(long currentTime){
  for(int i=0; i<NUMPIXELS; i++){
    int chgTime = changeTime[i];
    if(chgTime!=0){
      int tR = targetColor[i][0];
      int tG = targetColor[i][1];
      int tB = targetColor[i][2];
      /*Serial.print("TARGET: ");
      Serial.print(tR);
      Serial.print(",");
      Serial.print(tG);
      Serial.print(",");
      Serial.println(tB);*/
      
      if(targetColor[i][0]==-1){
        tR = groundColor[0];
        tG = groundColor[1];
        tB = groundColor[2];
      }
      float param = (float)(currentTime-startTime[i])/(float)chgTime;
      /*Serial.print("PARAM :");
      Serial.println(param);*/
      if(param>=1){
        if(targetColor[i][0]==-1){
          ownColor[i]=false;
        }
        changeTime[i] = 0;
        oldColor[i][0] = tR;
        oldColor[i][1] = tG;
        oldColor[i][2] = tB;
        currentColor[i][0] = round(tR);
        currentColor[i][1] = round(tG);
        currentColor[i][2] = round(tB);
        pixels.setPixelColor(i, pixels.Color(tR, tG, tB));
      }else{
        float r = (float)(tR-oldColor[i][0])*param+oldColor[i][0];
        float g = (float)(tG-oldColor[i][1])*param+oldColor[i][1];
        float b = (float)(tB-oldColor[i][2])*param+oldColor[i][2];
        /*Serial.print("OUT : ");
        Serial.print(r);
        Serial.print(",");
        Serial.print(g);
        Serial.print(",");
      
        Serial.println(b);*/
        currentColor[i][0] = round(r);
        currentColor[i][1] = round(g);
        currentColor[i][2] = round(b);
        
        pixels.setPixelColor(i, pixels.Color(round(r), round(g), round(b)));
      }
    }
    
  }
}
int oldBreathStatus[3];

void colorGround(long cTime){
  //Serial.println(cTime);
  
  if(breathTime<1){ 
    //Serial.println("KILL");
    return;
  }
  
  if(cTime-breathStart>breathTime){
    breathTime=0;
  }
  else if(cTime-breathStart<=breathTime*BREATH_FADE_TIME_P){
    //Serial.println("UP");
    //Serial.println("fadeUP");
    float param = min(1,((1-BREATH_LOW)/(float) (breathTime*BREATH_FADE_TIME_P))*(cTime-breathStart)+BREATH_LOW);
    //groundColor[0] = breathColor[0]*param;
    //groundColor[1] = breathColor[1]*param;
    //groundColor[2] = breathColor[2]*param;

    groundColor[0] = round((float)(breathColor[0]-oldBreathStatus[0])*param+oldBreathStatus[0]);
    groundColor[1] = round((float)(breathColor[1]-oldBreathStatus[1])*param+oldBreathStatus[1]);
    groundColor[2] = round((float)(breathColor[2]-oldBreathStatus[2])*param+oldBreathStatus[2]);
    
  }else if((int) (cTime-breathStart)>breathTime-breathTime*BREATH_BREATH_TIME_P){
    //Serial.println("DOWN");
    //fade Out
    //int delta = cTime-breathStart;
    //float stepPTime = (float)(1-BREATH_LOW)/(float) (breathTime*BREATH_FADE_TIME_P);
    //float param = (breathTime-delta)*-(stepPTime);
    float param = min(1,((1-BREATH_LOW)/(float) (breathTime*BREATH_FADE_TIME_P))*(breathTime-(cTime-breathStart))+BREATH_LOW);
    //float patam = max(0, (delta)*(1-BREATH_LOW));
    //Serial.println(param);
    //Serial.println(breathTime*BREATH_FADE_TIME_P);
    //Serial.println(breathTime-delta);
    
    groundColor[0] = breathColor[0]*param;
    groundColor[1] = breathColor[1]*param;
    groundColor[2] = breathColor[2]*param;
    
    
    
    
  }else{
    //Serial.println("NEW");
    
    float step = ((1-BREATH_CYC)/(breathTime*BREATH_BREATH_TIME_P));
    
    int delta=cTime-breathStart-breathTime*BREATH_FADE_TIME_P-1;
    
    bool goingDown =round(floor(delta/round(breathTime*BREATH_BREATH_TIME_P)))%2==0;
    //breath
    //int value = (cTime-breathStart) % round(BREATH_BREATH_TIME_P*breathTime+1);
    float value = ((delta)%round(breathTime*BREATH_BREATH_TIME_P))+1;

 

    
    float param = 0;
    if(goingDown){
      //going up
      
      //float param = min(1,((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
      //float param = min(1,((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
      
      param = (1-step*value);
      groundColor[0] = breathColor[0]*param;
      groundColor[1] = breathColor[1]*param;
      groundColor[2] = breathColor[2]*param; 
      
      
    }else{
      //firstBreath = true;
      
      //float param = max(0,((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
      //float param = max(0,-((1-BREATH_CYC)/(float) (breathTime*BREATH_BREATH_TIME_P))*(value)+BREATH_CYC);
      param = (BREATH_CYC+step*value);
      groundColor[0] = breathColor[0]*param;
      groundColor[1] = breathColor[1]*param;
      groundColor[2] = breathColor[2]*param; 
      
      
    }
    
     
    
    
    


    
  }


  
  for(int i=0; i<NUMPIXELS; i++){
    if(!ownColor[i]){
      pixels.setPixelColor(i, pixels.Color(groundColor[0], groundColor[1], groundColor[2]));
      oldColor[i][0] = groundColor[0];
      oldColor[i][1] = groundColor[1];
      oldColor[i][2] = groundColor[2];
      currentColor[i][0] = groundColor[0];
      currentColor[i][1] = groundColor[1];
      currentColor[i][2] = groundColor[2];
      
    }
  }
  //pixels.fill(pixels.Color(0,50,0), 0, 24);
}


long lastChange = millis();
int changedLED = -1;


int received[5];
byte receivedLen = 0;
uint8_t messageType = 0;
uint8_t messageLED_ID = 0;

uint8_t messageLED_RED = 0;
uint8_t messageLED_GREEN = 0;
uint8_t messageLED_BLUE = 0;

uint8_t messageFADE = 0;
uint8_t messageSIZE = 0;

uint16_t tmpbreathTime;
uint8_t tmpRed;
uint8_t tmpGreen;
uint8_t tmpBlue;


void loop() {
  long cTime = millis();
  if(Serial.available()>0){
    received[receivedLen++] = Serial.read();
    /*Serial.print(received[receivedLen-1]);
    Serial.println(" FOUND");
    Serial.print(receivedLen);
    Serial.println(" LEN");*/
    if(receivedLen==1){
      //first byte
      messageType = received[receivedLen-1] >> 6;
      
      //Serial.print("Type: ");
      //Serial.println(messageType);
      if(messageType>=0 && messageType<=1){
        //FOR FADING - ON AND BACK
        messageLED_ID=received[receivedLen-1] & 63;
        /*Serial.print("led id: ");
        Serial.println(messageLED_ID);*/
      }else if(messageType==2){
        //BREATH
        tmpbreathTime=received[receivedLen-1] & 63;
        //Serial.println(tmpbreathTime);
        tmpbreathTime*=500;
        //Serial.println(tmpbreathTime);

      }
    }else if(receivedLen>=2){
      //Serial.println(">=2");
      switch(receivedLen){
        case 2:
          if(messageType==0){
            messageLED_RED =received[receivedLen-1];  
          }else if(messageType==1){
            messageFADE = received[receivedLen-1] >> 3;
            messageSIZE = received[receivedLen-1] & 7;
            fade(cTime, messageLED_ID, messageLED_RED, messageLED_GREEN, messageLED_BLUE, messageFADE*100);
            fadeBack(cTime, messageLED_ID, messageFADE*100);
            for(int i=1; i<messageSIZE+1; i++){
              fadeBack(cTime, myModLED(messageLED_ID+i), messageFADE*100);
              fadeBack(cTime, myModLED(messageLED_ID-i), messageFADE*100);
              
            }
            /*Serial.println("FADEBACK");
            Serial.print("ID: ");
            Serial.println(messageLED_ID);
            Serial.print("size: ");
            Serial.println(messageSIZE);
            Serial.print("time: ");
            Serial.println(messageFADE*100);*/
            receivedLen=0;
            
          }else if(messageType==2){
              
              tmpRed =received[receivedLen-1];  
              
            }
          
          //Serial.print("RED: ");
          //Serial.println(messageLED_RED);
          break;
        case 3:
          if(messageType==0){
            messageLED_GREEN = received[receivedLen-1];
          }else if(messageType==2){
            
            tmpGreen=received[receivedLen-1];
              
          }
          
          //Serial.print("GREEN: ");
          //Serial.println(messageLED_GREEN);
          break;
        case 4:
          
          if(messageType==0){
            messageLED_BLUE =received[receivedLen-1];
          }else if(messageType==2){
            oldBreathStatus[0]=groundColor[0];
            oldBreathStatus[1]=groundColor[1];
            oldBreathStatus[2]=groundColor[2];
           

            
            breathColor[0]=tmpRed;
            breathColor[1]=tmpGreen;
            breathColor[2]=received[receivedLen-1];
            breathTime=tmpbreathTime;
            breathStart=cTime;
            /*Serial.print("rgb: ");
            Serial.print(breathColor[0]);
            Serial.print(",");
            Serial.print(breathColor[1]);
            Serial.print(",");
            Serial.println(breathColor[2]);

            Serial.print("time: ");
            Serial.println(breathTime); */
            
            
            receivedLen=0;
            
            
              
          }
         // Serial.print("BLUE: ");
          //Serial.println(messageLED_BLUE);
          break;
        case 5:
          messageFADE = received[receivedLen-1] >> 3;
          messageSIZE = received[receivedLen-1] & 7;
          /*Serial.print("TYPE: ");
          Serial.println(messageType);
          Serial.print("LED-ID: ");
          Serial.println(messageLED_ID); 
          Serial.print("RED: ");
          Serial.println(messageLED_RED);
          Serial.print("GREEN: ");
          Serial.println(messageLED_GREEN);
          Serial.print("BLUE: ");
          Serial.println(messageLED_BLUE);
  
          Serial.print("Fade: ");
          Serial.println(messageFADE*100);
  
          Serial.print("Size: ");
          Serial.println(messageSIZE);*/
          fade(cTime, messageLED_ID, messageLED_RED, messageLED_GREEN, messageLED_BLUE, messageFADE*100);
          float dec = 1;
          for(int i=1; i<messageSIZE+1; i++){
            dec*=sideLED;
            fade(cTime, myModLED(messageLED_ID+i), round(messageLED_RED*dec), round(messageLED_GREEN*dec), round(messageLED_BLUE*dec), messageFADE*100);
            fade(cTime, myModLED(messageLED_ID-i), round(messageLED_RED*dec), round(messageLED_GREEN*dec), round(messageLED_BLUE*dec), messageFADE*100);
            
          }
          receivedLen=0;

          break;
      }
      
    }

    
    /*if(receivedLen==5){
      fade(cTime, received[0], received[1], received[2], received[3], received[4]);
      receivedLen = 0;
      Serial.print(received[0]);
      Serial.print(",");
      Serial.print(received[1]);
      Serial.print(",");
      Serial.print(received[2]);
      Serial.print(",");
      Serial.print(received[3]);
      Serial.print(",");  
      Serial.println(received[4]);
    }*/
    
  }

  
  if((cTime-lastChange)==-3000L){
    if(changedLED>=0){
      //fade(cTime, changedLED, 255,0,0,1000);
      fadeBack(cTime, changedLED, 1000);
      fadeBack(cTime, myModLED(changedLED+1), 1000);
      fadeBack(cTime, myModLED(changedLED-1), 1000);
      
      changedLED=-1;
    }else{
      
      changedLED =random(NUMPIXELS);
      //changedLED = 5;
      if(random(2)==0){
        fade(cTime, changedLED, 0, 0, 255, 1000);  
        fade(cTime, myModLED(changedLED+1), 0, 0, 200, 1000);  
        fade(cTime, myModLED(changedLED-1), 0, 0, 200, 1000);  
      }else{
        fade(cTime, changedLED, 255, 0, 0, 1000);
        fade(cTime, myModLED(changedLED+1), 200, 0, 0, 1000);  
        fade(cTime, myModLED(changedLED-1), 200, 0, 0, 1000);  
      }
      
    }
    lastChange=cTime;
  }
  colorGround(cTime);
  handleFade(cTime);
  pixels.show();
  
  
  
  

  // put your main code here, to run repeatedly:
  /*while(Serial.available() > 0){
    iByte = Serial.read();
    if(iByte==a){
      if(green){
        pixels.fill(pixels.Color(115,0,0), 0, 24);
        green = false;
      }else{
        pixels.fill(pixels.Color(0,115,0), 0, 24);
        green = true;
      }
      pixels.show();
    }
    
    
    //Serial.print("i rec: ");
    //Serial.println(iByte, DEC);
  }*/
  
}
