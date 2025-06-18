#ifndef FIASCO_H
#define FIASCO_H

#include <stdio.h>
#include <stdbool.h>
#include <stdalign.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
  #define M_PI = 3.14159265358979323846264338327950288
#endif

#ifdef _WIN32
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT
#endif

#define CURSOR_OFFSET 196
#define WHEEL_OFFSET 204
#define MOUSE_OFFSET 212

char* current_dir();

typedef enum {
  PendingType,
  EngineType,
  LoadedType,
  FailedType
} TextureType;

typedef uint32_t TextureId;

typedef struct {
  uint8_t hash[8];
} TextureHash;

typedef struct {
  const char* path;
  TextureId id;
  bool insert_in_atlas;
} PendingTexture;

typedef struct {
  TextureHash version;
  const char* texture_path;
  const char* format_type;
  TextureId id;
  uint32_t width;
  uint32_t height;
  bool insert_in_atlas;
} LoadedTexture;

typedef struct {
  const char* texture_path;
  const char* failure_reason;
  TextureId id;
} FailedTexture;

typedef struct {
  const char* texture_path;
  TextureId id;
  uint32_t width;
  uint32_t height;
  bool in_atlas;
} EngineTexture;

typedef struct {
  float x, y;
} Vec2;

typedef struct {
  float x, y, z;
} Vec3;

typedef struct {
  float x, y, z, w;
} Vec4;

typedef struct {
  bool isPressed;
  bool justPressed;
  bool justReleased;
  bool isHeld;
} ButtonState;

typedef struct {
  float x;
  float y;
  ButtonState left;
  ButtonState right;
  ButtonState middle;
} MouseState;

typedef struct {
  const float width;
  const float height;
} Aspect;

typedef struct {
  float width;
  float height;
  float left;
  float right;
  float top;
  float bottom;
} Screen;

typedef struct {
  const float delta;
  const float frame_rate;
} FrameConstants;

typedef struct {
  Vec3 position;
  Vec2 scale;
  Vec2 skew;
  Vec2 pivot;
  float rotation;
  float _padding;
} Transform;

typedef struct {
  bool visible;
} ColorRender;

typedef struct {
  size_t num_sides;
  bool visible;
} CircleRender;

typedef struct {
  TextureId asset_id;
  bool visible;
} TextureRender;

typedef struct {
  uint32_t material_id; // 0 for white, 1 for missing, any other for specific material
  float data[256];
  uint32_t textures[16];
} MaterialParameters;

typedef enum {
  Left,
  Center,
  Right
} TextAlignment;

typedef struct {
  uint8_t text[256];
  bool visible;
  Vec2 bounds;
  float font_size;
  TextAlignment alignment;
} TextRender;

typedef struct {
  float r, g, b, a;
} Color;

typedef struct {
  float h, s, v, a;
} HSVA;

typedef enum {
  AsyncCompletion,
  Component,
  Resource
} ComponentType;

typedef enum {
  Completion,
  DataAccessMut,
  DataAccessRef,
  EventReader,
  EventWriter,
  Query
} ArgType;

typedef enum {
  LoadPendingTextureSuccess,
  TextureAssetManagerNull,
  OutputPendingTextureNull,
  LoadPendingTextureError,
} LoadTextureStatus;

typedef void* (*get_proc_addr)(const char*);
typedef int (*system_func)(const void**);

typedef unsigned short ComponentId;
typedef unsigned long EntityId;

typedef struct {
  ComponentId component_id;
  size_t component_size;
  void* component_val;
} ComponentRef;

typedef int (*for_each_t)(const void**, void*);
typedef int (*para_for_each_t)(const void**, const void*);
typedef size_t (*read_t)(void* reader, void* buf, size_t len);
typedef size_t (*write_t)(void* writer, void* buf, size_t len);

typedef void (*call_t)(ComponentId, const void*, size_t);
typedef void (*call_async_t)(ComponentId, const void*, size_t, const void*, size_t);
typedef void (*despawn_t)(EntityId);
typedef size_t (*event_count_t)(const void*);
typedef const unsigned long* (*event_get_t)(const void*, size_t);
typedef void (*event_send_t)(const void*, const char*, size_t);
typedef bool (*get_parent_t)(EntityId, unsigned long*);
typedef void (*set_parent_t)(EntityId, EntityId, bool);
typedef void (*set_system_enabled_t)(const char*, bool);
typedef EntityId (*spawn_t)(const ComponentRef*, size_t components_count);
typedef void (*query_for_each_t)(const void*, for_each_t, const void*);
typedef int (*query_get_t)(const void* arg_ptr, size_t index, const void** component_ptrs);
typedef int (*query_get_entity_t)(void*, EntityId, const void**);
typedef size_t (*query_len_t)(const void*);
typedef void (*query_par_for_each_t)(const void*, para_for_each_t, const void*);
typedef void (*add_components_t)(EntityId, size_t, const ComponentRef*, size_t);
typedef void (*remove_components_t)(EntityId, const ComponentId*, size_t);

