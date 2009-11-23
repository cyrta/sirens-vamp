#include "SpectralCentroid.h"

#include <cmath>
using namespace std;

SpectralCentroid::SpectralCentroid(float inputSampleRate) : Plugin(inputSampleRate) {
	m_sampleRate = inputSampleRate;
	
	barkUnits = NULL;
	barkWeights = NULL;
}

SpectralCentroid::~SpectralCentroid() {
	delete[] barkUnits;
	delete[] barkWeights;
}

string SpectralCentroid::getIdentifier() const {
	return "spectral-centroid";
}

string SpectralCentroid::getName() const {
	return "Spectral centroid";
}

string SpectralCentroid::getDescription() const {
	return "Calculates the Bark-weighted centroid of the spectrum.";
}

string SpectralCentroid::getMaker() const {
	return "Brandon Mechtley and Gordon Wichern";
}

int SpectralCentroid::getPluginVersion() const {
	return 1;
}

string SpectralCentroid::getCopyright() const {
	return "MIT";
}

SpectralCentroid::InputDomain SpectralCentroid::getInputDomain() const {
	return FrequencyDomain;
}

size_t SpectralCentroid::getPreferredBlockSize() const {
	return 0;
}

size_t SpectralCentroid::getPreferredStepSize() const {
	return 0;
}

size_t SpectralCentroid::getMinChannelCount() const {
	return 1;
}

size_t SpectralCentroid::getMaxChannelCount() const {
	return 1;
}

SpectralCentroid::ParameterList SpectralCentroid::getParameterDescriptors() const {
	ParameterList list;
	return list;
}

float SpectralCentroid::getParameter(string identifier) const {
	return 0;
}

void SpectralCentroid::setParameter(string identifier, float value)	 {
}

SpectralCentroid::OutputList SpectralCentroid::getOutputDescriptors() const {
	OutputList list;
	
	OutputDescriptor d;
	d.identifier = "spectral-centroid";
	d.name = "Spectral centroid";
	d.description = "Bark-weighted centroid of the frequency spectrum of the input signal.";
	d.unit = "barks";
	d.hasFixedBinCount = true;
	d.binCount = 1;
	d.hasKnownExtents = false;
	d.isQuantized = false;
	d.sampleType = OutputDescriptor::OneSamplePerStep;
	d.hasDuration = false;
	list.push_back(d);
	
	return list;
}

bool SpectralCentroid::initialise(size_t channels, size_t stepSize, size_t blockSize) {
	if (channels < getMinChannelCount() || channels > getMaxChannelCount())
		return false;
	else {
		m_blockSize = blockSize;
		
		barkUnits = new float[m_blockSize];
		barkWeights = new float[m_blockSize - 1];
		
		for (unsigned int i = 0; i < m_blockSize; i++)
			barkUnits[i] = hz_to_bark((m_sampleRate * i) / float(2 * (m_blockSize - 1)));
		
		for (unsigned int i = 0; i < m_blockSize - 1; i++)
			barkWeights[i] = barkUnits[i + 1] - barkUnits[i];
		
		return true;
	}
}

void SpectralCentroid::reset() {
}

SpectralCentroid::FeatureSet SpectralCentroid::process(const float *const *inputBuffers, Vamp::RealTime timestamp) {
	float sum = 0;
	
	float* weight_item = barkWeights;
	
	for (unsigned int i = 1; i < m_blockSize; i++) {
		float sample = inputBuffers[0][i];
		
		sum += sample * sample * *weight_item;
	}
	
	float centroid = 0;
	
	if (sum > 0) {
		weight_item = barkWeights;
		float* unit_item = barkUnits + 1;
	
		for (unsigned int i = 1; i < m_blockSize; i++) {
			float sample = inputBuffers[0][i];
		
			centroid += sample * sample * ((*weight_item) / sum) * *unit_item;
		
			unit_item ++; 
			weight_item ++;
		}
	}
	
	Feature f;
	f.hasTimestamp = false;
	f.values.push_back(centroid);
	
	FeatureSet fs;
	fs[0].push_back(f);
	return fs;
}

SpectralCentroid::FeatureSet SpectralCentroid::getRemainingFeatures() {
	return FeatureSet();
}

float SpectralCentroid::hz_to_bark(float hz) {
	return 6.0 * asinh(hz / 600.0);
}