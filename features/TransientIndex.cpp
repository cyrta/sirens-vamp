#include "TransientIndex.h"

#include <cmath>
using namespace std;

static const double PI = 2 * asin(1.0);

TransientIndex::TransientIndex(float inputSampleRate) : Plugin(inputSampleRate), m_blockSize(0) {
	m_sampleRate = inputSampleRate;
	
	filters = 30;
	mels = 15;
	
	mfccNew = NULL;
	mfccOld = NULL;
	dctMatrix = NULL;
	filterTemp = NULL;
	filterBank = NULL;
}

TransientIndex::~TransientIndex() {
	freeMemory();
}

string TransientIndex::getIdentifier() const {
	return "transient-index";
}

string TransientIndex::getName() const {
	return "Transient index";
}

string TransientIndex::getDescription() const {
	return "Calculates the magnitude of the difference in mel-frequency cepstral coefficients between consecutive frames.";
}

string TransientIndex::getMaker() const {
	return "Brandon Mechtley and Gordon Wichern";
}

int TransientIndex::getPluginVersion() const {
	return 1;
}

string TransientIndex::getCopyright() const {
	return "MIT";
}

TransientIndex::InputDomain TransientIndex::getInputDomain() const {
	return FrequencyDomain;
}

size_t TransientIndex::getPreferredBlockSize() const {
	return 0;
}

size_t TransientIndex::getPreferredStepSize() const {
	return 0;
}

size_t TransientIndex::getMinChannelCount() const {
	return 1;
}

size_t TransientIndex::getMaxChannelCount() const {
	return 1;
}

TransientIndex::ParameterList TransientIndex::getParameterDescriptors() const {
	ParameterList list;
	
	ParameterDescriptor filtersParameter;
	filtersParameter.identifier = "filters";
	filtersParameter.name = "Filters";
	filtersParameter.description = "Number of filters.";
	filtersParameter.unit = "filters";
	filtersParameter.minValue = 1;
	filtersParameter.maxValue = 50;
	filtersParameter.defaultValue = 30;
	filtersParameter.quantizeStep = 1;
	filtersParameter.isQuantized = true;
	list.push_back(filtersParameter);
	
	ParameterDescriptor melsParameter;
	melsParameter.identifier = "mels";
	melsParameter.name = "Mels";
	melsParameter.description = "Number of mels.";
	melsParameter.unit = "";
	melsParameter.minValue = 1;
	melsParameter.maxValue = 50;
	melsParameter.defaultValue = 15;
	melsParameter.quantizeStep = 1;
	melsParameter.isQuantized = true;
	list.push_back(melsParameter);
	
	return list;
}

float TransientIndex::getParameter(string identifier) const {	
	if (identifier == "filters")
		return float(filters);
	else if (identifier == "mels")
		return float(mels);
	else
		return 0;
}

void TransientIndex::setParameter(string identifier, float value)  {
	if (identifier == "filters") {
		filters = int(value);
		resetFilterBank();
	} else if (identifier == "mels") {
		mels = int(value);
		resetFilterBank();
	}
}

