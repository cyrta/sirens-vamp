#ifndef _SPECTRALSPARSITY_H_
#define _SPECTRALSPARSITY_H_

#include <vamp-sdk/Plugin.h>
using std::string;

class SpectralSparsity : public Vamp::Plugin {
public:
	SpectralSparsity(float inputSampleRate);
	virtual ~SpectralSparsity();
	
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
};

#endif
