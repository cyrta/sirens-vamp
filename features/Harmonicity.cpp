#include "Harmonicity.h"

#include <cmath>
using namespace std;

static const double PI = 2 * asin(1.0);

Harmonicity::Harmonicity(float inputSampleRate) : Plugin(inputSampleRate), m_blockSize(0) {
	m_sampleRate = inputSampleRate;
	
	searchRegionLength = 5;
	absThreshold = 1.0;
	threshold = 0.1;
	lpfCoefficient = 0.7;
	maxPeaks = 3;
	
	rawIndices.values = NULL;
	rawMagnitudes.values = NULL;
	accIndices.values = NULL;
	peakList.values = NULL;
}

Harmonicity::~Harmonicity() {
	freeMemory();
}

string Harmonicity::getIdentifier() const {
	return "harmonicity";
}

string Harmonicity::getName() const {
	return "Harmonicity";
}

string Harmonicity::getDescription() const {
	return "Calculates the degree to which the input sound originated from an harmonic source.";
}

string Harmonicity::getMaker() const {
	return "Sirens";
}

int Harmonicity::getPluginVersion() const {
	return 1;
}

string Harmonicity::getCopyright() const {
	return "MIT";
}

Harmonicity::InputDomain Harmonicity::getInputDomain() const {
	return FrequencyDomain;
}

size_t Harmonicity::getPreferredBlockSize() const {
	return 0;
}

size_t Harmonicity::getPreferredStepSize() const {
	return 0;
}

size_t Harmonicity::getMinChannelCount() const {
	return 1;
}

size_t Harmonicity::getMaxChannelCount() const {
	return 1;
}

Harmonicity::ParameterList Harmonicity::getParameterDescriptors() const {
	ParameterList list;
	
	ParameterDescriptor absThresholdParameter;
	absThresholdParameter.identifier = "abs-threshold";
	absThresholdParameter.name = "Absolute Threshold";
	absThresholdParameter.description = "Absolute threshold for peak picking.";
	absThresholdParameter.unit = "";
	absThresholdParameter.minValue = 0;
	absThresholdParameter.maxValue = 1;
	absThresholdParameter.defaultValue = 1;
	absThresholdParameter.isQuantized = false;
	list.push_back(absThresholdParameter);
	
	ParameterDescriptor thresholdParameter;
	thresholdParameter.identifier = "threshold";
	thresholdParameter.name = "Threshold";
	thresholdParameter.description = "Threshold of ratio of frequency bin to sum of surrounding bins to be considered a peak.";
	thresholdParameter.unit = "";
	thresholdParameter.minValue = 0;
	thresholdParameter.maxValue = 1;
	thresholdParameter.defaultValue = 0.1;
	thresholdParameter.isQuantized = false;
	list.push_back(thresholdParameter);
	
	ParameterDescriptor searchRegionLengthParameter;
	searchRegionLengthParameter.identifier = "search-region-length";
	searchRegionLengthParameter.name = "Search region length";
	searchRegionLengthParameter.description = "Length of window to consider around a frequency bin for peak picking.";
	searchRegionLengthParameter.unit = "bins";
	searchRegionLengthParameter.minValue = 0;
	searchRegionLengthParameter.maxValue = 10;
	searchRegionLengthParameter.defaultValue = 5;
	searchRegionLengthParameter.isQuantized = false;
	list.push_back(searchRegionLengthParameter);
	
	ParameterDescriptor maxPeaksParameter;
	maxPeaksParameter.identifier = "max-peaks";
	maxPeaksParameter.name = "Maximum number of peaks";
	maxPeaksParameter.description = "Maximum number of peaks to select for evaluation.";
	maxPeaksParameter.unit = "peaks";
	maxPeaksParameter.minValue = 0;
	maxPeaksParameter.maxValue = 10;
	maxPeaksParameter.defaultValue = 3;
	maxPeaksParameter.isQuantized = false;
	list.push_back(maxPeaksParameter);
	
	ParameterDescriptor lpfCoefficientParameter;
	lpfCoefficientParameter.identifier = "lpf-coefficient";
	lpfCoefficientParameter.name = "LPF coefficient";
	lpfCoefficientParameter.description = "Amount of smoothing.";
	lpfCoefficientParameter.unit = "";
	lpfCoefficientParameter.minValue = 0;
	lpfCoefficientParameter.maxValue = 1;
	lpfCoefficientParameter.defaultValue = 0.7;
	lpfCoefficientParameter.isQuantized = false;
	list.push_back(lpfCoefficientParameter);
	
	return list;
}