TransientIndex::OutputList TransientIndex::getOutputDescriptors() const {
	OutputList list;
	
	OutputDescriptor d;
	d.identifier = "transient-index";
	d.name = "Transient index";
	d.description = "magnitude of the difference in mel-frequency cepstral coefficients between consecutive frames";
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

bool TransientIndex::initialise(size_t channels, size_t stepSize, size_t blockSize) {
	if (channels < getMinChannelCount() || channels > getMaxChannelCount())
		return false;
	else {
		m_blockSize = blockSize;
		resetFilterBank();
		return true;
	}
}

void TransientIndex::reset() {
	resetFilterBank();
}

TransientIndex::FeatureSet TransientIndex::process(const float *const *inputBuffers, Vamp::RealTime timestamp) {
	// Calculate the MFCC vector for the current frame.
	for (unsigned int i = 0; i < filters; i++) {
		filterTemp[i] = 0;
		
		for (unsigned int j = 0; j < m_blockSize; j++)
			filterTemp[i] += filterBank[(i * m_blockSize) + j] * inputBuffers[0][j];
		
		filterTemp[i] = (filterTemp[i] > 0) ? log(filterTemp[i]) : 0;
	}
	
	for (unsigned int i = 0; i < mels; i++) {
		mfccNew[i] = 0;
	
		for (unsigned int j = 0; j < filters; j++)
			mfccNew[i] += dctMatrix[(i * filters) + j] * filterTemp[j];
	}
	
	// Calculate transient index.
	float sum_of_squared_error = 0;
	
	for (unsigned int i = 0; i < mels; i++) {
		float error = mfccNew[i] - mfccOld[i];
		sum_of_squared_error += error * error;
	}
	
	float index = sqrt(sum_of_squared_error);
	
	// Swap MFCC vectors.
	float* mfccTemp = mfccOld;
	mfccOld = mfccNew;
	mfccNew = mfccTemp;
	
	Feature f;
	f.hasTimestamp = false;
	f.values.push_back(index);
	
	FeatureSet fs;
	fs[0].push_back(f);
	return fs;
}

TransientIndex::FeatureSet TransientIndex::getRemainingFeatures() {
	return FeatureSet();
}


void TransientIndex::resetFilterBank() {
	freeMemory();
	
	if (m_blockSize > 0) {
		dctMatrix = new float[filters * mels];
		filterBank = new float[filters * m_blockSize];
		filterTemp = new float[filters];
	
		mfccNew = new float[mels];
		mfccOld = new float[mels];

		// Initialisation
		float min_mel = hz_to_mel(50.0);
		float max_mel = hz_to_mel(m_sampleRate / 2);
	
		float* filter_values = new float[m_blockSize];
		float* filter_centers = new float[filters + 2];
	
		for (unsigned int i = 0; i < m_blockSize; i++)
			filter_values[i] = (m_sampleRate * i) / float(2 * (m_blockSize - 1));
	
		for (unsigned int i = 0; i < mels; i++) {
			for (unsigned int j = 0; j < filters; j++)
				dctMatrix[i * filters + j] = cos((i + 1) * (PI / filters * (j + 0.5)));
		}
	
		for (unsigned int i = 0; i < filters + 2; i++)
			filter_centers[i] = mel_to_hz(min_mel + ((max_mel - min_mel) / (filters + 1)) * i);
	
		for (unsigned int i = 0; i < filters; i++) {
			for (unsigned int j = 0; j < m_blockSize; j++) {
				if (filter_values[j] >= filter_centers[i] && filter_values[j] < filter_centers[i + 1])
					filterBank[(i * m_blockSize) + j] = (filter_values[j] - filter_centers[i]) / (filter_centers[i + 1] - filter_centers[i]);
				else if (filter_values[j] >= filter_centers[i + 1] && filter_values[j] < filter_centers[i + 2])
					filterBank[(i * m_blockSize) + j] = (filter_values[j] - filter_centers[i + 2]) / (filter_centers[i + 1] - filter_centers[i + 2]);
				else
					filterBank[(i * m_blockSize) + j] = 0;
			}
		}
	
		for (unsigned int i = 0; i < mels; i++) {
			mfccNew[i] = 0;
			mfccOld[i] = 0;
		}
	
		for (unsigned int i = 0; i < filters; i++)
			filterTemp[i] = 0;

		delete[] filter_values;
		delete[] filter_centers;
	}
}

void TransientIndex::freeMemory() {
	if (dctMatrix)
		delete[] dctMatrix;
	
	if (filterBank)
		delete[] filterBank;
	
	if (filterTemp)
		delete[] filterTemp;
	
	if (mfccNew)
		delete[] mfccNew;
	
	if (mfccOld)
		delete[] mfccOld;	
}

float TransientIndex::hz_to_mel(float hz) {
	return 2595.0 * log10((hz / 700.0) + 1.0);
}

float TransientIndex::mel_to_hz(float mel) {
    return 700.0 * (pow(10.0, (mel / 2595.0)) - 1.0);
}
