/* 
   Alex Ehm 
   CPE 471 final project Fall 2016
   base Code:
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#include <algorithm>
#include <math.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "texture.h"
#include "cubemap.h"
#include "Particle.h"
#include "Multishape.h"
#include "Material.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
int g_width, g_height;

// Shader programs
shared_ptr<Program> phongProg;
shared_ptr<Program> groundProg;
shared_ptr<Program> particleProg;
shared_ptr<Program> cubemapProg;

// models
shared_ptr<Shape> bunny;
shared_ptr<Shape> cube;
vector<shared_ptr<Multishape>> stones;
vector<shared_ptr<Multishape>> trees;
static unsigned int diffStones = 3;
static unsigned int diffTrees = 5;

// materials
vector<Material> treeMaterials;
Material stoneMaterial;

// Position information for models
static unsigned int noStones = 160;
float randomPositionStones[320] = {0.0};
float randomRotStones[160] = {0.0};
static unsigned int noTrees = 160;
float randomPositionTrees[320] = {0.0};
float randomRotTrees[160] = {0.0};

// Ground plane
GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
int g_GiboLen;

// Lighting
vec3 lightColor;
vec3 lightPos;

// View
vec3 eyePos;
vec3 up;
vec3 lookAtPoint;
float deg80InRad = 1.39626;

// Matrix Stacks
shared_ptr<MatrixStack> P;
shared_ptr<MatrixStack> M;
shared_ptr<MatrixStack> V;

// Key down and movement information
bool wPressed, aPressed, sPressed, dPressed;
bool jump;
bool jumpUp;
double lastFrameTime;
double horizontalRotationAngle, verticalRotationAngle;
float moveSpeed;
float gravity = 0.004;
float velocity;
float maxMoveRange = 20.0;

// World extend
double worldExtend = 40.0;
double cubemapExtend = 45.0;

// Textures
Texture texture0;
GLint h_texture0;
Texture texture_A;
GLint h_texture_A;
Cubemap cubemap;
GLint h_cubemap;

// Fire aka particles
vec3 firePos;
vector<shared_ptr<Particle>> particles;
const int numP = 2500;
static GLfloat points[numP * 3];
static GLfloat pointColors[numP * 4];
static GLfloat lifes[numP];
GLuint pointsbuffer;
GLuint colorbuffer;
GLuint lifebuffer;
int frameCounter = 0;
int frameCounter2 = 0;
bool fireMove = false;
float maxParticleSize = 50.0;
float minParticleSize = 10.0;

// Display time to control fps
float t0_disp = 0.0f;
float t_disp = 0.0f;
bool keyToggles[256] = {false};
float t = 0.0f; //reset in init
float h = 0.0f;
vec3 g(0.0f, 0.0f, 0.0f);


static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
   if(key == GLFW_KEY_W && action == GLFW_PRESS) {
	   wPressed = true;
	}
   if(key == GLFW_KEY_A && action == GLFW_PRESS) {
      aPressed = true;
	}
   if(key == GLFW_KEY_S && action == GLFW_PRESS) {
	   sPressed = true;
	}
   if(key == GLFW_KEY_D && action == GLFW_PRESS) {
      dPressed = true;
	}
   if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
      if(!jump){
         jump = true;
         jumpUp = true;
         velocity = 0.2;
      }
	}
   if(key == GLFW_KEY_W && action == GLFW_RELEASE) {
	   wPressed = false;
	}
   if(key == GLFW_KEY_A && action == GLFW_RELEASE) {
      aPressed = false;
	}
   if(key == GLFW_KEY_S && action == GLFW_RELEASE) {
	   sPressed = false;
	}
   if(key == GLFW_KEY_D && action == GLFW_RELEASE) {
      dPressed = false;
	}

   if(key == GLFW_KEY_F && action == GLFW_PRESS) {
      fireMove = !fireMove;   
	}
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
      glfwGetCursorPos(window, &posX, &posY);
	}
   
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

static void createRandomSnowPointAt(int i){
   randomPositionStones[i * 2] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(worldExtend*2))) - worldExtend;
   randomPositionStones[i * 2 + 1] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(worldExtend*2))) - worldExtend;
}


static void createRandomBunnyPointAt(int i){
   randomPositionTrees[i * 2] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(worldExtend*2))) - worldExtend;
   randomPositionTrees[i * 2 + 1] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(worldExtend*2))) - worldExtend;
}

/* create random positions for trees and stones */
static void createRandomPositions(){

   for(int i = 0; i <= noStones; i++){
      createRandomSnowPointAt(i);
      randomRotStones[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(2*M_PI)));
   }

   for(int i = 0; i <= noTrees; i++){
      createRandomBunnyPointAt(i);
      randomRotTrees[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(2*M_PI)));
   }
}

