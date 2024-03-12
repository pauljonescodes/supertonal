# Notes

## Sun Feb 11 18:59:58 EST 2024

**Noise Gate:**

I know we simplified the noise gate for this build, but the parameters aren’t working GREAT. Would it be possible to have the parameters visible again in the next build and I can attempt to fine tune them better?

_(Paul: Added to “Hidden” tab)_

**Pedal Compressor:**

- Would be awesome if there was a meter to display the gain reduction

_(Soon)_

- Auto make-gain would be a great user friendly feature

_(Check)_

- Ratio can be fixed to whole numbers (ex: 2:1, 3:1, 4:1 instead of 3.27:1)

_(Check. Note: big jumps in ratio can result in sudden changes in amplitude and “pops”. In the production version, I likely need to “smooth” between 1 and 2, etc, even if we only allow an interval of a whole number. There are other instances of values that likely need smoothing, but because it’s a minor inconvenience for me to configure and I’m not sure what needs smoothing or what’ll ultimately make it to the final version, I have not bothered yet. If you hear bad sounds when setting any values, just let me know and I’ll smooth it for next time.)_

- “Blend” can be renamed to “Mix” as it’s a more adopted term among the industry and we can keep that consistent in our products

_(Check)_

**Delay Pedal:**

-Time should be in “ms” with a switch that changes it to a tempo sync (ex: ¼)

(Check. Note, because I am lazy, the tempo sync is currently in “denominator” form, meaning the value you select will be in the denominator of ½, ¼, 1/x etc)

- Low Pass Filter

_(Check)_

- High Pass Filter

_(Check, note Q of 0.7 right now, not configurable)_

- Modulation

(Something tells me it’d be cool to have a phaser on the left echo channel and chorus on the right, currently not implemented but would be down if you agree. Also of note, if you have the outputs configured to mono you do not get the left echo.)

- Independent L and R channel TIME control (ex: L channel = ¼ R channel = ½)

_(Check)_

In the name of making this a “One Stop Shop” guitar plug-in, the LPF/HPF + Modulation will give it the flexibility of being a clean exact replication delay OR a “Tape Delay”

This will encourage the user to use the onboard delay VS waves h-delay, soundtoys echoboy, etc

**Chorus: **

I know if I’m doing something wrong, but I can’t get it to manipulate the sound at all (yes it’s on, lol)

_(A meme in the programmer world: **It works on my machine**. I suspect it’s because the old default “mix” for phaser and chorus was 0, I’ve added it to the “hidden parameters tab”, though it should probably be removed, my hoarder instinct says ‘what if I need this’.)_

**Phaser Pedal:**

I know if I’m doing something wrong, but I can’t get it to manipulate the sound at all (yes it’s on, lol)

_(Ibid)_

**Reverb Pedal:**

It sounds pretty good. It’s a bit of a one trick pony.

_(I’m just happy it’s got a trick.)_

**EQ Low Pass Filter:**

- Is it possible to have a Q for this? I feel it’s really aggressive right now. If you’re able to give me a Q, I can dial it in and we can hide it from the end user.

_(Check. Note that because this parameter didn’t previously exist, is now 0, and was previously 0.7, this will result in a different sound upon default load.)_

**EQ High Pass Filter:**

- Is it possible to have a Q for this? I feel it’s really aggressive right now. If you’re able to give me a Q, I can dial it in and we can hide it from the end user.

_(Ibid)_

**EQ Section:**

- Can we have Q default value be 0.00?

_(Check)_

**Limiter:**

- What is the limiter ceiling? When it’s on it makes the output quiet and so I always turn it off. If there is a programmable limiter ceiling, could we make it -0.1 (just under clipping).

_(Maybe you didn’t see the parameters at the bottom of the “Mixer” tab?)_

**IDEAS:**

**“YUCK” BUTTON**: A feature like (if not exactly) like the Insufferable Midrange Filter from AIP. This is something I’ve done on EVERY guitar I’ve ever mixed for the past 10 years. It would cool to just have it be a part of the plug-in. (needs a better name than yuck…)

_(I’m down for this. If you can describe the processing this takes in terms that I can search and cross-reference, like the “order” of the EQ or “octave”, etc, I can research getting this in. I’m just not certain on the specifics of this.)_

**BITCRUSHER PEDAL**: Having some sort of bitcrsuher OR a way of turning down the sample rate would make for a super fucking badss sound. This is what I imagine could make our SICKO MODE

_(Also down, I found some code out there for bitcrushing, actually a pretty cool concept to implement in DSP. Perhaps in the next version.)_

## Friday, January 26, 2024 12:15:14 PM

- Real life TS, **unity is 12db** ???
- Undo (later)
- Tuner

  - https://github.com/Chowdhury-DSP/BYOD/blob/main/src/processors/utility/Tuner.cpp

- Meter

  - https://github.com/ffAudio/ff_meters/blob/master/LevelMeter/LevelMeterSource.h

  - Post mixer
    - Comp (unit) + meter + pre/post + https://github.com/p-hlp/CTAGDRC/blob/master/Source/PluginEditor.cpp

## Monday, January 22, 2024 10:55:35 AM

- Select IR

```C++
volume = fmax(-54 * pow(distortion, 2) - 32 * distortion + 3, -24)
```

## Sunday, January 21, 2024 6:57:15 PM

- **Pre-amp Stage**

  - Noise Gate is applied if enabled.
  - Pre-compression is applied if configured. _(Blend for parallel compression?)_
  - Tube Screamer effect is processed if enabled.
  - Mouse Drive effect is processed if enabled.

- **Amp Stages (if enabled, serial or parallel)**

  - For each stage (1 to 4):
    - Input Gain, Wave Shaping, and Output Gain are applied.
    - Dry/Wet Mixing is performed.

- **Amp Post Processing**

  - Apply Bias Processing.
  - Apply Post Compression with Gain. _(Blend for parallel compression?)_
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
