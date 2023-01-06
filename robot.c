#pragma config(Sensor, S1, c1,   sensorEV3_Color)
#pragma config(Sensor, S2, c2,  sensorEV3_Color)
#pragma config(Sensor, S3, c3,  sensorEV3_Color)
#pragma config(Motor,  motorB, lm, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rm, tmotorEV3_Large, PIDControl, encoder)
#define DIFF 10

int nMotorSpeedSetting = 25;
float P=0.3, I=0.00001, D=5.7, e=0.0, s=0.0,pre;
int gray[3],dark[3],bright[3],TURN[3], cnt=0, gcnt=0;
bool turnFlag = false, L=false, R=false, B=false, Gflag=false;

void scanLine()
{
	displayBigTextLine(1, "scan dark areal!");
	displayBigTextLine(4, "& push up button");
	while(getButtonPress(1) == 0) sleep(10); playTone(440, 20);
	dark[0] = getColorReflected(c1);
	dark[1] = getColorReflected(c2);
	dark[2] = getColorReflected(c3);
	sleep(1000);
	displayBigTextLine(1, "scan dark areal!");
	displayBigTextLine(4, "& push up button");
	while(getButtonPress(1) == 0) sleep(10); playTone(440, 20);
	bright[0] = getColorReflected(c1);
	bright[1] = getColorReflected(c2);
	bright[2] = getColorReflected(c3);
	sleep(1000);
	displayBigTextLine(1, "%d %d %d", bright[0], bright[1], bright[2]);
	displayBigTextLine(4, "%d %d %d", dark[0], dark[1], dark[2]);
	for(int i=0; i<3;++i) gray[i] = (dark[i]+bright[i])/2;
	displayBigTextLine(7, "%d %d %d", gray[0], gray[1], gray[2]);
	displayBigTextLine(10, "push up to start");
}

void go()
{
	pre =e; e=getColorReflected(c2) - gray; s+=e; pre=e-pre;
	setMotorSpeed(lm, nMotorSpeedSetting + round(e*P+s*I+pre*D);
	setMotorSpeed(rm, nMotorSpeedSetting - round(e*P+s*I+pre*D);
}

void isCross()
{
	if( getColorReflected(c1) < gray[0] || getColorReflected(c3) <gray[2])
	{
		setMotorSpeed(lm,20);
		setMotorSpeed(rm,20);
		sleep(160);
		if( getColorReflected(c1) < gray[0]) L=true;
		else if(L==true)
		{
			turnFlag = true;
			TURN[0]=1;
		}
		if(getColorReflected(c3) < gray[2]) R=true;
		else if(R ==true)
		{
			turnFlag = true;
			TURN[1]=1;
		}
		if(getColorReflected(c1) <= gray[0] && getColorReflected(c3) <= gray[2] && getColorReflected(c2) <= gray[1]) gcnt++;
		else gcnt = 0;
		if(gcnt>5) Gflag = true;
	}
	if(getColorReflected(c2) > gray[1] * 7/5) cnt++;
	else cnt=0;
	if(cnt > 450)
	{
		B=true;
		turnFlag = true;
		TURN[2]= 1;
	}
}

void turnLeft()
{
	eraseDisplay();
	if(L==true) displayBigTextLine(11,"turnleft");
	while(getColorReflected(c1)>gray[0])
	{
		setMotorSpeed(lm, -nMotorSpeedSetting*6/10);
		setMotorSpeed(rm, nMotorSpeedSetting*6/10);
	}
	while(getColorReflected(c2)>gray[1])
	{
		setMotorSpeed(lm, -nMotorSpeedSetting*6/10);
		setMotorSpeed(rm, nMotorSpeedSetting*6/10);
	}
	while(getColorReflected(c1)<=gray[1])
	{
		setMotorSpeed(lm, -nMotorSpeedSetting*6/10);
		setMotorSpeed(rm, nMotorSpeedSetting*6/10);
	}
	setMotorSpeed(lm, 0);
	setMotorSpeed(rm, 0);
}

void turnRight()
{
	while(getColorReflected(c2)<=gray[1])
	{
		setMotorSpeed(lm, nMotorSpeedSetting*6/10);
		setMotorSpeed(rm, -nMotorSpeedSetting*6/10);
	}
	while(getColorReflected(c2)>gray[1])
	{
		setMotorSpeed(lm, nMotorSpeedSetting*6/10);
		setMotorSpeed(rm, -nMotorSpeedSetting*6/10);
	}
	setMotorSpeed(lm, 0);
	setMotorSpeed(rm, 0);
}

void stopMotor()
{
	eraseDisplay();
	setMotorSpeed(lm,0);
	setMotorSpeed(rm,0);
	if( L == true) displayBigTextLine(1, "Right");
	if( R == true) displayBigTextLine(4, "Left");
	if( B == true) displayBigTextLine(7, "Back");
}

void statusReset()
{
	turnFlag = L = R = B = Gflag = false;
	cnt = gcnt = TURN[0] = TURN[1] = TURN[2] = 0;
}

task main()
{
	scanLine();
	while(getButtonPress(1) == 0) sleep(10);
	setMotorSpeed(lm, nMotorSpeedSetting);
	setMotorSpeed(rm, nMotorSpeedSetting);
	sleep(1000);
	while(true)
	{
		go();
		isCross();
		if(turnFlag)
		{
			setMotorSpeed(lm, 25);
			setMotorSpeed(rm, 20);
			sleep(530);
			stopMotor();
			if(TURN[0] == 1) turnLeft();
			else if (TURN[2] == 1)
			{
				setMotorSpeed(lm,-32);
				setMotorSpeed(rm,-13);
				sleep(630);
				turnLeft();
			}
			else if(TURN[1] == 1 && getColorReflected(c2) > gray[1]) turnRight();
			statusReset();
		}
		if(Gflag)
		{
			statusReset();
			break;
		}
	}
	playTone(240,20); sleep(200);
	stopMotor();
	while(getButtonPress(1) == 0) sleep(10);
}
