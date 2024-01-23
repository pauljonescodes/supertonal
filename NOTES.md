# Notes

## Monday, January 22, 2024 10:55:35 AM

- Sync BPM
- Select IR
- Reverb/Chorus/Phaser wet/dry
- EQ parameters

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