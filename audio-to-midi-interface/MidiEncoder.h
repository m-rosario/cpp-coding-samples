#include "MIDIUSB.h"

class MidiEncoder {
public:
	MidiEncoder(int minAmplitudeValue, int lowerDynamicThreshold, int upperDynamicThreshold, float velocityMappingConstant);
	void sendNoteOn(byte pitch, byte velocity); //Encode MIDI pitch and velocity, then send MIDI NOTE ON message
	void sendNoteOff(); //Send MIDI NOTE OFF to any active note
  	byte mapFrequencyToMidiPitch(float frequency); //Utility function to calculte MIDI pitch given the frequency
  	byte mapAmplitudeToMidiVelocity(int amplitude); //Utility funciton to calculate velocity given the amplitude
private:	
	const int PITCHES = 88; //Notes in the piano. Pitches of interest
	const int MIDI_PITCH_OFFSET = 21; //Offset between the pitch map used here and the MIDI pitch numbering

	int currentlyActivePitch = -1; //Stores the pitch that is currently on (NOTE ON)
	int lowerDynamicThreshold;  //Lower dynamic range threshold. Maps to 0 velocity
	int upperDynamicThreshold; //Upper dynamic range threshold. Maps to 127 velocity
	int minAmplitudeValue; //Center of the input range. Indicates a dead signal (silence)
	float velocityMappingConstant; //Decibel increase per velocity

	float pitchToFrequencyMap[88]; //88 elements hold the frequencies corresponding to the 88 pitches of the piano
};
