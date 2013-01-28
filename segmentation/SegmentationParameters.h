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

#ifndef __SEGMENTATIONPARAMETERS_H__
#define __SEGMENTATIONPARAMETERS_H__

#include <vector>
using namespace std;

/*
	What the parameters mean.
	
	First, read through Segmenter.h for information on how the segmentation algorithm works.
	Each feature has 12 parameters that need to be tuned for proper segmentation. Fortunately,
	several of these parameters are more or less redundant and can usually be reduced to a smaller
	set.
	
	Normalization
		1. minFeatureValue - this is smallest value you can expect to see in the feature. For example,
			for loudness, this might be ~-72dB. 
		2. maxFeatureValue - the largest value you can expect to see in the feature. All feature values
			are linearly interpolated between minFeatureValue and maxFeatureValue before segmentation.

	Prior probabilities:
		3. pLagPlus - poisson probability that onsets will lag behind the global mode. In other words,
			if a certain feature reacts more slowly on onsets, you can expect its onsets to occur
			~(1 / pLagPlus) frames after the global mode onset.
		4. pLagMinus - similar to above, but for ON->OFF transitions.
		
	Lowpass filter:
		5. alpha - a lowpass filter coefficient. Segmentation onsets are not always instantaneous,
			but occur over time, so the observed feature trajectory is essentially preprocessed with a
			LPF. For example, for loudness, we've found that values ~0.05 work best.
	
	Variances
		Measurement:
			6. r - measurement noise variance. This is how much variance of the signal is due to noise.
				Tune this according to how much noise you expect in the recording (generally larger for 
				outdoor recordings, very small for highly-edited recordings, such as in commercial media)
				This is one of the most important parameters. Large values will result in over-active
				onsets. Generally values below 0.01 are good.
				
		No change:
			7. cStayOff - maximum variance the signal can undergo for the OFF->OFF transition.
			8. cStayOn - maximum variance the signal can undergo for the ON->ON transition. Usually the same
				as cStayOff, but some features, such as spectral sparsity, oftentimes exhibit more variance
				when on than off. These values are usually fairly low (e.g. 0.0001).
			
			If these values are too high, generally there will be no change in the mode sequence. If they are
			too low, there will be too many changes. If one is higher than the other, it is generally more
			likely to occur. (e.g. cStayOn > cStayOff will usually result in longer segments with shorter gaps
			between them.)
		
		Change:
			9. cTurnOn - maximum variance the signal can undergo for the OFF->ONSET transition.
			10. cTurningOn - maximum variance the signal can undergo for the ONSET->ON transition.
			11. cTurnOff - maximum variance the signal can undergo for the ON->OFF transition.
			12. cNewSegment - maximum variance the signal can undergo for the ON->ONSET transition.
			
			These values can usually be the same, but they should be several orders of magnitude higher
			than the "no change" variances. e.g. 0.9 compared to 0.0001.
			
			If there are too many onsets, you may try lowering this parameter or increasing measurement noise.
*/

namespace Sirens {
	class SegmentationParameters {
	protected:
		bool initialized;
		
		// Feature normalization.
		double minFeatureValue;
		double maxFeatureValue;
	
		// Each feature lags behind the primary segemntation mode.
		double pLagPlus;
		double pLagMinus;
	
		// Low-pass filter on feature value.
		double alpha;
		
		// Kalman filter parameters.
		double r;
		
		// Prior probabilities for features' modes--initialize xInit/pInit.
		double cStayOff;
		double cStayOn;
		double cTurnOn;
		double cTurningOn;
		double cTurnOff;
		double cNewSegment;
				
		void createQTable();		
		void createFusionLogic();
		
	public:
		SegmentationParameters();
		~SegmentationParameters();
		
		// Attributes.
		void setMinFeatureValue(double value);
		void setMaxFeatureValue(double value);
		double getMinFeatureValue();
		double getMaxFeatureValue();
	
		void setPLagPlus(double value);
		void setPLagMinus(double value);
		double getPLagPlus();
		double getPLagMinus();
	
		void setAlpha(double value);
		double getAlpha();
		
		void setR(double value);
		void setCStayOff(double value);
		void setCStayOn(double value);
		void setCTurnOn(double value);
		void setCTurnOff(double value);
		void setCTurningOn(double value);
		void setCNewSegment(double value);
		double getR();
		double getCStayOff();
		double getCStayOn();
		double getCTurnOn();
		double getCTurnOff();
		double getCTurningOn();
		double getCNewSegment();
		
		double xInit[2];
		double pInit[2][2];
		double q[3][3];
		double fusionLogic[3][3][3][3];
		
		// Operations.
		void initialize();
	};
}

#endif
