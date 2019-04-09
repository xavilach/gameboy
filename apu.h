#ifndef APU_H_
#define APU_H_

/* Sound Controller
Pulse A:
0xFF10 NR10 Channel 1 Sweep register.
0xFF11 NR11 Channel 1 Sound length/Wave pattern duty.
0xFF12 NR12 Channel 1 Volume envelope.
0xFF13 NR13 Channel 1 Frequency (low).
0xFF14 NR14 Channel 1 Frequency (high).

Pulse B:
0xFF16 NR21 Channel 2 Sound length/Wave pattern duty.
0xFF17 NR22 Channel 2 Volume envelope.
0xFF18 NR23 Channel 2 Frequency (low).
0xFF19 NR24 Channel 2 Frequency (high).

Wave:
0xFF1A NR30 Channel 3 Sound ON/OFF.
0xFF1B NR31 Channel 3 Sound Length.
0xFF1C NR32 Channel 3 Select output level.
0xFF1D NR33 Channel 3 Frequency (low).
0xFF1E NR34 Channel 3 Frequency (high).

Noise:
0xFF20 NR41 Channel 4 Sound Length.
0xFF21 NR42 Channel 4 Volume envelope.
0xFF22 NR43 Channel 4 Polynomial counter.
0xFF23 NR44 Channel 4 Counter/Consecutive, Intial.

0xFF24 NR50 Channel control / ON-OFF / Volume.
0xFF25 NR51 Sound output terminal selection.
0xFF26 NR52 Sound ON/OFF

0xFF30 W    Wave ?
*/

#endif /*APU_H_*/