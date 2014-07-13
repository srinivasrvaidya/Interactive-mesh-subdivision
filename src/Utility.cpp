#include<GL/glut.h>
#include<GL/gl.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include <iostream>
#include <vector>

#include "Utility.h"
#define MAX_NEIGHBOURHOOD 20
#define ERROR 99999

using namespace std;
extern int subdivisionLevel; // remembers the subdivisionLevel, updated in loop_main()::keyPressed(..); Used in interactivity()

// used to hold initial control mesh model. and later used in interactivity()
std::vector<Vertex> ControlMeshvertexList;
std::vector<Face> ControlMeshfaceList;
std::vector<WingedEdge> ControlMeshWingedEdgeList;

// used in render() and in all private functions.
std::vector<Face> faceList;
std::vector<Vertex> vertexList;
std::vector<WingedEdge> WingedEdgeList;

// Temporary list used in subdivison() to holds new values.
std::vector<Face> faceList1;
std::vector<Vertex> vertexList1;
std::vector<WingedEdge> WingedEdgeList1;

void Utility::paintString(int x,int y,int size, char *str)
{
	glRasterPos2i(10,20);
	for(int i=0;i<size;i++)
	{
		char s = str[i];
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,s);
	}				
	glFlush();	
}

void Utility::calcNormal(int faceNum )
// calculates the normal; given faceId
{

	float vectorA[3], vectorB[3], vectorResultant[3], val;
	float normalX, normalY, normalZ;
	
   // building vectors.
   vectorA[0] = vertexList[faceList[faceNum].vertex2].x - vertexList[faceList[faceNum].vertex1].x ;
   vectorA[1] = vertexList[faceList[faceNum].vertex2].y - vertexList[faceList[faceNum].vertex1].y ;
   vectorA[2] = vertexList[faceList[faceNum].vertex2].z - vertexList[faceList[faceNum].vertex1].z ;
 
   vectorB[0] = vertexList[faceList[faceNum].vertex3].x - vertexList[faceList[faceNum].vertex1].x ;
   vectorB[1] = vertexList[faceList[faceNum].vertex3].y - vertexList[faceList[faceNum].vertex1].y ;
   vectorB[2] = vertexList[faceList[faceNum].vertex3].z - vertexList[faceList[faceNum].vertex1].z ;
 
   // findint the cross product 
   vectorResultant[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
   vectorResultant[1] = vectorB[0] * vectorA[2] - vectorA[0] * vectorB[2];
   vectorResultant[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];
 
	val = sqrt(vectorResultant[0]*vectorResultant[0] + vectorResultant[1]*vectorResultant[1] + vectorResultant[2]*vectorResultant[2] );
 
	normalX = vectorResultant[0]/val;
	normalY = vectorResultant[1]/val;
	normalZ = vectorResultant[2]/val;
		
	faceList[faceNum].xNormal = normalX;
	faceList[faceNum].yNormal = normalY;
	faceList[faceNum].zNormal = normalZ;
 	
 	glNormal3f(normalX, normalY, normalZ);
 	
	vertexList[faceList[faceNum].vertex1].xNormal += normalX;
	vertexList[faceList[faceNum].vertex2].xNormal += normalX;
	vertexList[faceList[faceNum].vertex3].xNormal += normalX;
	
	vertexList[faceList[faceNum].vertex1].yNormal += normalY;
	vertexList[faceList[faceNum].vertex2].yNormal += normalY;
	vertexList[faceList[faceNum].vertex3].yNormal += normalY;

	vertexList[faceList[faceNum].vertex1].zNormal += normalZ;
	vertexList[faceList[faceNum].vertex2].zNormal += normalZ;
	vertexList[faceList[faceNum].vertex3].zNormal += normalZ;

	vertexList[faceList[faceNum].vertex1].Normal += 1.0;
	vertexList[faceList[faceNum].vertex2].Normal += 1.0;
	vertexList[faceList[faceNum].vertex3].Normal += 1.0;
}

void Utility::calcNormalFlat( int v1, int v2, int v3 )
// calculates the normal for flat-shading. 
{

	float vectorA[3], vectorB[3], vectorResultant[3], val;
	float normalX, normalY, normalZ;
	
   // building vectors.
   vectorA[0] = vertexList[v2].x - vertexList[v1].x ;
   vectorA[1] = vertexList[v2].y - vertexList[v1].y ;
   vectorA[2] = vertexList[v2].z - vertexList[v1].z ;
 
   vectorB[0] = vertexList[v3].x - vertexList[v1].x ;
   vectorB[1] = vertexList[v3].y - vertexList[v1].y ;
   vectorB[2] = vertexList[v3].z - vertexList[v1].z ;
 
   // findint the cross product 
   vectorResultant[0] = vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2];
   vectorResultant[1] = vectorB[0] * vectorA[2] - vectorA[0] * vectorB[2];
   vectorResultant[2] = vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1];
 
	val = sqrt(vectorResultant[0]*vectorResultant[0] + vectorResultant[1]*vectorResultant[1] + vectorResultant[2]*vectorResultant[2] );
 
	normalX = vectorResultant[0]/val;
	normalY = vectorResultant[1]/val;
	normalZ = vectorResultant[2]/val;
		 	
 	glNormal3f(normalX, normalY, normalZ);
 	cout << endl << normalX << "  " << normalY << "   " << normalZ;
}
	
