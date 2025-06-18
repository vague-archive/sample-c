#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdalign.h> 
#include <string.h>
#include <fiasco.h>

#define MAX_IDS 50
#define INITIAL_THUMBS 5
#define CAMERA_MOVE_SCALE 300

const char *thumb_path = "/assets/thumb.png";

const char engine_version[3] = {0, 0, 20};

char **component_id_strs[MAX_IDS];
ComponentId component_ids[MAX_IDS];
Engine engine;

const ComponentId find_id(char* str) {
  for (int i = 0; i < MAX_IDS; i++) {
    if (component_id_strs[i] == NULL) {
      break;
    }
    if (strcmp(*component_id_strs[i], str) == 0) {
      return component_ids[i];
    }
  }
  return 0;
}

// COMPONENTS

char *THUMB_ID = "Thumb";

typedef struct {
  float angle;
  float speed;
} Thumb;

// END COMPONENTS

EntityId spawn_camera() {
  Camera camera;
  memset(&camera, 0, sizeof(Camera));

  camera.is_enabled = true;
  camera.viewport_ratio.height = 1;
  camera.viewport_ratio.width = 1;
  camera.orthographic_size = 1;
  camera.clear_color = (Color){0.15, .345, .5568, 1}; // it's like a blue

  ComponentRef cam_ref;
  cam_ref.component_id = find_id(FiascoIds.Camera);
  cam_ref.component_size = sizeof(camera);
  cam_ref.component_val = &camera;

  Transform transform;
  memset(&transform, 0, sizeof(Transform));

  transform.scale.x = 1;
  transform.scale.y = 1;

  ComponentRef transform_ref;
  transform_ref.component_id = find_id(FiascoIds.Transform);
  transform_ref.component_size = sizeof(transform);
  transform_ref.component_val = &transform;

  ComponentRef *bundle = (ComponentRef*)malloc(2 * sizeof(ComponentRef));
  bundle[0] = cam_ref;
  bundle[1] = transform_ref;

  EntityId entity_id = engine.spawn(bundle, 2);

  free(bundle);
  return entity_id;
}

const char *text = "Controls\nMove Camera: W/A/S/D\nZoom Camera: -/+\nRotate Camera: Q/E\nSpawn: Left Click";

EntityId spawn_text() {
  TextRender text_render;
  text_render.font_size = 42;
  text_render.visible = true;
  text_render.bounds = (Vec2){600,600};
  convert_string_to_uint8(text, text_render.text);

  ComponentRef text_render_ref;
  text_render_ref.component_id = find_id(FiascoIds.TextRender);
  text_render_ref.component_size = sizeof(TextRender);
  text_render_ref.component_val = &text_render;

  Transform transform;
  memset(&transform, 0, sizeof(Transform));
  transform.scale.x = 1;
  transform.scale.y = 1;

  ComponentRef transform_ref;
  transform_ref.component_id = find_id(FiascoIds.Transform);
  transform_ref.component_size = sizeof(transform);
  transform_ref.component_val = &transform;

  const uint8_t count = 2;
  ComponentRef *bundle = (ComponentRef*)malloc(count * sizeof(ComponentRef));
  bundle[0] = text_render_ref;
  bundle[1] = transform_ref;

  EntityId entity_id = engine.spawn(bundle, count);

  free(bundle);
  return entity_id;
}

