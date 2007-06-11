/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/**
 * @author Dmitriy S. Matveev, Viskov Nikolay 
 * @version $Revision$
 */
#include "Outline.h"

Outline::Outline(unsigned short pointsNumber, unsigned short commandNumber) {
	pointsCount = 0;
	commandsCount = 0;

	_points = new float[_pointsLength = pointsNumber];
	_commands = new unsigned char[_commandLenght = commandNumber];    

	/*for (commandsCount = 0 ; commandsCount < commandNumber; commandsCount ++) {
		this->_commands[commandsCount] = SEG_CLOSE;
	}

	commandsCount = 0;*/
}

Outline::~Outline() {
	delete[] _points;
	delete[] _commands;
}

void Outline::trim() {
    if (_commandLenght == commandsCount) {
		return;
	}

	//printf("_length = %u, commandsCount = %u\n", _commandLenght, commandsCount);

	unsigned char *commandMas = new unsigned char[commandsCount];
	float *pointsMas = new float[pointsCount];
	
    memcpy(commandMas, _commands, commandsCount);
    memcpy(pointsMas, _points, pointsCount * sizeof(float));

	delete[] _points;
	delete[] _commands;

	_points = pointsMas;
	_commands = commandMas;

    _commandLenght = commandsCount;
    _pointsLength = pointsCount;
}

void Outline::lineTo(float x, float y) {
	_points[pointsCount ++] = x;
	_points[pointsCount ++] = -y;
	_commands[commandsCount ++] = SEG_LINETO;
	//printf("SEG_LINETO ");
}

void Outline::moveTo(float x, float y) {
	_commands[commandsCount ++] = SEG_MOVETO;
	_points[pointsCount ++] = x;
	_points[pointsCount ++] = -y;
//	printf("SEG_MOVETO ");
}

void Outline::quadTo(float x1, float y1, float x2, float y2) {
	_points[pointsCount ++] = x1;
	_points[pointsCount ++] = -y1;
	_points[pointsCount ++] = x2;
	_points[pointsCount ++] = -y2;
	_commands[commandsCount ++] = SEG_QUADTO;
	//printf("SEG_QUADTO ");
}

void Outline::curveTo(float x1, float y1, float x2, float y2, float x3, float y3) {
	_points[pointsCount ++] = x1;
	_points[pointsCount ++] = -y1;
	_points[pointsCount ++] = x2;
	_points[pointsCount ++] = -y2;
	_points[pointsCount ++] = x3;
	_points[pointsCount ++] = -y3;
	_commands[commandsCount ++] = SEG_CUBICTO;
	//printf("SEG_CUBICTO ");
}

void Outline::closePath(void) {
	//if (_commands[commandsCount - 1] != SEG_CLOSE) {
		_commands[commandsCount ++] = SEG_CLOSE;
	//} 
        /*else {
		printf("two close path\n");
	}*/
	//printf("SEG_CLOSE\n");
}

unsigned short Outline::getPointsLength(void) {
	return _pointsLength;
}

unsigned short Outline::getCommandLength(void) {
    return _commandLenght;
}