float Harmonicity::getParameter(string identifier) const {
	if (identifier == "abs-threshold")
		return absThreshold;
	else if (identifier == "threshold")
		return threshold;
	else if (identifier == "search-region-length")
		return float(searchRegionLength);
	else if (identifier == "max-peaks")
		return float(maxPeaks);
	else if (identifier == "lpf-coefficient")
		return lpfCoefficient;
	else
		return 0;
}

void Harmonicity::setParameter(string identifier, float value)	{
	if (identifier == "abs-threshold")
		absThreshold = value;
	else if (identifier == "threshold")
		threshold = value;
	else if (identifier == "search-region-length")
		searchRegionLength = int(value);
	else if (identifier == "max-peaks")
		maxPeaks = int(value);
	else if (identifier == "lpf-coefficient")
		lpfCoefficient = value;
}

Harmonicity::OutputList Harmonicity::getOutputDescriptors() const {
	OutputList list;
	
	OutputDescriptor harmonicityOutput;
	harmonicityOutput.identifier = "harmonicity";
	harmonicityOutput.name = "Harmonicity";
	harmonicityOutput.description = "probability that the input audio came from an harmonic source.";
	harmonicityOutput.unit = "";
	harmonicityOutput.hasFixedBinCount = true;
	harmonicityOutput.binCount = 1;
	harmonicityOutput.hasKnownExtents = false;
	harmonicityOutput.isQuantized = false;
	harmonicityOutput.sampleType = OutputDescriptor::OneSamplePerStep;
	harmonicityOutput.hasDuration = false;
	list.push_back(harmonicityOutput);
	
	OutputDescriptor pitchOutput;
	pitchOutput.identifier = "pitch";
	pitchOutput.name = "Pitch";
	pitchOutput.description = "most likely pitch estimate.";
	pitchOutput.unit = "Hz";
	pitchOutput.hasFixedBinCount = true;
	pitchOutput.binCount = 1;
	pitchOutput.hasKnownExtents = false;
	pitchOutput.isQuantized = false;
	pitchOutput.sampleType = OutputDescriptor::OneSamplePerStep;
	pitchOutput.hasDuration = false;
	list.push_back(pitchOutput);
	
	return list;
}

bool Harmonicity::initialise(size_t channels, size_t stepSize, size_t blockSize) {
	if (channels < getMinChannelCount() || channels > getMaxChannelCount())
		return false;
	else {
		m_blockSize = blockSize;
		
		resetVectors();
		return true;
	}
}

void Harmonicity::reset() {
	resetVectors();
}

Harmonicity::FeatureSet Harmonicity::process(const float *const *inputBuffers, Vamp::RealTime timestamp) {
	pickPeaks(inputBuffers);
	
	pitch = 0;
	
	if (peakList.size == 1) {
		pitch = peakList.values[0].frequency;
	} else if (peakList.size != 0)
		goldsteinCalc();
	
	peakList.size = 0;
	
	Feature harmonicityFeature;
	harmonicityFeature.hasTimestamp = false;
	harmonicityFeature.values.push_back(harmonicity);
	
	Feature pitchFeature;
	pitchFeature.hasTimestamp = false;
	pitchFeature.values.push_back(pitch);
	
	FeatureSet fs;
	fs[0].push_back(harmonicityFeature);
	fs[1].push_back(pitchFeature);
	
	return fs;
}

