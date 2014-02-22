// crtsine.cpp STK tutorial program
#include <conio.h>
#include "SineWave.h"
#include "RtAudio.h"
using namespace stk;

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
	double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
	SineWave *sine = (SineWave *)dataPointer;
	register StkFloat *samples = (StkFloat *)outputBuffer;

	for (unsigned int i = 0; i<nBufferFrames; i++)
		*samples++ = sine->tick();

	return 0;
}

int main()
{
	// Set the global sample rate before creating class instances.
	Stk::setSampleRate(44100.0);

	SineWave sine;
	RtAudio dac;

	// Figure out how many bytes in an StkFloat and setup the RtAudio stream.
	RtAudio::StreamParameters parameters;
	parameters.deviceId = dac.getDefaultOutputDevice();
	parameters.nChannels = 1;
	RtAudioFormat format = (sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int bufferFrames = RT_BUFFER_SIZE;
	try {
		dac.openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tick, (void *)&sine);
	}
	catch (RtError &error) {
		error.printMessage();
		goto cleanup;
	}
	double f = 440.0;
	double twelveRoot2 = 1.0594630943592952645618252949463;

	sine.setFrequency(f);
	try {
		dac.startStream();
	}
	catch (RtError &error) {
		error.printMessage();
		goto cleanup;
	}

	// Block waiting here.
	int keyhit = 0;
	std::cout << "\nPlaying ... press <esc> to quit.\n";
	while (keyhit != 32 && keyhit != 27)
	{
		keyhit = _getch();
		if (tolower(keyhit) == 'a')
		{
			f = 220.0;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'g')
		{
			f /= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'h')
		{
			f *= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'f')
		{
			for (int i = 0; i < 2; ++i)
				f /= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'j')
		{
			for (int i = 0; i < 2; ++i)
				f *= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'd')
		{
			for (int i = 0; i < 3; ++i)
				f /= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'k')
		{
			for (int i = 0; i < 3; ++i)
				f *= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 's')
		{
			for (int i = 0; i < 4; ++i)
				f /= twelveRoot2;
			sine.setFrequency(f);
		}
		else if (tolower(keyhit) == 'l')
		{
			for (int i = 0; i < 4; ++i)
				f *= twelveRoot2;
			sine.setFrequency(f);
		}
		else
		{
			std::cout << "Freq: " << f << std::endl;
		}
	}

	// Shut down the output stream.
	try {
		dac.closeStream();
	}
	catch (RtError &error) {
		error.printMessage();
	}

cleanup:

	return 0;
}
