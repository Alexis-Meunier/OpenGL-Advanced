#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "bunny.hh"
#include "matrix.hh"

#define WINDOW_HORIZ_MID 512
#define WINDOW_VERT_MID 512

#define TEST_OPENGL_ERROR()                                                             \
  do {		  							\
    GLenum err = glGetError(); 					                        \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR! " << __LINE__ << std::endl;      \
  } while(0)


bool locked = true;

// 			      0.57735, -0.33333, 0.57735, 0.00000,
// 			      0.00000, 0.66667, 0.57735, 0.00000,
// 			      -0.57735, -0.33333, 0.57735, 0.00000,
// 			      0.00000, 0.00000, -17, 1.00000);
float camX = 15.0f, camY = 15.0f, camZ = -1.00020f;
float lightX = -10.0f, lightY = -10.0f, lightZ = -50.0f;
float horizAngl = 90.0f;
float vertAngl = 0.0f;
const float CAM_SPEED = 1.0f;
const float MOUSE_SENS = 0.2f;
float normal_length = 0.05f;

bool mouseWarped = false;

int lastMouseX = WINDOW_HORIZ_MID;
int lastMouseY = WINDOW_VERT_MID;

GLuint bunny_vao_id;
GLuint bunny_vao_id_geometry;
GLuint program_id;
GLuint program_id_geometry;

// void handleMouseLook(int x, int y) {
//     const int CX = WINDOW_HORIZ_MID;
//     const int CY = WINDOW_VERT_MID;

//     if (mouseWarped) {
//         mouseWarped = false;
//         return;
//     }

//     float dx = (x - CX) * MOUSE_SENS;
//     // We flip on y because the coordinates are inversed ?
//     float dy = (CY - y) * MOUSE_SENS;

//     horizAngl += dx;
//     vertAngl += dy;
//     // Clamp so we can't loop
//     if (vertAngl >  89.0f) vertAngl =  89.0f;
//     if (vertAngl < -89.0f) vertAngl = -89.0f;

//     mouseWarped = true;
//     glutWarpPointer(CX, CY);

//     glutPostRedisplay();
// }

// void get_camera_dirs(float& fx, float& fy, float& fz,
//                      float& rx, float& ry, float& rz)
// {
//     // get Angle as radians
//     float radhorizAngl = horizAngl * M_PI / 180.0f;
//     float radvertAngl = vertAngl * M_PI / 180.0f;

//     // Forward vector
//     fx = std::cos(radvertAngl) * std::cos(radhorizAngl);
//     fy = std::sin(radvertAngl);
//     fz = std::cos(radvertAngl) * std::sin(radhorizAngl);

//     float len = std::sqrt(fz*fz + fx*fx);
//     rx =  fz / len;
//     ry =  0.0f;
//     rz = -fx / len;
// }

// void handleKeyboard(unsigned char key, int x, int y) {
//     float fx, fy, fz, rx, ry, rz;
//     get_camera_dirs(fx, fy, fz, rx, ry, rz);
//     GLint normal_length_pos;

//     switch (key) {
//       // Z-Axis movement
//         case 'w': camX += fx * CAM_SPEED; camY += fy * CAM_SPEED; camZ += fz * CAM_SPEED; break;
//         case 's': camX -= fx * CAM_SPEED; camY -= fy * CAM_SPEED; camZ -= fz * CAM_SPEED; break;
//       // X-Axis movement
//         case 'd': camX -= rx * CAM_SPEED; camY -= ry * CAM_SPEED; camZ -= rz * CAM_SPEED; break;
//         case 'a': camX += rx * CAM_SPEED; camY += ry * CAM_SPEED; camZ += rz * CAM_SPEED; break;
//       // Y-Axis movement
//         case 'q': camY += CAM_SPEED; break;
//         case 'e': camY -= CAM_SPEED; break;
//         case 'u': lightX += fx * CAM_SPEED; lightY += fy * CAM_SPEED; lightZ += fz * CAM_SPEED; break;
//         case 'j': lightX -= fx * CAM_SPEED; lightY -= fy * CAM_SPEED; lightZ -= fz * CAM_SPEED; break;
//         case 'h': lightX -= rx * CAM_SPEED; lightY -= ry * CAM_SPEED; lightZ -= rz * CAM_SPEED; break;
//         case 'k': lightX += rx * CAM_SPEED; lightY += ry * CAM_SPEED; lightZ += rz * CAM_SPEED; break;
//         case 'y': lightY += CAM_SPEED; break;
//         case 'i': lightY -= CAM_SPEED; break;
//       // lock cursor when `space`
//         case ' ':
//           locked = !locked;
//           if (locked) {
//               glutSetCursor(GLUT_CURSOR_NONE);
//               glutPassiveMotionFunc(handleMouseLook);
//               glutWarpPointer(512, 512);
//           } else {
//               glutSetCursor(GLUT_CURSOR_INHERIT);
//               glutPassiveMotionFunc(nullptr);
//           }
//           break;
//         // case 13:
//         //   camX = 0.0f, camY = 0.0f, camZ = -50.0f;
//         //   horizAngl = 90.0f;
//         //   vertAngl = 0.0f;
//         //   break;
//         case '+':
//             normal_length += 0.01;
//             // glUseProgram(program_id_geometry);
//             // normal_length_pos = glGetUniformLocation(program_id_geometry, "normal_length");TEST_OPENGL_ERROR();
//             // glUniform1f(normal_length_pos, normal_length);
//             break;
//         case '-':
//             if (normal_length - 0.01 > 0)
//               normal_length -= 0.01;
//             // glUseProgram(program_id_geometry);
//             // normal_length_pos = glGetUniformLocation(program_id_geometry, "normal_length");TEST_OPENGL_ERROR();
//             // glUniform1f(normal_length_pos, normal_length);
//             break;
//       // if `escape` then close window
//         case 27: exit(0);
//     }

//     glutPostRedisplay();
// }

// void update_light() {
//     glUseProgram(program_id);TEST_OPENGL_ERROR();
//     GLint light_pos_location = glGetUniformLocation(program_id, "light_pos");
//     glUniform3f(light_pos_location, lightX, lightY, lightZ);
    
//     // glUseProgram(program_id_geometry);TEST_OPENGL_ERROR();
//     // light_pos_location = glGetUniformLocation(program_id_geometry, "light_pos");
//     // glUniform3f(light_pos_location, lightX, lightY, lightZ);
// }

// void update_camera() {
//     float fx, fy, fz, rx, ry, rz;
//     get_camera_dirs(fx, fy, fz, rx, ry, rz);

//     auto camera_mat = look_at(
//         camX, camY, camZ,
//         camX + fx, camY + fy, camZ + fz,
//         0.0f, 1.0f, 0.0f
//     );

//     glUseProgram(program_id);TEST_OPENGL_ERROR();
//     GLint camera_location = glGetUniformLocation(program_id, "camera");
//     glUniformMatrix4fv(camera_location, 1, GL_FALSE, camera_mat.get_values());
    
//     // glUseProgram(program_id_geometry);TEST_OPENGL_ERROR();
//     // camera_location = glGetUniformLocation(program_id_geometry, "camera");
//     // glUniformMatrix4fv(camera_location, 1, GL_FALSE, camera_mat.get_values());
// }


void window_resize(int width, int height) {
  //std::cout << "glViewport(0,0,"<< width << "," << height << ");TEST_OPENGL_ERROR();" << std::endl;
  glViewport(0,0,width,height);TEST_OPENGL_ERROR();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);TEST_OPENGL_ERROR();
  // update_camera();
  // update_light();

  glUseProgram(program_id);TEST_OPENGL_ERROR();
  glBindVertexArray(bunny_vao_id);TEST_OPENGL_ERROR();
  glDrawArrays(GL_PATCHES, 0, vertex_buffer_data.size());TEST_OPENGL_ERROR();
  // glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size());TEST_OPENGL_ERROR();
  glBindVertexArray(0);TEST_OPENGL_ERROR();

  // glUseProgram(program_id_geometry);TEST_OPENGL_ERROR();
  // glBindVertexArray(bunny_vao_id_geometry);TEST_OPENGL_ERROR();
  // glDrawArrays(GL_TRIANGLES, 0, vertex_buffer_data.size());TEST_OPENGL_ERROR();
  // glBindVertexArray(0);TEST_OPENGL_ERROR();
  glutSwapBuffers();
}

