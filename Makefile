BUNDLE = sapistaEQ.lv2
INSTALL_DIR = /usr/local/lib/lv2
CFLAGS = -Wall -O3 -fPIC -DPIC

$(BUNDLE): manifest.ttl eq1qm.ttl eq1qm.so eq1qs.ttl eq1qs.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp manifest.ttl eq1qm.ttl eq1qm.so eq1qs.ttl eq1qs.so $(BUNDLE)

all: eq1qm.so eq1qs.so

#EQ1Q Mono=====================================================================================
eq1qm.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 1" > eq_type.h
	echo "#define NUM_BANDS 1" >> eq_type.h
	echo "#define IS_DBUS 0" >> eq_type.h
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq1qm"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c -o eq1qm.so -lm -shared
	
#EQ1Q Stereo====================================================================================
eq1qs.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 2" > eq_type.h
	echo "#define NUM_BANDS 1" >> eq_type.h
	echo "#define IS_DBUS 0" >> eq_type.h
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq1qs"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c -o eq1qs.so -lm -shared

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

clean:
	rm -rf $(BUNDLE) eq_type.h eq1qm.so eq1qs.so 