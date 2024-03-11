#include "SignalAnalyzer.h"
#include "Arduino.h"

SignalAnalyzer::SignalAnalyzer(int samplingRate, int samplesPerMessage, int minAmplitudeValue, int lowerDynamicThreshold) {
	this->samplingRate = samplingRate;
	this->samplesPerMessage = samplesPerMessage;
	this->minAmplitudeValue = minAmplitudeValue;
	this->lowerDynamicThreshold = lowerDynamicThreshold;
}

//Receive the samples array and process it to find the peaks. Amplitude and frequency can be calculated with the peak indices.
void SignalAnalyzer::analyzeSamples(int samples[]) {
	this->samples = samples;
  //Set to -1 to indicate null values
	averagePeakValue = minAmplitudeValue;
  averagePeriod = -1;
  //Local variables to keep track of peaks found
  int previousPeakIndex = -1;
  int currentPeakIndex = -1;
  int peaksFound = 0;
  //This flag indicates that the signal has dropped below the zero amplitude value at least once. Peaks are located between zero crossings
	bool zeroCrossed = false;
  //Traverse samples and find peak between zero crossings
	for (int i = 0; i < samplesPerMessage; i++) {
		if (!zeroCrossed) {
      //Wait until the signal drops below the minimum amplitude (zero crossing)
			if (samples[i] <= minAmplitudeValue) {
        //First zero crossing occurred. Begin searching for the first peak value
				zeroCrossed = true;
        currentPeakIndex = i;
			}
		}
		else {
      //Search for next peak value
			if (samples[i] > samples[currentPeakIndex]) {
        //New local maximum found
				currentPeakIndex = i;
			}
			else if (samples[i] <= minAmplitudeValue && samples[currentPeakIndex] >= lowerDynamicThreshold) {
        //Second zero crossing occurred. Process the peak that was found
        peaksFound++;
        if(peaksFound == 1){
          //First peak found, initialize avaragePeakValue to this peak's value
          averagePeakValue = samples[currentPeakIndex];
        }
        else if(peaksFound == 2){
          //Second peak found. The averagePeriod is initialized as the difference between peaks
          averagePeakValue = (averagePeakValue + samples[currentPeakIndex])/2;
          averagePeriod = currentPeakIndex - previousPeakIndex;
        }
        else {
          //Beyond second peak. Values and periods are averaged
          averagePeakValue = (averagePeakValue + samples[currentPeakIndex])/2;
          averagePeriod = (averagePeriod + currentPeakIndex - previousPeakIndex)/2;
        }
        //Done with previousPeakIndex, replace with current and begin searching for the next
        previousPeakIndex = currentPeakIndex;
        currentPeakIndex = i;
        //This zero crossing counts as the first zero crossing for the next iteration, hence zeroCrossed remains true
			}
		}
	}
}

float SignalAnalyzer::getFrequency() {
  //If the average period could not be calculated return -1.0f to indicate invalid frequency
	if (averagePeriod == -1) return -1.0f;
  //Else calculate frequency from period
	return samplingRate / averagePeriod;
}

int SignalAnalyzer::getAmplitude() {
  return averagePeakValue;
}