Harmonicity::FeatureSet Harmonicity::getRemainingFeatures() {
	return FeatureSet();
}

void Harmonicity::freeMemory() {
	if (rawIndices.values)
		delete [] rawIndices.values;
	
	if (rawMagnitudes.values)
		delete [] rawMagnitudes.values;
	
	if (accIndices.values)
		delete [] accIndices.values;
	
	if (peakList.values)
		delete [] peakList.values;
}

void Harmonicity::resetVectors() {
	freeMemory();
	
	pitch = 0;
	harmonicity = 0;
	
	float min_hz = m_sampleRate >= 180 ? 90.0 : 0;
	float max_hz = m_sampleRate >= 7000 ? 3500.0 : m_sampleRate / 2;
	
	fftSize = (m_blockSize - 1) * 2;
	
	minFrequencyIndex = int(ceil(min_hz * float(fftSize) / m_sampleRate));
	maxFrequencyIndex = int(ceil(max_hz * float(fftSize) / m_sampleRate));
	
	int vector_size = maxFrequencyIndex - minFrequencyIndex;
	
	// Parameters.
	searchRegionLength2 = (searchRegionLength - 1) / 2;
	
	nMax = 10;
	filterOldValue = 0;
	kVar = 0.01 / sqrt(2.0);
	
	// Vectors for peaks, magnitudes, and indices.
	rawIndices.size = 0;
	rawMagnitudes.size = 0;
	accIndices.size = 0;
	peakList.size = 0;
	
	rawIndices.values = new int[vector_size];
	rawMagnitudes.values = new double[vector_size];
	accIndices.values = new int[vector_size];
	peakList.values = new Peak[vector_size];
}

