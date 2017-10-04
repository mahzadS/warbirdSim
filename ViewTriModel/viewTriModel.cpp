/*

viewTriModel.cpp

465 utility include files:  shader465.hpp, triModel465.hpp, glmUtils465.hpp  
Shaders:  simpleVertex.glsl  amd simpleFragment.glsl

C OpenGL Core 3.3 example that loads cube.tri model file
and rotates the cube.

User commands:
'a' switch between interval timer (25 fps) and idle timer (faster?)
'f' view from front (0, 0, 3) looking at origin
't' view from above (0, 3, 3) looking at origin
'b' view from below (0, -3, 0) looking at origin
'w' wireframe or surface 

Current state is displayed in the window title.
 
The cube.tri model uses 12 triangular surfaces to display 6 "quad" surfaces.  
So, there are 36 (3 * 12) vertices.

The triModel465.hpp utility creates colors, and normals for ever vertex.
However the simple*.glsl shaders do not use the normal. 
Since the same color is created for every vertex of every surface,
the object is rendered with flat shading.

A reference for estimating fps using glut functions:
http://mycodelog.com/2010/04/16/fps/

Mahzad Shahriari, Tom England, ......
10/09/2017
*/

// define your target operating system: __Windows__  __Linux__  __Mac__
# define __Windows__ 
# include "../includes465/include465.hpp"

///global variables///
const int nModels = 7, ruber = 0, unum = 1, duo= 2, primus = 3, secundus = 4, ship = 5, missle = 6, sky=7;

GLuint vColor[nModels], vNormal[nModels], vPosition[nModels];
GLuint buffer[nModels];//create and initilize a buffer object
/////////////////char * modelFile = "cube-1-1-1.tri";  // name of tri model file
char *modelFile[nModels] = {"ruber.tri", "unum.tri", "duo.tri", "primus.tri", "secundus.tri", "warbird.tri", "missle.tri"};


/////////const GLuint nVertices =   12 * 3;   //3 vertices per line of model file  
const GLuint nVertices[nModels] = {264*3, 264*3, 264*3, 264*3, 264*3, 996*3, 252*3};

///////////////////////////////////////////////////////////
//float boundingRadius;  // modelFile's bounding radius
float boundingRadius[nModels]; //modelFiles' bounding radius
float scaleValue[nModels]; //scaling size value

GLuint indxBufferObj; //indexBufferObject

int Index =  0, timeQuantum=5, pitch=0, roll=0, yaw=0, int accelerate=0, int speed=25;  // global variable indexing into VBO arrays
// display state and "state strings" for title display
bool perspective = 1, initialUpdate=1;
GLfloat aspectRatio;
// window title strings
char baseStr[50] =    "465 ViewTriModel Example {a, w, f, t, b} : ";
char viewStr[15] =    " front view";
char fpsStr[15], timerStr[20] = " interval timer";
char titleStr [100]; 
///added
char warbirdStr[50];
char viewStr[50] = "Camera warbird view";
char base1Str[50] = "Enum eye";
char case2Str[50] = "Duo eye";
//GLuint vao;  // VertexArrayObject
GLuint vao[nModels];//vetex array object


GLuint shaderProgram; //-----doesn't change
char * vertexShaderFile = "simpleVertex.glsl";
char * fragmentShaderFile = "simpleFragment.glsl";
GLuint MVP;  // ModelViewProjection matrix handle
GLuint MV; //ModelView Handle
GLuint NM; //NormalMatrix Handle
glm::mat4 projectionMatrix;     // set in display
glm::mat4 modelViewMatrix;      // set in display()
glm::mat4 modelViewProjectionMatrix;  // set in display()

// vectors and values for lookAt
glm::vec3 eye, at, up;
//glm::mat4 cameraUpdate(int cam);

////////////added
float modelSize[nModels] = {2000.0f, 200.0f, 400.0f, 100.0f, 150.0f, 100.0f, 45.0f};//models' size
float modelRadians[nModels] = {0.0f, 0.004f, 0.002f, 0.002f, 0.004f, 0.0f, 0.0f };//The radians of the models
glm::vec3 scale[nModels]; //set in int()
glm::vec3 translate[nModels]= {glm::vec3(0,0,0), glm::vec3(4000,0,0), glm::vec3(9000,0,0), glm::vec3(11000, 0, 0), glm::vec3(13000, 0,0), glm::vec3(15000, 0,0), glm::vec3(14500, 0, 0)};