EntityId spawn_thumb(Vec2 *vec, TextureId thumb_texture_id) {
  Transform transform;
  memset(&transform, 0, sizeof(Transform));

  float scale = random_float_range(30, 60);
  transform.position.x = vec->x;
  transform.position.y = vec->y;
  transform.scale.x = scale;
  transform.scale.y = scale;
  transform.rotation = random_float_range(0, 6);

  ComponentRef transform_ref;
  transform_ref.component_id = find_id(FiascoIds.Transform);
  transform_ref.component_size = sizeof(transform);
  transform_ref.component_val = &transform;

  Thumb thumb;
  thumb.angle = random_float_range(0, 6);
  thumb.speed = random_float_range(100, 1000);

  ComponentRef thumb_ref;
  thumb_ref.component_id = find_id(THUMB_ID);
  thumb_ref.component_size = sizeof(thumb);
  thumb_ref.component_val = &thumb;

  TextureRender texture_render;
  texture_render.asset_id = thumb_texture_id;
  texture_render.visible = true;

  ComponentRef texture_render_ref;
  texture_render_ref.component_id = find_id(FiascoIds.TextureRender);
  texture_render_ref.component_size = sizeof(texture_render);
  texture_render_ref.component_val = &texture_render;

  Color color;
  color.r = random_float();
  color.g = random_float();
  color.b = random_float();
  color.a = 1.0f;

  ComponentRef color_ref;
  color_ref.component_id = find_id(FiascoIds.Color);
  color_ref.component_size = sizeof(color);
  color_ref.component_val = &color;

  const int count = 4;
  ComponentRef *bundle = (ComponentRef*)malloc(count * sizeof(ComponentRef));
  bundle[0] = transform_ref;
  bundle[1] = thumb_ref;
  bundle[2] = texture_render_ref;
  bundle[3] = color_ref;

  EntityId entity_id = engine.spawn(bundle, count);

  free(bundle);
  return entity_id;
}

// SYSTEMS

typedef enum {
  ThumbMover,
  ThumbSpawnerOnce,
  Controller,
  AlignControlsText
} Systems;

int thumb_mover(const void** ptr) {
  const void *query = ptr[0];
  const FrameConstants *consts = (FrameConstants*)(ptr[1]);
  const Aspect *aspect = (Aspect*)(ptr[2]);

  Screen screen = aspect_to_screen(aspect);

  int count = engine.query_len(query);
  if (count == 0)
    return 0;

  for (int i = 0; i < count; i++) {
    const void *ids[3];
    int code = engine.query_get(query, i, (const void **)&ids);

    if (code != 0) {
      printf("thumb query get failed\n");
      return 1;
    }

    Thumb *thumb = (Thumb*)ids[0];
    Transform *transform = (Transform*)ids[1];
    Color *color = (Color*)ids[2];

    float speed = consts->delta * thumb->speed;
    transform->rotation -= consts->delta * 2;
    transform->position.x += cosf(thumb->angle) * speed;
    transform->position.y += sinf(thumb->angle) * speed;

    if (transform->position.x > screen.right) {
      transform->position.x = screen.right;
      thumb->angle = M_PI - thumb->angle;
    } else if (transform->position.x < screen.left) {
      transform->position.x = screen.left;
      thumb->angle = M_PI - thumb->angle;
    } else if (transform->position.y > screen.top) {
      transform->position.y = screen.top;
      thumb->angle = -thumb->angle;
    } else if (transform->position.y < screen.bottom) {
      transform->position.y = screen.bottom;
      thumb->angle = -thumb->angle;
    }

    HSVA hsv = rgb_to_hsv(*color);
    if (hsv.h >= 355) {
      hsv.h = 0;
    } else {
      hsv.h += consts->delta * 100;
    }

    Color c = hsv_to_rgb(hsv);
    color->r = c.r;
    color->g = c.g;
    color->b = c.b;
  }

  return 0;
}

int thumb_spawner_once(void** ptr) {
  const Aspect *aspect = (Aspect*)ptr[0];
  Screen screen = aspect_to_screen(aspect);
  void *gpu_interface = ptr[1];
  void *event_writer_new_texture = ptr[2];

  char *current_path = current_dir();
  strcat(current_path, thumb_path);

  PendingTexture pending_texture; 
  void *texture_asset_manager = engine.gpu_interface_get_texture_asset_manager_mut(gpu_interface);
  LoadTextureStatus status = engine.texture_asset_manager_load_texture(texture_asset_manager, event_writer_new_texture, current_path, true, &pending_texture);
  free(current_path);

  if (status != LoadPendingTextureSuccess) {
    printf("There was an error loading the texture. Status %u\n", status);
    return 1;
  }

  for (int i = 0; i < INITIAL_THUMBS; i++) {
    float x = random_float_range(screen.left, screen.right);
    float y = random_float_range(screen.bottom, screen.top);
    Vec2 vec = {x, y};
    spawn_thumb(&vec, pending_texture.id);
  }

  spawn_camera();
  spawn_text();

  return 0;
}

