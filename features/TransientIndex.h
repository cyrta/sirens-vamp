#ifndef _TRANSIENTINDEX_H_
#define _TRANSIENTINDEX_H_

#include <vamp-sdk/Plugin.h>
using std::string;

class TransientIndex : public Vamp::Plugin {
public:
	TransientIndex(float inputSampleRate);
	virtual ~TransientIndex();
	
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
	void freeMemory();
	void resetFilterBank();
	float hz_to_mel(float hz);
	float mel_to_hz(float mel);
	
	size_t m_blockSize;
	float m_sampleRate;
	
	unsigned int mels, filters;
	
	float* mfccOld;
	float* mfccNew;
	float* dctMatrix;
	float* filterTemp;
	float* filterBank;
};

#endif