// vectors for "model"
//glm::vec4 vertex[nVertices];
glm::vec4 vertex[nModels];
//glm::vec3 normal[nVertices];
glm::vec3 normal[nModels];
glm::vec4 diffuseColorMaterial[nModels];
//glm::vec4 diffuseColorMaterial[nVertices];


// rotation variables  -- the modelMatrix
GLfloat rotateRadian = 0.0f;
float eyeDistanceMultiplier = 10.0f;
float eyeDistance;
glm::mat4 identity(1.0f); 
//glm::mat4 rotation;  // the modelMatrix
glm::mat4 rotation[nModels];
///////////////////////////////////////////
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat3 normalMatrix;
glm::mat4 projectionMatrix;
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 orientation[nModels];


/////////////////////////// May need to remove
static const unsigned int indices[] = 
{
	0, 1, 2, // 0 left square bottom
	1, 2, 3, // 1 left square top
};





int timerDelay = 40, frameCount = 0;
double currentTime, lastTime, timeInterval;
bool idleTimerFlag = false;  // interval or idle timer ?
bool wireFrame = false;     // initially show surfaces

void init (void) {
 /* printf("init:  sizeof glm::vec4 %d point %d diffuseColorMaterial %d normal %d \n", 
    sizeof(glm::vec4), sizeof(vertex), sizeof(diffuseColorMaterial), sizeof(normal));*/

  shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);//loads shaderProgram
  glUseProgram(shaderProgram);

  //creating vaos and vbos
  glGenVertexArrays(nModels, vao); //(1, &vao)
  glBindVertexArray(vao[7]);
  //////////glBindVertexArray(vao);

  glGenBuffers(nModels, buffer);
  //glGenBuffers( 1, &buffer );

  glGenBuffers(1, &indxBufferObj);//added//////////////////////////
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indxBufferObj);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);////<-------
  glBindVertexArray(vao[7]);/////////////////////////////////////
  //initilizing a buffer object
  glDisableVertexAttribArray(buffer[7]);
  glBindBuffer(GL_ARRAY_BUFFER, buffer[7]);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////NEEDS ATTENTION //////////////////////////////////////////////////////////////////
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vao), vao);
  //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);

  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vao), sizeof(diffuseColorMaterial), diffuseColorMaterial);
  //  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex), sizeof(diffuseColorMaterial), diffuseColorMaterial);

  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vao) + sizeof(diffuseColorMaterial), sizeof(normal), normal);
 //  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex) + sizeof(diffuseColorMaterial), sizeof(normal), normal);
////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
  boundingRadius = loadTriModel(modelFile, nModels, vertex, diffuseColorMaterial, normal);
  if (boundingRadius == -1.0f) {
    printf("loadTriModel error:  returned -1.0f \n");
    exit(1); }
    else
      printf("loaded %s model with %7.2f bounding radius \n", modelFile, boundingRadius);
*/
  


/*

glBindBuffer( GL_ARRAY_BUFFER, buffer );
glBufferData( GL_ARRAY_BUFFER, sizeof(vertex) + sizeof(diffuseColorMaterial) + sizeof(normal), NULL, GL_STATIC_DRAW );

*/
  //////Setting up the vertex arrays
  vPosition[7] = glGetAttribLocation(shaderProgram, "vPosition");
  glVertexAttribPointer(vPosition[7], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  // set up vertex arrays (after shaders are loaded)
  // GLuint vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  //glEnableVertexAttribArray( vPosition );
  //glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  

 // GLuint vColor = glGetAttribLocation( shaderProgram, "vColor" );
  vColor[7] = glGetAttribLocation(shaderProgram, "vColor");
  //glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor[7], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertex)) );
  
  //GLuint vNormal = glGetAttribLocation( shaderProgram, "vNormal" );
  vNormal[7] = glGetAttribLocation(shaderProgram, "vNormal");
  //glEnableVertexAttribArray( vNormal);
  glVertexAttribPointer( vNormal[7], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertex) + sizeof(diffuseColorMaterial)) );
  //glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertex) + sizeof(diffuseColorMaterial)));

  //load buffers from the models files
  for (int i = 0; i < nModels; i++) {
	  if (i != 7) {
		  boundingRadius[i] = loadModelBuffer(modelFile[i], nVertices[i], vao[i], buffer[i], shaderProgram, vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal");
	  }
	  else {
		  boundingRadius[i] = 1;
	  }
	  //set model scales
	  scale[i] = glm::vec3(modelSize[i] * 1.0f / boundingRadius[i]);
	}



  MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");
  // following line is an example that can help debug problems w/ shaders
  // you can also print the locations of vPosition, vColor, and vNormal
  printf("Print location of shader variable:  MVP = %d \n", MVP);
/*
  eyeDistance = eyeDistanceMultiplier * boundingRadius;
  // initially use a front view
  eye = glm::vec3(0.0f, 0.0f,  eyeDistance);   // camera's position
  at = glm::vec3(0.0f, 0.0f, 0.0f);            // position camera is looking at
  up = glm::vec3(0.0f, 1.0f, 0.0f);            // camera'a up vector
*/
  viewMatrix = glm::lookAt(
	  glm::vec3(0.0f, 20000.0f, 0.0f), //eye position
	  glm::vec3(0), //look at position
	  glm::vec3(0.0f, 0.0f, -1.0f));//up vector


  // set render state values
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

  lastTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
  }

