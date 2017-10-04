/*

glmUtils465.hpp

GLM utility functions for console applications to test matrix/vector
operations for 465 and for openGL applications that have included
include465.hpp file.

void show*(....)
glm::vec3 get*(....)
float distance(v1, v2)
bool colinear(v1, v1, e)
glm::vec3 unitNormal(...)
bool isnanVec3(...)
bool isnanVec4(...)
bool isnanMat4(...)
float acosDotProduct(...)

Mike Barnes
10/20/2016
*/

// include the glm shader-like math library if needed
# ifndef __INCLUDES465__
# define __GLMUTILS__
# define GLM_FORCE_RADIANS  // use radians not angles
# define GLM_MESSAGES   // compiler messages
# include <glm/glm.hpp>
# include <glm/gtc/constants.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <glm/gtc/quaternion.hpp>
# include <glm/gtx/quaternion.hpp>
# endif

# ifndef __PI__   // set constant for PI
# define __PI__
const float PI = glm::pi<float>(); 
# endif

void showVec3(char * name, const glm::vec3 &v) {
  printf("%s:  ", name);
  printf("[%8.3f  %8.3f  %8.3f ] \n", v.x, v.y, v.z); 
  }

void showVec4(char * name, const glm::vec4 &v) {
  printf("%s:  ", name);
  printf("[%8.3f  %8.3f  %8.3f %8.3f ]  \n", v.x, v.y, v.z, v.w); 
  }

void showQuat(char * label, glm::quat aQuat) {
	printf("%s = (%6.3f, %6.3f, %6.3f, %6.3f)\n", label,
		aQuat.w, aQuat.x, aQuat.y, aQuat.z);
	}


void showMat4(char * label, const glm::mat4 &m) {
  printf("%s:  \n", label);
  printf("    Right      Up       At      Pos \n");
  printf("X %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][0], m[1][0], m[2][0], m[3][0]);
  printf("Y %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][1], m[1][1], m[2][1], m[3][1]);
  printf("Z %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][2], m[1][2], m[2][2], m[3][2]);
  printf("W %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][3], m[1][3], m[2][3], m[3][3]);
  }

// reference vector oriented + horizontal
glm::vec3 getRight(const glm::mat4 &m) {
  return glm::vec3(m[0][0], m[0][1], m[0][2]); }

// reference vector oriented - horizontal
glm::vec3 getLeft(const glm::mat4 &m) {
  return glm::vec3(-m[0][0], -m[0][1], -m[0][2]); }

// refernece vector oriented + vertical
glm::vec3 getUp(const glm::mat4 &m) {
  return glm::vec3(m[1][0], m[1][1], m[1][2]); }

// refernece vector oriented - vertical
glm::vec3 getDown(const glm::mat4 &m) {
  return glm::vec3(-m[1][0], -m[1][1], -m[1][2]); }

// reference vector oriented + depth
glm::vec3 getOut(const glm::mat4 &m) {
  return glm::vec3(m[2][0], m[2][1], m[2][2]); }

// reference vector oriented - depth
glm::vec3 getIn(const glm::mat4 &m) {
  return glm::vec3(-m[2][0],-m[2][1], -m[2][2]); }

// the position of "object" represented by tranformation matrix m
glm::vec3 getPosition(const glm::mat4 &m) {
  return glm::vec3(m[3][0], m[3][1], m[3][2]); }

// distance between two glm::vec3 values
float distance(glm::vec3 p1, glm::vec3 p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)  + pow(p1.z - p2.z, 2));
	}

// v1 and v2 are colinear within an epsilon (rounding) range
// epislon of 0.0 has no range, suggest 0.1 for orient towards
bool colinear(glm::vec3 &v1, glm::vec3 &v2, double epsilon) {
	glm::vec3 v1t, v2t;
	v1t = glm::abs(glm::normalize(v1));
	v2t = glm::abs(glm::normalize(v2));
	if (glm::distance(v1t, v2t) <= epsilon) return true;
	else return false;
	}

// compute unitNormal(vec4 point0, vec4 point1, vec4 point2) 
// for counter-clockwise vertex winding starting from point0
// vectors are (p0 - p1) and (p2 - p1)
// the unit normal is for p1, or the triangular surface
glm::vec3 unitNormal(glm::vec4 &point0, glm::vec4 &point1, 
  glm::vec4 &point2) {
  glm::vec3 v1 = glm::vec3(point1 - point0);
  glm::vec3 v2 = glm::vec3(point1 - point2);
  glm::vec3  normal = glm::normalize(glm::cross(v1, v2));
  return normal;
  }

// Not-A-Number tests for vectors and matrix
// An extension of the C  bool isnan(number).

// Is a vec3 not-a-number (nan)
bool isNanVec3(glm::vec3 &v) {
	if (isnan(v.x) || isnan(v.y) || isnan(v.z)) 
			return true;
		else
			return false;
	}

// Is a vec4 not-a-number (nan)
bool isNanVec4(glm::vec4 &v) {
	if (isnan(v.x) || isnan(v.y) || isnan(v.z) || isnan(v.w))
		return true;
	else
		return false;
}

// Is a matr non-a-number (nan)
bool isNanMat4(const glm::mat4 &m) {
	bool result = false;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			if(isnan(m[i][j])) result = true;
	return result;
	}

/*
acos(dotProduct), where dotProduct is slightly larger than 1.0f
or slightly smaller than -1.0f can cause nan due to float imprecision.
So avoid.
*/
float acosDotProduct(float dotProduct) {
	if (dotProduct >= 0.999f) return 0.0f;
	else if (dotProduct <= -0.999f) return PI;
	else return acos(dotProduct);
	}