// TEXTURE ASSET MANAGER
typedef TextureId (*texture_asset_manager_white_texture_id_t)();
typedef TextureId (*texture_asset_manager_missing_texture_id_t)();
typedef TextureId (*texture_asset_manager_register_next_texture_id_t)(void* gpu_interface);
typedef TextureHash (*texture_asset_manager_generate_hash_t)(const uint8_t*, uint32_t);
typedef uint32_t (*texture_asset_manager_create_pending_texture_t)(TextureId id, const char* path, bool insert_in_atlas, PendingTexture* out);
typedef void (*texture_asset_manager_free_pending_texture_t)(PendingTexture*);
typedef void (*texture_asset_manager_free_engine_texture_t)(EngineTexture*);
typedef void (*texture_asset_manager_free_loaded_texture_t)(LoadedTexture*);
typedef void (*texture_asset_manager_free_failed_texture_t)(FailedTexture*);
typedef TextureType (*texture_asset_manager_get_texture_type_by_id_t)(void*, TextureId);
typedef uint32_t (*texture_asset_manager_get_pending_texture_by_id_t)(void*, TextureId, PendingTexture*);
typedef uint32_t (*texture_asset_manager_get_engine_texture_by_id_t)(void*, TextureId, EngineTexture*);
typedef uint32_t (*texture_asset_manager_get_loaded_texture_by_id_t)(void*, TextureId, LoadedTexture*);
typedef uint32_t (*texture_asset_manager_get_failed_texture_by_id_t)(void*, TextureId, FailedTexture*);
typedef TextureType (*texture_asset_manager_get_texture_type_by_path_t)(void*, const char*);
typedef uint32_t (*texture_asset_manager_get_pending_texture_by_path_t)(void*, const char*, PendingTexture*);
typedef uint32_t (*texture_asset_manager_get_engine_texture_by_path_t)(void*, const char*, EngineTexture*);
typedef uint32_t (*texture_asset_manager_get_loaded_texture_by_path_t)(void*, const char*, LoadedTexture*);
typedef uint32_t (*texture_asset_manager_get_failed_texture_by_path_t)(void*, const char*, FailedTexture*);
typedef bool (*texture_asset_manager_are_ids_loaded_t)(void*, const TextureId*, uint32_t);
typedef bool (*texture_asset_manager_is_id_loaded_t)(void*, TextureId);
typedef uint32_t (*texture_asset_manager_load_texture_t)(void* texture_asset_manager, const void* event_writer, char* texture_path, bool in_atlas, const PendingTexture* texture);
typedef void* (*gpu_interface_get_texture_asset_manager_mut_t)(void* gpu_interface);

typedef struct {
  call_t call;
  call_async_t call_async;
  despawn_t despawn;
  event_count_t event_count;
  event_get_t event_get;
  event_send_t event_send; 
  get_parent_t get_parent;
  set_parent_t set_parent;
  set_system_enabled_t set_system_enabled;
  spawn_t spawn;
  query_for_each_t query_for_each ;
  query_get_t query_get;
  query_get_entity_t query_get_entity;
  query_len_t query_len;
  query_par_for_each_t query_par_for_each ;
  add_components_t add_components ;
  remove_components_t remove_components;

  // TEXTURE ASSET MANAGER
  texture_asset_manager_white_texture_id_t texture_asset_manager_white_texture_id;
  texture_asset_manager_missing_texture_id_t texture_asset_manager_missing_texture_id;
  texture_asset_manager_register_next_texture_id_t texture_asset_manager_register_next_texture_id;
  texture_asset_manager_generate_hash_t texture_asset_manager_generate_hash;
  texture_asset_manager_create_pending_texture_t texture_asset_manager_create_pending_texture;
  texture_asset_manager_free_pending_texture_t texture_asset_manager_free_pending_texture;
  texture_asset_manager_free_engine_texture_t texture_asset_manager_free_engine_texture;
  texture_asset_manager_free_loaded_texture_t texture_asset_manager_free_loaded_texture;
  texture_asset_manager_free_failed_texture_t texture_asset_manager_free_failed_texture;
  texture_asset_manager_get_texture_type_by_id_t texture_asset_manager_get_texture_type_by_id;
  texture_asset_manager_get_pending_texture_by_id_t texture_asset_manager_get_pending_texture_by_id;
  texture_asset_manager_get_engine_texture_by_id_t texture_asset_manager_get_engine_texture_by_id;
  texture_asset_manager_get_loaded_texture_by_id_t texture_asset_manager_get_loaded_texture_by_id;
  texture_asset_manager_get_failed_texture_by_id_t texture_asset_manager_get_failed_texture_by_id;
  texture_asset_manager_get_texture_type_by_path_t texture_asset_manager_get_texture_type_by_path;
  texture_asset_manager_get_pending_texture_by_path_t texture_asset_manager_get_pending_texture_by_path;
  texture_asset_manager_get_engine_texture_by_path_t texture_asset_manager_get_engine_texture_by_path;
  texture_asset_manager_get_loaded_texture_by_path_t texture_asset_manager_get_loaded_texture_by_path;
  texture_asset_manager_get_failed_texture_by_path_t texture_asset_manager_get_failed_texture_by_path;
  texture_asset_manager_are_ids_loaded_t texture_asset_manager_are_ids_loaded;
  texture_asset_manager_is_id_loaded_t texture_asset_manager_is_id_loaded;
  texture_asset_manager_load_texture_t texture_asset_manager_load_texture;
  gpu_interface_get_texture_asset_manager_mut_t gpu_interface_get_texture_asset_manager_mut;
} Engine;

