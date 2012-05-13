PLUGIN_LIBRARY_NAME = sirens-vamp
PLUGIN_CODE_OBJECTS = plugins.o features/Loudness.o features/TemporalSparsity.o features/SpectralSparsity.o features/SpectralCentroid.o features/TransientIndex.o features/Harmonicity.o support/CircularArray.o
VAMP_SDK_INCLUDE_DIR = /usr/local/include
VAMP_SDK_LIB_DIR = /usr/local/lib/vamp

##  Uncomment these for an OS/X native build using command-line tools:
# CXXFLAGS = -I$(VAMP_SDK_INCLUDE_DIR) -Wall -fPIC
# PLUGIN_EXT = .dylib
# PLUGIN = $(PLUGIN_LIBRARY_NAME)$(PLUGIN_EXT)
# LDFLAGS = -dynamiclib -install_name $(PLUGIN) $(VAMP_SDK_LIB_DIR)/libvamp-sdk.a -exported_symbols_list vamp-plugin.list

##  Uncomment these for an OS/X universal binary using command-line tools:
CXXFLAGS = -isysroot /Developer/SDKs/MacOSX10.6.sdk -arch i386 -arch x86_64 -I$(VAMP_SDK_INCLUDE_DIR) -Wall -fPIC
PLUGIN_EXT = .dylib
PLUGIN = $(PLUGIN_LIBRARY_NAME)$(PLUGIN_EXT)
LDFLAGS = -dynamiclib -install_name $(PLUGIN) $(VAMP_SDK_LIB_DIR)/libvamp-sdk.a -exported_symbols_list vamp-plugin.list

##  Uncomment these for Linux using the standard tools:
# CXXFLAGS = -I$(VAMP_SDK_INCLUDE_DIR) -Wall -fPIC
# PLUGIN_EXT = .so
# PLUGIN = $(PLUGIN_LIBRARY_NAME)$(PLUGIN_EXT)
# LDFLAGS = -shared -Wl,-soname=$(PLUGIN) $(VAMP_SDK_LIB_DIR)/libvamp-sdk.a -Wl,--version-script=vamp-plugin.map

##  Uncomment these for a cross-compile from Linux to Windows using MinGW:
# CXX = i586-mingw32msvc-g++
# CXXFLAGS = -I$(VAMP_SDK_INCLUDE_DIR) -Wall 
# PLUGIN_EXT = .dll
# PLUGIN = $(PLUGIN_LIBRARY_NAME)$(PLUGIN_EXT)
# LDFLAGS = --static-libgcc -Wl,-soname=$(PLUGIN) -shared $(VAMP_SDK_LIB_DIR)/libvamp-sdk.a

##  Uncomment these for OpenSolaris using SunStudio compiler and GNU make:
# CXX = CC
# CXXFLAGS = -G -I$(VAMP_SDK_INCLUDE_DIR) +w -KPIC
# PLUGIN_EXT = .so
# PLUGIN = $(PLUGIN_LIBRARY_NAME)$(PLUGIN_EXT)
# LDFLAGS = -G -h$(PLUGIN) $(VAMP_SDK_LIB_DIR)/libvamp-sdk.a -Qoption ld -Mvamp-plugin.map

##  All of the above
$(PLUGIN): $(PLUGIN_CODE_OBJECTS)
	   $(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o
	rm -f features/*.o
	rm -f support/*.o
