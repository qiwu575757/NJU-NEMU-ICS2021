#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  int input = inl(KBD_ADDR);

  if (input == AM_KEY_NONE) { // 当前无按键可获取
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  } else { // 表示当前按下或者释放按键
    kbd->keydown = (kbd->keycode == AM_KEY_NONE); // 如果之前没有按键，说明此时按键了
    kbd->keycode = input & (~KEYDOWN_MASK);
  }
}
