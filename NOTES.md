# Notes

## Friday, January 26, 2024 12:15:14 PM

- Input gain 
- Meter
  - https://github.com/ffAudio/ff_meters/blob/master/LevelMeter/LevelMeterSource.h
- Tuner
  - https://github.com/Chowdhury-DSP/BYOD/blob/main/src/processors/utility/Tuner.cpp
- Noise gate
  - Should have on and off/Bypass
  - Attack should start at **5ms**
  - Remove ratio **(Infinite)**
  - Release **100**

- Stages
  - Pedals
    - Pre
      - Comp (pedal)
      - Real life TS, unity is 12db, add high shelf
      - Real life RAT, all right is off, high pass
      - GE-7 (100,  200,  400,  800,  1.6k, 3.2k, 6.4k, Level, gain +/- 15db)
      -   Q: (2.83, 2.33, 2.45, 2.77, 2.09, 3.46, [low pass])

    - FX loop
      - Delay
      - Chorus (no mix, on/off)
      - Flanger

  - Amp
    - Gain (input gain to function, boost)
    - Resonance (low shelf, 100hz, -6 to 6)
    - Bass (low notch, 200hz, 1.5Q, -6 to 6)
    - Middle (mid notch, 1000hz, 1.5Q, -6 to 6)
    - Treble (high notch, 5000hz, 1.5Q, -6 to 6)
    - Prescence (high shelf, 10000hz, -6 to 6 )
    - Master (output gain from function, cut)

  - Cab

  - Post mixer
    - Comp (unit) + meter + pre/post
      - https://github.com/p-hlp/CTAGDRC/blob/master/Source/PluginEditor.cpp
    - HPF + bypass
    - 20 -> 500 + Q + Gain + bypass
    - 125 -> 4k + Q + Gain + bypass
    - 500 - 8k + Q + Gain + bypass
    - 2k -> 20k + Q + Gain + bypass
    - LPF + bypass
- Output

- Undo

## Monday, January 22, 2024 10:55:35 AM

- Select IR

```C++
volume = fmax(-54 * pow(distortion, 2) - 32 * distortion + 3, -24)
```

## Sunday, January 21, 2024 6:57:15 PM

- **Pre-amp Stage**
  - Noise Gate is applied if enabled.
  - Pre-compression is applied if configured. *(Blend for parallel compression?)*
  - Tube Screamer effect is processed if enabled.
  - Mouse Drive effect is processed if enabled.

- **Amp Stages (if enabled, serial or parallel)**
  - For each stage (1 to 4):
    - Input Gain, Wave Shaping, and Output Gain are applied.
    - Dry/Wet Mixing is performed.

- **Amp Post Processing**
  - Apply Bias Processing.
  - Apply Post Compression with Gain. *(Blend for parallel compression?)*
  - High Pass Filter is applied if enabled.
  - Mid Peak Filter is applied if enabled.
  - High Shelf Filter is applied if enabled.
  - Low Pass Filter is applied if enabled.

- **Amp FX Loop Processing**
  - Delay effects are processed per channel.
  - Chorus effect is processed if configured.
  - Phaser effect is processed if configured.

- **Room Processing**
  - Cabinet Impulse Response Convolution is applied if enabled.
  - Reverb is applied.
  - Output Gain is adjusted.
  - Limiter is applied at the end.