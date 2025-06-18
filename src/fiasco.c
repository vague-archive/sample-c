#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fiasco.h>
#include <math.h>
#include <limits.h>

#ifdef _WIN32
  #include <direct.h> // For _getcwd on Windows
  #define getcwd _getcwd
#else
  #include <unistd.h> // For getcwd on POSIX systems
#endif

// Warning: caller must `free()` returned ptr
char* current_dir() {
  char* buffer = (char*)malloc(1024);
  if (buffer == NULL) {
      return NULL;
  }

  if (getcwd(buffer, 1024) != NULL) {
      return buffer;
  } else {
      free(buffer);
      return NULL;
  }
}

const FiascoIds_t FiascoIds = {
  "void_public::colors::Color",
  "void_public::Camera",
  "void_public::Transform",
  "void_public::input::InputState",
  "void_public::graphics::TextureRender",
  "void_public::graphics::ColorRender",
  "void_public::graphics::TextRender",
  "void_public::graphics::CircleRender",
  "void_public::FrameConstants",
  "void_public::Aspect",
  "void_public::EntityId",
  "game_asset::ecs_module::GpuInterface",
  "game_asset::ecs_module::MaterialManager",
  "gpu_web::GpuResource",
  "gpu_web::gpu_config::GpuConfig"
};

const FiascoEvents_t FiascoEvents = {
  "Graphics.NewTexture"
};

void convert_string_to_uint8(const char *input, uint8_t output[256]) {
  memset(output, 0, 256);
  strncpy((char *)output, input, 255); 
  output[255] = '\0';
}

const int make_api_version(int major, int minor, int patch) {
  return (major << 25) | (minor << 15) | patch;
}

const float random_float_range(float min, float max) {
  return min + (max - min) * ((float)rand() / RAND_MAX);
}

const float random_float() {
  return (float)rand() / RAND_MAX;
}

HSVA rgb_to_hsv(Color rgb) {
  HSVA hsv;
  float min, max, delta;

  min = fminf(rgb.r, fminf(rgb.g, rgb.b));
  max = fmaxf(rgb.r, fmaxf(rgb.g, rgb.b));
  hsv.v = max;

  delta = max - min;
  if (delta < 1e-6) {
      hsv.h = 0.0f;
      hsv.s = 0.0f;
      return hsv;
  }

  hsv.s = delta / max;

  if (max == rgb.r) {
      hsv.h = fmodf((rgb.g - rgb.b) / delta, 6.0f);
  } else if (max == rgb.g) {
      hsv.h = (rgb.b - rgb.r) / delta + 2.0f;
  } else {
      hsv.h = (rgb.r - rgb.g) / delta + 4.0f;
  }

  hsv.h *= 60.0f;
  if (hsv.h < 0.0f) hsv.h += 360.0f;

  return hsv;
}

Color hsv_to_rgb(HSVA hsv) {
  Color rgb;
  float c = hsv.v * hsv.s;
  float x = c * (1.0f - fabsf(fmodf(hsv.h / 60.0f, 2.0f) - 1.0f));
  float m = hsv.v - c;

  if (hsv.h < 60.0f) {
      rgb.r = c, rgb.g = x, rgb.b = 0;
  } else if (hsv.h < 120.0f) {
      rgb.r = x, rgb.g = c, rgb.b = 0;
  } else if (hsv.h < 180.0f) {
      rgb.r = 0, rgb.g = c, rgb.b = x;
  } else if (hsv.h < 240.0f) {
      rgb.r = 0, rgb.g = x, rgb.b = c;
  } else if (hsv.h < 300.0f) {
      rgb.r = x, rgb.g = 0, rgb.b = c;
  } else {
      rgb.r = c, rgb.g = 0, rgb.b = x;
  }

  rgb.r += m;
  rgb.g += m;
  rgb.b += m;

  return rgb;
}

ButtonState button_state(uint8_t byte) {
  ButtonState state;
  state.isPressed = (0b01 & byte) == 0b01;  // Current: 1 (pressed)
  state.justPressed = (0b11 & byte) == 0b01; // Current: 1, Previous: 0
  state.justReleased = (0b11 & byte) == 0b10; // Current: 0, Previous: 1
  state.isHeld = (0b11 & byte) == 0b11; // Current: 1, Previous: 1
  return state;
}

ButtonState key(KeyCode code, void *ptr) {
  uint8_t byte = ((uint8_t*)ptr)[code];
  return button_state(byte);
}

MouseState mouse(void *ptr) {
  MouseState state;

  uint8_t left = ((uint8_t*)ptr)[MOUSE_OFFSET];
  uint8_t right = ((uint8_t*)ptr)[MOUSE_OFFSET + 1];
  uint8_t middle = ((uint8_t*)ptr)[MOUSE_OFFSET + 2];

  state.left = button_state(left);
  state.right = button_state(right);
  state.middle = button_state(middle);

  state.x = *(float*)((uint8_t*)ptr + CURSOR_OFFSET);
  state.y = *(float*)((uint8_t*)ptr + CURSOR_OFFSET + sizeof(float));

  return state;
}

Vec2 wheel(void* ptr) {
  Vec2 vec;
  vec.x = *(float*)((uint8_t*)ptr + WHEEL_OFFSET);
  vec.y = *(float*)((uint8_t*)ptr + WHEEL_OFFSET + sizeof(float));
  return vec;
}

Vec2 mouse_to_screen(MouseState mouse, const Aspect *aspect) {
  Vec2 vec;
  vec.x = mouse.x - (aspect->width / 2);
  vec.y = (aspect->height / 2) - mouse.y;
  return vec;
}

Screen aspect_to_screen(const Aspect *aspect) {
  float half_screen_width = aspect->width / 2;
  float half_screen_height = aspect->height / 2;

  Screen screen;
  screen.width = aspect->width;
  screen.height = aspect->height;
  screen.top = half_screen_height;
  screen.bottom = -half_screen_height;
  screen.right = half_screen_width;
  screen.left = -half_screen_width;

  return screen;
}