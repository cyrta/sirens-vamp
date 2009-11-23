#include <vamp/vamp.h>
#include <vamp-sdk/PluginAdapter.h>

#include "features/Loudness.h"
#include "features/TemporalSparsity.h"
#include "features/SpectralSparsity.h"
#include "features/SpectralCentroid.h"
#include "features/TransientIndex.h"
#include "features/Harmonicity.h"

// Declare one static adapter here for each plugin class in this library.
static Vamp::PluginAdapter<Loudness> loudnessAdapter;
static Vamp::PluginAdapter<TemporalSparsity> temporalSparsityAdapter;
static Vamp::PluginAdapter<SpectralSparsity> spectralSparsityAdapter;
static Vamp::PluginAdapter<SpectralCentroid> spectralCentroidAdapter;
static Vamp::PluginAdapter<TransientIndex> transientIndexAdapter;
static Vamp::PluginAdapter<Harmonicity> harmonicityAdapter;

// This is the entry-point for the library, and the only function that needs to be publicly exported.
const VampPluginDescriptor* vampGetPluginDescriptor(unsigned int version, unsigned int index) {
    if (version < 1) return 0;
	
    switch (index) {
		case 0: return loudnessAdapter.getDescriptor();
		case 1: return temporalSparsityAdapter.getDescriptor();
		case 2: return spectralSparsityAdapter.getDescriptor();
		case 3: return spectralCentroidAdapter.getDescriptor();
		case 4: return transientIndexAdapter.getDescriptor();
		case 5: return harmonicityAdapter.getDescriptor();
    	default: return 0;
    }
}
