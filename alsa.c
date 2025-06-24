#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

int gethandleelem(
    snd_mixer_t **handle, snd_mixer_selem_id_t **sid, snd_mixer_elem_t **elem,
    int iscapture
)
{
  const char *card = "default";

  snd_mixer_open(handle, 0);
  snd_mixer_attach(*handle, card);
  snd_mixer_selem_register(*handle, NULL, NULL);
  snd_mixer_load(*handle);
  snd_mixer_selem_id_malloc(sid);
  snd_mixer_selem_id_set_index(*sid, 0);
  snd_mixer_selem_id_set_name(*sid, iscapture ? CAPTURE : PLAYBACK);

  if (!(*elem = snd_mixer_find_selem(*handle, *sid))) {
    if (iscapture) {
      perror("can't find selem " CAPTURE);
    } else {
      perror("can't find selem " PLAYBACK);
    }
    return 1;
  }
  return 0;
}

void initplaybackvolume(snd_mixer_elem_t *elem, long *min, long *max, long *val, int *mute)
{
  snd_mixer_selem_get_capture_volume_range(elem, min, max);
  snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, val);
  snd_mixer_selem_get_capture_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, mute);
}

void initcapturevolume(snd_mixer_elem_t *elem, long *min, long *max, long *val, int *mute)
{
  snd_mixer_selem_get_playback_volume_range(elem, min, max);
  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, val);
  snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, mute);
}

void readvolume(int iscapture, long *min, long *max, long *val, int *mute)
{
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  snd_mixer_elem_t *elem;

  if (gethandleelem(&handle, &sid, &elem, iscapture) == 1) {
    return;
  }

  if (iscapture)
    initplaybackvolume(elem, min, max, val, mute);
  else
    initcapturevolume(elem, min, max, val, mute);

  snd_mixer_close(handle);
  snd_mixer_selem_id_free(sid);
}

void togglemute(int iscapture, int *mute)
{
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  snd_mixer_elem_t *elem;

  if (gethandleelem(&handle, &sid, &elem, iscapture) == 1) {
    return;
  }

  *mute = !mute;
  if (iscapture) {
    snd_mixer_selem_set_capture_switch_all(elem, *mute);
  } else {
    snd_mixer_selem_set_playback_switch_all(elem, *mute);
  }

  snd_mixer_close(handle);
  snd_mixer_selem_id_free(sid);
}

void changevolume(int up, int iscapture, long *min, long *max, long *val, long step)
{
  long delta, nv;
  snd_mixer_t *handle;
  snd_mixer_selem_id_t *sid;
  snd_mixer_elem_t *elem;

  if (gethandleelem(&handle, &sid, &elem, iscapture) == 1) {
    return;
  }

  delta = (*max - *min) * step / 100;
  nv = up ? *val + delta : *val - delta;
  if (nv > *max)
    nv = *max;
  if (nv < *min)
    nv = *min;

  if (iscapture) {
    snd_mixer_selem_set_capture_volume_all(elem, nv);
    *val = nv;
  } else {
    snd_mixer_selem_set_playback_volume_all(elem, nv);
    *val = nv;
  }

  snd_mixer_close(handle);
  snd_mixer_selem_id_free(sid);
}
