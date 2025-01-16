#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define SCREEN_H 300
#define SCREEN_W 400

size_t __am_video_read(uintptr_t reg, void * buf, size_t size) {
  switch (reg) {
  case _DEVREG_VIDEO_INFO: {
    _DEV_VIDEO_INFO_t * info = (_DEV_VIDEO_INFO_t *)buf;
    // 400x300x32
    info->width = SCREEN_W;
    info->height = SCREEN_H;
    return sizeof(_DEV_VIDEO_INFO_t);
  }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void * buf, size_t size) {
  switch (reg) {
  case _DEVREG_VIDEO_FBCTL: {
    /*
  向屏幕(x, y)坐标处绘制w*h的矩形图像
  图像像素按行优先方式存储在pixels中
  每个像素用32位整数以00RRGGBB的方式描述颜色
  */
    _DEV_VIDEO_FBCTL_t * ctl = (_DEV_VIDEO_FBCTL_t *)buf;
    uint32_t * fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (int i = 0; i < ctl->h; i++) {
      for (int j = 0; j < ctl->w; j++) {
        if ((ctl->y + i) > (SCREEN_H - 1) || (ctl->x + j) > (SCREEN_W - 1)) break;
        fb[(ctl->y + i) * SCREEN_W + ctl->x + j] = ctl->pixels[i * ctl->w + j];
      }
    }
    if (ctl->sync) {
      outl(SYNC_ADDR, 0);
    }
    return size;
  }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t * fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i++) fb[i] = i;
  draw_sync();
}