void init_glut(int &argc, char *argv[]) {
  //glewExperimental = GL_TRUE;
  glutInit(&argc, argv);
  glutInitContextVersion(4,5);
  glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(1024, 1024);
  glutInitWindowPosition ( 100, 100 );
  glutCreateWindow("Shader Programming");

  glutDisplayFunc(display);
  // glutPassiveMotionFunc(handleMouseLook);
  // glutKeyboardFunc(handleKeyboard);

  glutReshapeFunc(window_resize);
}

bool init_glew() {
  if (glewInit()) {
    std::cerr << " Error while initializing glew";
    return false;
  }
  return true;
}

void init_GL() {
  glEnable(GL_DEPTH_TEST);TEST_OPENGL_ERROR();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);TEST_OPENGL_ERROR();
  glEnable(GL_CULL_FACE);TEST_OPENGL_ERROR();
  glClearColor(0.4,0.4,0.4,1.0);TEST_OPENGL_ERROR();
}


void init_object_vbo(GLuint prog_id, GLuint& bunny_id) {
  int max_nb_vbo = 5;
  int nb_vbo = 0;
  int index_vbo = 0;
  GLuint vbo_ids[max_nb_vbo];

  GLint vertex_location = glGetAttribLocation(prog_id,"position");TEST_OPENGL_ERROR();
  GLint normal_smooth_location = glGetAttribLocation(prog_id,"normalSmooth");TEST_OPENGL_ERROR();

  glGenVertexArrays(1, &bunny_id);TEST_OPENGL_ERROR();
  glBindVertexArray(bunny_id);TEST_OPENGL_ERROR();

  if (vertex_location!=-1) nb_vbo++;
  if (normal_smooth_location!=-1) nb_vbo++;
  glGenBuffers(nb_vbo, vbo_ids);TEST_OPENGL_ERROR();

  if (vertex_location!=-1) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[index_vbo++]);TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size()*sizeof(float), vertex_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
    glVertexAttribPointer(vertex_location, 3, GL_FLOAT, GL_FALSE, 0, 0);TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();
  }

  if (normal_smooth_location!=-1) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[index_vbo++]);TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, normal_smooth_buffer_data.size()*sizeof(float), normal_smooth_buffer_data.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();
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
  GLint camera_location = glGetUniformLocation(prog_id, "camera");TEST_OPENGL_ERROR();
  GLint proj_location = glGetUniformLocation(prog_id, "projection");TEST_OPENGL_ERROR();
  GLint light_pos_location = glGetUniformLocation(prog_id, "light_pos");TEST_OPENGL_ERROR();
  GLint light_color_location = glGetUniformLocation(prog_id, "light_color");TEST_OPENGL_ERROR();
  // GLint normal_length_location = glGetUniformLocation(prog_id, "normal_length");TEST_OPENGL_ERROR();

  matrix4 bunny_camera_mat(
    {
      {0.57735, -0.33333, 0.57735, 0.00000},
      {0.00000, 0.66667, 0.57735, 0.00000},
      {-0.57735, -0.33333, 0.57735, 0.00000},
      {0.00000, 0.00000, -17, 1.00000}
    }
  );

  matrix4 plane_proj_mat(
    {
      {250.00000, 0.00000, 0.00000, 0.00000},
      {0.00000, 250.00000, 0.00000, 0.00000},
      {0.00000, 0.00000, -1.02020, -10.10101},
      {0.00000, 0.00000, -1.00000, 0.00000}
    }
  );

  // matrix4 proj_mat(
  //   {
  //     {15.00000, 0.00000, 0.00000, 0.00000},
  //     {0.00000, 15.00000, 0.00000, 0.00000},
  //     {0.00000, 0.00000, -1.00020, -1.00000},
  //     {0.00000, 0.00000, -10.00100, 0.00000}
  //   }
  // );

  glUniformMatrix4fv(camera_location, 1, GL_FALSE, bunny_camera_mat.get_values());TEST_OPENGL_ERROR();
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, plane_proj_mat.get_values());TEST_OPENGL_ERROR();
  glUniform3f(light_pos_location, -10.0, -10.0, -50.0);TEST_OPENGL_ERROR();
  glUniform3f(light_color_location, 1.0, 1.0, 1.0);TEST_OPENGL_ERROR();
  // glUniform1f(normal_length_location, 0.05f);TEST_OPENGL_ERROR();
  return true;
}


