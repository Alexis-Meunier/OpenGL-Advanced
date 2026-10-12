#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>

#include "bunny.hh"
#include "matrix.hh"

#define WINDOW_HORIZ_MID 512
#define WINDOW_VERT_MID 512

#define TEST_OPENGL_ERROR()                                                             \
  do {		  							\
    GLenum err = glGetError(); 					                        \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR! " << __LINE__ << " Error code: " << err << std::endl;      \
  } while(0)

const GLuint WIDTH = 1024;
const GLuint HEIGHT = 1024;

GLfloat anim_time = 0.0f;

GLuint bunny_vao_id;
GLuint bunny_vao_id_geometry;
GLuint program_id;
GLuint program_id_geometry;
GLuint program_id_compute;
GLuint frame_buffer_id;
GLuint fbo_texture;
GLuint fbo_depth;

float camYaw = 0.6f;
float camPitch = 0.5f;
float camDist = 2.5f;
float aspect = 1.0f;
bool dragging = false;
int lastX = 0, lastY = 0;
float lightX = 0.0, lightY = 5.0, lightZ = 0.0;

GLuint nbDivisions = 10;
GLuint middleDivisions1 = 5;
GLuint middleDivisions2 = 10;

void handleKeyboard(unsigned char key, int x, int y) {
    GLint div_location;
    (void)x;
    (void)y;

    switch (key)
    {
        case 's': lightX += 0.2; lightY += 0.0; lightZ += 0.0; break;
        case 'w': lightX -= 0.2; lightY -= 0.0; lightZ -= 0.0; break;
        case 'd': lightX -= 0.0; lightY -= 0.0; lightZ -= 0.2; break;
        case 'a': lightX += 0.0; lightY += 0.0; lightZ += 0.2; break;
        case 'q': lightY += 0.2; break;
        case 'e': lightY -= 0.2; break;
        // case '+':
        //     nbDivisions += 1;
        //     div_location = glGetUniformLocation(program_id, "nb_divisions");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, nbDivisions);TEST_OPENGL_ERROR();
        //     break;
        // case '-':
        //     if (nbDivisions > 0)
        //       nbDivisions -= 1;
        //     div_location = glGetUniformLocation(program_id, "nb_divisions");TEST_OPENGL_ERROR();
        //     glUniform1f(div_location, nbDivisions);TEST_OPENGL_ERROR();
        //     break;
        case '7':
            middleDivisions1 += 1;
            div_location = glGetUniformLocation(program_id, "middle_divisions1");TEST_OPENGL_ERROR();
            glUniform1f(div_location, middleDivisions1);TEST_OPENGL_ERROR();
            break;
        case '8':
            if (middleDivisions1 > 0)
              middleDivisions1 -= 1;
            div_location = glGetUniformLocation(program_id, "middle_divisions1");TEST_OPENGL_ERROR();
            glUniform1f(div_location, middleDivisions1);TEST_OPENGL_ERROR();
            break;
        case '4':
            middleDivisions2 += 1;
            div_location = glGetUniformLocation(program_id, "middle_divisions2");TEST_OPENGL_ERROR();
            glUniform1f(div_location, middleDivisions2);TEST_OPENGL_ERROR();
            break;
        case '5':
            if (middleDivisions2 > 0)
              middleDivisions2 -= 1;
            div_location = glGetUniformLocation(program_id, "middle_divisions2");TEST_OPENGL_ERROR();
            glUniform1f(div_location, middleDivisions2);TEST_OPENGL_ERROR();
            break;
      // if `escape` then close window
        case 27: exit(0);
    }

    GLint light_location = glGetUniformLocation(program_id, "light_pos");TEST_OPENGL_ERROR();
    glUniform3f(light_location, lightX, lightY, lightZ);TEST_OPENGL_ERROR();
    glutPostRedisplay();TEST_OPENGL_ERROR();
}

static void upload_mat4(GLuint prog, const char* name, matrix4& m) {
  GLfloat* v = m.get_values();
  glUniformMatrix4fv(glGetUniformLocation(prog, name), 1, GL_TRUE, v);TEST_OPENGL_ERROR();
  delete[] v;
}

void update_projection() {
  if (!program_id) return;
  const float n = 0.05f, f = 100.0f;
  const float fovy = 60.0f * M_PI / 180.0f;
  const float top = n * std::tan(fovy / 2.0f);
  const float right = top * aspect;

  matrix4 proj = frustum(-right, right, -top, top, n, f);

  glUseProgram(program_id);TEST_OPENGL_ERROR();
  upload_mat4(program_id, "projection", proj);TEST_OPENGL_ERROR();
}

void update_camera() {
  if (!program_id) return;

  float ex = camDist * std::cos(camPitch) * std::sin(camYaw);
  float ey = camDist * std::sin(camPitch);
  float ez = camDist * std::cos(camPitch) * std::cos(camYaw);

  matrix4 view = look_at(ex, ey, ez,
                         0.0f, 0.0f, 0.0f,
                         0.0f, 1.0f, 0.0f);

  glUseProgram(program_id);TEST_OPENGL_ERROR();
  upload_mat4(program_id, "camera", view);TEST_OPENGL_ERROR();
  glUniform3f(glGetUniformLocation(program_id, "cameraPos"), ex, ey, ez);TEST_OPENGL_ERROR();
}

void on_mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    dragging = (state == GLUT_DOWN);
    lastX = x; lastY = y;
  }
}