void Harmonicity::pickPeaks(const float *const *inputBuffers) {
	// Search through all bins, chopping off the beginning and end, so we can slide a searchRegionLength window across the spectrum.
	for (unsigned int k = minFrequencyIndex + searchRegionLength2; k < maxFrequencyIndex - searchRegionLength2; k++) {
		// Find the maximum amplitude in the search region surrounding the current frequency.
		float maxel = 0;
		
		for (unsigned int i = k - searchRegionLength2; i <= k + searchRegionLength2; i++) {
			if (maxel < inputBuffers[0][i])
				maxel = inputBuffers[0][i];
		}
		
		// Save the bin with the maximum amplitude in the search region.
		if (inputBuffers[0][k] >= maxel) {
			rawIndices.values[rawIndices.size] = k;
			rawIndices.size ++;
			rawMagnitudes.values[rawMagnitudes.size] = inputBuffers[0][k];
			rawMagnitudes.size ++;
		}
	}
	
	// Find the maximum amplitude from the spectrum.
	float max_peak_mag = 0;
	
	for (unsigned int i = 0; i < rawMagnitudes.size; i++) {
		if (max_peak_mag < rawMagnitudes.values[i])
			max_peak_mag = rawMagnitudes.values[i];
	}
			
	// Accept only frequency bins where the amplitudes threshold is greater than the absolute threshold 
	// and threshold relative to the maximum amplitude.
	for (unsigned int k = 0; k < rawIndices.size; k++) {
		if ((inputBuffers[0][rawIndices.values[k]] > threshold * max_peak_mag) && (inputBuffers[0][rawIndices.values[k]] > absThreshold)) {
			accIndices.values[accIndices.size] = rawIndices.values[k];
			accIndices.size ++;
		}
	}
		
	// Pick peaks from bins that have high amplitudes compared to their surrounding bins.
	for (unsigned int k = 0; k < accIndices.size; k++) {
		int ind = accIndices.values[k];
		
		// Get surrounding amplitudes.
		float y1 = log(inputBuffers[0][ind - 1]);
		float y2 = log(inputBuffers[0][ind]);
		float y3 = log(inputBuffers[0][ind + 1]);
		
		float denom = (2 * (y1 - 2 * y2 + y3));
		float freq_bin_zero = denom > 0 ? (y1 - y3) / denom : 0;
		
		float f = (freq_bin_zero > 0) ? (y1 - y2) / (1 + 2 * freq_bin_zero) : (y3 - y2) / (1 - 2 * freq_bin_zero);

		tempPeak.amplitude = exp(y2 - f * freq_bin_zero * freq_bin_zero);
		tempPeak.frequency = m_sampleRate * float(freq_bin_zero + ind) / float(fftSize);
		
		peakList.values[peakList.size] = tempPeak;
		peakList.size ++;
	}
	
	if (peakList.size > 1) {
		// If we have more peaks that maximum, get rid of the peaks with lower amplitudes.
		if (peakList.size > maxPeaks) {
			partial_sort(peakList.values, peakList.values + maxPeaks, peakList.values + peakList.size, SortAmplitudes);
			peakList.size = maxPeaks;
		}
		
		// And then sort by frequency, ascending.
		sort(peakList.values, peakList.values + peakList.size, SortFrequencies);
	}
	
	// Reset.
	accIndices.size = rawIndices.size = rawMagnitudes.size = 0;
}

	
// Evaluates a Gaussian distribution modeling the probability that a given peak is a harmonic of f0.
float Harmonicity::goldsteinGaussian(float x1, float x2, int n1, int n2, float f0, float k) {
	// mu = f0 * N, where N = [n1, n2]
	float mu1 = f0 * n1;
	float mu2 = f0 * n2;
	
	// sigma = diag((K * mu).^2)
	float sigma_component1 = k * mu1;
	float sigma_component2 = k * mu2;
	
	float sigma_inverse1 = 1.0 / (sigma_component1 * sigma_component1);
	float sigma_inverse2 = 1.0 / (sigma_component2 * sigma_component2);
	
	// xminmu = x - mu
	float x_minus_mu1 = x1 - mu1;
	float x_minus_mu2 = x2 - mu2;
	
	// constant term = 1 / (2PI ^ D/2 * sqrt(det(sigma))
	float constant = 1.0 / (2.0 * PI * sigma_component1 * sigma_component2);
	
	// P(f0) = constant * exp(-1/2 * (x - mu)' * inv(sigma) * (x - mu)
	float p_f0 = constant * exp(
		-0.5 * (
			(sigma_inverse1 * (x_minus_mu1 * x_minus_mu1)) + 
			(sigma_inverse2 * (x_minus_mu2 * x_minus_mu2))
		)
	);
	
	return p_f0;
}

void Harmonicity::goldsteinCalc() {
	unsigned int p1, p2, n1, n2;
	float p0, f0, f1, f2, f1_rat, f2_rat, p_temp, f_temp;
	
	p0 = 0;
	f0 = peakList.values[0].frequency;
	
	for (p1 = 0; p1 < peakList.size - 1; p1++) {
		for (p2 = p1 + 1; p2 < peakList.size;  p2++) {
			f1 = peakList.values[p1].frequency;
			f2 = peakList.values[p2].frequency;
			
			for (n1 = 0; n1 < nMax - 1; n1++) {
				for (n2 = n1 + 1; n2 < nMax; n2++) {
					f1_rat = f1 / (n1 + 1.0);
					f2_rat = f2 / (n2 + 1.0);
					f_temp = ((f1_rat * f1_rat) + (f2_rat * f2_rat)) / (f1_rat + f2_rat);
					p_temp = goldsteinGaussian(f1, f2, n1 + 1, n2 + 1, f_temp, kVar);
					
					if ((p_temp > p0)) {
						f0 = f_temp;
						p0 = p_temp;
					}
				}
			}
		}
	}
	
	pitch = f0;
	harmonicity = p0;
}

bool SortAmplitudes(const Peak& peak1, const Peak& peak2) {
	return peak1.amplitude > peak2.amplitude;
}

bool SortFrequencies(const Peak& peak1, const Peak& peak2) {
	return peak1.frequency < peak2.frequency;
}