typedef struct {
  char* Color;
  char* Camera;
  char* Transform;
  char* Inputs;
  char* TextureRender;
  char* ColorRender;
  char* TextRender;
  char* CircleRender;
  char* FrameConstants;
  char* Aspect;
  char* EntityId;
  char* GpuInterface;
  char* MaterialManager;
  char* GpuResource;
  char* GpuConfig;
} FiascoIds_t;

const FiascoIds_t FiascoIds;

typedef struct {
  char* NewTexture;
} FiascoEvents_t;

const FiascoEvents_t FiascoEvents;

typedef struct {
  float x, y, width, height;
} Viewport;

typedef struct {
  Vec4 x_axis;
  Vec4 y_axis;
  Vec4 z_axis;
  Vec4 w_axis;
} Mat4;

typedef struct {
  float value;
  bool is_some;
} Option_f32;

typedef struct {
  uint32_t value;
  bool is_some;
} Option_u32;

typedef struct {
  Mat4 view_matrix;
  Mat4 projection_matrix;
  Color clear_color;
  Viewport viewport_ratio;
  Option_f32 aspect_ratio_override;
  Option_u32 render_target_texture_id;
  float orthographic_size;
  int32_t render_order;
  bool is_enabled;
  uint8_t _padding[7];
} Camera;

