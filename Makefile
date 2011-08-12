BUNDLE = sapistaEQ.lv2
INSTALL_DIR = /usr/local/lib/lv2
CFLAGS = -Wall -O3 -fPIC -DPIC -ffast-math

$(BUNDLE): manifest.ttl eq1qm.ttl eq1qm.so eq1qs.ttl eq1qs.so \
	   		eq4qm.ttl eq4qm.so eq4qs.ttl eq4qs.so \
			eq6qm.ttl eq6qm.so eq6qs.ttl eq6qs.so \
			eq10qm.ttl eq10qm.so eq10qs.ttl eq10qs.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp manifest.ttl eq1qm.ttl eq1qm.so eq1qs.ttl eq1qs.so \
	   		eq4qm.ttl eq4qm.so eq4qs.ttl eq4qs.so \
			eq6qm.ttl eq6qm.so eq6qs.ttl eq6qs.so \
			eq10qm.ttl eq10qm.so eq10qs.ttl eq10qs.so \
			$(BUNDLE)

all:	eq1qm.so eq1qs.so \
	eq4qm.so eq4qs.so \
	eq6qm.so eq6qs.so \
	eq10qm.so eq10qs.so

#EQ1Q Mono=====================================================================================
eq1qm.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 1" > eq_type.h
	echo "#define NUM_BANDS 1" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq1qm"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c dsp/smooth.c -o eq1qm.so -lm -shared
	
#EQ1Q Stereo====================================================================================
eq1qs.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 2" > eq_type.h
	echo "#define NUM_BANDS 1" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq1qs"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq1qs.so -lm -shared

#EQ4Q Mono=====================================================================================
eq4qm.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 1" > eq_type.h
	echo "#define NUM_BANDS 4" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq4qm"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq4qm.so -lm -shared

#EQ4Q Stereo=====================================================================================
eq4qs.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 2" > eq_type.h
	echo "#define NUM_BANDS 4" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq4qs"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq4qs.so -lm -shared

#EQ6Q Mono=====================================================================================
eq6qm.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 1" > eq_type.h
	echo "#define NUM_BANDS 6" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq6qm"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq6qm.so -lm -shared

#EQ6Q Stereo=====================================================================================
eq6qs.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 2" > eq_type.h
	echo "#define NUM_BANDS 6" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq6qs"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq6qs.so -lm -shared

#EQ10Q Mono=====================================================================================
eq10qm.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 1" > eq_type.h
	echo "#define NUM_BANDS 10" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq10qm"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq10qm.so -lm -shared

#EQ10Q Stereo=====================================================================================
eq10qs.so: eq.c dsp/filter.c dsp/vu.c
	#Step1: Generate the header file
	echo "#define NUM_CHANNELS 2" > eq_type.h
	echo "#define NUM_BANDS 10" >> eq_type.h
	
	echo "#define EQ_URI      "'"http://eq10q.sourceforge.net/eq/eq10qs"'";" >> eq_type.h
	
	#Setp2: Compile the plugin core
	gcc  $(CFLAGS) eq.c dsp/filter.c dsp/vu.c  dsp/smooth.c -o eq10qs.so -lm -shared

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

clean:
	rm -rf $(BUNDLE) eq_type.h eq1qm.so eq1qs.so eq4qm.so eq4qs.so eq6qm.so eq6qs.so eq10qm.so eq10qs.so