void Utility::loadPlyFileToWingedEdged(char *filename)
// loads contents read from filename.ply file into vertexList, faceList and WingedEdgeList.
{
	int headerEndFlag=0,vertexEndFlag=0;
	int i=0,j=1,v1,v2;
	char str[500],tempStr1[20],tempStr2[20];
	double centoidVertex[3];
	double plyVertex1, plyVertex2, plyVertex3;
	int edgeCounter=0, faceCounter=0;
	int plyFace[10];
	int plyfaceCount, plyVertexCount, plyFaceDimensionCount;

	FILE *plyReader;
	
	if ((plyReader = fopen(filename, "r")) == NULL) {
		std::cout << "File not found!";
		return;
	}
		
	
	while (strstr(str, "comment")) 
	// running down the comments.
	{
		fgets(str, 100, plyReader);
	}	
			
	while(!feof(plyReader))
	{
		if(fgets(str,200,plyReader))	{	
			if(headerEndFlag!=1)
			// reading header 
			{  
				if(strstr(str,"element vert"))
					sscanf(str,"%s %s %d",tempStr1,tempStr2,&plyVertexCount); 	
				if(strstr(str,"element face"))
					sscanf(str,"%s %s %d",tempStr1,tempStr2,&plyfaceCount);
				if(strstr(str,"end_header"))
					headerEndFlag=1;
				
		}
		else 
		if(vertexEndFlag!=1)
		{
			if(i<plyVertexCount)
			{
				sscanf(str,"%lf %lf %lf",&plyVertex1, &plyVertex2, &plyVertex3);
				
				Vertex vertexUnit(i,plyVertex1,plyVertex2,plyVertex3);	
				vertexList.push_back(vertexUnit);
		
				i++;			
			}
			else
			{
				vertexEndFlag=1;
				sscanf(str,"%d %d %d %d %d %d %d %d %d %d %d",&plyFaceDimensionCount,&plyFace[0],&plyFace[1],&plyFace[2],&plyFace[3],&plyFace[4],&plyFace[5],&plyFace[6],&plyFace[7],&plyFace[8],&plyFace[9]);	
			
				
				findCentroid(plyFaceDimensionCount, centoidVertex, plyFace);
				Vertex VertexUnit(vertexList.size() , centoidVertex[0], centoidVertex[1], centoidVertex[2]);	
				vertexList.push_back(VertexUnit);
						
				j=0;
				
				// Triangulation of the polygon.
				for(int loopVar = 0; loopVar < plyFaceDimensionCount ; loopVar++)
				{ 		
					v1 = loopVar;
					v2 = (loopVar+1)%(plyFaceDimensionCount);
				
					if( checkEdgeEntryUpdateIfExits(vertexList.size() - 1, plyFace[v1], faceCounter, 1) == 0 ) 
					{
						WingedEdge WingedEdgeUnit1(edgeCounter++, vertexList.size() - 1, plyFace[v1], faceCounter);
						WingedEdgeList.push_back(WingedEdgeUnit1);
					}		
					if( checkEdgeEntryUpdateIfExits(plyFace[v1], plyFace[v2], faceCounter, 1) == 0 ) 
					{
						WingedEdge WingedEdgeUnit1(edgeCounter++, plyFace[v1], plyFace[v2], faceCounter);
						WingedEdgeList.push_back(WingedEdgeUnit1);
					}		
					if( checkEdgeEntryUpdateIfExits(plyFace[v2], vertexList.size() - 1, faceCounter, 1) == 0 ) 
					{
						WingedEdge WingedEdgeUnit1(edgeCounter++, plyFace[v2], vertexList.size() - 1, faceCounter);
						WingedEdgeList.push_back(WingedEdgeUnit1);
					}
					
					// adding entry into faceList		
					Face fUnit(faceCounter, plyFace[v1], plyFace[v2], vertexList.size() - 1);
					faceList.push_back(fUnit);
					faceCounter++;
					
				//	calcNormal(faceList.size()-1);
				}
							
			}
		}
		else
		{
			if(j<plyfaceCount)
			{
				sscanf(str,"%d %d %d %d %d %d %d %d %d %d %d",&plyFaceDimensionCount,&plyFace[0],&plyFace[1],&plyFace[2],&plyFace[3],&plyFace[4],&plyFace[5],&plyFace[6],&plyFace[7],&plyFace[8],&plyFace[9]);	
			
				findCentroid(plyFaceDimensionCount, centoidVertex, plyFace);
				Vertex VertexUnit((vertexList.size()) , centoidVertex[0], centoidVertex[1], centoidVertex[2]);						
				vertexList.push_back(VertexUnit);
								
				// Triangulation of the polygon.				
				for(int loopVar = 0; loopVar < plyFaceDimensionCount ; loopVar++)
				{ 		
					v1 = loopVar;
					v2 = (loopVar+1)%(plyFaceDimensionCount);
							
					if( checkEdgeEntryUpdateIfExits(vertexList.size() - 1, plyFace[v1], faceCounter, 1) == 0 ) 
					{
						WingedEdge WingedEdgeUnit1(edgeCounter++, vertexList.size() - 1, plyFace[v1], faceCounter);
						WingedEdgeList.push_back(WingedEdgeUnit1);
					}		
					if( checkEdgeEntryUpdateIfExits(plyFace[v1], plyFace[v2], faceCounter, 1) == 0 ) 
					{
						WingedEdge WingedEdgeUnit1(edgeCounter++, plyFace[v1], plyFace[v2], faceCounter);
						WingedEdgeList.push_back(WingedEdgeUnit1);
					}		
					if( checkEdgeEntryUpdateIfExits(plyFace[v2], vertexList.size() - 1, faceCounter, 1) == 0 ) 
					{
						WingedEdge WingedEdgeUnit1(edgeCounter++, plyFace[v2], vertexList.size() - 1, faceCounter);
						WingedEdgeList.push_back(WingedEdgeUnit1);
					}
					
					// adding entry into faceList
					Face fUnit(faceCounter, plyFace[v1], plyFace[v2], vertexList.size() - 1);
					faceList.push_back(fUnit);						
					faceCounter++;
					
				//	calcNormal(faceList.size()-1);
				}
				j++;
			}
		  }
	   }		
	 }

		entryEdgeIntoVertexList();   // fills edgeId in the vertexList
		fillFaceTable(faceCounter);    // fills edgeId in the faceList
		
		// saving initial configurations. - later used for interactivity	
		ControlMeshWingedEdgeList = WingedEdgeList;
		ControlMeshvertexList = vertexList;	
		ControlMeshfaceList = faceList;
		
		
		for (std::vector<Face>::iterator it = faceList.begin() ; it != faceList.end(); ++it)
		{
			calcNormal(it->faceId);
		}
		

}	


