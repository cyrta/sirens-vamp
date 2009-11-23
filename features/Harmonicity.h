#ifndef _HARMONICITY_H_
#define _HARMONICITY_H_

#include <vamp-sdk/Plugin.h>
using std::string;

struct Peak {
	double amplitude;
	double frequency;
};

struct HarmonicityDoubleVector {
	double* values;
	unsigned int size;
};

struct HarmonicityIntVector {
	int* values;
	unsigned int size;
};

struct HarmonicityPeakVector {
	Peak* values;
	unsigned int size;
};

class Harmonicity : public Vamp::Plugin {
public:
	Harmonicity(float inputSampleRate);
	virtual ~Harmonicity();
	
	string getIdentifier() const;
	string getName() const;
	string getDescription() const;
	string getMaker() const;
	int getPluginVersion() const;
	string getCopyright() const;
	
	InputDomain getInputDomain() const;
	size_t getPreferredBlockSize() const;
	size_t getPreferredStepSize() const;
	size_t getMinChannelCount() const;
	size_t getMaxChannelCount() const;
	
	ParameterList getParameterDescriptors() const;
	float getParameter(string identifier) const;
	void setParameter(string identifier, float value);
	
	OutputList getOutputDescriptors() const;
	
	bool initialise(size_t channels, size_t stepSize, size_t blockSize);
	void reset();
	
	FeatureSet process(const float *const *inputBuffers, Vamp::RealTime timestamp);
	
	FeatureSet getRemainingFeatures();
	
protected:
	size_t m_blockSize;
	float m_sampleRate;
	
	float absThreshold, threshold, kVar;
	float fftMax, pitch, filterOldValue, lpfCoefficient, harmonicity;
	unsigned int maxPeaks, nMax, fftSize, searchRegionLength, searchRegionLength2;
	unsigned int minFrequencyIndex, maxFrequencyIndex;
	
	HarmonicityIntVector rawIndices, accIndices;
	HarmonicityDoubleVector rawMagnitudes;
	HarmonicityPeakVector peakList;
	
	Peak tempPeak;
	
	void pickPeaks(const float *const *inputBuffers);
	float goldsteinGaussian(float x1, float x2, int n1, int n2, float f0, float k);
	void goldsteinCalc();
	void resetVectors();
	void freeMemory();
};

bool SortAmplitudes(const Peak& peak1, const Peak& peak2);
bool SortFrequencies(const Peak& peak1, const Peak& peak2);

#endif
