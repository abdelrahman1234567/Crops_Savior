
import grafica.*;
import java.util.Random;
import processing.serial.*;

public GPlot plot1, plot2, plot3, plot4;
public GPointsArray polygonPoints;
public float[] gaussianStack;
public float[] uniformStack;
public int gaussianCounter;
public int uniformCounter;
public PImage mug;
public PShape star;
public Random r;
Serial myPort;  // Create object from Serial class
int val;      // Data received from the serial port
int Temp = 0, Hum = 0,CO2 = 0, Rain = 0;
boolean send = false;
int day=1,month=1,year=2022;
int today = 1;
float Percentage;

ArrayList<TEXTBOX> textboxes = new ArrayList<TEXTBOX>();

public void setup() {
  size(1100, 660);
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
  
  Percentage = random(10,25);
  
  InitLayout();
  // Create a polygon to display inside the plot  
  polygonPoints = new GPointsArray(5);
  polygonPoints.add(2, 0.15);
  polygonPoints.add(6, 0.12);
  polygonPoints.add(15, 0.3);
  polygonPoints.add(8, 0.6);
  polygonPoints.add(1.5, 0.5);

  // Setup for the Rain plot 
  plot1 = new GPlot(this);
  plot1.setPos(350, 300);
  plot1.setDim(250, 250);
  plot1.getTitle().setText("Rain Quantity");
  plot1.getXAxis().getAxisLabel().setText("Day");
  plot1.getYAxis().getAxisLabel().setText("Quantity (L/s)");
  
  // Setup for the Temperature and Humidity Plot
  plot2 = new GPlot(this);
  plot2.setPos(0, 300);
  plot2.setDim(250, 250);
  plot2.getTitle().setText("Temperature and Humidity");
  plot2.getXAxis().getAxisLabel().setText("Day");
  plot2.getYAxis().getAxisLabel().setText("Celsius, Percentage");
  
  // Setup for the CO2 concentration Plot
  plot3 = new GPlot(this);
  plot3.setPos(700, 300);
  plot3.setDim(250, 250);
  plot3.getTitle().setText("CO2 concentration");
  plot3.getXAxis().getAxisLabel().setText("Day");
  plot3.getYAxis().getAxisLabel().setText("ppm");


  star = loadShape("star.svg");
  star.disableStyle();
  textFont(createFont("Times New Roman", 36));
}
int index = 0;
public void draw() {
  if ( myPort.available() > 0) {  // If data is available,
    val = myPort.read();         // read it and store it in val
    index ++;
    if(index == 1){ //Reading Humidity
      for(int i = 0;i < 10;i++){
        if((val&(1<<i)) != 0){
          Hum += (1<<i);
        }
        delay(100);
      }
      delay(3000);
    }
    // Reading temperature
    if(index == 2){
      for(int i = 0;i < 10;i++){
        if((val&(1<<i)) != 0){
          Temp += (1<<i);
        }
        delay(100);
      }
      delay(3000);
    }
    // Reading CO2 concentration
    if(index == 3){
      for(int i = 0;i < 10;i++){
        if((val&(1<<i)) != 0){
          CO2 += (1<<i);
        }
        delay(100);
      }
      delay(3000);
    }
    // Reading Rain quantity
    if(index == 4){
      for(int i = 0;i < 10;i++){
        if((val&(1<<i)) != 0){
          Rain += (1<<i);
        }
        delay(100);
      }
      delay(3000);
    }
  }
  
  background(255);
  textSize(20);
  text("       Day            Month             Year", 50, 50);
  textSize(15);
  text("*Please press ENTER to show the prediction on this date.", 50, 130);
  textSize(20);
  text("Rain quantity                                 CO2 concentration                                 Temperature                                 Humidity",70,180);
  noFill();
  stroke(0);
  rect(30, 200, 200,70);
  rect(320, 200, 200,70);
  rect(610, 200, 200,70);
  rect(870, 200, 200,70);
  fill(0);
  text(Rain + "L/s", 70, 220);
  text(CO2 + " ppm", 370, 220);
  text(Temp + " C", 670, 220);
  text(Hum + "%", 970, 220);
  for (TEXTBOX t : textboxes) t.DRAW();
  if(send){
    int t = (year-2010-1) * 365 + (month-1) * 30 + day;
    float Predicted_Temp = (28.0786) + (0.000119159) * t;
    float Predicted_Hum = (49.7199) + (-1.92923e-07) * t;
    float Predicted_Ins = (Predicted_Temp-26) * Percentage;
    textSize(16);
    text("The Predicted values on  " + day + "/" + month + "/" + year + "  are:" , 430, 50);
    text("Predicted Temperature ", 430, 80);
    text("Predicted Humidity ", 650, 80);
    text("Predicted Crop Loss ", 870, 80);
    text(Predicted_Temp + " C", 450, 120);
    text(Predicted_Hum + " ppm", 670, 120);
    text(Predicted_Ins + "%", 890, 120);
    noFill();
    stroke(0);
    rect(430, 100, 140,40);
    rect(650, 100, 140,40);
    rect(870, 100, 140,40);
    fill(0);
  }
  plot1.addPoint(today, Rain);
  plot2.addPoint(today, Temp);
  plot3.addPoint(today, CO2);

  // Draw the first plot  
  plot1.beginDraw();
  plot1.drawBackground();
  plot1.drawBox();
  plot1.drawXAxis();
  plot1.drawYAxis();
  plot1.drawTitle();
  plot1.drawGridLines(GPlot.BOTH);
  plot1.drawLines();
  plot1.drawPoints(star);
  plot1.endDraw();
  
  // Draw the second plot  
  plot2.beginDraw();
  plot2.drawBackground();
  plot2.drawBox();
  plot2.drawXAxis();
  plot2.drawYAxis();
  plot2.drawTitle();
  plot2.drawGridLines(GPlot.BOTH);
  plot2.drawLines();
  plot2.drawPoints(star);
  plot2.endDraw();
  
   // Draw the third plot  
  plot3.beginDraw();
  plot3.drawBackground();
  plot3.drawBox();
  plot3.drawXAxis();
  plot3.drawYAxis();
  plot3.drawTitle();
  plot3.drawGridLines(GPlot.BOTH);
  plot3.drawLines();
  plot3.drawPoints(star);
  plot3.endDraw();
}

void InitLayout() {
   TEXTBOX day = new TEXTBOX();
   day.W = 100;
   day.H = 35;
   day.X = 50;
   day.Y = 70;
   textboxes.add(day);
   
   TEXTBOX month = new TEXTBOX();
   month.W = 100;
   month.H = 35;
   month.X = 160;
   month.Y = 70;
   textboxes.add(month);
   
   TEXTBOX year = new TEXTBOX();
   year.W = 100;
   year.H = 35;
   year.X = 270;
   year.Y = 70;
   textboxes.add(year);
}

void mousePressed() {
   for (TEXTBOX t : textboxes) {
      t.PRESSED(mouseX, mouseY);
   }
}

void keyPressed() {
   for (TEXTBOX t : textboxes) {
      if (t.KEYPRESSED(key, keyCode)) {
         send = true;
      }
   }
   if(send){
     day = int(textboxes.get(0).Text);
     month = int(textboxes.get(1).Text);
     year = int(textboxes.get(2).Text); 
   }
}