// set viewport and projectionMatrix on window resize events
void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  aspectRatio = (GLfloat) width /  (GLfloat) height;

  projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 1.0f, 100000.0f); 
  printf("reshape: width = %4d height = %4d aspect = %5.2f \n", width, height, aspectRatio);
  }

// update and display animation state in window title
void updateTitle() {
  strcpy(titleStr, baseStr);
  strcat(titleStr, viewStr);
  strcat(titleStr, timerStr); 
  strcat(titleStr, fpsStr);
  // printf("title string = %s \n", titleStr);
  glutSetWindowTitle( titleStr);
  }

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0, 0, 0, 0);
  // update model view projection matrix
  //modelViewMatrix = glm::lookAt(eye, at, up) * rotation;  // modelview = view * model   
  //modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
  //glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
  // draw
  ////////// for loop added
  for (int i= 0; i< nModels; i++)
  {
	  modelMatrix = orientation[i];
	  modelViewMatrix = viewMatrix * modelMatrix;
	  modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
	  glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
	  glUniformMatrix3fv(MV, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	  glUniformMatrix3fv(NM, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	  normalMatrix = glm::mat3(modelViewMatrix);


	  glBindVertexArray(vao[i]);
	  glDrawArrays(GL_TRIANGLES, 0, nVertices[i]);

	  if (wireFrame)
		  glDrawArrays(GL_LINES, 0, nVertices[i]);
	  //glDrawArrays(GL_LINES, 0, nVertices);
	  else
		  glDrawArrays(GL_TRIANGLES, 0, nVertices[i]); 


  } 
  
  glutSwapBuffers();
  frameCount++;
  // see if a second has passed to set estimated fps information
  currentTime = glutGet(GLUT_ELAPSED_TIME);  // get elapsed system time
  timeInterval = currentTime - lastTime;
  if ( timeInterval >= 1000) {
    sprintf(fpsStr, " fps %4d", (int) (frameCount / (timeInterval / 1000.0f)) );
    lastTime = currentTime;
    frameCount = 0;
    updateTitle();
    }
  }

// for use with Idle and intervalTimer functions 
// to set rotation
void update(int i){
//void update(void){
glutTimerFunc(timeQuantum, update, 1);
/*
if (ship==1){
ship=0;
warship();
}

*/

for (int i = 0; i < nModels; i++) {
	if (i == ship) {
		rotation[i] = glm::rotate(rotation[i], float(pitch*0.02), glm::vec3(1, 0, 0));
		rotation[i] = glm::rotate(rotation[i], float(yaw*0.02), glm::vec3(0, 1, 0));
		rotation[i] = glm::rotate(rotation[i], float(roll*0.02), glm::vec3(0, 0, 1));

		if (!initialUpdate) {
			translate[i] = getPosition(orientation[i]) + getIn(rotation[i])* float(accelerate *speed);
		}
		else initialUpdate = 0;
		orientation[i] = glm::translate(identity, (translate[i] * rotation[i] * glm::scale(identity, glm::vec3(scale[i]))));
	}
	else{//orbits
		rotation[i] = glm::rotate(rotation[i], modelRadians[i], glm::vec3(0, 1, 0));
		orientation[i] = rotation[i] * glm::translate(identity, translate[i] * glm::scale(identity, glm::vec3(scale[i]));
		if(i==primus || i== secundus){//Lunar orbits
			orientation[i] = glm::translate(identity, getPosition(orientation[duo]))*glm::rotate(rotation[i], modelRadians[i], glm::vec3(0, 1, 0)*glm::translate(identity, translate[i] * glm::scale(identity, glm::vec3(scale[i]));
		}
	
	}
}
/*
  rotateRadian += 0.1f;
  if (rotateRadian >  2 * PI ) rotateRadian = 0.0f;
  rotation = glm::rotate(identity, rotateRadian, glm::vec3(0, 1, 0));

  */
accelerate= yaw = pitch = roll = 0; //if the key is not pressed anymore stop rotation
//viewMatrix = cameraUpdate(0);

  glutPostRedisplay();
}

// Estimate FPS, use for fixed interval timer driven animation
void intervalTimer (int i) { 
  glutTimerFunc(timerDelay, intervalTimer, 1);   
  if (! idleTimerFlag) update();  // fixed interval timer
  }

void keyboard (unsigned char key, int x, int y) {
  switch(key) {
    case 033 : case 'q' :  case 'Q' : exit(EXIT_SUCCESS); break;
    case 'a' : case 'A' :  // change animation timer
      // printf("%s   %s\n", timerStr, fpsStr);
      if (idleTimerFlag) { // switch to interval timer  
         glutIdleFunc(NULL);
         strcpy(timerStr, " interval timer");  
         idleTimerFlag = false;  
         }         
      else   {         // switch to idle timer
         glutIdleFunc(update); 
         strcpy(timerStr, " idle timer");
         idleTimerFlag = true;
         }
      break;
    case 'b' : case 'B' :  // bottom view
      eye = glm::vec3(0.0f, -eyeDistance, eyeDistance);   // camera is below origin on +Z
      at = glm::vec3(0.0f, 0.0f, 0.0f);                   // looking at origin
      up = glm::vec3(0.0f, 1.0f, 0.0f);                   // camera'a up vector
      strcpy(viewStr, " bottom view"); break;
    case 'f' : case 'F' :  // front view
      eye = glm::vec3(eyeDistance, 0.0f, eyeDistance);   // camera is to the right and on Z
      at = glm::vec3(0.0f, 0.0f, 0.0f);                  // looking at origin
      up = glm::vec3(0.0f, 1.0f, 0.0f);                  // camera'a up vector
      strcpy(viewStr, " front view"); break;
    case 't' : case 'T' :  // right view
      eye = glm::vec3(0.0f, eyeDistance, eyeDistance);     // camera is on Z and above origin
      at = glm::vec3(0.0f, 0.0f, 0.0f);                    // camera is looking at origin
      up = glm::vec3(0.0f, 1.0f, 0.0f);                    // camera's up is Y
      strcpy(viewStr, " top view"); break;
    case 'w' : case 'W' : // wireframe or surface rendering
      wireFrame = ! wireFrame; break;
    }
  updateTitle();
  }
    
int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  # ifdef __Mac__
  // Can't change the version in the GLUT_3_2_CORE_PROFILE
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
  # endif
  # ifndef __Mac__
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  # endif
  glutInitWindowSize(800, 600);
  // set OpenGL and GLSL versions to 3.3 for Comp 465/L, comment to see highest versions
  # ifndef __Mac__
  glutInitContextVersion(3, 3);  
  glutInitContextProfile(GLUT_CORE_PROFILE);
  # endif
  glutCreateWindow("465 CubeTriModel Example {a, o, p, w, f, t, b} : perspective front view");
  // initialize and verify glew
  glewExperimental = GL_TRUE;  // needed my home system 
  GLenum err = glewInit();  
  if (GLEW_OK != err) 
      printf("GLEW Error: %s \n", glewGetErrorString(err));      
    else {
      printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
      printf("OpenGL %s, GLSL %s\n", 
        glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));
      }
  // initialize scene
  init();

  // set glut callback functions
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(NULL);  // start with intervalTimer
  glutTimerFunc(timerDelay, update, 1);
  //  glutTimerFunc(timerDelay, intervalTimer, 1);

  glutMainLoop();
  printf("done\n");
  return 0;
  }
  

