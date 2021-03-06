#include <Arduino.h>
#include "Boby.h"


Boby::Boby(){
	Steppers = new BobySteppers();
}

void Boby::init()
{
	analogReference(INTERNAL); //reference analogique 2.56V

	setCalibration(Boby_MM_TO_STEP, Boby_RAD_TO_STEP);
	_deltaArc = Boby_DELTA_ARC;

	Steppers->setMaxSpeed(300.0);
	Steppers->setSpeed(300.0);
	Steppers->enable();

}

void Boby::run(){
  Steppers->run();
}

void Boby::setCalibration(int distance, int rotation){
	_mmToStep = distance;
	_radToStep = rotation;
}

void Boby::setSpeed(float vitesse){
	Steppers->setSpeed(vitesse);
}

void Boby::setSpeed( float vitesseD, float vitesseG){
	Steppers->setSpeed(vitesseD, vitesseG);
}

void Boby::logSpeed(){
	Serial.print("Vitesse : "); Serial.print(Steppers->getSpeed(0)); Serial.print(" | "); Serial.println(Steppers->getSpeed(1));
	Serial.print("Vitesse max : "); Serial.print(Steppers->getMaxSpeed(0)); Serial.print(" | "); Serial.println(Steppers->getMaxSpeed(1));
}


void Boby::turnGoDegree(float angle, long ligne){
  angle = angle * DEG_TO_RAD ; // Passage en radians
  turnGo(angle, ligne);
}

void Boby::turnGo(float angle, long ligne){

  if(angle > 0 && angle < PI){
    gauche( int( (angle * _radToStep)) );
  }
  else if( angle >= PI ){
	  droite(int( ( (angle-PI) * _radToStep)) );
  }
  else if( angle < 0 ){
    droite(int( -( angle * _radToStep)) );
  }
  else{
    stop(100);
  }

  if( ligne > 0 ){
    avant( (ligne * _mmToStep)/10 );
  }
  else if( ligne < 0 ){
    arriere( -( ligne * _mmToStep)/10 );
  }
  else{
    stop(100);
  }
}


void Boby::avant(long pas){
	Steppers->moveTo(-pas, pas);
	Steppers->runSpeedToPosition(); //Blockling...
	Steppers->setPositions();
}

void Boby::arriere(long pas){
	Steppers->moveTo(pas, -pas);
	Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();
}

void Boby::gauche(long pas){
	Steppers->moveTo(-pas, -pas);
	Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();

}

void Boby::droite(long pas){
	Steppers->moveTo(pas, pas);
	Steppers->runSpeedToPosition();//Blockling...
	Steppers->setPositions();
}


//Battery Power save !!!!
void Boby::stop(long temps){
	if(temps > 40){
		delay(20);
		Steppers->disable();
		delay(temps-40);
		Steppers->enable();
		delay(20);
	}else{
		delay(temps);
	}
}

//Battery Power save !!!!
void Boby::stop(){
	Steppers->disable();
}

void Boby::tournerGauche(long angleDegree){
	gauche(long((angleDegree * DEG_TO_RAD * _radToStep)));
}

void Boby::tournerDroite(long angleDegree){
	droite(long((angleDegree * DEG_TO_RAD *_radToStep)));
}

void Boby::avancer(long distanceMillimeter){
	turnGoDegree(0, distanceMillimeter);
}

void Boby::reculer(long distanceMillimeter){
	turnGoDegree(0, -distanceMillimeter);
}

void Boby::polygone(unsigned int nbrCote, unsigned int longueur){
	if (nbrCote >=3)
	{
		float polyAngle = 360 / nbrCote;
		turnGoDegree(0,longueur);
		for (unsigned int i=1 ; i<nbrCote ; i++)
		{
			turnGoDegree(polyAngle,longueur);
		}
		turnGoDegree(polyAngle,0);
	}
}

void Boby::rectangle(unsigned int largeur, unsigned int longueur){
	turnGoDegree(0,largeur);
	turnGoDegree(90,longueur);
	turnGoDegree(90,largeur);
	turnGoDegree(90,longueur);
	turnGoDegree(90,0);
}

void Boby::carre(unsigned int longueur){
	rectangle(longueur,longueur);
}

void Boby::cercle(unsigned int diametre){
	float angleCercle = 11 * DEG_TO_RAD ; // Passage en radians
	unsigned int corde = (diametre * angleCercle )/2 ;
	polygone(32,corde * 1.25); // 33=arrondi de 360/11
}

void Boby::arc( float rayon,float angle){
	int pasD, pasG;
	if(angle > 0){
		pasD = ((rayon - _deltaArc) * angle*DEG_TO_RAD) * (_mmToStep/10);
		pasG = ((rayon + _deltaArc) * angle*DEG_TO_RAD) * (_mmToStep/10);
	}else{
		pasG = ((rayon - _deltaArc) * angle*DEG_TO_RAD) * (_mmToStep/10);
		pasD = ((rayon + _deltaArc) * angle*DEG_TO_RAD) * (_mmToStep/10);
	}
	Steppers->moveTo(pasD, pasG);
}



float Boby::distance_Milieu(){
  pinMode(pinUltraTriggerMilieu, OUTPUT);
  digitalWrite(pinUltraTriggerMilieu, LOW);
  delayMicroseconds(5);
  digitalWrite(pinUltraTriggerMilieu, HIGH);
  delayMicroseconds(7);
  digitalWrite(pinUltraTriggerMilieu, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pinUltraEchoMilieu, INPUT);
  float duration = pulseIn(pinUltraEchoMilieu, HIGH);
 
  // Convert the time into a distance
  return (duration/2) / 2.91; // distance in mm
}

float Boby::distance_Droite(){
  pinMode(pinUltraTriggerDroite, OUTPUT);
  digitalWrite(pinUltraTriggerDroite, LOW);
  delayMicroseconds(5);
  digitalWrite(pinUltraTriggerDroite, HIGH);
  delayMicroseconds(7);
  digitalWrite(pinUltraTriggerDroite, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pinUltraEchoDroite, INPUT);
  float duration = pulseIn(pinUltraEchoDroite, HIGH);
 
  // Convert the time into a distance
  return (duration/2) / 2.91; // distance in mm
}

float Boby::distance_Gauche(){
  pinMode(pinUltraTriggerGauche ,OUTPUT);
  digitalWrite(pinUltraTriggerGauche, LOW);
  delayMicroseconds(5);
  digitalWrite(pinUltraTriggerGauche, HIGH);
  delayMicroseconds(7);
  digitalWrite(pinUltraTriggerGauche, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pinUltraEchoGauche, INPUT);
  float duration = pulseIn(pinUltraEchoGauche, HIGH);
 
  // Convert the time into a distance
  return (duration/2) / 2.91; // distance in mm
}