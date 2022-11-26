#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

static AM_GPU_CONFIG_T gpu_config;
static AM_GPU_FBDRAW_T gpu_fbdraw;
#define NUM_LINGTH 32

size_t serial_write(const void *buf, size_t offset, size_t len)
{
  yield();
  char *str = (char *)buf;
  for (size_t i = 0; i < len; ++i)
  {
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  yield();
  static AM_INPUT_KEYBRD_T kbd;
  ioe_read(AM_INPUT_KEYBRD, &kbd);
  if (kbd.keycode == AM_KEY_NONE)
    return 0;

  if (kbd.keydown)
    strncat(buf, "kd ", len);
  else
    strncat(buf, "ku ", len);
  strncat(buf, keyname[kbd.keycode], len - 3);
  strncat(buf, "\n", len - 3 - strlen(keyname[kbd.keycode]));

  return strlen(buf);
}

// 整数转化为字符串
static char *_itoa(int num, char nums[]) {
  char reversed[NUM_LINGTH];
  if (num == 0) {
    strcpy(nums, "0\0");
  } else {
    int i = 0;
    while (num != 0) {
      reversed[i] = num % 10 + '0';
      num /= 10;
      i ++;
    }

    for (int j = 0; j < i; j ++) {
      nums[i-j-1] = reversed[j];
    }
    nums[i] = '\0';
  }

  return nums;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  yield();
  int width = gpu_config.width, height = gpu_config.height;

  char nums[NUM_LINGTH];
  strcpy(buf, "width = "); //using strcpy is important to clean the old message in the file.
  strcat(buf, _itoa(width, nums));
  strcat(buf, ", height = ");
  strcat(buf, _itoa(height, nums));
  strcat(buf, "\n");

  return strlen((char *)(buf));
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  yield();
  //另外我们约定每次绘图后总是马上将frame buffer中的内容同步到屏幕上.
    if (len == 0)
    {
      gpu_fbdraw.sync = 1; // 用于立刻刷新屏幕
      ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
      return 0;
    }

  int width = gpu_config.width;

  gpu_fbdraw.pixels = (void *)buf;
  gpu_fbdraw.w = len;
  gpu_fbdraw.h = 1;
  gpu_fbdraw.x = offset % width;
  gpu_fbdraw.y = offset / width;
  gpu_fbdraw.sync = 0;
  ioe_write(AM_GPU_FBDRAW, &gpu_fbdraw);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
  ioe_read(AM_GPU_CONFIG, &gpu_config);
}
