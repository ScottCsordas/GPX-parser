/*
Scott Csordas
0998149
2750 Assignment 1, main file
*/

#include "LinkedListAPI.h"
#include "GPXParser.h"
#include "helper.h"
#include <stdio.h>

int main(){
	//char fileName[100] = "simple.gpx";//"Reyn-Rozh_parking(0rt_1trk_1seg_400m).gpx";
	//char* tempStr;
	//GPXdoc *doc;
	//doc = createGPXdoc(fileName);
	//printf("%s\n", doc->creator);
	//tempStr = GPXdocToString(doc);
	//printf("%s\n", tempStr);
	//free(tempStr);
	//printf("%d\n", getNumAttributes(doc))
	//writeGPXdoc(doc, "writeFile");;
	//validateGPXDoc(doc, "gpx.xsd");
	//deleteGPXdoc(doc);
	JSONtoWaypoint("{\"lat\":123,\"lon\":124}");
	JSONtoRoute("{\"name\":\"lon\"}");
	JSONtoWaypoint("{\"verion\":123,\"creator\":\"124\"}");
	//printf("%f \n", round10(407));
	//printf("%f \n", haversine(123.6205 , 44.5951 ,123.6205 , 44.596));
	return 0;
}
