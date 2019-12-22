#include "common.h"
#include <amdev.h>
#include "proc.h"

size_t serial_write(const void *buf, size_t offset, size_t len) {
  //_yield();
  char *s = (char*)buf;
    size_t i = 0;
    while(i<len){
      _putc(s[i]);
      i++;
    }
    return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

extern int read_key();
extern unsigned int uptime();
#define KEYDOWN_MASK 0x8000

size_t events_read(void *buf, size_t offset, size_t len) {
  //_yield();
  /*int key = read_key();
  int down = 0;
  if (key & 0x8000) { 
    key ^= 0x8000;
    down = 1;
  }
  if (key != _KEY_NONE){
    //sprintf(buf,"%s %s\n", down?"kd":"ku", keyname[key]);
    if (down){
      sprintf(buf, "kd %s\n", keyname[key]);
      if (keyname[key][0] == 'F'){
        Log("F* key down\n");
        if (keyname[key][1] == '1')
          set_fg_pcb(1);
        else if (keyname[key][1] == '2')
          set_fg_pcb(2);
        else if (keyname[key][1] == '3')
          set_fg_pcb(3);
      }
    }
    else{
      sprintf(buf, "ku %s\n", keyname[key]);
    }
  }
  else{
    unsigned int t = uptime();
    sprintf(buf,"t %d\n",t);
  }
  return strlen(buf);*/
  int keycode = read_key();
  if ((keycode & ~KEYDOWN_MASK) == _KEY_NONE) {
    sprintf(buf, "t %d\n", uptime());
  } else if (keycode & KEYDOWN_MASK) {
    sprintf(buf, "kd %s\n", keyname[keycode & ~KEYDOWN_MASK]);
    if (keyname[keycode & ~KEYDOWN_MASK][0] == 'F') {
      Log("F key down!");
      switch (keyname[keycode & ~KEYDOWN_MASK][1]) {
        case '1':
          set_fg_pcb(1);
          break;
        case '2':
          set_fg_pcb(2);
          break;
        case '3':
          set_fg_pcb(3);
          break;
        default:
          break;
      }
    }
  } else {
    sprintf(buf, "ku %s\n", keyname[keycode & ~KEYDOWN_MASK]);
  }
return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};
extern char* strncpy(char* dst, const char* src, size_t n);

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //memcpy(buf,(void*)dispinfo+offset,len);
  strncpy(buf,dispinfo+offset,len);
  return len;
}

extern int screen_width();
extern int screen_height();
extern void draw_rect(unsigned int *, int, int, int, int);

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //_yield();
  int w = screen_width();
  int col = (offset/4) % w;
  int row = (offset/4) / w;
  draw_rect((unsigned int*)buf,col,row,len/4,1);
  return len;
}

extern void draw_sync();

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",screen_width(),screen_height());
}
