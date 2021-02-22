/*
Scott Csordas
0998149
2750 Assignment 1, helper functions
*/

#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "GPXParser.h"

//additional helper functions prototypes
void getRoutes(xmlNode * a_node, List* routes);
void getTracks(xmlNode * a_node, List* tracks);
void getSegments(xmlNode * a_node, List* segments);
void getWaypoints(xmlNode * a_node, List* waypoints);
void getWaypointRoutes(xmlNode * a_node, List* waypoints);
void getWaypointsGPX(xmlNode * a_node, List* waypoints);
void getAttributes(xmlNode * a_node, List* attributes);
void getAttributesRT(xmlNode * a_node, List* attributes);
void getName(xmlNode *a_node, char* name);
float haversine(float lon1, float lat1, float lon2, float lat2);
char* fileNametoGPXJSON(char* filename);
char* fileNametoRouteListJSON(char* filename);
char* fileNametoTrackListJSON(char* filename);
char* routeToAttJSONlist(char *routeName, char *fileName);
char* trackToAttJSONlist(char *trackName, char *fileName);
void writefromJSON(char* JSON, char* fileName);

//A2 helpers
xmlDoc* GPXtoXml(GPXdoc* doc);
#endif
