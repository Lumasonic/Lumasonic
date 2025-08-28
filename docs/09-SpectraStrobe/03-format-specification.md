### Overview

AudioStrobe and SpectraStrobe take a similar approach to encoding and decoding visual control signals from
within audio recordings, however AudioStrobe only encodes a single stereo control signal, where as SpectraStrobe
encodes three separate stereo control signals which correspond to individual [red, green, and blue
LEDs](https://en.wikipedia.org/wiki/Light-emitting_diode).

For encoding, high frequency [sine wave](https://en.wikipedia.org/wiki/Sine_wave) tones are added into
the audio content of the recording. Decoding involves using specialized filters during playback to isolate these
high frequency tones from the rest of the audio content, which are then used to control the brightness levels
of the AVS device's LEDs.

The brightness of the LEDs is based on the [amplitude](https://en.wikipedia.org/wiki/Amplitude) (volume) of
the corresponding high frequency control tone(s).

---

### Control Tone Frequencies

The following table lists the various control tone frequencies:

| Format | Frequency | Purpose |
|--------|-----------|---------|
| AudioStrobe | 19200 Hz | controls the brighness of the LEDs; does not specify LED color |
| SpectraStrobe | 18200 Hz | reference tone used to indicate the SpectraStrobe format |
| SpectraStrobe | 18700 Hz | controls the brightness of the red LEDs |
| SpectraStrobe | 19200 Hz | controls the brightness of the green LEDs |
| SpectraStrobe | 19700 Hz | controls the brightness of the blue LEDs |

***Note**: The SpectraStrobe reference tone at 18200 Hz also requires a specific stereo panning pattern to be
applied in order for it to be correctly identified as a SpectraStrobe signal.*

---

### Reference Tone

Since both AudioStrobe and SpectraStrobe share a frequency space (19200 Hz), in order for an AVS device
to be able to detect which type of material it is playing back, a reference tone must be added at 18200 Hz
to indicate the presence of SpectraStrobe. 

A tone is also a sine wave but requires a special [stereo panning](https://en.wikipedia.org/wiki/Panning_(audio))
pattern to initiate detection.

The SpectraStrobe 18200 Hz reference tone must be panned hard left/right (100% left then 100% right) in a
repeating pattern at a rate between 10-50 cycles per second (Hz). Using audio mixing metaphors, and speaking
with the terminology of a [Low Frequency Oscillator (LFO)](https://en.wikipedia.org/wiki/Low-frequency_oscillation),
this would be interpreted as:

`waveform: sine, frequency: 20 Hz, offset: 0%, amplitude: 100%`

which would be applied to the stereo panning of the SpectraStrobe reference tone track within the mix.

***Note**: A different panning LFO waveform can be used, such as square or pulse, but since these create
more audible artifacts in the audio, and the SpectraStrobe decoder is somewhat forgiving, a sine wave will
avoid any audible artifacts from the reference tone.*