void on_motion(int x, int y) {
  if (!dragging) return;

  camYaw   -= (x - lastX) * 0.005f;
  camPitch += (y - lastY) * 0.005f;
  if (camPitch >  1.5f)  camPitch =  1.5f;
  if (camPitch < 0.05f)  camPitch = 0.05f;
  lastX = x; lastY = y;

  update_camera();TEST_OPENGL_ERROR();
  glutPostRedisplay();TEST_OPENGL_ERROR();
}

void on_wheel(int wheel, int dir, int x, int y) {
  (void)wheel;
  (void)x;
  (void)y;
  camDist -= dir * 0.2f;
  if (camDist < 0.5f)  camDist = 0.5f;
  if (camDist > 20.0f) camDist = 20.0f;
  update_camera();TEST_OPENGL_ERROR();
  glutPostRedisplay();TEST_OPENGL_ERROR();
}

void window_resize(int width, int height) {
  //std::cout << "glViewport(0,0,"<< width << "," << height << ");TEST_OPENGL_ERROR();" << std::endl;
  glViewport(0, 0, width, height);TEST_OPENGL_ERROR();
  aspect = (height > 0) ? (float)width / (float)height : 1.0f;
  update_projection();TEST_OPENGL_ERROR();
}

void display() {
  // glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
  glViewport(0, 0, WIDTH, HEIGHT);TEST_OPENGL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();

  glUseProgram(program_id_compute);TEST_OPENGL_ERROR();
  glDispatchCompute(1024, 1, 1);TEST_OPENGL_ERROR();
  glMemoryBarrier(GL_ALL_BARRIER_BITS);TEST_OPENGL_ERROR();

  glUseProgram(program_id);TEST_OPENGL_ERROR();
  glBindVertexArray(bunny_vao_id);TEST_OPENGL_ERROR();
  glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size() / 3);TEST_OPENGL_ERROR();
  glBindVertexArray(0);TEST_OPENGL_ERROR();

  // glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer_id);
  // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // glBlitFramebuffer(0, 0, WIDTH, HEIGHT,
  //                   0, 0, WIDTH, HEIGHT,
  //                   GL_COLOR_BUFFER_BIT, GL_NEAREST);TEST_OPENGL_ERROR();

  // glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glutSwapBuffers();TEST_OPENGL_ERROR();
}

void init_glut(int &argc, char *argv[]) {
  //glewExperimental = GL_TRUE;
  glutInit(&argc, argv);
  glutInitContextVersion(4,5);
  glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);TEST_OPENGL_ERROR();
  glutInitWindowPosition ( 100, 100 );TEST_OPENGL_ERROR();
  glutCreateWindow("Shader Programming");TEST_OPENGL_ERROR();

  glutDisplayFunc(display);TEST_OPENGL_ERROR();
  glutMouseFunc(on_mouse);TEST_OPENGL_ERROR();
  glutMotionFunc(on_motion);TEST_OPENGL_ERROR();
  glutMouseWheelFunc(on_wheel);TEST_OPENGL_ERROR();
  glutKeyboardFunc(handleKeyboard);TEST_OPENGL_ERROR();

  glutReshapeFunc(window_resize);TEST_OPENGL_ERROR();
}

bool init_glew() {
  if (glewInit()) {
    std::cerr << " Error while initializing glew";
    return false;
  }
  glGetError();
  return true;
}

