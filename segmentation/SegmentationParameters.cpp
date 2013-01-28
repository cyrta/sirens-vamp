/*
	Copyright 2009 Arizona State University
	
	This file is part of Sirens.
	
	Sirens is free software: you can redistribute it and/or modify it under the terms 
	of the GNU Lesser General Public License as  published by the Free Software 
	Foundation, either version 3 of the License, or (at your option) any later version.
	
	Sirens is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
	PURPOSE.  See the GNU General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License along
	with Sirens. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SegmentationParameters.h"

namespace Sirens {
	SegmentationParameters::SegmentationParameters() {
		xInit[0] = 0;
		xInit[1] = 0;
		
		pInit[0][0] = 1;
		pInit[0][1] = 0;
		pInit[1][0] = 0;
		pInit[1][1] = 1;
		
		initialized = false;
	}
	
	SegmentationParameters::~SegmentationParameters() {
	}

	void SegmentationParameters::setPLagPlus(double value) {
		pLagPlus = value;
	}

	void SegmentationParameters::setPLagMinus(double value) {
		pLagMinus = value;
	}

	double SegmentationParameters::getPLagPlus() {
		return pLagPlus;
	}

	double SegmentationParameters::getPLagMinus() {
		return pLagMinus;
	}

	void SegmentationParameters::setAlpha(double value) {
		alpha = value;
	}

	double SegmentationParameters::getAlpha() {
		return alpha;
	}
	
	void SegmentationParameters::setR(double value) {
		r = value;
	}
	
	void SegmentationParameters::setCStayOff(double value) {
		cStayOff = value;
	}
	
	void SegmentationParameters::setCStayOn(double value) {
		cStayOn = value;
	}
	
	void SegmentationParameters::setCTurnOn(double value) {
		cTurnOn = value;
	}
	
	void SegmentationParameters::setCTurningOn(double value) {
		cTurningOn = value;
	}
	
	void SegmentationParameters::setCTurnOff(double value) {
		cTurnOff = value;
	}
	
	void SegmentationParameters::setCNewSegment(double value) {
		cNewSegment = value;
	}
	
	double SegmentationParameters::getR() {
		return r;
	}
	
	double SegmentationParameters::getCStayOff() {
		return cStayOff;
	}
	
	double SegmentationParameters::getCStayOn() {
		return cStayOn;
	}
	
	double SegmentationParameters::getCTurnOn() {
		return cTurnOn;
	}
	
	double SegmentationParameters::getCTurnOff() {
		return cTurnOff;
	}
	
	double SegmentationParameters::getCTurningOn() {
		return cTurningOn;
	}
	
	double SegmentationParameters::getCNewSegment() {
		return cNewSegment;
	}
	
	void SegmentationParameters::createFusionLogic() {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					for (int l = 0; l < 3; l++)
						fusionLogic[i][j][k][l] = 0;
				}
			}
		}	
		
		fusionLogic[0][0][0][0] = 1;
	
		fusionLogic[0][0][1][0] = 1 - pLagMinus;
		fusionLogic[0][0][1][2] = pLagMinus;

		fusionLogic[0][0][2][0] = 1 - pLagMinus;
		fusionLogic[0][0][2][2] = pLagMinus;

		fusionLogic[0][1][0][0] = pLagPlus;
		fusionLogic[0][1][0][1] = 1 - pLagPlus;
	
		fusionLogic[0][1][1][0] = pLagPlus - pLagPlus * pLagMinus;
		fusionLogic[0][1][1][1] = 1 - pLagPlus;
		fusionLogic[0][1][1][2] = pLagPlus * pLagMinus;

		fusionLogic[0][1][2][0] = pLagPlus - pLagPlus * pLagMinus;
		fusionLogic[0][1][2][1] = 1 - pLagPlus;
		fusionLogic[0][1][2][2] = pLagPlus * pLagMinus;

		fusionLogic[1][2][0][0] = pLagPlus;
		fusionLogic[1][2][0][1] = 1 - pLagPlus;

		fusionLogic[1][2][1][2] = 1;

		fusionLogic[1][2][2][0] = pLagPlus - pLagPlus * pLagMinus;
		fusionLogic[1][2][2][1] = 1 - pLagPlus;
		fusionLogic[1][2][2][2] = pLagPlus * pLagMinus;

		fusionLogic[2][0][0][0] = 1;
	
		fusionLogic[2][0][1][0] = 1 - pLagMinus;
		fusionLogic[2][0][1][2] = pLagMinus;

		fusionLogic[2][0][2][0] = 1 - pLagMinus;
		fusionLogic[2][0][2][2] = pLagMinus;

		fusionLogic[2][1][0][0] = pLagPlus;
		fusionLogic[2][1][0][1] = 1 - pLagPlus;

		fusionLogic[2][1][1][0] = pLagPlus - pLagPlus * pLagMinus;
		fusionLogic[2][1][1][1] = 1 - pLagPlus;
		fusionLogic[2][1][1][2] = pLagPlus * pLagMinus;

		fusionLogic[2][1][2][0] = pLagPlus - pLagPlus * pLagMinus;
		fusionLogic[2][1][2][1] = 1 - pLagPlus;
		fusionLogic[2][1][2][2] = pLagPlus * pLagMinus;

		fusionLogic[2][2][0][0] = pLagPlus;
		fusionLogic[2][2][0][1] = 1 - pLagPlus;

		fusionLogic[2][2][1][2] = 1;

		fusionLogic[2][2][2][2] = 1;
	}
	
	/*
	 * q(state[t - 1], state[t]) is the process variance for state transitions in the Kalman Filter.
	 * Some values are irrelevant, because the prior mode transition probabilities do not allow for them
	 * (see Segmenter::createModeLogic.)
	 * 
	 * In general, change (turnOn/TurnOff/newSegment) variances should be about equal and should be much larger
	 * than those for no change (stayOff/StayOn)
	 */
	void SegmentationParameters::createQTable() {
		q[0][0] = cStayOff;		// off -> off
		q[0][1] = cTurnOn;		// off -> onset
		q[0][2] = 0;			// off -> on is impossible (irrelevant)
		q[1][0] = 0;			// onset -> off is impossible (irrelevant)
		q[1][1] = 0;			// onset -> onset is impossible (irrelevant)
		q[1][2] = 99999999999;	// onset -> on always happens.
		q[2][0] = cTurnOff;		// on -> off
		q[2][1] = cNewSegment;	// on -> onset (overlapping segments)
		q[2][2] = cStayOn;		// on -> on
	}
	
	void SegmentationParameters::initialize() {
		if (!initialized) {
			createFusionLogic();
			createQTable();
			
			initialized = true;
		}
	}
}