/* code to define the ground plane */
static void initGeom() {

   float g_groundSize = worldExtend;
   float g_groundY = 0.0;

  // A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
    float GrndPos[] = {
    -g_groundSize, 0.0, -g_groundSize,
    -g_groundSize, 0.0,  g_groundSize,
     g_groundSize, 0.0,  g_groundSize,
     g_groundSize, 0.0, -g_groundSize
    };

    float GrndNorm[] = {
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0
    };

 static GLfloat GrndTex[] = {
      0, 0, // back
      0, g_groundSize,
      g_groundSize, g_groundSize,
      g_groundSize, 0   };

    unsigned short idx[] = {0, 1, 2, 0, 2, 3 };


   GLuint VertexArrayID;
	//generate the VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    g_GiboLen = 6;
    glGenBuffers(1, &GrndBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

    glGenBuffers(1, &GrndNorBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);
    
	 glGenBuffers(1, &GrndTexBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

    glGenBuffers(1, &GIndxBuffObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);


   //generate vertex buffer to hand off to OGL - using instancing
   glGenBuffers(1, &pointsbuffer);
   //set the current state to focus on our vertex buffer
   glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
   //actually memcopy the data - only do this once
   glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
   
	glGenBuffers(1, &colorbuffer);
   //set the current state to focus on our vertex buffer
   glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
   //actually memcopy the data - only do this once
   glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &lifebuffer);
   //set the current state to focus on our vertex buffer
   glBindBuffer(GL_ARRAY_BUFFER, lifebuffer);
   //actually memcopy the data - only do this once
   glBufferData(GL_ARRAY_BUFFER, sizeof(lifes), NULL, GL_STREAM_DRAW);
}

static void initPositions()
{
   firePos = vec3(0.0, -1.0, 0.0);
   eyePos = vec3(0.0, 0.0, 10.0);
   lookAtPoint = vec3(0.0, 0.0, 9.0);
   up = vec3(0.0, 1.0, 0.0);

   createRandomPositions();
}

static void initLight()
{
   lightColor = vec3(1.0, 1.0, 1.0);
   lightPos = vec3(firePos.x, 1.0, firePos.z);
}

static void loadModels()
{
   // load different tree models
   for(int i = 2; i < 2+diffTrees; i++){
      shared_ptr<Multishape> tree = make_shared<Multishape>();
      trees.push_back(tree);
      tree->loadMesh(RESOURCE_DIR + "free lowpoly trees pack.obj", i*2, (i*2)+1);
      tree->resize();
	   tree->init();
      
   }

   // load different stone models
   for(int i = 1; i < 1+diffStones; i++){
      shared_ptr<Multishape> stone = make_shared<Multishape>();
      stones.push_back(stone);
      stone->loadMesh(RESOURCE_DIR + "Boulder" + to_string(i) + ".obj");
      stone->resize();
	   stone->init();
      
   }

   // load bunny model
	bunny = make_shared<Shape>();
	bunny->loadMesh(RESOURCE_DIR + "bunny.obj");
	bunny->resize();
	bunny->init();

   // load cube model
	cube = make_shared<Shape>();
	cube->loadMesh(RESOURCE_DIR + "cube.obj");
	cube->resize();
	cube->init();
}

static void loadPhongShaders()
{
	phongProg = make_shared<Program>();
	phongProg->setVerbose(true);
	phongProg->setShaderNames(RESOURCE_DIR + "phong_vert.glsl", RESOURCE_DIR + "phong_frag.glsl");
	phongProg->init();
   phongProg->addUniform("M");
   phongProg->addUniform("MVP");
   phongProg->addUniform("transInvM");
	phongProg->addUniform("diffuseMatColor");
   phongProg->addUniform("shininess");
	phongProg->addUniform("specularMatColor");
	phongProg->addUniform("lightColor");
	phongProg->addUniform("lightPos");
   phongProg->addUniform("ambientMatColor");
   phongProg->addUniform("viewVec");
   phongProg->addUniform("eyePos");
	phongProg->addAttribute("vertPos");
	phongProg->addAttribute("vertNor");
   phongProg->addAttribute("vertTex");
}

static void loadGroundShaders()
{
	groundProg = make_shared<Program>();
	groundProg->setVerbose(true);
	groundProg->setShaderNames(RESOURCE_DIR + "ground_vert.glsl", RESOURCE_DIR + "ground_frag.glsl");
	groundProg->init();
   groundProg->addUniform("M");
	groundProg->addUniform("MVP");
   groundProg->addUniform("transInvM");
	groundProg->addUniform("Texture0");
	groundProg->addAttribute("vertPos");
   groundProg->addAttribute("vertNor");
	groundProg->addAttribute("vertTex");
	groundProg->addUniform("lightColor");
	groundProg->addUniform("lightPos");
   groundProg->addUniform("viewVec");
   groundProg->addUniform("eyePos");
	groundProg->addTexture(&texture0);
}

static void loadParticleShaders()
{
	cubemapProg = make_shared<Program>();
	cubemapProg->setVerbose(true);
	cubemapProg->setShaderNames(RESOURCE_DIR + "cubemap_vert.glsl", RESOURCE_DIR + "cubemap_frag.glsl");
	cubemapProg->init();
   cubemapProg->addUniform("MVP");
	cubemapProg->addUniform("cubemap");
	cubemapProg->addAttribute("vertPos");
   cubemapProg->addAttribute("vertNor");
	cubemapProg->addAttribute("vertTex");
}

static void loadCubemapShaders()
{
	particleProg = make_shared<Program>();
	particleProg->setVerbose(true);
	particleProg->setShaderNames(RESOURCE_DIR + "particle_vert.glsl", RESOURCE_DIR + "particle_frag.glsl");
	particleProg->init();
   particleProg->addUniform("MVP");
	particleProg->addAttribute("vertPos");
	particleProg->addAttribute("Pcolor");
	particleProg->addAttribute("lifePercent");
	particleProg->addTexture(&texture_A);
}

static void loadShaders()
{
   loadPhongShaders();
   loadGroundShaders();
   loadParticleShaders();
   loadCubemapShaders();
}

static void loadTextures()
{
   texture0.setFilename(RESOURCE_DIR + "grass.bmp");
   texture0.setUnit(0);
   texture0.setName("Texture0");
   texture0.init();
   texture_A.setFilename(RESOURCE_DIR+"fireatlas.bmp");
   texture_A.setUnit(0);
   texture_A.setName("alphaTexture");
   texture_A.init();

   cubemap.setFilename(RESOURCE_DIR + "nightsky/");
   cubemap.setUnit(0);
   cubemap.setName("cubemap");
   cubemap.init();
}

static void initParticles()
{
   int n = numP;
   for(int i = 0; i < n; ++i) {
      auto particle = make_shared<Particle>(firePos); 
      particles.push_back(particle);
      particle->load();
   }
}

static void initMaterials()
{
   Material m;
   m.setAmbient(0.02, 0.12, 0.04);
   m.setDiffuse(0.11, 0.6, 0.2);
   m.setSpecular(0.15, 0.83, 0.28);
   m.setShininess(4.0);

   Material m2;
   m2.setAmbient(0.15, 0.09, 0.03);
   m2.setDiffuse(0.55, 0.27, 0.07);
   m2.setSpecular(0.82, 0.41, 0.12);
   m2.setShininess(4.0);

   
   treeMaterials.push_back(m2);   
   treeMaterials.push_back(m);

   stoneMaterial.setAmbient(0.13, 0.13, 0.14);
   stoneMaterial.setDiffuse(0.3, 0.3, 0.4);
   stoneMaterial.setSpecular(0.3, 0.3, 0.4);
   stoneMaterial.setShininess(4.0);
}

static void init()
{
	GLSL::checkVersion();

   glfwSetTime(0.0);

   loadModels();
   initMaterials();
   initPositions();
   initLight();

   moveSpeed = 0.1;

   t = 0.0f;
   h = 0.01f;
   g = vec3(0.0f, -0.01f, 0.0f);

	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
  	glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
   glfwSetCursorPos(window, width/2, height/2);
   horizontalRotationAngle = -0.5 * M_PI;
   verticalRotationAngle = 0.0;

	// Set background color.
	glClearColor(.12f, .34f, .56f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

   // Load Shaders
   loadShaders();
   
   // Intialize textures
   loadTextures();

   // Initialize the particles
   initParticles();

}


void updateGeom() {
	vec3 pos;
	vec4 col;
   float life;

	//go through all the particles and update the CPU buffer
   for (int i = 0; i < numP; i++) {
		pos = particles[i]->getPosition();
		col = particles[i]->getColor();
      life = particles[i]->getLifePercent(t);
		points[i*3+0] =pos.x; 
		points[i*3+1] =pos.y; 
		points[i*3+2] =pos.z; 
		pointColors[i*4+0] =col.r + col.a/10; 
		pointColors[i*4+1] =col.g + col.g/10; 
		pointColors[i*4+2] =col.b + col.b/10;
		pointColors[i*4+3] =col.a;
      lifes[i] = life;
	} 

	//update the GPU data
	glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numP*3, points);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numP*4, pointColors);
   glBindBuffer(GL_ARRAY_BUFFER, lifebuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(lifes), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numP, lifes);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   
}

