/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include <time.h>
#include <iostream>
#include <iomanip>
#include "camera.h"
#include "controllers.h"

pinBus CELLBUS, EXBUS, ESC, VSensor[4], TSensor[4];
MCU* BMS_MCU = new MCU(&CELLBUS, &EXBUS, &ESC);
cellUnit* cellUnits[4];
float VData[4][3000] = { 0 }, TData[4][3000] = { 0 };
float initV = 4200;
float initT = 27;
float amperage = 0;
camera*view;
long tick = 0;
bool preCharging = false;
int ticksLeftForCharge =120;

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	sceneroot(0, 0, 0),
	currView(new camera(gfx, sceneroot))
{
	view = new camera(gfx, vec(1, 0, 0));
	CELLBUS.pinState = 0;
	EXBUS.pinState = 0;
	ESC.pinState = 0;
	for (int c = 0; c < 4; c++) {
		cellUnits[c] = new cellUnit(&CELLBUS, &ESC, &(VSensor[c]), &(TSensor[c]), 4 + c);
		VData[c][0] = initV;
		TData[c][0] = initT;
		VSensor[c].pinState = *reinterpret_cast<int*>(&initV);
		TSensor[c].pinState = *reinterpret_cast<int*>(&initT);
	}


}


void Game::Go()
{
	srand(time(0));
	gfx.BeginFrame();	
	srand(time(NULL));
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();

}

void Game::UpdateModel(){
	BMS_MCU->doTick();
	
	for (int i = 0; i < 4; i++) {
		float nextV = VSensor[i].data() - (0.019*amperage/pow(cos((VSensor[i].data() )/850 - 3.1415 * 3.25),2)) - (0.1-amperage/200.0) * (float(rand()) / RAND_MAX);
		nextV = max(nextV, float(50.0));
		VSensor[i].pinState = *reinterpret_cast<int*>(&(nextV));
		
		float nextT = TSensor[i].data() * (1.00002 + (abs(amperage)*VSensor[i].data()/1000.0)/300000.0) + 0.001*(initT- TSensor[i].data());
		TSensor[i].pinState = *reinterpret_cast<int*>(&(nextT));

		cellUnits[i]->doTick();
	}
	if (ESC.pinState && !BMS_MCU->chargerConnected && EXBUS.pinState) {
		amperage = 10;
	}
	if ( (ESC.pinState == 0 || !BMS_MCU->HV_power )&& amperage && !preCharging) {
		amperage = 0;
		ticksLeftForCharge = 120;
	}

	if (wnd.kbd.KeyIsPressed('E')) {
		ESC.pinState = 0;
		BMS_MCU->HV_power = false;
		amperage = 0;
		ticksLeftForCharge = 120;
	}

	if (preCharging || wnd.kbd.KeyIsPressed('S') && !ESC.pinState && BMS_MCU->LV_power && !BMS_MCU->LEDActive) {
		if (ticksLeftForCharge) {
			ESC.pinState = 1111;
			ticksLeftForCharge--;
			amperage = 3;
			preCharging = true;
		}
		else {
			preCharging = false;
			ESC.pinState = 0b1111;
			BMS_MCU->HV_power = true;
			BMS_MCU->HVOn();
			amperage = 15;
		}
	}
	if (wnd.kbd.KeyIsPressed('C') && !(BMS_MCU->LV_power) ){
		BMS_MCU->powerOn();
		for (int i = 0; i < 4; i++)cellUnits[i]->powerOn();
	}
	if (wnd.kbd.KeyIsPressed('X') && (BMS_MCU->LV_power)) {
		BMS_MCU->LV_power=0;
		BMS_MCU->LEDActive = 0;
		amperage = 0;
		ESC.pinState = 0;
		for (int i = 0; i < 4; i++)cellUnits[i]->LV_power=0;

	}
	if (wnd.kbd.KeyIsPressed('R') && !(BMS_MCU->HV_power)) {
		BMS_MCU->powerOn();
		for (int i = 0; i < 4; i++)cellUnits[i]->powerOn();
		BMS_MCU->chargerConnected = true;
		BMS_MCU->HV_power = true;
		ESC.pinState = 0b1111;
		amperage = -15;
	}
}

void Game::ComposeFrame()
{
	tick++;
	for (int i = 0; i < 4; i++) {
		VData[i][tick % 3000] = VSensor[i].data();
		TData[i][tick % 3000] = TSensor[i].data();
	}

	Color cellCol[4] = { Colors::Yellow,Colors::Red,Colors::Magenta,Colors::Blue };
	view->putStr("TIME " + to_string(tick), 250, 20, 0.5);
	
	//Voltage Graph Const
	view->CdrawLine(50, 50, 50, 300, Colors::White);
	view->CdrawLine(350, 300, 50, 300, Colors::White);
	view->putChar('V', 10, 150);
	for (int i = 0; i < 4; i++) {
		view->putStr(to_string(int(VData[i][tick%3000])), 200 + i * 50, 320, 0.2,cellCol[i]);
		view->graphArray(VData[i], 3000, 50, 300, 350, 50, 2000, 5000, cellCol[i]);
	}
	
	//Temp Graph Const
	view->CdrawLine(50, 50+350, 50, 300 + 350, Colors::White);
	view->CdrawLine(350, 300 + 350, 50, 300 + 350, Colors::White);
	view->putChar('T', 10, 500);

	for (int i = 0; i < 4; i++) {
		view->putStr(to_string(int(TData[i][tick % 3000]))+"."+to_string(int(TData[i][tick % 3000] * 10) % 10), 220 + i * 40, 690, 0.2, cellCol[i]);
		view->graphArray(TData[i], 3000, 50, 650, 350, 400, 0, 80, cellCol[i]);
	}
	
	//ESC Light
	view->putStr("ESC", 680, 20,1, ESC.pinState ? Colors::Green : Colors::Red);

	//HV Light
	view->putStr("HV", 680, 60,1, BMS_MCU->HV_power? Colors::Green : Colors::Red);

	//LV Light
	view->putStr("LV", 680, 100, 1, BMS_MCU->LV_power ? Colors::Green : Colors::Red);
	//Cell Lights
	view->putStr("CELLS", 680, 150,0.8,Colors::Gray);
	for (int i=0;i<4;i++)
		view->putChar('1'+i, 700+40*i, 190, !cellUnits[i]->LV_power ? Colors::Red : cellUnits[i]->criticalCond?Colors::Magenta:Colors::Green);

	//BMS LED
	view->putStr("BMS FLT", 680, 240, 1, BMS_MCU->LEDActive ? Colors::Red : Colors::Black);

	//Bus States
	view->putStr("CELLBUS  "+to_string(CELLBUS.data()), 600, 350, 0.5);
	view->putStr("EXBUS    "+to_string(EXBUS.pins()), 600, 370, 0.5);
	view->putStr("ESC WIRE "+to_string(ESC.pinState), 600, 390, 0.5);

	//PreCharge
	if(preCharging) view->putStr("PRECHARGE " + to_string(100 * (1 - ticksLeftForCharge/120.0 ) ), 600, 410, 0.5);
	view->putStr("AMPS " + to_string(amperage), 600, 430, 0.5);

	//BMS RECHARGE
	view->putStr("RECHARGE", 600, 460, 1, BMS_MCU->chargerConnected ? Colors::Green : Colors::Black);

	//INFO
	view->putStr("C LV ON - S START - E STOP - R CHARGE - X RESET", 20, 730, 0.6);
}