bool init_shaders() {
  std::string vertex_src = load("base_bunny/vertex.shd");
  std::string fragment_src = load("base_bunny/fragment.shd");
  std::string tesselation_control_src = load("tesselation/control.shd");
  std::string tesselation_evaluation_src = load("tesselation/evaluation.shd");
  // std::string vertex_geometry_src = load("normal_bunny/vertex.shd");
  // std::string geometry_src = load("normal_bunny/geometry.shd");
  // std::string fragment_geometry_src = load("normal_bunny/fragment.shd");
  GLuint shader_id[4];
  GLint compile_status = GL_TRUE;
  char *vertex_shd_src = (char*)std::malloc(vertex_src.length()*sizeof(char));
  char *fragment_shd_src = (char*)std::malloc(fragment_src.length()*sizeof(char));
  char *tesselation_control_shd_src = (char*)std::malloc(tesselation_control_src.length()*sizeof(char));
  char *tesselation_evaluation_shd_src = (char*)std::malloc(tesselation_evaluation_src.length()*sizeof(char));
  // char *vertex_geometry_shd_src = (char*)std::malloc(vertex_geometry_src.length()*sizeof(char));
  // char *geometry_shd_src = (char*)std::malloc(geometry_src.length()*sizeof(char));
  // char *fragment_geometry_shd_src = (char*)std::malloc(fragment_geometry_src.length()*sizeof(char));
  vertex_src.copy(vertex_shd_src, vertex_src.length());
  fragment_src.copy(fragment_shd_src, fragment_src.length());
  tesselation_control_src.copy(tesselation_control_shd_src, tesselation_control_src.length());
  tesselation_evaluation_src.copy(tesselation_evaluation_shd_src, tesselation_evaluation_src.length());
  // vertex_geometry_src.copy(vertex_geometry_shd_src,vertex_geometry_src.length());
  // geometry_src.copy(geometry_shd_src,geometry_src.length());
  // fragment_geometry_src.copy(fragment_geometry_shd_src,fragment_geometry_src.length());


  shader_id[0] = glCreateShader(GL_VERTEX_SHADER);TEST_OPENGL_ERROR();
  shader_id[1] = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();
  shader_id[2] = glCreateShader(GL_TESS_CONTROL_SHADER);TEST_OPENGL_ERROR();
  shader_id[3] = glCreateShader(GL_TESS_EVALUATION_SHADER);TEST_OPENGL_ERROR();
  // shader_id[2] = glCreateShader(GL_VERTEX_SHADER);TEST_OPENGL_ERROR();
  // shader_id[3] = glCreateShader(GL_GEOMETRY_SHADER);TEST_OPENGL_ERROR();
  // shader_id[4] = glCreateShader(GL_FRAGMENT_SHADER);TEST_OPENGL_ERROR();

  glShaderSource(shader_id[0], 1, (const GLchar**)&(vertex_shd_src), 0);TEST_OPENGL_ERROR();
  glShaderSource(shader_id[1], 1, (const GLchar**)&(fragment_shd_src), 0);TEST_OPENGL_ERROR();
  glShaderSource(shader_id[2], 1, (const GLchar**)&(tesselation_control_shd_src), 0);TEST_OPENGL_ERROR();
  glShaderSource(shader_id[3], 1, (const GLchar**)&(tesselation_evaluation_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[2], 1, (const GLchar**)&(vertex_geometry_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[3], 1, (const GLchar**)&(geometry_shd_src), 0);TEST_OPENGL_ERROR();
  // glShaderSource(shader_id[4], 1, (const GLchar**)&(fragment_geometry_shd_src), 0);TEST_OPENGL_ERROR();
  for(int i = 0 ; i < 4; i++) {
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
      glDeleteShader(shader_id[3]);
      // glDeleteShader(shader_id[4]);
      return false;
    }
  }
  std::free(vertex_shd_src);
  std::free(fragment_shd_src);
  std::free(tesselation_control_shd_src);
  std::free(tesselation_evaluation_shd_src);


  GLint link_status=GL_TRUE;
  program_id=glCreateProgram();TEST_OPENGL_ERROR();
  if (program_id==0) return false;
  for(int i = 0 ; i < 4 ; i++) {
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
    glDeleteShader(shader_id[3]);TEST_OPENGL_ERROR();
    // glDeleteShader(shader_id[4]);TEST_OPENGL_ERROR();
    program_id=0;
    return false;
  }

  // link_status=GL_TRUE;
  // program_id_geometry=glCreateProgram();TEST_OPENGL_ERROR();
  // if (program_id_geometry==0) return false;
  // for(int i = 2 ; i < 5 ; i++) {
  //     glAttachShader(program_id_geometry, shader_id[i]);TEST_OPENGL_ERROR();
  // }
  // glLinkProgram(program_id_geometry);TEST_OPENGL_ERROR();
  // glGetProgramiv(program_id_geometry, GL_LINK_STATUS, &link_status);
  // if (link_status!=GL_TRUE) {
  //   GLint log_size;
  //   char *program_log;
  //   glGetProgramiv(program_id_geometry, GL_INFO_LOG_LENGTH, &log_size);
  //   program_log = (char*)std::malloc(log_size+1); /* +1 pour le caractere de fin de chaine '\0' */
  //   if(program_log != 0) {
  //     glGetProgramInfoLog(program_id_geometry, log_size, &log_size, program_log);
  //     std::cerr << "Program " << program_log << std::endl;
  //     std::free(program_log);
  //   }
  //   glDeleteProgram(program_id_geometry);TEST_OPENGL_ERROR();
  //   glDeleteShader(shader_id[0]);TEST_OPENGL_ERROR();
  //   glDeleteShader(shader_id[1]);TEST_OPENGL_ERROR();
  //   glDeleteShader(shader_id[2]);TEST_OPENGL_ERROR();
  //   glDeleteShader(shader_id[3]);TEST_OPENGL_ERROR();
  //   glDeleteShader(shader_id[4]);TEST_OPENGL_ERROR();
  //   program_id_geometry=0;
  //   return false;
  // }
  // glUseProgram(program_id_geometry);
  // GLint normal_length_pos = glGetUniformLocation(program_id_geometry, "normal_length");TEST_OPENGL_ERROR();
  // glUniform1f(normal_length_pos, normal_length);
  glUseProgram(program_id);TEST_OPENGL_ERROR();
  return true;
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
  glutMainLoop();
}