int controller(void **ptr) {
  void *input = ptr[0];
  void *camera_query = ptr[1];
  const Aspect *aspect = (Aspect*)ptr[2];
  const FrameConstants *frame = (FrameConstants*)ptr[3];
  void *texture_render_query = ptr[4];

  uint32_t camera_count = engine.query_len(camera_query);
  if (camera_count > 0) {
    const void *ids[2];
    uint32_t code = engine.query_get(camera_query, 0, (const void **)&ids);

    if (code != 0) {
      printf("cam query get failed\n");
      return 1;
    }

    Camera *camera = (Camera*)ids[0];
    Transform *transform = (Transform*)ids[1];

    // Move camera position with WASD
    if(key(KeyA, input).isHeld) {
      transform->position.x -= frame->delta * CAMERA_MOVE_SCALE;
    }
    if(key(KeyW, input).isHeld) {
      transform->position.y += frame->delta * CAMERA_MOVE_SCALE;
    }
    if(key(KeyD, input).isHeld) {
      transform->position.x += frame->delta * CAMERA_MOVE_SCALE;
    }
    if(key(KeyS, input).isHeld) {
      transform->position.y -= frame->delta * CAMERA_MOVE_SCALE;
    }

    // Zoom out with -
    if (key(Minus, input).isHeld && camera->orthographic_size > 0) {
      camera->orthographic_size -= frame->delta;
    }
    // Zoom in with +
    if (key(Equal, input).isHeld) {
      camera->orthographic_size += frame->delta;
    }

    // Rotate camera left with Q
    if (key(KeyQ, input).isHeld) {
      transform->rotation += frame->delta;
    }
    // Rotate camera right with E
    if (key(KeyE, input).isHeld) {
      transform->rotation -= frame->delta;
    }
  }

  MouseState mouse_state = mouse(input);
  if (mouse_state.left.isHeld) {
    Vec2 vec = mouse_to_screen(mouse_state, aspect);

    int texture_render_count = engine.query_len(texture_render_query);
    if (texture_render_count > 0) {
      const void *ids[1];
      const uint32_t ok = engine.query_get(texture_render_query, 0, ids);

      if (ok != 0) {
        printf("texture_render_query get 0 failed\n");
        return 1;
      }

      TextureRender *texture_render = (TextureRender*)ids[0];
      spawn_thumb(&vec, texture_render->asset_id);
    }

  }

  return 0;
}

int align_controls_text(void** ptr) {
  const void *query = ptr[0];
  const Aspect *aspect = (Aspect*)(ptr[1]);

  Screen screen = aspect_to_screen(aspect);

  int count = engine.query_len(query);
  if (count == 0)
    return 0;


  for (int i = 0; i < count; i++) {
    const void *ids[2];
    int code = engine.query_get(query, i, (const void **)&ids);

    if (code != 0) {
      printf("text controls query get failed\n");
      return 1;
    }

    Transform *transform = (Transform*)ids[0];

    transform->position.x = screen.left;
    transform->position.y = screen.top - 100;
  }

  return 0;
}

// END SYSTEMS

char* name() {
  return "Jason C Game";
}
int init() {
  return 0;
}
int deinit() {
  return 0;
}
int component_deserialize_json() {
  return 0;
}

int void_target_version() {
  printf("void_target_version called\n");
  return make_api_version(engine_version[0], engine_version[1], engine_version[2]);
}

