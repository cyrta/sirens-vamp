#include "TemporalSparsity.h"

#include <cmath>
using namespace std;

TemporalSparsity::TemporalSparsity(float inputSampleRate) : Plugin(inputSampleRate), m_blockSize(0) {
	windowSize = 50;
	
	rmsWindow = NULL;
	resetWindow();
}

TemporalSparsity::~TemporalSparsity() {
	delete rmsWindow;
}

string TemporalSparsity::getIdentifier() const {
	return "temporal-sparsity";
}

string TemporalSparsity::getName() const {
	return "Temporal sparsity";
}

string TemporalSparsity::getDescription() const {
	return "Calculates the maximum RMS level divided by the total RMS level over a sliding window.";
}

string TemporalSparsity::getMaker() const {
	return "Sirens";
}

int TemporalSparsity::getPluginVersion() const {
	return 1;
}

string TemporalSparsity::getCopyright() const {
	return "MIT";
}

TemporalSparsity::InputDomain TemporalSparsity::getInputDomain() const {
	return TimeDomain;
}

size_t TemporalSparsity::getPreferredBlockSize() const {
	return 0;
}

size_t TemporalSparsity::getPreferredStepSize() const {
	return 0;
}

size_t TemporalSparsity::getMinChannelCount() const {
	return 1;
}

size_t TemporalSparsity::getMaxChannelCount() const {
	return 1;
}

TemporalSparsity::ParameterList TemporalSparsity::getParameterDescriptors() const {
	ParameterList list;
	
	ParameterDescriptor d;
	d.identifier = "window-size";
	d.name = "Window Size";
	d.description = "How long the sliding window should be in frames.";
	d.unit = "frames";
	d.minValue = 1;
	d.maxValue = 100;
	d.defaultValue = 50;
	d.quantizeStep = 1;
	d.isQuantized = true;
	list.push_back(d);

	return list;
}

float TemporalSparsity::getParameter(string identifier) const {
	if (identifier == "window-size")
		return float(rmsWindow->getMaxSize());
	
	return 0;
}

void TemporalSparsity::setParameter(string identifier, float value)	 {
	if (identifier == "window-size") {
		windowSize = int(value);
		resetWindow();
	}
}

TemporalSparsity::OutputList TemporalSparsity::getOutputDescriptors() const {
	OutputList list;
	
	OutputDescriptor d;
	d.identifier = "temporal-sparsity";
	d.name = "Temporal Sparsity";
	d.description = "Maximum RMS level divided by the total RMS level over a sliding window.";
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

bool TemporalSparsity::initialise(size_t channels, size_t stepSize, size_t blockSize) {
	if (channels < getMinChannelCount() || channels > getMaxChannelCount())
		return false;
	else {
		m_blockSize = blockSize;
		return true;
	}
}

void TemporalSparsity::reset() {
	resetWindow();
}

TemporalSparsity::FeatureSet TemporalSparsity::process(const float *const *inputBuffers, Vamp::RealTime timestamp) {
	// Calculate RMS.
	float sum_of_squares = 0;
	
	for (unsigned int i = 0; i < m_blockSize; i++) {
		float sample = inputBuffers[0][i];
		sum_of_squares += sample * sample;
	}
	
	rmsWindow->addValue(sqrt(sum_of_squares / float(m_blockSize)));
	
	float sparsity = 0;
	
	if (rmsWindow->getSize() >= 1) {
		float max = 0;
		float sum = 0;
		
		int rms_size = rmsWindow->getSize();
		int max_size = rmsWindow->getMaxSize();
		float* rms_item = rmsWindow->getData();
		
		for (int i = 0; i < rms_size; i++) {
			max = max > *rms_item ? max : *rms_item;
			sum += (*rms_item);
			
			rms_item ++;
		}
		
		sparsity = sum > 0 ? (float(rms_size) / float(max_size)) * (max / sum) : 0;	
	}
	
	Feature f;
	f.hasTimestamp = false;
	f.values.push_back(sparsity);
	
	FeatureSet fs;
	fs[0].push_back(f);
	return fs;	
	
	return FeatureSet();
}

TemporalSparsity::FeatureSet TemporalSparsity::getRemainingFeatures() {
	return FeatureSet();
}

void TemporalSparsity::resetWindow() {
	if (rmsWindow)
		delete rmsWindow;
	
	rmsWindow = new CircularArray(windowSize);
}
