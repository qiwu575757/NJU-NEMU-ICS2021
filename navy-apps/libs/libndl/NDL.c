
#include <NDL.h>

static int evtdev = -1;
static int fbdev = -1;
static int dispinfo_dev = -1;
static int screen_w = 0, screen_h = 0;
int width = 0, height = 0;

// 以毫秒为单位返回系统时间
uint32_t NDL_GetTicks() {
  static struct timeval timeval;

  int ret = _gettimeofday(&timeval, NULL);
  if (ret != 0) {
    printf("Error in time test.\n");
    return 0;
  }

  return timeval.tv_usec / 1000 + timeval.tv_sec * 1000;
}

int NDL_PollEvent(char *buf, int len) {
  assert(evtdev != -1);
  int ret = read(evtdev, buf, len);

  return ret;
}

// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
void NDL_OpenCanvas(int *w, int *h) {
  if (*w == 0 && *h == 0)
  {
    *w = width;
    *h = height;
  }

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  if (w == 0 && h == 0)
  {
    w = width;
    h = height;
  }
  for (size_t row = 0; row < h; ++row)
  {
    lseek(fbdev, x + (y + row) * width, SEEK_SET);
    write(fbdev, pixels + row * w, w);
  }
  write(fbdev, 0, 0);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  evtdev = open("/dev/events", 0, 0);
  fbdev = open("/dev/fb",  0, 0);
  dispinfo_dev = open("/proc/dispinfo", 0, 0);

  FILE *fp = fopen("/proc/dispinfo", "r");
  fscanf(fp, "width = %d, height = %d\n", &width, &height);

  return 0;
}

void NDL_Quit() {
    close(evtdev);
    close(dispinfo_dev);
}