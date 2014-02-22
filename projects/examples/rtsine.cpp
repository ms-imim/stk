// rtsine.cpp STK tutorial program

#include "SineWave.h"
#include "RtWvOut.h"
#include <cstdlib>

using namespace stk;

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::showWarnings( true );

  int nFrames = 100000;
  SineWave sine;
  RtWvOut *dac = 0;

  try {
    // Define and open the default realtime output device for one-channel playback
    dac = new RtWvOut( 1 );
  }
  catch ( StkError & ) {
    exit( 1 );
  }

  double fStart = 441.0;
  double fStop = 882.0;
  double fStep = (fStop - fStart) / nFrames;
  sine.setFrequency( fStart );

  // Option 1: Use StkFrames
  /*
  StkFrames frames( nFrames, 1 );
  try {
    dac->tick( sine.tick( frames ) );
  }
  catch ( StkError & ) {
    goto cleanup;
  }
  */

  // Option 2: Single-sample computations
  for ( int i=0; i<nFrames; i++ ) {
    try {
		fStart += fStep;
		sine.setFrequency(fStart);
		dac->tick( sine.tick() );
     }
    catch ( StkError & ) {
      goto cleanup;
    }
  }

 cleanup:
  delete dac;

  return 0;
}