typedef enum {
  Backquote = 0,
  Backslash = 1,
  BracketLeft = 2,
  BracketRight = 3,
  Comma = 4,
  Digit0 = 5,
  Digit1 = 6,
  Digit2 = 7,
  Digit3 = 8,
  Digit4 = 9,
  Digit5 = 10,
  Digit6 = 11,
  Digit7 = 12,
  Digit8 = 13,
  Digit9 = 14,
  Equal = 15,
  IntlBackslash = 16,
  IntlRo = 17,
  IntlYen = 18,
  KeyA = 19,
  KeyB = 20,
  KeyC = 21,
  KeyD = 22,
  KeyE = 23,
  KeyF = 24,
  KeyG = 25,
  KeyH = 26,
  KeyI = 27,
  KeyJ = 28,
  KeyK = 29,
  KeyL = 30,
  KeyM = 31,
  KeyN = 32,
  KeyO = 33,
  KeyP = 34,
  KeyQ = 35,
  KeyR = 36,
  KeyS = 37,
  KeyT = 38,
  KeyU = 39,
  KeyV = 40,
  KeyW = 41,
  KeyX = 42,
  KeyY = 43,
  KeyZ = 44,
  Minus = 45,
  Period = 46,
  Quote = 47,
  Semicolon = 48,
  Slash = 49,
  AltLeft = 50,
  AltRight = 51,
  Backspace = 52,
  CapsLock = 53,
  ContextMenu = 54,
  ControlLeft = 55,
  ControlRight = 56,
  Enter = 57,
  MetaLeft = 58,
  MetaRight = 59,
  ShiftLeft = 60,
  ShiftRight = 61,
  Space = 62,
  Tab = 63,
  Convert = 64,
  KanaMode = 65,
  Lang1 = 66,
  Lang2 = 67,
  Lang3 = 68,
  Lang4 = 69,
  Lang5 = 70,
  NonConvert = 71,
  Delete = 72,
  End = 73,
  Help = 74,
  Home = 75,
  Insert = 76,
  PageDown = 77,
  PageUp = 78,
  ArrowDown = 79,
  ArrowLeft = 80,
  ArrowRight = 81,
  ArrowUp = 82,
  NumLock = 83,
  Numpad0 = 84,
  Numpad1 = 85,
  Numpad2 = 86,
  Numpad3 = 87,
  Numpad4 = 88,
  Numpad5 = 89,
  Numpad6 = 90,
  Numpad7 = 91,
  Numpad8 = 92,
  Numpad9 = 93,
  NumpadAdd = 94,
  NumpadBackspace = 95,
  NumpadClear = 96,
  NumpadClearEntry = 97,
  NumpadComma = 98,
  NumpadDecimal = 99,
  NumpadDivide = 100,
  NumpadEnter = 101,
  NumpadEqual = 102,
  NumpadHash = 103,
  NumpadMemoryAdd = 104,
  NumpadMemoryClear = 105,
  NumpadMemoryRecall = 106,
  NumpadMemoryStore = 107,
  NumpadMemorySubtract = 108,
  NumpadMultiply = 109,
  NumpadParenLeft = 110,
  NumpadParenRight = 111,
  NumpadStar = 112,
  NumpadSubtract = 113,
  Escape = 114,
  Fn = 115,
  FnLock = 116,
  PrintScreen = 117,
  ScrollLock = 118,
  Pause = 119,
  BrowserBack = 120,
  BrowserFavorites = 121,
  BrowserForward = 122,
  BrowserHome = 123,
  BrowserRefresh = 124,
  BrowserSearch = 125,
  BrowserStop = 126,
  Eject = 127,
  LaunchApp1 = 128,
  LaunchApp2 = 129,
  LaunchMail = 130,
  MediaPlayPause = 131,
  MediaSelect = 132,
  MediaStop = 133,
  MediaTrackNext = 134,
  MediaTrackPrevious = 135,
  Power = 136,
  Sleep = 137,
  AudioVolumeDown = 138,
  AudioVolumeMute = 139,
  AudioVolumeUp = 140,
  WakeUp = 141,
  Hyper = 142,
  Super = 143,
  Turbo = 144,
  Abort = 145,
  Resume = 146,
  Suspend = 147,
  Again = 148,
  Copy = 149,
  Cut = 150,
  Find = 151,
  Open = 152,
  Paste = 153,
  Props = 154,
  Select = 155,
  Undo = 156,
  Hiragana = 157,
  Katakana = 158,
  F1 = 159,
  F2 = 160,
  F3 = 161,
  F4 = 162,
  F5 = 163,
  F6 = 164,
  F7 = 165,
  F8 = 166,
  F9 = 167,
  F10 = 168,
  F11 = 169,
  F12 = 170,
  F13 = 171,
  F14 = 172,
  F15 = 173,
  F16 = 174,
  F17 = 175,
  F18 = 176,
  F19 = 177,
  F20 = 178,
  F21 = 179,
  F22 = 180,
  F23 = 181,
  F24 = 182,
  F25 = 183,
  F26 = 184,
  F27 = 185,
  F28 = 186,
  F29 = 187,
  F30 = 188,
  F31 = 189,
  F32 = 190,
  F33 = 191,
  F34 = 192,
  F35 = 193
} KeyCode;

HSVA rgb_to_hsv(Color rgb);
Color hsv_to_rgb(HSVA hsv);
const int make_api_version(int major, int minor, int patch);
const float random_float_range(float min, float max);
const float random_float();
ButtonState key(KeyCode code, void *ptr);
MouseState mouse(void *ptr);
Vec2 wheel(void *ptr);
Vec2 mouse_to_screen(MouseState mouse, const Aspect *aspect);
Screen aspect_to_screen(const Aspect *aspect);
void convert_string_to_uint8(const char *input, uint8_t output[256]);

EXPORT int void_target_version();
EXPORT int init();
EXPORT int deinit();
EXPORT char* name();
EXPORT int component_deserialize_json();
EXPORT void set_component_id(char *string_id, ComponentId id);
EXPORT size_t component_size(char *component_id);
EXPORT char* component_string_id(size_t component_index);
EXPORT size_t component_align(char *string_id);
EXPORT ComponentType component_type(char *string_id);
EXPORT size_t systems_len();
EXPORT bool system_is_once(size_t system_index);
EXPORT char* system_name(size_t system_index);
EXPORT system_func system_fn(size_t system_index);
EXPORT size_t system_args_len(size_t system_index);
EXPORT ArgType system_arg_type(size_t system_index, size_t arg_index);
EXPORT char* system_arg_component(size_t system_index, size_t arg_index);
EXPORT char* system_arg_event(size_t system_index, size_t arg_index);
EXPORT size_t system_query_args_len(size_t system_index, size_t arg_index);
EXPORT ArgType system_query_arg_type(size_t system_index, size_t arg_index, size_t query_index);
EXPORT char* system_query_arg_component(size_t system_index, size_t arg_index, size_t query_index);
EXPORT void load_engine_proc_addrs(get_proc_addr get_proc);
EXPORT char* component_async_completion_callable(const char *string_id);
EXPORT int resource_init(char *string_id, void *val);
EXPORT int resource_deserialize(char *string_id, void *val, void *reader, read_t read);
EXPORT int resource_serialize(char *string_id, void *val, void *writer, write_t write);

#endif