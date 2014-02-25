// bethree.cpp STK tutorial program
#include <conio.h>
#include "JCRev.h"
#include "Chorus.h"
#include "Moog.h"
#include "RtAudio.h"
using namespace stk;

// The TickData structure holds all the class instances and data that
// are shared by the various processing functions.
struct TickData {
	Instrmnt *instrument;
	JCRev reverb;
	Chorus chorus;
	StkFloat frequency;
	StkFloat volume;
	StkFloat t60;
	long counter;
	bool done;

	// Default constructor.
	TickData()
		: instrument(0), volume(1.0), counter(0), t60(0.5), done(false) {}
};

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *userData)
{
	TickData *data = (TickData *)userData;
	register StkFloat *samples = (StkFloat *)outputBuffer;

	for (unsigned int i = 0; i<nBufferFrames; i++) 
	{
		*samples++ = data->volume * data->reverb.tick(data->chorus.tick(data->instrument->tick()));
	}

	if (data->counter > 80000)
		//data->done = true;

	return 0;
}

int main()
{
	// Set the global sample rate and rawwave path before creating class instances.
	Stk::setSampleRate(44100.0);
	//::setRawwavePath("../../rawwaves/");

	TickData data;
	RtAudio dac;

	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		dac.openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&data);
	}
	catch (RtError& error) {
		error.printMessage();
		goto cleanup;
	}

	try {
		// Define and load the instrument
		data.instrument = new Moog();
		Moog * pMoog = dynamic_cast<Moog*>(data.instrument);
		// pMoog->setModulationDepth(0.125);
		// pMoog->setModulationSpeed(1.0);

		// Set the reverb parameters.
		data.reverb.setT60(data.t60);
		data.reverb.setEffectMix(0.1);
		data.chorus.setModDepth(0.005);
		data.chorus.setModFrequency(5.0);
		data.chorus.setEffectMix(0.5);

	}
	catch (StkError &) {
		goto cleanup;
	}
	double twelveRoot2 = 1.0594630943592952645618252949463;
	data.frequency = 220.0;
	data.instrument->noteOn(data.frequency, 0.5);

	try {
		dac.startStream();
	}
	catch (RtError &error) {
		error.printMessage();
		goto cleanup;
	}

	// Block waiting until callback signals done.
	int keyhit = 0;
	std::cout << "\nPlaying ... press <esc> to quit.\n";
	std::cout << "[s] -4 semitone" << std::endl;
	std::cout << "[d] -3 semitone" << std::endl;
	std::cout << "[f] -2 semitone" << std::endl;
	std::cout << "[g] -1 semitone" << std::endl;
	std::cout << "[h] +1 semitone" << std::endl;
	std::cout << "[j] +2 semitone" << std::endl;
	std::cout << "[k] +3 semitone" << std::endl;
	std::cout << "[l] +4 semitone" << std::endl;
	std::cout << std::endl;
	std::cout << "[a] reset to 220 Hz" << std::endl;
	std::cout << std::endl;
	std::cout << "[other keys] display current frequency" << std::endl;
	while (keyhit != 32 && keyhit != 27)
	{
		keyhit = _getch();
		if (tolower(keyhit) == 'a')
		{
			data.frequency = 220.0;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'g')
		{
			data.frequency /= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'h')
		{
			data.frequency *= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'f')
		{
			for (int i = 0; i < 2; ++i)
				data.frequency /= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'j')
		{
			for (int i = 0; i < 2; ++i)
				data.frequency *= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'd')
		{
			for (int i = 0; i < 3; ++i)
				data.frequency /= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'k')
		{
			for (int i = 0; i < 3; ++i)
				data.frequency *= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 's')
		{
			for (int i = 0; i < 4; ++i)
				data.frequency /= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else if (tolower(keyhit) == 'l')
		{
			for (int i = 0; i < 4; ++i)
				data.frequency *= twelveRoot2;
			data.instrument->setFrequency(data.frequency);
		}
		else
		{
			std::cout << "Freq: " << data.frequency << std::endl;
		}
	}

	// Shut down the callback and output stream.
	try {
		dac.closeStream();
	}
	catch (RtError &error) {
		error.printMessage();
	}

cleanup:
	delete data.instrument;

	return 0;
}
