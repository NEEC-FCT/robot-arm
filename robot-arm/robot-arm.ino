#include <PS2X_lib.h>
#include <Servo.h>

/********************************************************
 * Code made by Leandro Filipe, the best member of NEEC *
 * Adapted by Rui Goncalves, the greatest member of NEEC*
 ********************************************************/

PS2X ps2x;

Servo rotation;
Servo xtranslation;
Servo ztranslation;
Servo grip;

int error = 0;
int degree = 0;

int maxRotation = 180;
int minRotation = 0;
int maxGrip = 120;
int minGrip =30;
int maxXtranslation = 156;
int minXtranslation = 64;
int maxZtranslation = 125;
int minZtranslation = 0;

int rotationPosition = 85;
int gripPosition = 80;
int xTranslatePosition = 110;
int zTranslatePosition = 63;

int stickToSpeed(int stick) {
  int stickSpeed = (int) ps2x.Analog(stick);
  return map(stickSpeed, 0, 255, 5, -5 );
}

double calculateMinZTranslation(double x) {
  double terms[] = {
    1.6789249136265130e+002,
    -9.9924936155846245e-001,
    -2.5907326094454319e-001,
    3.0060362257979398e-002,
    -1.4169882412462624e-003,
    3.2939893292947497e-005,
    -3.8434020112360762e-007,
    2.6254543593079611e-009,
    -4.5765609121390441e-011,
    1.1144020747201437e-012,
    -1.3744927692397326e-014,
    7.6491731715700222e-017,
    -4.8608992623188306e-020,
    -1.0520678949706218e-021,
    -6.0852486556343547e-024,
    1.3565780972029589e-025,
    -8.2884278288506980e-028,
    2.4251138481492706e-030,
    -2.9175659422229069e-033
  };

  double t = 1;
  double r = 0;
  for (double c : terms) {
    r += c * t;
    t *= x;
  }
  return r;
}

double calculateMaxXTranslation(double x) {
  double terms[] = {
    1.1923838409988089e+002,
    3.7711592659514348e-002,
    1.9704520448807624e-003
  };

  double t = 1;
  double r = 0;
  for (double c : terms) {
    r += c * t;
    t *= x;
  }
  return r;
}

double calculateMinXTranslation(double x) {
  double terms[] = {
    -2.8480008594715473e+003,
    5.3974321671658345e+002,
    -3.5184662559537401e+001,
    1.1734995070605425e+000,
    -2.2362676095680940e-002,
    2.4050916157218849e-004,
    -1.0875785026427224e-006,
    -5.5107508634531047e-009,
    1.0916043625286214e-010,
    -6.6708382875742947e-013,
    1.9581521048088512e-015,
    -2.3227363707923104e-018
  };

  double t = 1;
  double r = 0;
  for (double c : terms) {
    r += c * t;
    t *= x;
  }
  return r;
}

void beginPS2() {
  byte type = 0;
  error = ps2x.config_gamepad(3, 5, 4, 6, true, true); //GamePad(clock, command, attention, data, Pressures?, Rumble?)
  switch (error) {
    case 0: Serial.println("Found Controller, configured successful"); break;
    case 1: Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips"); break;
    case 2: Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips"); break;
    case 3: Serial.println("Controller refusing to enter Pressures mode, may not support it."); break;
  }
  type = ps2x.readType();
  switch (type) {
    case 0: Serial.println("Unknown Controller type"); break;
    case 1: Serial.println("DualShock Controller Found"); break;
    case 2: Serial.println("GuitarHero Controller Found"); break;
  }
}

void setup() {
  Serial.begin(9600);
  beginPS2();
  ztranslation.attach(10);
  rotation.attach(11);
  xtranslation.attach(9);
  grip.attach(12);

  rotation.write(rotationPosition);
  grip.write(gripPosition);
  ztranslation.write(zTranslatePosition);
  xtranslation.write(xTranslatePosition);
}

void loop() {

  if (error == 1) {
    return;
  } else {
    ps2x.read_gamepad();

    if (int rotationSpeed = stickToSpeed(PSS_LX)) {
      rotationPosition = max(min(rotationPosition + rotationSpeed, maxRotation), minRotation);
      rotation.write(rotationPosition);
    }

    if (ps2x.ButtonPressed(PSB_R2)) {
      gripPosition = maxGrip;
      grip.write(gripPosition);
    }

    if (ps2x.ButtonPressed(PSB_L2)) {
      gripPosition = minGrip;
      grip.write(gripPosition);
    }

    if (int zSpeed = stickToSpeed(PSS_LY)) {
      zTranslatePosition = max(min(zTranslatePosition - zSpeed, maxZtranslation), max(minZtranslation, calculateMinZTranslation((double) xTranslatePosition)));
      ztranslation.write(zTranslatePosition);
    }

    if (int xSpeed = stickToSpeed(PSS_RY)) {
      xTranslatePosition = max(min(xTranslatePosition + xSpeed, min(maxXtranslation, calculateMaxXTranslation((double) zTranslatePosition))), max(minXtranslation, calculateMinXTranslation((double) zTranslatePosition)));
      xtranslation.write(xTranslatePosition);
    }
  }
  delay(10);
}