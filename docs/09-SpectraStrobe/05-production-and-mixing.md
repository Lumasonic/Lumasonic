### How SpectraStrobe Works
In SpectraStrobe all `control tones` are high frequency `sine` waves (including the [reference tone](spectrastrobe-format-specification#reference-tone)). 

During playback of the SpectraStrobe content special filters are used to isolate the color control
tones in their respective frequency bands: `18200 Hz`, `18700 Hz`, `19200 Hz`, `19700 Hz`

For the color tones, the amplitude is used to control the respective brightness of the LEDs on the AVS device. 

***Note:** The reference tone at 18200 Hz does not directly control brightness, 
but indicates to the playback device that SpectraStrobe content is present.*

For the color control tones `18700 Hz`, `19200 Hz`, `19700 Hz`, the amplitude of the signals in relation to
the amplitude of the reference tone `18200 Hz` determines the brightness of each LED channel.

---

### Creating SpectraStrobe Signals

There are three main methods of creating a SpectraStrobe mix:

1. Use a SpectraStrobe-specific audio plug-in, such as [PrismVST](../03-Prism-VST/01-prism-plugin-overview.md)
2. Use a tone generator to generate sine wave tones at the appropriate frequency and volume that match the duration of
   your entire mix and place these into an audio track within your DAW.
3. Use smaller forward-looped samples of each tone frequency, place them into a [sampler](https://en.wikipedia.org/wiki/Sampler_(musical_instrument))
   (software or hardware) and trigger their natural note (usually C-3) using MIDI or equivalent to add the
   tones to your mix

For the most turnkey approach, favor a SpectraStrobe capable audio plug-in, and choose method 1.

If you are more comfortable working with larger bounced audio tracks in a multi-track project, choose method 2.

If you would rather keep file size to a minimum and are more comfortable working with digital samplers, choose
method 3.

***Note:** With any method, the SpectraStrobe reference tone must have [the appropriate panning dynamics](spectrastrobe-format-specification#reference-tone)
applied otherwise the AVS device will not recognize your recording as SpectraStrobe and will fall back to AudioStrobe instead.
SpectraStrobe plug-ins such as [PrismVST](../03-Prism-VST/01-prism-plugin-overview.md) will take care of this for you automatically.*

The DAW you use will determine the best ways to manipulate the color control tracks' volume and panning
to control the LED signals. This can often be achieved through track automation by editing control envelopes
of the track's volume and panning. Your DAW also might allow you to apply [LFO](https://en.wikipedia.org/wiki/Low-frequency_oscillation)
control over track volume and panning.


---

### Audio Mixing Theory

Digital [audio mixing](https://en.wikipedia.org/wiki/Audio_mixing) usually consists of a piece of software
or equipment known as a [Digital Audio Workstation (DAW)](https://en.wikipedia.org/wiki/Digital_audio_workstation).

#### Adding SpectraStrobe to the Mix
To add SpectraStrobe information to an audio mixing session, one should create separate stereo tracks for each
tone required.

***Note:** If you are using a plug-in like [PrismVST](../03-Prism-VST/01-prism-plugin-overview.md), only one track is required.*

Except for the SpectraStrobe reference tone, varying the volume of the track will
vary the brightness of the corresponding LED channel on the AVS device, and varying the panning of the track
will vary the left/right eye balance of the corresponding LED channel.

The following tables demonstrate the basic LED control theory for each tone track in the mix:

| Track Volume Level (dB) | LED Brightness |
|-------------------|--------------------|
| -INF dB | LED color channel is completely off |
| reference tone dB level | LED color channel is completely on; full brightness |

| Track Panning Balance (%) | LED Balance |
|-------------------|--------------------|
| 100% Left | LED color channel only displays on left eye; right eye off |
| 0% (centered pan) | LED color channel displays equally on left and right eye |
| 100% Right | LED color channel only displays on right eye; left eye off|

***Note:** These tables are only used to indicate the extremes of the range. As volume and panning are altered,
the corresponding LED values (brightness or left/right eye balance) will gradually change in respect to the
chosen level.*

---

### Headroom Considerations

During audio mixing, all of the independent audio tracks will be summed into the final stereo mix. One must be
aware of the available [headroom](https://en.wikipedia.org/wiki/Headroom_(audio_signal_processing)) in the overall
mix. If the final audio signal level reaches or exceeds 0 dB on either left or right channel it produces
digital clipping, which is a form of digital distortion, and is generally considered to be undesirable.

***Note:** Mixing SpectraStrobe content can be deceiving because a certain amount of headroom will be consumed by the SpectraStrobe
control tones. Even though the tones will be essentially inaudible, make sure to consider the headroom implications
for the rest of your mix. The SpectraStrobe tones will behave as 4 stereo tracks, each with output between -32 dB and -24 dB
when fully on.*

***Note:** One way to achieve proper signal integrity is to EQ or filter all non-SpectraStrobe tracks so that 
frequencies starting in the range of 13 KHz and ending at 17 KHz begin to taper off, with the steepest cutoff at 
17 KHz. This leaves the frequency space for 18 KHz and above free for SpectraStrobe control signals without
interference from the non-SpectraStrobe tracks.*

---

### Equalization

Since SpectraStrobe inserts the visual control signals into the audio recording itself, it is
possible for unwanted frequency energy to "bleed" into the control and reference frequency bands causing unwanted
glitches and imprecise control of each color channel on the LEDs. This effect can be minimized using proper
track equalization (EQ) and filtering. In general, the steeper the equalization or filters, the better, as it
will minimize this frequency-based interference.

Ideally you would use the steepest (highest order/highest dB cut) [bandpass filter](https://en.wikipedia.org/wiki/Band-pass_filter)
available in your DAW on each SpectraStrobe tone track. You can also create your own bandpass filter by combining both a [lowpass filter](https://en.wikipedia.org/wiki/Low-pass_filter)
and a [highpass filter](https://en.wikipedia.org/wiki/High-pass_filter) together in series on each track,
and setting their cutoff frequency to the frequency of the respective tones. Keeping Q/resonance at 0 is
recommended as it may unintentionally amplify the energy around the filter cutoff point.

| Audio Track | Suggested Filter |
|-------------|------------------|
| Reference Tone | bandpass (or combined lowpass & highpass) w/ cutoff @ 18.2 KHz |
| Red Tone |  bandpass (or combined lowpass & highpass) w/ cutoff @ 18.7 KHz |
| Green Tone |  bandpass (or combined lowpass & highpass) w/ cutoff @ 19.2 KHz |
| Blue Tone |  bandpass (or combined lowpass & highpass) w/ cutoff @ 19.7 KHz |

***Note:** If creating an AudioStrobe track it corresponds to the green color tone @ 19.2 KHz.*

#### Equalization and Filtering of Audible Tracks

As mentioned in [headroom considerations](#headroom-considerations), if you find that the audible portions
of your mix (i.e. the music/sound parts) appear to be interfering with the control tones' frequency band,
then you can place an EQ or [lowpass filter](https://en.wikipedia.org/wiki/Low-pass_filter) such that frequencies
above 17 KHz are steeply removed. This will sacrifice some high frequency presence from the mix, but the
benefit will be a cleaner signal for the SpectraStrobe control tones. If using an
equalizer, you might want to slowly start cutting frequency content around 13 KHz with growing steepness
until about 17 KHz where the cut should be as steep as your equalizer offers.

#### Computer System Audio Equalization
Some computers have factory-installed or user-installed audio processing software that runs in the
background and equalizes and processes all system audio. This software can sometimes affect
the integrity of SpectraStrobe content. When creating or listening to SpectraStrobe content on
a computer, one should either temporarily disable or permanently uninstall such software. 