void Utility::findCentroid(int noOfVerticesPerFace, double new_vertex[3], int* vertexIndex)
// finds centroid of polygon. new_vertex[0] holds centriod x-cordinate value, new_vertex[1] for y and new_vertex[2] for z;
{
	
	new_vertex[0] = 0.0;
	new_vertex[1] = 0.0;
	new_vertex[2] = 0.0;
	
	for(int loopVar  = 0; loopVar < noOfVerticesPerFace ; loopVar++)
	{
		new_vertex[0] += vertexList[vertexIndex[loopVar]].x;
		new_vertex[1] += vertexList[vertexIndex[loopVar]].y;
		new_vertex[2] += vertexList[vertexIndex[loopVar]].z;
	}
	
	new_vertex[0] = new_vertex[0] / (double)noOfVerticesPerFace;
	new_vertex[1] = new_vertex[1] / (double)noOfVerticesPerFace;
	new_vertex[2] = new_vertex[2] / (double)noOfVerticesPerFace;
}

void Utility::render(int displayControlMesh, int selectVertex)
// renders the mesh by reading data from faceList and vertexList.
{
	
	int v1, v2, v3;
	for (std::vector<Face>::iterator it = faceList.begin() ; it != faceList.end(); ++it)
	{
		
		v1 = it->vertex1;
		v2 = it->vertex2;
		v3 = it->vertex3;
		
		/* --- code for skeleton model --- */
		
		glColor3f(1, 0, 1);
		glBegin(GL_LINE_LOOP);
			glVertex3d(vertexList[v1].x, vertexList[v1].y, vertexList[v1].z); 
			glVertex3d(vertexList[v2].x, vertexList[v2].y, vertexList[v2].z); 
			glVertex3d(vertexList[v3].x, vertexList[v3].y, vertexList[v3].z); 
		glEnd();
	
/*	    --- code for shading ---
		glColor3f(1, 0, 1);	
		glBegin(GL_TRIANGLES);
		
	//		glNormal3d( (vertexList[v1].xNormal/vertexList[v1].Normal) , (vertexList[v1].yNormal/vertexList[v1].Normal) , (vertexList[v1].zNormal/vertexList[v1].Normal));		
	//		glNormal3f( it->xNormal , it->yNormal , it->zNormal );
			calcNormalFlat(v1,v2,v3);
			glVertex3d(vertexList[v1].x, vertexList[v1].y, vertexList[v1].z); 
	//		glNormal3d( (vertexList[v2].xNormal/vertexList[v2].Normal) , (vertexList[v2].yNormal/vertexList[v2].Normal) , (vertexList[v2].zNormal/vertexList[v2].Normal));
			glVertex3d(vertexList[v2].x, vertexList[v2].y, vertexList[v2].z); 
	//		glNormal3d( (vertexList[v3].xNormal/vertexList[v3].Normal) , (vertexList[v3].yNormal/vertexList[v3].Normal) , (vertexList[v3].zNormal/vertexList[v3].Normal));
			glVertex3d(vertexList[v3].x, vertexList[v3].y, vertexList[v3].z); 
		glEnd();
			
		cout << endl << " ----------------------------------- " << it->faceId ;
		cout << endl << vertexList[v1].xNormal << "  " << vertexList[v1].yNormal << "  " << vertexList[v1].zNormal << " : " << vertexList[v1].Normal;
		cout << endl << vertexList[v2].xNormal << "  " << vertexList[v2].yNormal << "  " << vertexList[v2].zNormal << " : " << vertexList[v2].Normal;
		cout << endl << vertexList[v3].xNormal << "  " << vertexList[v3].yNormal << "  " << vertexList[v3].zNormal << " : " << vertexList[v3].Normal;
	*/	
	}
	if( displayControlMesh == 1 )
	{
		for (std::vector<Face>::iterator it = ControlMeshfaceList.begin() ; it != ControlMeshfaceList.end(); ++it)
		{
						
			v1 = it->vertex1;
			v2 = it->vertex2;
			v3 = it->vertex3;
			
			glColor3f(1, 1, 1);
			glBegin(GL_LINE_LOOP);
				glVertex3d(ControlMeshvertexList[v1].x, ControlMeshvertexList[v1].y, ControlMeshvertexList[v1].z); 
				glVertex3d(ControlMeshvertexList[v2].x, ControlMeshvertexList[v2].y, ControlMeshvertexList[v2].z); 
				glVertex3d(ControlMeshvertexList[v3].x, ControlMeshvertexList[v3].y, ControlMeshvertexList[v3].z); 
			glEnd();
		}
						
		glColor3f(1, 0, 0);					
		glPointSize(10);
	
		glBegin(GL_POINTS);
			glVertex3d(ControlMeshvertexList[selectVertex].x, ControlMeshvertexList[selectVertex].y, ControlMeshvertexList[selectVertex].z);
		glEnd();
	}
	
	
	glFlush();

}

