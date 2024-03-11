#include "SignalAnalyzer.h"
#include "MidiEncoder.h"

const int SAMPLING_RATE = 50000; //More than five times the Nyquist frequency
const int SAMPLES_PER_MESSAGE = 5000; //Fits two periods of the lowest frequency
const float MIN_FREQUENCY = 20.00f; //Slightly below the frequency of the lowest key in a piano
const float MAX_FREQUENCY = 4400.00f; //Slightly above the frequency of the highest key in a piano
const int MIN_AMPLITUDE = 511; //Half the analog input range (zero amplitude, -Infinity dBFS)
const int LOW_THRESHOLD = 543; //-24dBFS from max amplitude
const int HIGH_THRESHOLD = 873; //-3dBFS from max amplitude
const float VELOCITY_MAPPING_CONSTANT = 0.16540f; //Decibel increase per velocity 
const unsigned long TIME_CALIBRATION = 6; //Microseconds to subtract from sampling period to account for the time it takes the Arduino device to execute instructions. Determined through testing

int* samples; //Pointer to samples array
byte previousMidiPitch; //Holds the last MIDI pitch detected
unsigned long microsecondsBetweenSamples; //Number of microseconds between analog signal reads (sampling)
unsigned long nextSamplingTime; //Time when the next sampling must occur in order to ensure constant sampling rate
SignalAnalyzer* signalAnalyzer; //Pointer to SignalAnalyzer object, used to calculate amplitude and frequency
MidiEncoder* midiEncoder; //Pointer to MidiEncoder object, used to encode MIDI pitch and velocity. Sends MIDI NOTE ON and MIDI NOTE OFF messages

//One-time execution to initialize program
void setup() {
    signalAnalyzer = new SignalAnalyzer(SAMPLING_RATE, SAMPLES_PER_MESSAGE, MIN_AMPLITUDE, LOW_THRESHOLD);
    midiEncoder = new MidiEncoder(MIN_AMPLITUDE, LOW_THRESHOLD, HIGH_THRESHOLD, VELOCITY_MAPPING_CONSTANT);
    samples = new int[SAMPLES_PER_MESSAGE];
    microsecondsBetweenSamples = 1000000/SAMPLING_RATE; //1000000 microseconds in a second
    previousMidiPitch = 0;
    //Serial port used for reading and writing at a maximum rate of 115200 baud/s (one of Arduino's preset values)
    Serial.begin(115200);
    //Initialize Arduino built-in LED, used for detecting very high input signals (-3dBFS) and alerting the user
    pinMode(LED_BUILTIN, OUTPUT);
}

//Function that runs in an infinite loop while microcontroller remains powered on.
void loop() {
    //Read samples from ADC pin
    for (int i = 0; i <= SAMPLES_PER_MESSAGE; i++) {
        nextSamplingTime = micros() + microsecondsBetweenSamples;
        //Loop and do nothing until it is time to read the next sample
        while(nextSamplingTime - TIME_CALIBRATION > micros()){}
        samples[i] = analogRead(A0);
    }

    //Analyze samples and process MIDI message
    signalAnalyzer->analyzeSamples(samples);
    int currentSampleArrayAmplitude = signalAnalyzer->getAmplitude();
    if (currentSampleArrayAmplitude >= LOW_THRESHOLD) {
        if(currentSampleArrayAmplitude >= HIGH_THRESHOLD){
          //Turn on the Arduino LED to indidicate a high input signal
          digitalWrite(LED_BUILTIN, HIGH);
        }
        else{
          //Turn off the Arduino LED, or keep off if it wasn't on already
          digitalWrite(LED_BUILTIN, LOW);
        }
        float currentSampleArrayFrequency = signalAnalyzer->getFrequency();
        if (currentSampleArrayFrequency >= MIN_FREQUENCY && currentSampleArrayFrequency <= MAX_FREQUENCY) {
          byte pitch = midiEncoder->mapFrequencyToMidiPitch(currentSampleArrayFrequency);
          byte velocity = midiEncoder->mapAmplitudeToMidiVelocity(currentSampleArrayAmplitude);
          //The MIDI NOTE ON message is sent if and only if the current pitch matches the previously detected pitch.
          //This helps avoid jitter and noise, since it ensures that messages are sent only when the detection is consistent
          if(pitch == previousMidiPitch){
            midiEncoder->sendNoteOn(pitch, velocity);
          }
          else{
            previousMidiPitch = pitch;
          }
        }
    }
    else {
        midiEncoder->sendNoteOff();
        previousMidiPitch = 0; //Reset to indicate that there is no pitch
        //Turn off the Arduino LED, or keep off if it wasn't on already
        digitalWrite(LED_BUILTIN, LOW);
    }
}
