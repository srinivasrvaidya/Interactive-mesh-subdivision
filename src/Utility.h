#include<GL/glut.h>
#include "WingedEdge.h"

class Utility
{
	public:
	
	void paintString(int ,int ,int , char *);	
	void loadPlyFileToWingedEdged(char *);
	void calcNormal(int );
	void calcNormalFlat(int , int , int);
	void normalise();
	void render(int, int);
	void subdivision();
	void interactivity(int, int);

	
	private:
	void entryEdgeIntoVertexList();
	void fillFaceTable(int );
	void fillEdgeTableWithRightFace();
	void findCentroid(int, double*,int*);
	void computeNewVertexValue(int, int*, int);
	
	int getEdgeIdGivenVertex(int );
	int getEdgeIdGivenFaceId(int );	
	int checkEdgeEntryUpdateIfExits(int, int, int, int);
	int findVertexneighbourhood(int, int *);
	int find3rdVertexOfNeighbourFace(int, int, int);
	int getEdgeIdGiven2Vertices(int , int);
	int getComputedEdgeSplittingVertexId(int, int, int, int);
};
