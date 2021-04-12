# Audio Manipulation Program
An audio manipulation program written in C++ using catch.hpp for testing

1) To build:
Run **make** in root project folder

For testing with catch.hpp:
Run **make test** in root project folder

2) Sample audio files have been provided in **sample_inpu.zipt**. These should be used for the program's invocation as described below:

3) Arguments and program invocation:
 
**samp -r sampleRateInHz -b bitCount -c noChannels [-o outFileName ] [<ops>] soundFile1 [soundFile2]
Description**

• -r Specifies the number of samples per second of the audio file(s) (usually 44100)

• -b Specifies the size (in bits) of each sample. Only 8bit and 16bit are supported in this program. More on this later on.

• -c Number of channels in the audio file(s). This program only supports 1 (mono) or 2 (stereo).

• “outFileName” is the name of the newly created sound clip (defaults to “out”).

• <ops> is ONE of the following:
  • -add: add soundFile1 and soundFile2
  • -cut r1 r2: remove samples over range [r1,r2] (inclusive) (assumes one sound file)
 
  • -radd r1 r2 s1 s2 : add soundFile1 and soundFile2 over sub-ranges indicated (in seconds). The ranges must be equal in length.

  • -cat: concatenate soundFile1 and soundFile2
  
  • -v r1 r2: volume factor for left/right audio (def=1.0/1.0) (assumes one sound file)
  
  • -rev: reverse sound file (assumes one sound file only)
  
  • -rms: Prints out the RMS of the sound file (assumes one sound file only).
  
  • -norm r1 r2: normalize file for left/right audio (assumes one sound file only and that r1 and r2 are floating point RMS values).
  
  • -fadein n: n is the number of seconds (floating point number) to slowly increase the volume (from 0) at the start of soundFile1 (assumes one sound file).

  • -fadeout n: n is the number of seconds (floating point number) to slowly decrease the volume (from 1.0 to 0) at the end of soundFile1 (assumes one sound file).
  
• “soundFile1” is the name of the input .raw file. A second sound file is required for some operations as indicted above.
  
The sample rate, bit count and number of channels should be used for both the input files and the resulting output file.

**Input:**
The format of the input .raw audio files is simply a stream of samples (a binary file). If you know the size of each element (8/16 bit and number of channels), and the size of the file (using seekg and tellg as done here: http://www.cplusplus.com/reference/istream/istream/tellg/) you can tell how many samples is contained in the file.
