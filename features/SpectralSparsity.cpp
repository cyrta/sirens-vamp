#include "SpectralSparsity.h"

SpectralSparsity::SpectralSparsity(float inputSampleRate) : Plugin(inputSampleRate), m_blockSize(0) {
}

SpectralSparsity::~SpectralSparsity() {
}

string SpectralSparsity::getIdentifier() const {
	return "spectral-sparsity";
}

string SpectralSparsity::getName() const {
	return "Spectral sparsity";
}

string SpectralSparsity::getDescription() const {
	return "Calculates the \"peakiness\" of the spectrum: maximum divided by total power.";
}

string SpectralSparsity::getMaker() const {
	return "Brandon Mechtley and Gordon Wichern";
}

int SpectralSparsity::getPluginVersion() const {
	return 1;
}

string SpectralSparsity::getCopyright() const {
	return "MIT";
}

SpectralSparsity::InputDomain SpectralSparsity::getInputDomain() const {
	return FrequencyDomain;
}

size_t SpectralSparsity::getPreferredBlockSize() const {
	return 0;
}

size_t SpectralSparsity::getPreferredStepSize() const {
	return 0;
}

size_t SpectralSparsity::getMinChannelCount() const {
	return 1;
}

size_t SpectralSparsity::getMaxChannelCount() const {
	return 1;
}

SpectralSparsity::ParameterList SpectralSparsity::getParameterDescriptors() const {
	ParameterList list;
	return list;
}

float SpectralSparsity::getParameter(string identifier) const {
	return 0;
}

void SpectralSparsity::setParameter(string identifier, float value)	 {
}

SpectralSparsity::OutputList SpectralSparsity::getOutputDescriptors() const {
	OutputList list;
	
	OutputDescriptor d;
	d.identifier = "spectral-sparsity";
	d.name = "Spectral sparsity";
	d.description = "ratio of maximum spectral power to total power.";
	d.unit = "";
	d.hasFixedBinCount = true;
	d.binCount = 1;
	d.hasKnownExtents = false;
	d.isQuantized = false;
	d.sampleType = OutputDescriptor::OneSamplePerStep;
	d.hasDuration = false;
	list.push_back(d);

	return list;
}

bool SpectralSparsity::initialise(size_t channels, size_t stepSize, size_t blockSize) {
	if (channels < getMinChannelCount() || channels > getMaxChannelCount())
		return false;
	else {
		m_blockSize = blockSize;
		return true;
	}
}

void SpectralSparsity::reset() {
	// Clear buffers, reset stored values, etc.
}

SpectralSparsity::FeatureSet SpectralSparsity::process(const float *const *inputBuffers, Vamp::RealTime timestamp) {
	float max = 0;
	float sum = 0;
	
	for (unsigned int i = 0; i < m_blockSize; i++) {
		float bin = inputBuffers[0][i];
		
		max = max > bin ? max : bin;
		sum += bin;
	}
	
	float sparsity = sum > 0 ? (max / sum) : 0;
	
	Feature f;
	f.hasTimestamp = false;
	f.values.push_back(sparsity);
	
	FeatureSet fs;
	fs[0].push_back(f);
	return fs;
}

SpectralSparsity::FeatureSet SpectralSparsity::getRemainingFeatures() {
	return FeatureSet();
}