void init_GL() {
  glEnable(GL_DEPTH_TEST);TEST_OPENGL_ERROR();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);TEST_OPENGL_ERROR();
  glEnable(GL_CULL_FACE);TEST_OPENGL_ERROR();
  glClearColor(0.4,0.4,0.4,1.0);TEST_OPENGL_ERROR();
  glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void init_object_vbo(GLuint prog_id, GLuint& bunny_id) {
  // int max_nb_vbo = 5;
  // int nb_vbo = 0;
  // int index_vbo = 0;
  // GLuint vbo_ids[max_nb_vbo];
  GLint vertex_location = glGetAttribLocation(prog_id,"position");TEST_OPENGL_ERROR();
  GLint normal_smooth_location = glGetAttribLocation(prog_id,"normalSmooth");TEST_OPENGL_ERROR();
  glGenVertexArrays(1, &bunny_id);TEST_OPENGL_ERROR();
  glBindVertexArray(bunny_id);TEST_OPENGL_ERROR();

  // glUseProgram(program_id_compute);
  GLuint compute_ssbo[3];
  glGenBuffers(3, compute_ssbo);

  // glUseProgram(program_id);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, compute_ssbo[0]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, vertex_buffer_data.size()*sizeof(float), vertex_buffer_data.data(), GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();
  // glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
  // glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, compute_ssbo[1]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, normal_smooth_buffer_data.size()*sizeof(float), normal_smooth_buffer_data.data(), GL_DYNAMIC_DRAW);TEST_OPENGL_ERROR();
  // glVertexAttribPointer(normal_smooth_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
  // glEnableVertexAttribArray(normal_smooth_location);TEST_OPENGL_ERROR();

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, compute_ssbo[2]);
  glBufferData(GL_SHADER_STORAGE_BUFFER, vertex_buffer_data.size()*sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();


  // if (vertex_location!=-1) nb_vbo++;
  // if (normal_smooth_location!=-1) nb_vbo++;
  // glGenBuffers(nb_vbo, vbo_ids);TEST_OPENGL_ERROR();

  glUseProgram(program_id);
  if (vertex_location!=-1) {
    glBindBuffer(GL_ARRAY_BUFFER, compute_ssbo[0]);TEST_OPENGL_ERROR();
    // glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size()*sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
    glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();
  }

  if (normal_smooth_location!=-1) {
    glBindBuffer(GL_ARRAY_BUFFER, compute_ssbo[1]);TEST_OPENGL_ERROR();
    // glBufferData(GL_ARRAY_BUFFER, normal_smooth_buffer_data.size()*sizeof(float), normal_smooth_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
    glVertexAttribPointer(normal_smooth_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(normal_smooth_location);TEST_OPENGL_ERROR();
  }

  glBindVertexArray(0);
}


std::string load(const std::string &filename) {
  std::ifstream input_src_file(filename, std::ios::in);
  std::string ligne;
  std::string file_content="";
  if (input_src_file.fail()) {
    std::cerr << "FAIL\n";
    return "";
  }
  while(getline(input_src_file, ligne)) {
    file_content = file_content + ligne + "\n";
  }
  file_content += '\0';
  input_src_file.close();
  return file_content;
}

bool init_POV(GLuint prog_id) {
  glUseProgram(prog_id);TEST_OPENGL_ERROR();
  GLint light_pos_location   = glGetUniformLocation(prog_id, "light_pos");
  GLint light_color_location = glGetUniformLocation(prog_id, "light_color");

  glUniform3f(light_pos_location, lightX, lightY, lightZ);
  glUniform3f(light_color_location, 1.0f, 1.0f, 1.0f);

  // GLint div_location = glGetUniformLocation(program_id, "nb_divisions");
  // glUniform1f(div_location, nbDivisions);

  // GLint mid_div_location1 = glGetUniformLocation(program_id, "middle_divisions1");
  // glUniform1f(mid_div_location1, middleDivisions1);

  // GLint mid_div_location2 = glGetUniformLocation(program_id, "middle_divisions2");
  // glUniform1f(mid_div_location2, middleDivisions2);

  update_projection();
  update_camera();
  return true;
}

bool init_frame_buffers()
{
  // Setup -- Creation & Binding
  glCreateFramebuffers(1, &frame_buffer_id);
  // if (frame_buffer_id < 0)
  //   throw std::runtime_error("Could not create Frame Buffer");
  std::cout << "Created frame buffer at location: " << frame_buffer_id << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

  glGenTextures(1, &fbo_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_texture);

  glGenRenderbuffers(1, &fbo_depth);
  glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth);

  // Operation -- Generation & Allocation ?
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, WIDTH, HEIGHT);
  
  // Attachment -- Attach to FBO
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);

  // // Draw
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();

  // glUseProgram(program_id);TEST_OPENGL_ERROR();
  // glBindVertexArray(bunny_vao_id);TEST_OPENGL_ERROR();
  // glDrawArrays(GL_PATCHES, 0, 4);TEST_OPENGL_ERROR();
  // glBindVertexArray(0);TEST_OPENGL_ERROR();

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "FBO incomplet: 0x" << std::hex << status << std::dec << std::endl;
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);TEST_OPENGL_ERROR();
  glBindTexture(GL_TEXTURE_2D, 0);TEST_OPENGL_ERROR();
  glBindRenderbuffer(GL_RENDERBUFFER, 0);TEST_OPENGL_ERROR();
  return true;
}