void Utility::interactivity(int selectVertex, int select)
// handles the interactivity. 
{
			selectVertex = selectVertex % ControlMeshvertexList.size();
			
			if ( select == 1)
			 	ControlMeshvertexList[selectVertex].y += 0.025;
			 
			if ( select == 2)
			 	ControlMeshvertexList[selectVertex].x -= 0.025;
		
			if ( select == 3)
			 	ControlMeshvertexList[selectVertex].y -= 0.025;
		
			if ( select == 4)
			 	ControlMeshvertexList[selectVertex].x += 0.025;
		
			render(1, select);
			
			// updating with new co-ordinate values. 
			WingedEdgeList = ControlMeshWingedEdgeList;
			vertexList = ControlMeshvertexList  ;	
			faceList = ControlMeshfaceList ;
			
			for(int i=0; i < subdivisionLevel ; i++)
				subdivision();
}

void Utility::subdivision()
// loop-subdivision algorithm implementation.
{	
	int vertex1, vertex2, vertex3;
	int countNeighours=0;
	int edgeId12, edgeId13, edgeId23;
	int vertex12, vertex13, vertex23;
	int neighbourhoodVertexList[MAX_NEIGHBOURHOOD];
	int rightFace, leftFace;
	int vertexLeft, vertexRight;
	int faceCounter = 0;
	
	// clears previous data( holds sub-division data)
	WingedEdgeList1.clear();
	vertexList1.clear();
	faceList1.clear(); 
	
	for (std::vector<Vertex>::iterator it = vertexList.begin() ; it != vertexList.end(); ++it)
	// this loop computes new vertex co-ordinates. and this new vertex c-ordinates is used by renderer.
	{
			countNeighours = findVertexneighbourhood(it->vertexId, neighbourhoodVertexList);
			computeNewVertexValue(it->vertexId, neighbourhoodVertexList, countNeighours);
	}
		
	
	for (std::vector<Face>::iterator it = faceList.begin() ; it != faceList.end(); ++it)
	// this loop, we will iterate through all faces. each faces -> contributes to 4 new faces and 3 new vertices. in faceList, we can directly access its 3 edges.
	{
		
		vertex1 = it->vertex1;
		vertex2 = it->vertex2;
		vertex3 = it->vertex3;
	
		edgeId12 = getEdgeIdGiven2Vertices(vertex1, vertex2);
		edgeId13 = getEdgeIdGiven2Vertices(vertex1, vertex3);
		edgeId23 = getEdgeIdGiven2Vertices(vertex2, vertex3);
		
		// processing edge12. 
		if( WingedEdgeList[edgeId12].isEdgeSplitted == false )
		{
			rightFace = WingedEdgeList[edgeId12].rightFace;
			leftFace = WingedEdgeList[edgeId12].leftFace;
			
			vertexLeft = find3rdVertexOfNeighbourFace(vertex1, vertex2, rightFace);
			vertexRight = find3rdVertexOfNeighbourFace(vertex1, vertex2, leftFace);	
			
			vertex12 = getComputedEdgeSplittingVertexId(vertex1, vertex2, vertexLeft, vertexRight);
			
			WingedEdgeList[edgeId12].isEdgeSplitted = true;
			WingedEdgeList[edgeId12].edgeBreakingvertexId = vertex12;
			
		}
		else
		{
			vertex12 = WingedEdgeList[edgeId12].edgeBreakingvertexId;	
		}

		// processing edge13.
		if( WingedEdgeList[edgeId13].isEdgeSplitted == false )
		{
			rightFace = WingedEdgeList[edgeId13].rightFace;
			leftFace = WingedEdgeList[edgeId13].leftFace;
			
			vertexLeft = find3rdVertexOfNeighbourFace(vertex1, vertex3, rightFace);
			vertexRight = find3rdVertexOfNeighbourFace(vertex1, vertex3, leftFace);	
			
			vertex13 = getComputedEdgeSplittingVertexId(vertex1, vertex3, vertexLeft, vertexRight);
	
			WingedEdgeList[edgeId13].isEdgeSplitted = true;
			WingedEdgeList[edgeId13].edgeBreakingvertexId = vertex13;
		}
		else
		{
			vertex13 = WingedEdgeList[edgeId13].edgeBreakingvertexId;	
		}
		
		
		// processing edge23.
		if( WingedEdgeList[edgeId23].isEdgeSplitted == false )
		{
			rightFace = WingedEdgeList[edgeId23].rightFace;
			leftFace = WingedEdgeList[edgeId23].leftFace;
			
			vertexLeft = find3rdVertexOfNeighbourFace(vertex2, vertex3, rightFace);
			vertexRight = find3rdVertexOfNeighbourFace(vertex2, vertex3, leftFace);	

			vertex23 = getComputedEdgeSplittingVertexId(vertex2, vertex3, vertexLeft, vertexRight);
	
			WingedEdgeList[edgeId23].isEdgeSplitted = true;
			WingedEdgeList[edgeId23].edgeBreakingvertexId = vertex23;
		}
		else
		{
			vertex23 = WingedEdgeList[edgeId23].edgeBreakingvertexId;	
		}

		// Processing and pushing details of 1st face;		
		if( checkEdgeEntryUpdateIfExits(vertex1, vertex12, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex1, vertex12, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex12, vertex13, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex12, vertex13, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex13, vertex1, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex13, vertex1, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		
		Face fUnit(faceCounter, vertex1, vertex12, vertex13);
		faceList1.push_back(fUnit);
					
		faceCounter++;
		
		
		// Processing and pushing details of 2nd face;
		if( checkEdgeEntryUpdateIfExits(vertex12, vertex2, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex12, vertex2, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex2, vertex23, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex2, vertex23, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex23, vertex12, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex23, vertex12, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		
		Face fUnit2(faceCounter, vertex12, vertex2, vertex23);
		faceList1.push_back(fUnit2);
		
		faceCounter++;
		
		// Processing and pushing details of 3rd face;
		if( checkEdgeEntryUpdateIfExits(vertex13, vertex23, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex13, vertex23, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex23, vertex3, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex23, vertex3, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex3, vertex13, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex3, vertex13, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		
		Face fUnit3(faceCounter, vertex13, vertex23, vertex3);
		faceList1.push_back(fUnit3);
		faceCounter++;
		
		// Processing and pushing details of 4th face;
		if( checkEdgeEntryUpdateIfExits(vertex12, vertex23, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex12, vertex23, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex23, vertex13, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex23, vertex13, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		if( checkEdgeEntryUpdateIfExits(vertex13, vertex12, faceCounter, 2) == 0 ) 
		{
			WingedEdge WE(WingedEdgeList1.size(), vertex13, vertex12, faceCounter);
			WingedEdgeList1.push_back(WE);
		}
		
		Face fUnit4(faceCounter, vertex12, vertex23, vertex13);
		faceList1.push_back(fUnit4);
		faceCounter++;
						
	}
	
	// now assigning WingedEdgeList1 contents to WingedEdge. 
	WingedEdgeList = WingedEdgeList1;
	vertexList = vertexList1;	
	faceList = faceList1;
		
	entryEdgeIntoVertexList();
	fillFaceTable(faceCounter);
	
/*	    - Used for duBugging!!  
 * 		- lists out all details of vertexList, faceList and WingedEdgeList
 
		std::cout <<  "vertexId"  << " " << "    x" << " "<<  "     y" << " " << "      z" << " " << "    edgeId" << "    newValueComputed" << endl;
		for (std::vector<Vertex>::iterator it = vertexList.begin() ; it != vertexList.end(); ++it)
			std::cout <<  it->vertexId << "        " << it->x << "        " <<  it->y << "        " << it->z << "        " << it->edgeId << "         " << it->newValueComputed << endl;
*/

//		std::cout <<  "it->faceId" << "        " << "it->edgeId" << "        " << endl;
		for (std::vector<Face>::iterator it = faceList.begin() ; it != faceList.end(); ++it)
		{
			//std::cout <<  it->faceId << "        " << it->edgeId << "        " << it->vertex1 << "        " << it->vertex2 << "        " << it->vertex3 << endl;
			calcNormal(it->faceId);
		}
/*
		std::cout << " WingedEDge " << endl << "edgeId" << " " << "  v1" << " " << "  V2" << "  " << "  L.F" << "  R.F" << endl;
		for (std::vector<WingedEdge>::iterator it = WingedEdgeList.begin() ; it != WingedEdgeList.end(); ++it)
			std::cout <<  it->edgeId << "      " << it->vertex1 << "       " << it->vertex2 << "       " << it->leftFace << "       " << it->rightFace << endl;
*/
	
	glFlush();
	

}

int Utility::getComputedEdgeSplittingVertexId(int vertexA,int vertexB, int vertexLeft, int vertexRight)
// computes the new splitting vertex, pushes it to vertexList and returns vertexId
{
		double new_x, new_y, new_z;

		new_x = ((vertexList[vertexA].x + vertexList[vertexB].x) * (3.0/8.0)) + ((vertexList[vertexLeft].x + vertexList[vertexRight].x) * (1.0/8.0)); 
		new_y = ((vertexList[vertexA].y + vertexList[vertexB].y) * (3.0/8.0)) + ((vertexList[vertexLeft].y + vertexList[vertexRight].y) * (1.0/8.0));
		new_z = ((vertexList[vertexA].z + vertexList[vertexB].z) * (3.0/8.0)) + ((vertexList[vertexLeft].z + vertexList[vertexRight].z) * (1.0/8.0));

		Vertex vertexUnit(vertexList1.size(),new_x, new_y, new_z);	
		vertexList1.push_back(vertexUnit);
		
		return (vertexList1.size()-1);  
}

int Utility::getEdgeIdGiven2Vertices(int vertexA, int vertexB)
// travers the edgeList and find edgeId of <vertexA, vertexB>
{

	for (std::vector<WingedEdge>::iterator it = WingedEdgeList.begin() ; it != WingedEdgeList.end(); ++it)
	{	
		if ( (it->vertex1 == vertexA && it->vertex2 == vertexB) || (it->vertex1 == vertexB && it->vertex2 == vertexA) )
			return it->edgeId;
	}
}

int Utility::find3rdVertexOfNeighbourFace(int vertexA, int vertexB, int faceId)
// comparing each 'vertex' of faceId face to the 2 parameters passed; if its not equal then its the required vertex, return it.
{
		int vertex;
		
		if( faceList[faceId].vertex1 != vertexA && faceList[faceId].vertex1 != vertexB ) 
			vertex = faceList[faceId].vertex1;
		else
		if( faceList[faceId].vertex2 != vertexA && faceList[faceId].vertex2 != vertexB )
			vertex = faceList[faceId].vertex2;
		else
			vertex = faceList[faceId].vertex3;
	
	return vertex;
}
	
void Utility::computeNewVertexValue(int vertexId, int* neighbourhoodVertexList, int countNeighours)
{
	
	double beta = 10 + countNeighours;
	double new_x = ( vertexList[vertexId].x * 10.0 ) / beta;
	double new_y = ( vertexList[vertexId].y * 10.0 ) / beta; 
	double new_z = ( vertexList[vertexId].z * 10.0 ) / beta;
	
	for (int loopVar=0; loopVar < countNeighours ; loopVar++)
	{
		new_x += ( vertexList[neighbourhoodVertexList[loopVar]].x /  beta ) ;
		new_y += ( vertexList[neighbourhoodVertexList[loopVar]].y /  beta ) ;
		new_z += ( vertexList[neighbourhoodVertexList[loopVar]].z /  beta ) ;
	}
	
	Vertex vertexUnit(vertexList1.size(),new_x, new_y, new_z);	
	vertexList1.push_back(vertexUnit);

}

int Utility::findVertexneighbourhood(int vertex, int* neighbourhoodvertexList)
// finds the neighbourhood of a given vertex ie,, finds vertex's which share common edge. and returns its count.
{
	int counter = 0;
	for (std::vector<WingedEdge>::iterator it = WingedEdgeList.begin() ; it != WingedEdgeList.end(); ++it)
	{
		
		if(it->vertex1 == vertex )
		{
			neighbourhoodvertexList[counter++] = it->vertex2; 
			continue;
		}
			
		if(it->vertex2 == vertex)
		{
			neighbourhoodvertexList[counter++] = it->vertex1; 
			continue;
		}
		
	}
	return counter;
}


int Utility::checkEdgeEntryUpdateIfExits(int v1, int v2, int face, int selector)
// check it <v1,v2> edge already exits in the WingedEdgeList. If it then, 
{
	
	if(selector == 1)   
	// called from loadPlyFileIntoWingedEdge(); called only once.;  works with WingedEdgeList
	{
		for (std::vector<WingedEdge>::iterator it = WingedEdgeList.begin() ; it != WingedEdgeList.end(); ++it)
			if( (it->vertex1 == v1 && it->vertex2 == v2) || (it->vertex1 == v2 && it->vertex2 == v1) )
			{	
				it->rightFace = face;
				return 1;
			}
	}
	
	if(selector == 2)  
	// called from subdivion(); works with WingedEdgeList1
	{
		for (std::vector<WingedEdge>::iterator it = WingedEdgeList1.begin() ; it != WingedEdgeList1.end(); ++it)
			if( (it->vertex1 == v1 && it->vertex2 == v2) || (it->vertex1 == v2 && it->vertex2 == v1) )
			{	
				it->rightFace = face;
				return 1;
			}			
	}	
	
	return 0;   // edge <v1,v2> not found the WingedEdgeList.
}

void Utility::entryEdgeIntoVertexList()
{
	
	for (std::vector<Vertex>::iterator it = vertexList.begin() ; it != vertexList.end(); ++it)
	{
		if( (it->edgeId = getEdgeIdGivenVertex(it->vertexId)) == ERROR )
			cout << endl << "Error: Utility.cpp: call to getEdgeIdGivenVertex() did not find " << it->vertexId << endl;				
	}
	

}

void Utility::fillFaceTable(int faceCount)
{
	int id;
	for(int i=0;i<faceCount; i++)
	{
		if( (id = getEdgeIdGivenFaceId(i)) != ERROR )
		{
			faceList[i].edgeId = id;
		}
		else
			cout << endl << " Error Utility.cpp: fillFaceTable " << i << endl;		
	}		
	
}


int Utility::getEdgeIdGivenVertex(int vertexId)
{

	for (std::vector<WingedEdge>::iterator it = WingedEdgeList.begin() ; it != WingedEdgeList.end(); ++it)
	{
		if( vertexId == it->vertex1 || vertexId == it->vertex2)
			return it->edgeId;
	}
	
	return ERROR; // error signal. 
}

int Utility::getEdgeIdGivenFaceId(int faceID)
{
	for (std::vector<WingedEdge>::iterator it = WingedEdgeList.begin() ; it != WingedEdgeList.end(); ++it)
	{
		if( faceID == it->leftFace || faceID == it->rightFace )
			return it->edgeId;
		
	}
	return ERROR; // error signal. 	
}