int current_index = 0;
void set_component_id(char *string_id, ComponentId id) {
  printf("%d set_component_id called %s - %d\n", current_index, string_id, id);

  component_id_strs[current_index] = (char**)malloc(sizeof(char*));

  if (component_id_strs[current_index] == NULL) {
      printf("Memory allocation for pointer failed for index %d!\n", current_index);
      return;
  }

  *component_id_strs[current_index] = (char*)malloc(strlen(string_id) + 1);

  if (*component_id_strs[current_index] == NULL) {
      printf("Memory allocation for string failed for index %d!\n", current_index);
      free(component_id_strs[current_index]);
      return;
  }

  strcpy(*component_id_strs[current_index], string_id);
  component_ids[current_index] = id;
  current_index++;
}

size_t component_size(char *component_id) {
  printf("component_size called %s\n", component_id);

  if (strcmp(component_id, THUMB_ID) == 0) return sizeof(Thumb);

  return 0;
}

char* component_string_id(size_t component_index) {
  printf("component_string_id called %zu\n", component_index);

  if (component_index == 0) return THUMB_ID;

  return NULL;
}

size_t component_align(char *string_id) {
  printf("component_align called %s\n", string_id);

  if (strcmp(string_id, THUMB_ID) == 0) {
    return _Alignof(Thumb);
  }

  return 0;
}

ComponentType component_type(char *string_id) {
  printf("component_type called %s\n", string_id);
  return Component;
}

size_t systems_len() {
  printf("systems_len called\n");
  return 4;
}

bool system_is_once(size_t system_index) {
  printf("system_is_once called %zu\n", system_index);

  if (system_index == ThumbMover) return false;
  if (system_index == ThumbSpawnerOnce) return true;
  if (system_index == Controller) return false;
  if (system_index == AlignControlsText) return false;

  return false;
}

char* system_name(size_t system_index) {
  printf("system_name called %zu\n", system_index);

  if (system_index == ThumbMover) return "thumb_mover";
  if (system_index == ThumbSpawnerOnce) return "thumb_spawner_once";
  if (system_index == Controller) return "controller";
  if (system_index == AlignControlsText) return "align_controls_text";

  return NULL;
}

system_func system_fn(size_t system_index) {
  printf("system_fn called %zu\n", system_index);

  if (system_index == ThumbMover) return (system_func)thumb_mover;
  if (system_index == ThumbSpawnerOnce) return (system_func)thumb_spawner_once;
  if (system_index == Controller) return (system_func)controller;
  if (system_index == AlignControlsText) return (system_func)align_controls_text;

  return NULL;
}

size_t system_args_len(size_t system_index) {
  printf("system_args_len called %zu\n", system_index);

  if (system_index == ThumbMover) return 3;
  if (system_index == ThumbSpawnerOnce) return 3;
  if (system_index == Controller) return 5;
  if (system_index == AlignControlsText) return 2;

  return 0;
}

ArgType system_arg_type(size_t system_index, size_t arg_index) {
  printf("system_arg_type called %zu - %zu\n", system_index, arg_index);
  
  if (system_index == ThumbMover) {
    if (arg_index == 0) return Query; // Query<Thumb, Transform, ColorRender>
    if (arg_index == 1) return DataAccessRef; // FrameConstants
    if (arg_index == 2) return DataAccessRef; // Aspect
  }

  if (system_index == ThumbSpawnerOnce) {
    if (arg_index == 0) return DataAccessRef; // Aspect
    if (arg_index == 1) return DataAccessRef; // GpuInterface
    if (arg_index == 2) return EventWriter; // EventWriter<NewTexture> 
  }

  if (system_index == Controller) {
    if (arg_index == 0) return DataAccessRef; // Input
    if (arg_index == 1) return Query; // Query<Camera, Transform>
    if (arg_index == 2) return DataAccessRef; // Aspect
    if (arg_index == 3) return DataAccessRef; // FrameConstants
    if (arg_index == 4) return Query; // Query<TextureRender>
  }

  if (system_index == AlignControlsText) {
    if (arg_index == 0) return Query; // Query<Transform, TextRender>
    if (arg_index == 1) return DataAccessRef; // Aspect
  }
  
  return Query;
}