bool init_shaders() {
  // std::string vertex_src = load("base_bunny/vertex.shd");
  // std::string fragment_src = load("base_bunny/fragment.shd");
  std::string vertex_src = load("compute/vertex.shd");
  std::string fragment_src = load("compute/fragment.shd");
  std::string compute_src = load("compute/compute.shd");
  // std::string vertex_src = load("tesselation/vertex.shd");
  // std::string fragment_src = load("tesselation/fragment.shd");
  // std::string tesselation_control_src = load("tesselation/control.shd");
  // std::string tesselation_evaluation_src = load("tesselation/evaluation.shd");
  // std::string vertex_geometry_src = load("normal_bunny/vertex.shd");
  // std::string geometry_src = load("normal_bunny/geometry.shd");
  // std::string fragment_geometry_src = load("normal_bunny/fragment.shd");
  GLuint shader_id[3];
  GLint compile_status = GL_TRUE;
  char *vertex_shd_src = (char*)std::malloc(vertex_src.length()*sizeof(char));
  char *fragment_shd_src = (char*)std::malloc(fragment_src.length()*sizeof(char));
  char *compute_shd_src = (char*)std::malloc(compute_src.length()*sizeof(char));
  // char *tesselation_control_shd_src = (char*)std::malloc(tesselation_control_src.length()*sizeof(char));
  // char *tesselation_evaluation_shd_src = (char*)std::malloc(tesselation_evaluation_src.length()*sizeof(char));
  // char *vertex_geometry_shd_src = (char*)std::malloc(vertex_geometry_src.length()*sizeof(char));
  // char *geometry_shd_src = (char*)std::malloc(geometry_src.length()*sizeof(char));
  // char *fragment_geometry_shd_src = (char*)std::malloc(fragment_geometry_src.length()*sizeof(char));
  vertex_src.copy(vertex_shd_src, vertex_src.length());
  fragment_src.copy(fragment_shd_src, fragment_src.length());
  compute_src.copy(compute_shd_src, compute_src.length());
  // tesselation_control_src.copy(tesselation_control_shd_src, tesselation_control_src.length());
  // tesselation_evaluation_src.copy(tesselation_evaluation_shd_src, tesselation_evaluation_src.length());
  // vertex_geometry_src.copy(vertex_geometry_shd_src,vertex_geometry_src.length());
  // geometry_src.copy(geometry_shd_src,geometry_src.length());
  // fragment_geometry_src.copy(fragment_geometry_shd_src,fragment_geometry_src.length());


  shader_id[0] = glCreateShader(GL_VERTEX_SHADER);TEST_OPENGL_ERROR();
  shader_id[1] = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();
  shader_id[2] = glCreateShader(GL_COMPUTE_SHADER);TEST_OPENGL_ERROR();
  // shader_id[2] = glCreateShader(GL_TESS_CONTROL_SHADER);TEST_OPENGL_ERROR();
  // shader_id[3] = glCreateShader(GL_TESS_EVALUATION_SHADER);TEST_OPENGL_ERROR();
  // shader_id[2] = glCreateShader(GL_VERTEX_SHADER);TEST_OPENGL_ERROR();
  // shader_id[3] = glCreateShader(GL_GEOMETRY_SHADER);TEST_OPENGL_ERROR();
  // shader_id[4] = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();

  glShaderSource(shader_id[0], 1, (const GLchar**)&(vertex_shd_src), 0);TEST_OPENGL_ERROR();
  glShaderSource(shader_id[1], 1, (const GLchar**)&(fragment_shd_src), 0);TEST_OPENGL_ERROR();
  glShaderSource(shader_id[2], 1, (const GLchar**)&(compute_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[2], 1, (const GLchar**)&(tesselation_control_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[3], 1, (const GLchar**)&(tesselation_evaluation_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[2], 1, (const GLchar**)&(vertex_geometry_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[3], 1, (const GLchar**)&(geometry_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[4], 1, (const GLchar**)&(fragment_geometry_shd_src), 0);TEST_OPENGL_ERROR();
  for(int i = 0 ; i < 3; i++) {
    glCompileShader(shader_id[i]);TEST_OPENGL_ERROR();
    glGetShaderiv(shader_id[i], GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE) {
      GLint log_size;
      char *shader_log;
      glGetShaderiv(shader_id[i], GL_INFO_LOG_LENGTH, &log_size);
      shader_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
      if(shader_log != 0) {
	glGetShaderInfoLog(shader_id[i], log_size, &log_size, shader_log);
	std::cerr << "SHADER " << i << ": " << shader_log << std::endl;
	std::free(shader_log);
      }
      std::free(vertex_shd_src);
      std::free(fragment_shd_src);
      glDeleteShader(shader_id[0]);
      glDeleteShader(shader_id[1]);
      glDeleteShader(shader_id[2]);
      // glDeleteShader(shader_id[3]);
      // glDeleteShader(shader_id[4]);
      return false;
    }
  }
  std::free(vertex_shd_src);
  std::free(fragment_shd_src);
  std::free(compute_shd_src);
  // std::free(tesselation_control_shd_src);
  // std::free(tesselation_evaluation_shd_src);


  GLint link_status=GL_TRUE;
  program_id=glCreateProgram();TEST_OPENGL_ERROR();
  std::cout << "Program_id: " << program_id << std::endl;
  if (program_id==0) return false;
  for(int i = 0 ; i < 2; i++) {
    glAttachShader(program_id, shader_id[i]);TEST_OPENGL_ERROR();
  }
  glLinkProgram(program_id);TEST_OPENGL_ERROR();
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if (link_status!=GL_TRUE) {
    GLint log_size;
    char *program_log;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);
    program_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
    if(program_log != 0) {
      glGetProgramInfoLog(program_id, log_size, &log_size, program_log);
      std::cerr << "Program " << program_log << std::endl;
      std::free(program_log);
    }
    glDeleteProgram(program_id);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[0]);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[1]);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[2]);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[3]);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[4]);TEST_OPENGL_ERROR();
    program_id=0;
    return false;
  }

  link_status=GL_TRUE;
  program_id_compute=glCreateProgram();TEST_OPENGL_ERROR();
  if (program_id_compute==0) return false;
  for(int i = 2 ; i < 3 ; i++) {
      glAttachShader(program_id_compute, shader_id[i]);TEST_OPENGL_ERROR();
  }
  glLinkProgram(program_id_compute);TEST_OPENGL_ERROR();
  glGetProgramiv(program_id_compute, GL_LINK_STATUS, &link_status);
  if (link_status!=GL_TRUE) {
    GLint log_size;
    char *program_log;
    glGetProgramiv(program_id_compute, GL_INFO_LOG_LENGTH, &log_size);
    program_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
    if(program_log != 0) {
      glGetProgramInfoLog(program_id_compute, log_size, &log_size, program_log);
      std::cerr << "Program " << program_log << std::endl;
      std::free(program_log);
    }
    glDeleteProgram(program_id_compute);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[0]);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[1]);TEST_OPENGL_ERROR();
    glDeleteShader(shader_id[2]);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[3]);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[4]);TEST_OPENGL_ERROR();
    program_id_compute=0;
    return false;
  }
  glUseProgram(program_id_compute);TEST_OPENGL_ERROR();
  GLint vertex_location = glGetUniformLocation(program_id_compute, "nb_vertex");TEST_OPENGL_ERROR();
  glUniform1ui(vertex_location, vertex_buffer_data.size() / 3);TEST_OPENGL_ERROR();
  // GLint normal_length_pos = glGetUniformLocation(program_id_geometry, "normal_length");TEST_OPENGL_ERROR();
  // glUniform1f(normal_length_pos, normal_length);
  glUseProgram(program_id);TEST_OPENGL_ERROR();
  return true;
}

void anim() {
  GLint anim_time_location;
  glUseProgram(program_id_compute);
  anim_time_location =
  glGetUniformLocation(program_id_compute, "anim_time");
  glUniform1f(anim_time_location, anim_time);
  anim_time += 0.1;
  // testez une borne max.
  glutPostRedisplay();
}

void timer(int value) {
  anim();
  (void)value;
  glutTimerFunc(33,timer,0);
}

void init_anim() {
  glutTimerFunc(200,timer,0);
}

int main(int argc, char *argv[]) {
  init_glut(argc, argv);
  if (!init_glew())
    std::exit(-1);
  init_GL();
  init_shaders();
  init_object_vbo(program_id, bunny_vao_id);
  // init_object_vbo(program_id_geometry, bunny_vao_id_geometry);
  init_POV(program_id);
  // init_POV(program_id_geometry);
  // init_frame_buffers();
  init_anim();
  glutMainLoop();
}
