#ifndef KEYEVENT_H
#define KEYEVENT_H

#define KEY_PRESSED   1
#define KEY_RELEASED  2

#define KEY_PAUSE  19
#define KEY_ENTER  13
#define KEY_SPACE  32
#define KEY_ESCAPE 27
#define KEY_BACK_SPACE     8 
#define KEY_NUMPAD_PLUS  107
#define KEY_NUMPAD_MINUS 109

enum {
  KEY_LEFT = 37,
  KEY_UP, KEY_RIGHT, KEY_DOWN,

  KEY_0 = 48,
  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7,
  KEY_8, KEY_9,

  KEY_A = 65,
  KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
  KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, 
  KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V,
  KEY_W, KEY_X, KEY_Y, KEY_Z,

  KEY_NUMPAD0 = 96,
  KEY_NUMPAD1, KEY_NUMPAD2, KEY_NUMPAD3, KEY_NUMPAD4,
  KEY_NUMPAD5, KEY_NUMPAD6, KEY_NUMPAD7, KEY_NUMPAD8,
  KEY_NUMPAD9,

  KEY_F1 = 112,
  KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7,
  KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12
};

class KeyEvent
{
  public:
    KeyEvent(int id);
    KeyEvent(const KeyEvent& copy);
    KeyEvent &operator= (const KeyEvent& copy);

    bool displayable();
    char getKeyChar();
    int  getKeyID();

  private:
    int id;
};

#endif
