#include <stdio.h>
#include "sdl.h"
 
#define APP_TITLE "Python Music"
#define PI  3.141592653589793238462
#define B   (2.0 * PI / 44100.0)
#define TRANSITION_MILLIS 50
#define TRANSITION_SAMPLES (44.1 * TRANSITION_MILLIS)
#define MAX_OSCILLATORS 4
#define MAX_AMPLITUDE  (0x7FFFFFFF >> 2)

typedef Sint32 AudioSample;

typedef struct {
	int    i; // position in oscillators array
	double a0;
	double a1;
	double b0;
	double b1;
	Uint32 s;
	Uint32 remaining_samples;
	double delta_a0;
	double delta_a1;
} Oscillator;

int quitting = 0;
static SDL_AudioDeviceID	audio_dev;
static SDL_AudioSpec		audio_spec;
static Oscillator oscillators[MAX_OSCILLATORS];

static void fatal(const char * fmt, ...) {
	va_list v;
	char buf[1024];

	va_start(v, fmt);
	vsnprintf(buf, 1024, fmt, v);
	va_end(v);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, APP_TITLE, buf, NULL);
	exit(1);
}

/* Add len samples from oscillator into audio sample buffer. */
static void mix_oscillator(Oscillator * o, AudioSample * buf, int len) {
	for (int i = 0; i < len; ++i) {
		if (o->remaining_samples > 0) {
			o->a0 -= o->delta_a0;
			o->a1 += o->delta_a1;
			if (--o->remaining_samples == 0) {
				o->a0 = o->a1;
				o->b0 = o->b1;
				o->a1 = 0;
				o->b1 = 0;
				if (o->a0 + 0.0001 > MAX_AMPLITUDE) o->a0 = MAX_AMPLITUDE;
			}
		}
		buf[i] += o->a0 * sin(o->b0 * o->s) + o->a1 * sin(o->b1 * o->s);
		++o->s;
	}
//printf("%f %f %f %f\n", o->a0, o->a1, o->b0, o->b1);
}

/* Fill audio sample buffer with samples.  Called in audio thread. */
static void sdl_audio_callback(void * userdata, Uint8 * stream, int len) {
	AudioSample * buf;
	int samples;
	int i;

	if (quitting) return;
	memset(stream, audio_spec.silence, len);
	buf = (AudioSample *) stream;
	samples = len / sizeof(AudioSample);
	for (i = 0; i < MAX_OSCILLATORS; ++i) {
		mix_oscillator(&oscillators[i], buf, samples);
	}
}

/*
init_audio can be safely called redundantly; 
the initialization code only runs once. 
*/
void init() {
	SDL_AudioSpec want;

	if (SDL_WasInit(SDL_INIT_AUDIO)) return;
	if (SDL_Init(SDL_INIT_AUDIO)) fatal(SDL_GetError());
	atexit(SDL_Quit);

	memset(oscillators, 0, MAX_OSCILLATORS * sizeof(Oscillator));
	
	SDL_zero(want);
	want.freq      = 44100;
	want.format    = AUDIO_S32LSB; 
	want.channels  = 1;
	want.samples   = 4096;
	want.callback  = sdl_audio_callback;

	audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &audio_spec, 0);
	if (audio_dev == 0) fatal(SDL_GetError());

	SDL_PauseAudioDevice(audio_dev, 0);
}

void stop() {
	quitting = 1;
	SDL_PauseAudioDevice(audio_dev, 1);
	SDL_CloseAudioDevice(audio_dev);
}

void set_oscillator(int i, double f, double a) {
	if (i < 0 || i >= MAX_OSCILLATORS) fatal("In set_oscillator: invalid index into oscillators.");
	Oscillator * o = &(oscillators[i]);
	if (a < 0 || a > 1.0) {
		fatal("in update_oscillator amplitude should be in [0, 1]");
	}
	SDL_LockAudioDevice(audio_dev);
	if (o->remaining_samples > 0) {
		SDL_UnlockAudioDevice(audio_dev);
		printf("transition not complete\n");
		return;
	}
	o->remaining_samples = TRANSITION_SAMPLES;
	o->delta_a0 = o->a0 / (double) TRANSITION_SAMPLES;
	o->delta_a1 = a * MAX_AMPLITUDE / (double) TRANSITION_SAMPLES;
	o->a1 = 0;
	o->b1 = B * f;
	SDL_UnlockAudioDevice(audio_dev);
}

