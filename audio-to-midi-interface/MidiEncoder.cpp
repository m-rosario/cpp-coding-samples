#include "MidiEncoder.h"
#include "math.h"
#include "Arduino.h"

MidiEncoder::MidiEncoder(int minAmplitudeValue, int lowerDynamicThreshold, int upperDynamicThreshold, float velocityMappingConstant) {
	this->lowerDynamicThreshold = lowerDynamicThreshold;
	this->upperDynamicThreshold = upperDynamicThreshold;
	this->minAmplitudeValue = minAmplitudeValue;
	this->velocityMappingConstant = velocityMappingConstant;
  	//Fill the pitch to frequency map by calculating the frequency for each pitch of interest. The offset is necessary to match the array indices with the MIDI pitch numbering system.
	for (int p = 0; p < PITCHES; p++) {
		pitchToFrequencyMap[p] = 440 * pow(2, (p + MIDI_PITCH_OFFSET - 69) / 12.0);
	}
}

//Prepare and send a MIDI NOTE ON message for given frequency and amplitude
// midiEventPacket_t parameters: {Status, Status Byte, Data Byte 1, Data Byte 2}
void MidiEncoder::sendNoteOn(byte pitch, byte velocity) {
  	//If there are other active pitches turn them off
	if (currentlyActivePitch != -1) {
		if (currentlyActivePitch != pitch) {
			sendNoteOff();
		}
		else {
			return; //Same pitch is already on, return and leave the note on
		}
	}
  Serial.print("Sending NOTE ON to MIDI pitch: "); Serial.print(pitch); Serial.print(" | velocity: "); Serial.println(velocity); //For logging and debugging
	midiEventPacket_t noteOn = { 0x09, 0x90, pitch, velocity};
	MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
	currentlyActivePitch = pitch;
}

//Prepare and send a MIDI NOTE OFF message for currently active note
// midiEventPacket_t parameters: {Status, Status Byte, Data Byte 1, Data Byte 2}
void MidiEncoder::sendNoteOff() {
	if (currentlyActivePitch == -1) return; //Already off
  Serial.print("Sending NOTE OFF to MIDI pitch: "); Serial.println(currentlyActivePitch); //For logging and debugging
	midiEventPacket_t noteOff = { 0x08, 0x80, currentlyActivePitch, 0x00 };
	MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
	currentlyActivePitch = -1;
}

byte MidiEncoder::mapFrequencyToMidiPitch(float frequency) {
	for (int p = 1; p < PITCHES; p++) {
		if (pitchToFrequencyMap[p] >= frequency && frequency >= pitchToFrequencyMap[p - 1]) {
			//Corresponding pitch between p and p-1. Find the closest match
      		//Use the MIDI Pitch to Frequency formula and to calculate the frequency of p - 0.5, which maps to the midpoint frequency between p and p-1
      		double midPointFrequency = 440 * pow(2, (p - 0.5 + MIDI_PITCH_OFFSET - 69) / 12.0);
			if (midPointFrequency > frequency) {
				return (byte) (p - 1 + MIDI_PITCH_OFFSET); //Return the lower pitch
			}
			else{
				return (byte) (p + MIDI_PITCH_OFFSET); //Return the higher pitch
			}
		}
	}
	return (byte) -1; //Return invalid pitch
}

byte MidiEncoder::mapAmplitudeToMidiVelocity(int amplitude) {
	if (amplitude >= this->upperDynamicThreshold) {
		return (byte) 127; //Max velocity
	}
	else if (amplitude <= this->lowerDynamicThreshold) {
		return (byte) 0; //Min velocity
	}
	else {
    	//Amplitude between thresholds, use the decibel scale and the mapping constant to calculate velocity   
    	float velocity = (20.0 * log10((float)amplitude - (float)this->minAmplitudeValue) - 20.0 * log10((float)this->lowerDynamicThreshold - (float)this->minAmplitudeValue)) / this->velocityMappingConstant;
		return (byte) velocity;
	}
}