char* system_arg_component(size_t system_index, size_t arg_index) {
  printf("system_arg_component called %zu - %zu\n", system_index, arg_index);
  
  if (system_index == ThumbMover) {
    // 0 - Query<Thumb, Transform, ColorRender>
    if (arg_index == 1) return FiascoIds.FrameConstants;
    if (arg_index == 2) return FiascoIds.Aspect;
  }

  if (system_index == ThumbSpawnerOnce) {
    if (arg_index == 0) return FiascoIds.Aspect;
    if (arg_index == 1) return FiascoIds.GpuInterface;
  }

  if (system_index == Controller) {
    if (arg_index == 0) return FiascoIds.Inputs;
    // 1 - Query<Camera, Transform>
    if (arg_index == 2) return FiascoIds.Aspect;
    if (arg_index == 3) return FiascoIds.FrameConstants;
    // 4 - Query<TextureRender>
  }

  if (system_index == AlignControlsText) {
    // 0 - Query<Transform, TextRender>
    if (arg_index == 1) return FiascoIds.Aspect;
  }
  
  return NULL;
}

char* system_arg_event(size_t system_index, size_t arg_index) {
  printf("system_arg_event called %zu - %zu\n", system_index, arg_index);

  if (system_index == ThumbSpawnerOnce) {
    if (arg_index == 2) return FiascoEvents.NewTexture;
  }

  return NULL;
}

size_t system_query_args_len(size_t system_index, size_t arg_index) {
  printf("system_query_args_len called %zu - %zu\n", system_index, arg_index);
  
  if (system_index == ThumbMover) {
    if (arg_index == 0) return 3;
  }

  if (system_index == ThumbSpawnerOnce) {
    // noop
  }

  if (system_index == Controller) {
    if (arg_index == 1) return 2; 
    if (arg_index == 4) return 1; 
  }

  if (system_index == AlignControlsText) {
    if (arg_index == 0) return 2;
  }
  
  return -1;
}

ArgType system_query_arg_type(size_t system_index, size_t arg_index, size_t query_index) {
  printf("system_query_arg_type called %zu - %zu - %zu\n", system_index, arg_index, query_index);
  return DataAccessMut;
}

char* system_query_arg_component(size_t system_index, size_t arg_index, size_t query_index) {
  printf("system_query_arg_component called %zu - %zu - %zu\n", system_index, arg_index, query_index);
  
  if (system_index == ThumbMover) {
    if (arg_index == 0) {
      if (query_index == 0) return THUMB_ID;
      if (query_index == 1) return FiascoIds.Transform;
      if (query_index == 2) return FiascoIds.Color;
    }
  }

  if (system_index == ThumbSpawnerOnce) {
    // noop
  }

  if (system_index == Controller) {
    if (arg_index == 1) {
      if (query_index == 0) return FiascoIds.Camera;
      if (query_index == 1) return FiascoIds.Transform;
    }
    if (arg_index == 4) {
      if (query_index == 0) return FiascoIds.TextureRender;
    }
  }

  if (system_index == AlignControlsText) {
    if (arg_index == 0) {
      if (query_index == 0) return FiascoIds.Transform;
      if (query_index == 1) return FiascoIds.TextRender;
    }
  }

  return NULL;
}