// Sort particles by their z values in camera space
class ParticleSorter {
public:
   bool operator()(const shared_ptr<Particle> p0, const shared_ptr<Particle> p1) const
   {
      // Particle positions in world space
      const vec3 &x0 = p0->getPosition();
      const vec3 &x1 = p1->getPosition();
      // Particle positions in camera space
      vec4 x0w = C * vec4(x0.x, x0.y, x0.z, 1.0f);
      vec4 x1w = C * vec4(x1.x, x1.y, x1.z, 1.0f);
      return x0w.z < x1w.z;
   }
  
   mat4 C; // current camera matrix
};
ParticleSorter sorter;

void updateParticles(FractalNoise *noiseMaker) {

  //update the particles
  for(auto particle : particles) {
   particle->update(t, h, g, keyToggles, noiseMaker);
  }
  t += h;
 
	sorter.C = V->topMatrix(); 
	sort(particles.begin(), particles.end(), sorter);
}

static void bindMatricies()
{
   mat4 MVP = P->topMatrix() * V->topMatrix() * M->topMatrix();
   mat4 transInvM = transpose(inverse(M->topMatrix()));
   glUniformMatrix4fv(phongProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   glUniformMatrix4fv(phongProg->getUniform("MVP"), 1, GL_FALSE, value_ptr(MVP));
   glUniformMatrix4fv(phongProg->getUniform("transInvM"), 1, GL_FALSE, value_ptr(transInvM));
}

/* draws a bunny at a specific location
   just because in every scene there should be a oversized bunny
   because: Bunnies! */
static void drawBunny(){

   glUniform3f(phongProg->getUniform("ambientMatColor"), 0.1913, 0.0735, 0.0225);
   glUniform3f(phongProg->getUniform("diffuseMatColor"), 0.7038, 0.27048, 0.0828);
   glUniform3f(phongProg->getUniform("specularMatColor"), 0.257, 0.1376, 0.08601);
   glUniform1f(phongProg->getUniform("shininess"), 12.8);

   M->pushMatrix();
   M->translate(vec3(19.0, 0.0, 19.0));

   bindMatricies();
	bunny->draw(phongProg);

   M->popMatrix();
}

/* Draw the trees */
static void drawTree(){	

   for(int i = 0; i <= noTrees; i++){
      M->pushMatrix();
      M->translate(vec3(randomPositionTrees[i * 2], 0, randomPositionTrees[i * 2 + 1]));
      M->rotate(randomRotTrees[i], vec3(0,1,0));

      M->translate(vec3(0, 2.0, 0.0));
      M->rotate(1.0 * M_PI, vec3(0,1,0));
      M->scale(vec3(4.0));

      bindMatricies();

      trees[i%diffTrees]->draw(phongProg, treeMaterials);
      M->popMatrix();
   }

}

/* Draw the stones */
void drawStones(){

   for(int i = 0; i <= noStones; i++){
      M->pushMatrix();
      M->translate(vec3(randomPositionStones[i * 2], -1.0, randomPositionStones[i * 2 + 1]));
      M->rotate(randomRotStones[i], vec3(0,1,0));

      bindMatricies();

      stones[i%diffStones]->draw(phongProg, stoneMaterial);
      M->popMatrix();
   }
}

/* Scale the Particle Size dependent on distance to fire */
static void scaleParticles(){
   float fireDist = distance(vec2(firePos.x, firePos.z), vec2(eyePos.x, eyePos.z));
   float pSize;
   float sUnit = (maxParticleSize - minParticleSize) / 13.0;
   if(fireDist <= 2.0){
      pSize = maxParticleSize;
   } else if(fireDist >= 15.0){
      pSize = minParticleSize;
   } else {
      float distScale = fireDist - 2;
      distScale -= 13;
      distScale *= -1;
      
      pSize = minParticleSize + distScale * sUnit;
      
   }   
   glPointSize(pSize);

}

/* calculate the current view rotation dependent on mouse input */
static void calcViewRotation(){
   double xMouse, yMouse;
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   double halfWidth = double(width/2);
   double halfHeight = double(height/2);
   glfwGetCursorPos(window, &xMouse, &yMouse);

   glfwSetCursorPos(window, width/2, height/2);

   horizontalRotationAngle += ((xMouse - halfWidth)/halfWidth) * 0.5 * M_PI;
   verticalRotationAngle -= ((yMouse - halfHeight)/halfHeight) * 0.5 * M_PI;

   if (verticalRotationAngle > deg80InRad) {
      verticalRotationAngle = deg80InRad;
   } else if (verticalRotationAngle < -deg80InRad) {
      verticalRotationAngle = -deg80InRad;
   }

}

/* calculate eyePos and lookAt for move */
static void move(vec3 moveVec){
   vec3 newEyePos = eyePos + (moveSpeed * moveVec);

   if(newEyePos.x > maxMoveRange) {
      newEyePos.x = maxMoveRange;
   } else if(newEyePos.x < -maxMoveRange) {
      newEyePos.x = -maxMoveRange;
   }

   if(newEyePos.z > maxMoveRange) {
      newEyePos.z = maxMoveRange;
   } else if(newEyePos.z < -maxMoveRange) {
      newEyePos.z = -maxMoveRange;
   }

   vec3 actualMove = newEyePos - eyePos;
   eyePos = newEyePos;
   lookAtPoint += actualMove;
}

/* move in the world */
static void calculateMovement(){

   calcViewRotation();
   vec3 offset = vec3( cos(verticalRotationAngle) * cos(horizontalRotationAngle),
                           sin(verticalRotationAngle),
                           cos(verticalRotationAngle) * cos((0.5 * M_PI) - horizontalRotationAngle) );

   lookAtPoint = eyePos + offset;
   
   vec3 w = normalize(lookAtPoint - eyePos) * (-1.0f);
   vec3 u = normalize(cross(up, w));


   vec3 strafe = vec3(u.x, 0.0, u.z);
   vec3 zoom = vec3(w.x, 0.0, w.z);
   
   if(wPressed){
      move(-zoom);
   }
   if(sPressed){
      move(zoom);
   }
   if(aPressed){
      move(-strafe);
   }
   if(dPressed){
      move(strafe);
   }

   if(jump){
      eyePos = vec3(eyePos.x, eyePos.y + velocity, eyePos.z);
      lookAtPoint = vec3(lookAtPoint.x, lookAtPoint.y + velocity, lookAtPoint.z);
      if (eyePos.y <= 0.0001) {
         eyePos = vec3(eyePos.x, 0.0, eyePos.z);
         jump = false;
      }         
      velocity -= gravity;
   }
}

static void movingFire()
{
   if(fireMove){
      frameCounter++;
      frameCounter = frameCounter%5;

      if(frameCounter == 0){

         frameCounter2++;
         frameCounter2 = frameCounter2%4;

         float noiz = 0.2;

         if(frameCounter2 == 0){
            lightPos = vec3(lightPos.x+noiz, lightPos.y, lightPos.z+noiz);
         } else if(frameCounter2 == 1){
            lightPos = vec3(lightPos.x-noiz, lightPos.y, lightPos.z+noiz);
         } else if(frameCounter2 == 2){
            lightPos = vec3(lightPos.x-noiz, lightPos.y, lightPos.z-noiz);
         } else {
            lightPos = vec3(lightPos.x+noiz, lightPos.y, lightPos.z-noiz);
         }
      }
   }

}

static void drawCubemap()
{
      M->pushMatrix();
      M->scale(vec3(cubemapExtend, cubemapExtend, cubemapExtend));      

      cubemapProg->bind();

      mat4 MVP = P->topMatrix() * V->topMatrix() * M->topMatrix();
	   glUniformMatrix4fv(cubemapProg->getUniform("MVP"), 1, GL_FALSE, value_ptr(MVP));
      
      cubemap.bind();
   
	   cube->draw(cubemapProg);

      M->popMatrix();
}

static void drawGroundplane()
{
      M->pushMatrix();

	   //draw the ground plane	
	   groundProg->bind();

      glUniform3fv(groundProg->getUniform("lightPos"), 1,  value_ptr(lightPos));
      glUniform3fv(groundProg->getUniform("lightColor"), 1,  value_ptr(lightColor));

      M->pushMatrix();
      M->translate(vec3(0.0, -1., 0.0));

      mat4 MVP = P->topMatrix() * V->topMatrix() * M->topMatrix();
      mat4 transInvM = transpose(inverse(M->topMatrix()));
      vec3 viewVec = lookAtPoint - eyePos;

      glUniform3fv(groundProg->getUniform("eyePos"), 1, value_ptr(eyePos));
      glUniform3fv(groundProg->getUniform("viewVec"), 1,  value_ptr(viewVec));

      glUniformMatrix4fv(groundProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
      glUniformMatrix4fv(groundProg->getUniform("MVP"), 1, GL_FALSE, value_ptr(MVP));
      glUniformMatrix4fv(groundProg->getUniform("transInvM"), 1, GL_FALSE, value_ptr(transInvM));

	   glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	   glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	    
	   glEnableVertexAttribArray(2);
      glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, (void*)(0 * sizeof(unsigned short)));

      M->popMatrix();
}

static void drawObjects()
{
      phongProg->bind();
   
      vec3 eyeV(V->topMatrix() * vec4(eyePos, 1.0));
      vec3 viewVec = lookAtPoint - eyePos;

      glUniform3fv(phongProg->getUniform("lightPos"), 1,  value_ptr(lightPos));
      glUniform3fv(phongProg->getUniform("lightColor"), 1,  value_ptr(lightColor));

      glUniform3fv(phongProg->getUniform("eyePos"), 1, value_ptr(eyePos));
      glUniform3fv(phongProg->getUniform("viewVec"), 1,  value_ptr(viewVec));

      drawTree();
      drawStones();
      drawBunny();
}

static void drawParticles(FractalNoise *noiseMaker)
{
	particleProg->bind();
	updateParticles(noiseMaker);
	updateGeom();

   mat4 MVP = P->topMatrix() * V->topMatrix() * M->topMatrix();

   glUniformMatrix4fv(particleProg->getUniform("MVP"), 1, GL_FALSE, value_ptr(MVP));

   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,(void*)0);
   
	glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,(void*)0);
   
	glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, lifebuffer);
   glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0,(void*)0);

	glVertexAttribDivisor(0, 1);
	glVertexAttribDivisor(1, 1);
   glVertexAttribDivisor(2, 1);   

   scaleParticles();
	glDrawArraysInstanced(GL_POINTS, 0, 1, numP);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
   glVertexAttribDivisor(2, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
	particleProg->unbind();
}

static void render(FractalNoise *noiseMaker)
{

	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   float aspect = width/(float)height;

   P = make_shared<MatrixStack>();
   M = make_shared<MatrixStack>();
   V = make_shared<MatrixStack>();

   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

   calculateMovement();

   movingFire();

   V->loadIdentity();
   V->lookAt(eyePos, lookAtPoint, up);
   
   

   M->pushMatrix();
   M->loadIdentity();
     
   drawCubemap();
   drawGroundplane();
   drawObjects();
   drawParticles(noiseMaker);

   M->popMatrix();
   P->popMatrix();

}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
      cout << "The correct format is: './P3 resource_directory'" << endl;
      cout << "eg. ./P3 ../resources/" << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(1920, 1080, "Alex Ehm", glfwGetPrimaryMonitor(), NULL);
   //window = glfwCreateWindow(1920, 1080, "Alex Ehm", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
   glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
   //set the mouse call back
   glfwSetMouseButtonCallback(window, mouse_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

	// Initialize scene. Note geometry initialized in init now
	init();
   initGeom();

   FractalNoise *noiseMaker = new FractalNoise();
	noiseMaker->setBaseFrequency(4.0f);

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render(noiseMaker);
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
