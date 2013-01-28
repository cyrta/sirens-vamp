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

#ifndef __SEGMENTER_H__
#define __SEGMENTER_H__

#include "../Feature.h"
#include "../FeatureSet.h"

#include <vector>
using namespace std;

/*
	How segmentation works:
	Segmentation is performed by deciding a "global mode," which can be in one of 3 states:
		1: ON, 2: OFF, or 3: ONSET
		The following mode transitions are possible:
		ON -> ON (mode stays on)
		ON -> OFF (mode turns off)
		ON -> ONSET (overlapping segment is triggered while current one is on. They will
			both end when the next OFF occurs.)
		OFF -> OFF (mode stays off)
		OFF -> ONSET (mode turns on)
		ONSET -> ON (segment is turning on--onsets are ALWAYS proceeded by ON modes.)
		
	Additionally, there are modes for each feature that can take the same values.
	This means, that for N features, there are 3^(N + 1) possible states.
	
	Every frame, a Kalman filter is performed for every possible state transition (there 
	are #states^2 of these) for every feature. This corresponds to N * 3^(2(N + 1)) filters
	evaluated each frame. 
	
	Each Kalman filter attempts to predict the value of the input feature trajectory given
	a certain known measurement noise (SegmentationParameters::getR) and a covariance, which
	varies depending on which state transition is taking place. For example, if we want to
	predict the feature trajectory given that it is in the "ON" state and it will remain in
	the "ON" state the next frame, we will use SegmentationParameters::getCSTayOn for the
	process variance.
	
	From this estimation, we get a cost, which is related to the error in estimating the
	feature trajectory. So, for each Kalman filter, we have a cost.
	
	The Viterbi algorithm is used to compute the optimal state sequence. Essentially,
	one can think of the Viterbi algorithm as a shortest path algorithm on a graph: for each
	frame, you have one node per possible state (3^(N + 1) nodes). These nodes are fully
	connected to the previous frame's nodes. Each edge has a weight, corresponding to the
	cost of the transition, calculated as the sum of costs for that transition (from the Kalman)
	filter) across all features, plus additional cost corresponding to the prior probability of
	that transition (for example, some transitions are never allowed to happen, such as ones that
	result in the global mode transitioning from OFFSET to OFF, as OFFSET modes are always
	proceeded by ON modes.) Viterbi then finds the shortest path through the network, which gives
	you the most likely mode sequence.
	
	For more information about the algorithm implemented here, see:
	G. Wichern, H. Thornburg, B. Mechtley, A. Fink, A. Spanias, and K. Tu, "Robust multi-feature
	segmentation and indexing for natural sound environments," in Proc. of IEEE/EURASIP International
	Workshop on Content Based Multimedia Indexing (CBMI), Bordeaux, France, July 2007.
	
	Additional information about switching state-space models (of which this is one) can be found in:
	Z. Ghahramani and G. E. Hinton, Variational learning for switching-state-space models, Neural
	Computation 12(4), 963-996, 1998.
	
	For information on how to determine the parameters used in this algorithm, please see
	SegmentationParameters.h
*/

namespace Sirens {
	class ViterbiDistribution {
	public:
		double mean[2];
		double covariance[2][2];
		double cost;
		
		ViterbiDistribution() {
			mean[0] = 0;
			mean[1] = 0;
			cost = 0;
			
			covariance[0][0] = 1;
			covariance[0][1] = 0;
			covariance[1][0] = 0;
			covariance[1][1] = 1;
		}
	};
	
	class Segmenter {
	private:
		FeatureSet* featureSet;
		vector<Feature*> features;
		
		// Initialization.
		bool initialized;
		int frames;
		
		double pNew, pOff;								// Prior poisson probabilities for the global mode.
		vector<double> y;								// Feature vector for the current frame.
		
		vector<vector<double> > modeTransitions;		// Global mode transition probabilities. (3x3)
		vector<vector<double> > probabilityMatrix;		// Log-scaled prior transition probabilities for all state combinations.
		
		vector<vector<int> > modeMatrix;				// Modes of every feature (and global mode) for each state.
				
		// Viterbi.
		vector<vector<double> > costs;					// Costs of every possible state transition.
		vector<vector<int> > psi;						// Stored state sequences.
		vector<double> oldCosts;						// Minimum cost list for previous frame.
		
		// Distributions for Viterbi.
		vector<vector<ViterbiDistribution> > maxDistributions;				// Distributions that correspond to minimum cost transitions.
		vector<vector<vector<ViterbiDistribution> > > newDistributions;		// Distributions for every possible transition and every feature.
		
		// Helpers for indexing large matrices.
		vector<int> getFeatureModes(int state);		// Return mode of every feature (plus global mode) for a particular state.
		int getStateCount();						// How many possible states are in the system (3 ^ (features + 1))
		
		// Algorithms.
		double KalmanLPF(double y, double p[2][2], double x[2], double r, double q, double alpha);
		void viterbi(int frame);
		
		vector<int> modes;
		
	public:	
		Segmenter(double p_new = 0, double p_old = 0);
		~Segmenter();
		
		// Features.
		void setFeatureSet(FeatureSet* feature_set);
		FeatureSet* getFeatureSet();
		
		// Attributes.
		void setPNew(double value);
		void setPOff(double value);
		void setDelay(int value);
		
		double getPNew();
		double getPOff();
		int getDelay();
		
		// Initialization.
		void createModeLogic();
		void createProbabilityTable();
		void initialize();				
		
		// Segmentation. This is what users call.
		void segment();
		
		// Retrieve results after segmentation.
		vector<vector<int> > getSegments();
		vector<int> getModes();
	};
}

#endif
