# Sirens-Vamp
Sirens is a collection of tools for segmentation, indexing, and retrieval of environmental sounds. For a working version of Sirens, see [sirens](http://github.com/plant/sirens-ruby). Sirens-Vamp is a project aimed at implementing the feature extraction and segmentation routines as [Vamp](http://vamp-plugins.com) plugins. In this way, hopefully the code can be more modular. For example, segmentation or retrieval could be done using feature sets more commonly used for music rather than natural sounds. Additionally, one can do analyses with these features using audio packages such as [Audacity](http://audacity.sf.net), [Sonic Visualizer](http://www.sonicvisualiser.org/), [Sonic Annotator](http://omras2.org/SonicAnnotator), and [VamPy](http://www.vamp-plugins.org/vampy.html).

Eventually, use of Sirens-Vamp will eclipse that of Sirens, as segmentation can be performed on feature extraction output from Sonic Annotator. Currently, however, the Sirens C++ library benefits from multithreaded feature extraction, which Sonic Annotator does not yet support.

# Installation
## OSX
Grab the [OS X Universal Binary](http://cloud.github.com/downloads/plant/sirens-vamp/sirens-vamp.dylib) and place it in /Library/Audio/Plug-Ins/Vamp or $HOME/Library/Audio/Plug-Ins/Vamp. You can now use the Sirens features from any application that supports Vamp. I strongly suggest [Sonic Visualizer](http://www.sonicvisualiser.org/) for interactively visualizing feature trajectories and [Sonic Annotator](http://omras2.org/SonicAnnotator) for running batch feature extraction processes for input into statistical packages such as Matlab.

## Other platforms / from source
A Makefile is included with the source that contains commented compilation commands for various platforms. Comment out the bits for OS X and uncomment the relevant commands for your platform.

# Acknowledgements
Sirens-Vamp is based off research within the [Arts, Media and Engineering](http://ame.asu.edu/) program at [Arizona State University](http://asu.edu/). For more information, the following papers may be of interest:

1. Gordon Wichern, H. Thornburg, B. Mechtley, A. Fink, A Spanias, and K. Tu, “Robust multi-feature segmentation and indexing for natural sound environments,” in _Proc. of IEEE/EURASIP International Workshop on Content Based Multimedia Indexing (CBMI)_, Bordeaux France, July 2007. [(PDF)](http://www.public.asu.edu/~gwichern/CBMI07.pdf)
2. Gordon Wichern, J. Xue, H. Thornburg, and A. Spanias, “Distortion-aware query-by-example of environmental sounds,” in _Proc. of IEEE Workshop on Applications of Signal Processing to Audio and Acoustics (WASPAA)_, New Paltz, NY, October 2007. [(PDF)](http://www.public.asu.edu/~gwichern/WASPAA07.pdf)
3. J. Xue, Gordon Wichern, H. Thornburg, and A. Spanias, “Fast query-by-example of environmental sounds via robust and efficient cluster-based indexing,” in _Proc. of IEEE International Conference on Acoustics Speech and Signal Processing (ICASSP)_, Las Vegas, NV, April 2008. [(PDF)](http://www.public.asu.edu/~gwichern/cluster_ICASSP08.pdf)

Additionally, work on Sirens is supported by the [National Science Foundation](http://www.nsf.gov/) under Grants NSF IGERT DGE-05-04647 and NSF CISE Research Infrastructure 04-03428.

# Copyright
Copyright 2009 Arizona State University, released under the MIT License.
