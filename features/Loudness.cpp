#include "Loudness.h"

#include <cmath>
using namespace std;

Loudness::Loudness(float inputSampleRate) : Plugin(inputSampleRate), m_blockSize(0) {
}

Loudness::~Loudness() {
}

string Loudness::getIdentifier() const {
	return "loudness";
}

string Loudness::getName() const {
	return "Loudness";
}

string Loudness::getDescription() const {
	return "Calculates dB-scaled RMS level of the input frame.";
}

string Loudness::getMaker() const {
	return "Sirens";
}

int Loudness::getPluginVersion() const {
	return 1;
}

string Loudness::getCopyright() const {
	return "MIT";
}

Loudness::InputDomain Loudness::getInputDomain() const {
	return TimeDomain;
}

size_t Loudness::getPreferredBlockSize() const {
	return 0;
}

size_t Loudness::getPreferredStepSize() const {
	return 0;
}

size_t Loudness::getMinChannelCount() const {
	return 1;
}

size_t Loudness::getMaxChannelCount() const {
	return 1;
}

Loudness::ParameterList Loudness::getParameterDescriptors() const {
	ParameterList list;
	return list;
}

float Loudness::getParameter(string identifier) const {
	return 0;
}

void Loudness::setParameter(string identifier, float value)	 {
}

Loudness::OutputList Loudness::getOutputDescriptors() const {
	OutputList list;
	
	OutputDescriptor d;
	d.identifier = "loudness";
	d.name = "Loudness";
	d.description = "dB-scaled RMS level of the input sound.";
	d.unit = "dB";
	d.hasFixedBinCount = true;
	d.binCount = 1;
	d.hasKnownExtents = false;
	d.isQuantized = false;
	d.sampleType = OutputDescriptor::OneSamplePerStep;
	d.hasDuration = false;
	list.push_back(d);
	
	return list;
}

bool Loudness::initialise(size_t channels, size_t stepSize, size_t blockSize) {
	if (channels < getMinChannelCount() || channels > getMaxChannelCount())
		return false;
	else {
		m_blockSize = blockSize;
		return true;
	}
}

void Loudness::reset() {
}

Loudness::FeatureSet Loudness::process(const float *const *inputBuffers, Vamp::RealTime timestamp) {
	float sum_of_squares = 0;
	
	for (unsigned int i = 0; i < m_blockSize; i++) {
		float sample = inputBuffers[0][i];
		sum_of_squares += sample * sample;
	}
	
	float loudness = sum_of_squares > 0 ? 20 * log10(sum_of_squares / (float) m_blockSize) : 0;
	
	Feature f;
	f.hasTimestamp = false;
	f.values.push_back(loudness);
	
	FeatureSet fs;
	fs[0].push_back(f);
	return fs;
}

Loudness::FeatureSet Loudness::getRemainingFeatures() {
	return FeatureSet();
}
