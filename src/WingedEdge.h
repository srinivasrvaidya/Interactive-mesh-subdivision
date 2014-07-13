
class Vertex
{
	public:

	Vertex(int Id, float cordinate_x, float cordinate_y, float cordinate_z)
	{
		vertexId = Id;
		x = cordinate_x;
		y = cordinate_y;
		z = cordinate_z;
	}
	
	int vertexId;
	GLdouble x;
	GLdouble y;
	GLdouble z;
	int edgeId;
	
	GLdouble xNormal;
	GLdouble yNormal;
	GLdouble zNormal;
	GLdouble Normal=0.0;	
	bool newValueComputed = false;
};

class Face
{
	public:
	
	Face(int fid, int v1, int v2, int v3)
	{
		faceId = fid;
		vertex1 = v1;
		vertex2 = v2;
		vertex3 = v3;
	}
	int faceId;
	int edgeId;
	
	int vertex1;
	int vertex2;
	int vertex3;
	
	float xNormal;
	float yNormal;
	float zNormal;
};

class WingedEdge
{
	public:
	
	WingedEdge(int eid, int v1, int v2, int left_face) 
	{
		edgeId = eid;
		vertex1 = v1;
		vertex2 = v2;
		leftFace = left_face;
	}
	
	int edgeId;
	
	int vertex1;
	int vertex2;
	
	int leftFace;
	int rightFace;
	
	bool isEdgeSplitted = false;
	int edgeBreakingvertexId;
	
};