void load_engine_proc_addrs(get_proc_addr get_proc) {
  printf("load_engine_addrs called\n");
  engine.call = get_proc("call");
  engine.call_async = get_proc("call_async");
  engine.despawn = get_proc("despawn");
  engine.event_count = get_proc("event_count");
  engine.event_get = get_proc("event_get");
  engine.event_send = get_proc("event_send");
  engine.get_parent = get_proc("get_parent");
  engine.set_parent = get_proc("set_parent");
  engine.set_system_enabled = get_proc("set_system_enabled");
  engine.spawn = get_proc("spawn");
  engine.query_for_each = get_proc("query_for_each");
  engine.query_get = get_proc("query_get");
  engine.query_get_entity = get_proc("query_get_entity");
  engine.query_len = get_proc("query_len");
  engine.query_par_for_each = get_proc("query_par_for_each");
  engine.add_components = get_proc("add_components");
  engine.remove_components = get_proc("remove_components");

  // TEXTURE ASSET MANAGER
  engine.texture_asset_manager_white_texture_id = get_proc("texture_asset_manager_white_texture_id");
  engine.texture_asset_manager_missing_texture_id = get_proc("texture_asset_manager_missing_texture_id");
  engine.texture_asset_manager_register_next_texture_id = get_proc("texture_asset_manager_register_next_texture_id");
  engine.texture_asset_manager_generate_hash = get_proc("texture_asset_manager_generate_hash");
  engine.texture_asset_manager_create_pending_texture = get_proc("texture_asset_manager_create_pending_texture");
  engine.texture_asset_manager_free_pending_texture = get_proc("texture_asset_manager_free_pending_texture");
  engine.texture_asset_manager_free_engine_texture = get_proc("texture_asset_manager_free_engine_texture");
  engine.texture_asset_manager_free_loaded_texture = get_proc("texture_asset_manager_free_loaded_texture");
  engine.texture_asset_manager_free_failed_texture = get_proc("texture_asset_manager_free_failed_texture");
  engine.texture_asset_manager_get_texture_type_by_id = get_proc("texture_asset_manager_get_texture_type_by_id");
  engine.texture_asset_manager_get_pending_texture_by_id = get_proc("texture_asset_manager_get_pending_texture_by_id");
  engine.texture_asset_manager_get_engine_texture_by_id = get_proc("texture_asset_manager_get_engine_texture_by_id");
  engine.texture_asset_manager_get_loaded_texture_by_id = get_proc("texture_asset_manager_get_loaded_texture_by_id");
  engine.texture_asset_manager_get_failed_texture_by_id = get_proc("texture_asset_manager_get_failed_texture_by_id");
  engine.texture_asset_manager_get_texture_type_by_path = get_proc("texture_asset_manager_get_texture_type_by_path");
  engine.texture_asset_manager_get_pending_texture_by_path = get_proc("texture_asset_manager_get_pending_texture_by_path");
  engine.texture_asset_manager_get_engine_texture_by_path = get_proc("texture_asset_manager_get_engine_texture_by_path");
  engine.texture_asset_manager_get_loaded_texture_by_path = get_proc("texture_asset_manager_get_loaded_texture_by_path");
  engine.texture_asset_manager_get_failed_texture_by_path = get_proc("texture_asset_manager_get_failed_texture_by_path");
  engine.texture_asset_manager_are_ids_loaded = get_proc("texture_asset_manager_are_ids_loaded");
  engine.texture_asset_manager_is_id_loaded = get_proc("texture_asset_manager_is_id_loaded");
  engine.texture_asset_manager_load_texture = get_proc("texture_asset_manager_load_texture");
  engine.gpu_interface_get_texture_asset_manager_mut = get_proc("gpu_interface_get_texture_asset_manager_mut");
}

char* component_async_completion_callable(const char *string_id) {
  printf("component_async_completion_callable called %s\n", string_id);
  return NULL;
}

int resource_init(char *string_id, void *val) {
  printf("resource_init called %s\n", string_id);
  return 0;
}

int resource_deserialize(char *string_id, void *val, void *reader, read_t read) {
  printf("resource_deserialize called\n");
  return 0;
}

int resource_serialize(char *string_id, void *val, void *writer, write_t write) {
  printf("resource_serialize called\n");
  return 0;
}