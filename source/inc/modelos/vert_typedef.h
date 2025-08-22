#include <vector>

typedef struct {
float position[3];
float uv[2];
float normal[3];
float tangent[3];
} vertex;

struct Triangle
{
	u16 index[3];
};

typedef struct
{
	std::vector<vertex> vertlist;
	std::vector<u16> indexlist;
	/*C3D_BufInfo* bufInfo;

	void* v;
	void* i;*/
} _3DObject;