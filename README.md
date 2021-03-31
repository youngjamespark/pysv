# python-pysv
    - Python bindings of sv56demo.c and actlevel.c in ITU-T G.191 (https://www.itu.int/rec/T-REC-G.191/en)
        - Speech Voltmeter and normalize
    - sampling rate conversion (http://shibatch.sourceforge.net/)
    
# Requirements
    - python 3.8 
    - MSVC compiler
    - SWIG

# Build and Install
$ python setup.py install

# Example for narrow band and wide band
    - test.py
        - only working on *.wav
        - when using it, please note that the sampling rate (frequency) should be 16000 or 8000. 
        - calculate(char *filein)
        - normalize(char *src_file, char *dst_file, double target_dB)
# Example for sampling rate conversion
    - sr_test.py
        - only working *.wav
        - samplerate_change(char *src_file, char *dst_file, int target_rate)
        - verified with adobe audition

