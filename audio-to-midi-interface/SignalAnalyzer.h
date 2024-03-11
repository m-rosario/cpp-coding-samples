class SignalAnalyzer {
public:
	SignalAnalyzer(int samplingRate, int samplesPerMessage, int minAmplitudeValue, int lowerDynamicThreshold);

	void analyzeSamples(int samples[]); //Obtain the samples and analyze them to obtain the averagePeakValue and averagePeriod

	float getFrequency(); //Obtain the frequency calculated in the analysis
	int getAmplitude(); //Obtain the amplitude calculated in the analysis
private:
	int* samples; //Array to hold a portion of the input signal for analysis
  	int averagePeakValue; //The average peak value of the analyzed signal
  	int averagePeriod; //The average sample distance between peaks in the analyzed signal

	int samplingRate; //Rate at which the input is sampled
	int samplesPerMessage; //Number of messages to process before returnning response
  	int lowerDynamicThreshold;  //Lower dynamic range threshold. Maps to 0 velocity
  	int minAmplitudeValue; //Center of the input range. Indicates a dead signal (silence)
};
