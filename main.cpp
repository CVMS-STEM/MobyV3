/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       C:\Users\williDoo                                         */
/*    Created:      Fri Jul 16 2021                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// l                    motor_group   1, 2            
// r                    motor_group   10, 9           
// fork                 motor_group   3, 8            
// c                    motor         4               
// i                    inertial      11              
// rot                  rotation      12              
// Controller1          controller                    
// intake               motor         5               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;
// global variables
bool turbo = false, conveyorstarted = false, bal = false, balancing;
double rVelocity, lVelocity;

//vexcodeinit
void hi(){
  vexcodeInit()
  ;
}
void t(){
  if (Controller1.ButtonA.pressing()){
    turbo = !(turbo);
  }
}
void screen(){
  Controller1.Screen.clearScreen();
  if (i.roll() != gyr){
    Controller1.Screen.clearLine(1);
    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("gyro: ");
    Controller1.Screen.print(i.roll());
    gyr = i.roll();
  } if (tur != turbo){
    Controller1.Screen.clearLine(2);
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print("turbo: ");
    Controller1.Screen.print(turbo);
    tur = turbo; 
  } if (rot.position(rotationUnits::deg) != fk){
    Controller1.Screen.clearLine(3);
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("fork: ");
    Controller1.Screen.print(rot.position(rotationUnits::deg));
    fk = rot.position(rotationUnits::deg);
  }
}
//auto-balance
void balance(){
  balancing = false;
  if (Controller1.ButtonB.pressing()){
    while (!(-5 < i.roll()&&i.roll() < 5)){
      balancing = true;
      Controller1.Screen.print(i.roll());  
      if (i.roll() < 30 || i.roll() > 120){
        r.spinFor(directionType::fwd, 30, rotationUnits::deg, false);
        l.spinFor(directionType::fwd, 30, rotationUnits::deg, false);      
      } else if (120 < i.roll() || i.roll() < 210){ 
        r.spinFor(directionType::rev, 30, rotationUnits::deg,false);
        l.spinFor(directionType::rev, 30, rotationUnits::deg, false);
        
      }
      if (Controller1.ButtonB.pressing()){
        break;
      }
      wait(20, msec);
    }
    if (balancing){
      r.stop(brakeType::hold);
      l.stop(brakeType::hold);
      Controller1.rumble(".");
    } 
  }
}
// actually driving the robot
// found a big way to cut if statements, shortening this by about 20 lines
void driveCheck(){
  rVelocity = 0;
  lVelocity = 0;
  if(Controller1.Axis3.position() != 0){   
    if (turbo) { // if turbo mode is on, slightly faster
      rVelocity = 0.65 * Controller1.Axis3.position() - 0.25 * Controller1.Axis1.position();
      lVelocity = 0.65 * Controller1.Axis3.position() + 0.25 * Controller1.Axis1.position();
    } else { // 
      rVelocity = 0.35 * Controller1.Axis3.position() - 0.15 * Controller1.Axis1.position();
      lVelocity = 0.35 * Controller1.Axis3.position() + 0.15 * Controller1.Axis1.position();
    }
  } else {
    if (turbo){
      rVelocity = -0.35 * Controller1.Axis1.position();
      lVelocity = 0.35 * Controller1.Axis1.position(); 
    } else { 
      rVelocity = -0.25 * Controller1.Axis1.position();
      lVelocity = 0.25 * Controller1.Axis1.position();
    }
  }
  r.spin(directionType::fwd, rVelocity, velocityUnits::pct);
  l.spin(directionType::fwd, lVelocity, velocityUnits::pct);
}

//spin fork
void Claw(){
  while (Controller1.ButtonL1.pressing()){
    
    if (rot.position(rotationUnits::deg) <= 31) {
      fork.spin(directionType::fwd, 20, velocityUnits::pct);
    } else {
      Controller1.rumble("-");
    }
  } while (Controller1.ButtonR1.pressing()){
    if (rot.position(rotationUnits::deg) >= 0){
      fork.spin(directionType::rev, 20, velocityUnits::pct);
    } else {
      Controller1.rumble("-");
    }
  }
  fork.stop();
}

//conveyor
void conveyor(){
  if (Controller1.ButtonX.pressing()){
    conveyorstarted = !(conveyorstarted);
  }
  if (conveyorstarted){
    c.spin(directionType::fwd, 75, velocityUnits::pct);
    intake.spin(directionType::fwd, 60, velocityUnits::pct);
  } else{
    c.stop();
    intake.stop();
  }
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  rot.resetPosition();
  while (true){
    screen();
    driveCheck();
    t();
    balance();
    Claw();
    conveyor();
  
  }
}
