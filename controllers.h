#include <iostream>
#include <thread>

struct pinBus {
	int pinState=0;

	int pins() {
		return pinState;
	};
	float data() {
		return *reinterpret_cast<float*>(&pinState);
	};
	void setPin(int sig) {
		pinState = sig;
	}
};

class microController {
public:
	pinBus* CellBus=NULL;
	pinBus* ExportBus = NULL;
	pinBus* ESC = NULL;
	bool LV_power = false;
	microController(){}

	void pinOut(int bus, int sig) {
		pinBus* temp;
		switch(bus){
		case 0:
			temp = CellBus;
			break;
		case 1:
			temp = ExportBus;
			break;
		default:
			temp = ESC;
			break;
		}
		temp->setPin(sig);
	}
	
};

class MCU : public microController {
public:
	bool LEDActive, HV_power, chargerConnected;
	bool critical[4] = { 0,0,0,0 };
	MCU(pinBus* bus1, pinBus* bus2, pinBus* esc) {
		LV_power = false;
		LEDActive = false;
		HV_power = false;
		chargerConnected = false;
		CellBus = bus1;
		ExportBus = bus2;
		ESC = esc;
	};
	void powerOn() {
		LV_power = true;
		nextOp = 0;
	}
	void HVOn() {
		if (!LV_power)return;
		HV_power = true;
	}
	void doTick() {
		if (HV_power && ESC->pinState == 0) {
			HV_power = false;
			chargerConnected = false;
		}
		switch (nextOp) {
		case 0:
			busCheck();
			break;
		case 1:
			irqSignal();
			break;
		case 2:
			readFlags();
			break;
		case 3:
			takeVData();
			break;
		case 4:
			takeTData();
			break;
		case 5:
			respondECU();
			break;
		case 6:
			checkCapacity();
		default:
			return;
		}
	}
private:
	pinBus TReading[4] = { 0 }, VReading[4] = { 0 };
	const int irq[4] = { 4, 5, 6, 7 };
	const int critFlag = 0b111000;
	const int capacity = 4300;
	int nextOp = -1;
	int REG1=0,REG2=0;
	
	void busCheck() {
		if (chargerConnected) nextOp = 6;
		if (!HV_power) {
			nextOp = 0;
			return;
		}
		nextOp = 1;
	}
	void irqSignal() {
		if (REG1 == 4) {
			REG1 = 0;
			nextOp = chargerConnected ? 6:5;
			return;
		}
		pinOut(0, irq[REG1]);
		REG1 += 1;
		nextOp = 2;
	}
	void readFlags() {
		if ( CellBus->pins() < 8) {
			REG2 += 1;
			if (REG2 == 10) eStop();
			return;
		}
		REG2 = 0;
		if (CellBus->pinState == critFlag) {
			critical[REG1] = 1;
			nextOp = 3;
			return;
		}
		nextOp = 1;
	}
	void takeVData() {
		VReading[REG1].pinState = CellBus->pins();
		nextOp = 4;
	}
	void takeTData() {
		TReading[REG1].pinState = CellBus->pins();
		nextOp = 1;
		pinOut(0, 0);
		
	}
	void respondECU() {
	
		if (REG2 == 0) {
			pinOut(1, 8);
			REG2 += 1;
			nextOp = 5;
			return;
		}
		int avgV = 0;
		int avgT = 0;
		int t=0;
		if (critical[0]+critical[1]+critical[2]+critical[3]) {
			for (int i = 0; i < 4; i++) {
				if (critical[i]) {
					if (! chargerConnected && VReading[i].data() <= 2400)
						eStop();
					avgV += VReading[i].data();
					avgT += TReading[i].data();
					t++;
					pinOut(1, avgV);
					pinOut(1, avgT);
				}
			}


		}
		REG2 = 0;
		nextOp = 0;
	}

	int sum(short b) {
		int s = 0;
		while (b){
			s += b % 2;
			b /= 2;
			pinOut(1, s);
		}
		return s;
	}
	void checkCapacity() {
		REG2 = 0;
		while (REG2 < 4) {
			if (VReading[REG2].data() >= capacity) {
				HV_power = false;
				chargerConnected = false;
				nextOp = 0;
				return;
			}
			REG2 += 1;
		}
		REG2 = 0;
		nextOp = 1;
	}
	void eStop() {
		chargerConnected = false;
		ESC->pinState=0;
		LEDActive = true;
		HV_power = false;
	}

};

class cellUnit : public microController {
public:
	bool criticalCond = false;
	cellUnit(pinBus* cb, pinBus* esc, pinBus* vs, pinBus* ts, int in) {
		VSensor = vs;
		TSensor = ts;
		CellBus = cb;
		ESC = esc;
		irqNum = in;
	}
	void powerOn() {
		if (LV_power)return;
		LV_power = true;
		nextOp = 0;
	}
	void doTick() {
		if (CellBus->pinState == irqNum) {
			if (rand() / RAND_MAX >= pow(TSensor->data()/80.0, 5) )return;
			updateCrit();
			pinOut(0, 0b1000 + criticalCond * 0b110000);
			if (criticalCond)nextOp = 3;
			else nextOp = 0;
		}
		if (ESC->pinState == 0)nextOp = -1;
		switch (nextOp) {
		case 0:
			maxCheck();
			break;
		case 1:
			sendV();
			break;
		case 2:
			sendT();
			break;
		case 3:
			wait(1);
			break;
		default:
			return;
		}
	}
private:
	const float VMAX = 4400;
	const float VMIN = 2400;
	const float OVCT = 4250;
	const float UVCT = 2700;
	const float TMAX =  60;
	const float TMIN = -20;
	const float HTT = 45;
	pinBus* VSensor;
	pinBus* TSensor;
	int irqNum;
	int nextOp = -1;
	int REG1 = 0, REG2 = 0;

	void maxCheck() {
		if (VSensor->data() >= VMAX || TSensor->data() >= TMAX || TSensor->data() <= TMIN) {
			ESC->pinState = 0;
			nextOp = -1;
		}
		else nextOp = 0;
	}
	void updateCrit() {
		maxCheck();
		criticalCond = VSensor->data() >= OVCT || VSensor->data() < UVCT || TSensor->data() > HTT;
	}
	void sendV() {
		pinOut(0, VSensor->pinState);
		nextOp = 2;
	}
	void sendT() {
		pinOut(0, TSensor->pinState);
		nextOp = 0;
	}
	void wait(int i) {
		nextOp = i;
	}
};