### Supported Stereo Formats

| Precision | File Format | Bit Depth | Pro | Con |
|----------|--------------|-----------|-----|-----|
| Lossless | .wav | 16 bit | accurate/uncompressed playback | large file size |
| Lossy | .mp3 | 16 bit | smaller file size | possible compression artifacts |

***Note:** All final mixes should be stereo.*

---

### Wave Files

**Recommended Settings: 44100 hz or 48000 hz, 16 bit**

[Wave files (.wav)](https://en.wikipedia.org/wiki/WAV) use uncompressed audio and offer perfect playback of the
original digital recordings but have the largest file size.

***Note:** Although it is possible to have 32 bit wave files, 16 bit is recommended for compatibility
when playing back from the Kasina directly.*

---

### MP3 Files

**Recommended Settings: 44100 hz or 48000 hz, 16 bit, [320 Kbps constant bit rate (CBR)](https://en.wikipedia.org/wiki/Constant_bitrate)**

[MP3 files (.mp3)](https://en.wikipedia.org/wiki/MP3) use varying methods of audio compression to achieve smaller
file sizes at the cost of introducing inaccuracies during audio playback. As long as a `constant bit rate of 320 Kbps` is used, the compression artifacts introduced should be completely neglibable both from a listening perspective and from an LED control signal perspective.

***Note:** Although it is possible to use both variable bit rate (VRB) and higher compression settings (like 128 Kbps, etc.)
this is not recommended for use with SpectraStrobe as this will introduce too many sample
inaccuracies during playback and likely lead to visual control errors or failure to detect visual control in general.*
