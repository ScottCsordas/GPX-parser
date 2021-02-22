/*
Scott Csordas
0998149
2750 Assignment 1, parser functions
code on how to use the xml parser in createGPXdoc obtained from
http://www.xmlsoft.org/examples/tree1.c 19/09/2019
*/
#include "helper.h"
#include "GPXParser.h"
//#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI 3.14159
#endif


//testing function to print xml tree nodes names taken from https://moodle.socs.uoguelph.ca/pluginfile.php/42297/mod_resource/content/0/libXmlExample.c 19/09/2019
/*static void print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        // if (cur_node->content != NULL ){
        //     printf("  content: %s\n", cur_node->content);
        // }

        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }

        print_element_names(cur_node->children);
    }
}*/

//creates a gpx doc by parsing an xml file
GPXdoc* createGPXdoc(char* fileName)
{
	xmlDoc *parsed = NULL;
	xmlNode *rootElement = NULL;
	GPXdoc* doc;
	doc = malloc(sizeof(GPXdoc));
  //if no file name is given return NULL
  if (fileName == NULL){
    return NULL;
  }
	//parse xml file into a struct
	parsed = xmlReadFile(fileName, NULL, 0);
  //If an invalid file is given return NULL
	if(parsed == NULL){
		printf("could not parse file\n");
		return NULL;
	}
	rootElement = xmlDocGetRootElement(parsed);
	if(rootElement == NULL){
		printf("invalid file\n");
		return NULL;
	}
	//temp print the root element
	//print_element_names(rootElement);

  //get attributes of the gpx doc for loop taken from https://moodle.socs.uoguelph.ca/pluginfile.php/42297/mod_resource/content/0/libXmlExample.c
	strcpy(doc->namespace, "http://www.topografix.com/GPX/1/1");
	//printf("%s\n", doc->namespace);
	xmlAttr *attr;
  for (attr = rootElement->properties; attr != NULL; attr = attr->next){
    xmlNode *value = attr->children;
    char *attrName = (char *)attr->name;
    char *cont = (char *)(value->content);
    //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
    if(strcmp(attrName, "version") == 0){
      doc->version = atof(cont);
      //printf("%.1f\n", doc->version);
    }
    else if(strcmp(attrName, "creator") == 0){
			char* creator;
			creator = malloc(256);
			strcpy(creator, cont);
      doc->creator = creator;
      //printf("%s\n", doc->creator);
    }
  }
  doc->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
  doc->routes = initializeList(routeToString, deleteRoute, compareRoutes);
  doc->tracks = initializeList(trackToString, deleteTrack, compareTracks);

  getTracks(rootElement->children, doc->tracks);
	getRoutes(rootElement->children, doc->routes);
	getWaypointsGPX(rootElement->children, doc->waypoints);

  xmlFreeDoc(parsed);
	xmlCleanupParser();
	return doc;
}

//helper function to get a list of routes
void getRoutes(xmlNode * a_node, List* routes)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "rte") == 0) {
			//printf("node type: Element, name: %s\n", cur_node->name);
			Route *newRoute;
			newRoute = malloc(sizeof(Route));
			newRoute->name = malloc(1000);
			strcpy(newRoute->name, "");
			newRoute->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
			newRoute->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
			/*xmlAttr *attr;
			for (attr = cur_node->properties; attr != NULL; attr = attr->next){
		    xmlNode *value = attr->children;
		    //char *attrName = (char *)attr->name;
		    char *cont = (char *)(value->content);
		    insertBack(newRoute->attributes, cont);
				printf("%s\n", cont);
	    }*/
			getName(cur_node->children, newRoute->name);
			getAttributesRT(cur_node->children, newRoute->attributes);
			getWaypointRoutes(cur_node->children, newRoute->waypoints);
			insertBack(routes, newRoute);
		}
		getRoutes(cur_node->children, routes);
	}
}

//helper function to get the list of tracks
void getTracks(xmlNode * a_node, List* tracks)
{
  xmlNode *cur_node = NULL;

  for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
    char *nodeName = (char *)cur_node->name;
      if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "trk") == 0) {
        //printf("node type: Element, name: %s\n", cur_node->name);

			 	Track* newTrack;
				newTrack = malloc(sizeof(Track));
				newTrack->name = malloc(1000);
				strcpy(newTrack->name, "");
				newTrack->segments = initializeList(trackSegmentToString, deleteTrackSegment, compareTrackSegments);
				newTrack->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
				//get the name
				xmlAttr *attr;
				/*if(newTrack->name == NULL){
					strcpy(newTrack->name, "");
					printf("%s\n", newTrack->name);)
				}*/
				for (attr = cur_node->properties; attr != NULL; attr = attr->next){
			    xmlNode *value = attr->children;
					char *attrName = (char *)attr->name;
			    //char *attrName = (char *)attr->name;
			    char *cont = (char *)(value->content);
			      //insertBack(newTrack->attributes, cont);
						printf("%s\n", cont);
						if(strcmp(attrName, "name") == 0){
				      newTrack->name = cont;
				    }
		    }
				getName(cur_node->children, newTrack->name);
				getAttributesRT(cur_node->children, newTrack->attributes);
				getSegments(cur_node->children, newTrack->segments);
				insertBack(tracks, newTrack);
      }
			/*if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "name") == 0) {
        printf("node type: Element, name: %s\n", cur_node->name);
				xmlNode *value = cur_node->children;
				char *cont = (char *)(value->content);
				newTrack->name = cont;
				printf("%s\n", 	newTrack->name);
			}*/
      getTracks(cur_node->children, tracks);
  }
}

//helper function to get alist of track segments
void getSegments(xmlNode * a_node, List* segments)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "trkseg") == 0) {
			//printf("node type: Element, name: %s\n", cur_node->name);
			TrackSegment *newTrkseg;
			newTrkseg = malloc(sizeof(TrackSegment));
			newTrkseg->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
			getWaypoints(cur_node->children, newTrkseg->waypoints);
			insertBack(segments, newTrkseg);
		}
		getSegments(cur_node->children, segments);
	}
}

//helper function to get a list of waypoints for trksegs
void getWaypoints(xmlNode * a_node, List* waypoints)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "trkpt") == 0){
			//printf("node type: Element, name: %s\n", cur_node->name);
			Waypoint *newWay;
			newWay = malloc(sizeof(Waypoint));
			newWay->name = malloc(1000);
			strcpy(newWay->name, "");
			newWay->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
			xmlAttr *attr;
		  for (attr = cur_node->properties; attr != NULL; attr = attr->next){
		    xmlNode *value = attr->children;
		    char *attrName = (char *)attr->name;
		    char *cont = (char *)(value->content);
		    //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
		    if(strcmp(attrName, "lon") == 0){
		      newWay->longitude = atof(cont);
		      //printf("%f\n", newWay->longitude);
		    }
		    else if(strcmp(attrName, "lat") == 0){
		      newWay->latitude = atof(cont);
		      //printf("%f\n", newWay->latitude);
		    }
		  }
			getName(cur_node->children, newWay->name);
			getAttributes(cur_node->children, newWay->attributes);
			insertBack(waypoints, newWay);
		}
		getWaypoints(cur_node->children, waypoints);
	}
}
//helper function to get a list of waypoints for routes
void getWaypointRoutes(xmlNode * a_node, List* waypoints)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "rtept") == 0){
			//printf("node type: Element, name: %s\n", cur_node->name);
			Waypoint *newWay;
			newWay = malloc(sizeof(Waypoint));
			newWay->name = malloc(1000);
			strcpy(newWay->name, "");
			newWay->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
			xmlAttr *attr;
		  for (attr = cur_node->properties; attr != NULL; attr = attr->next){
		    xmlNode *value = attr->children;
		    char *attrName = (char *)attr->name;
		    char *cont = (char *)(value->content);
		    //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
		    if(strcmp(attrName, "lon") == 0){
		      newWay->longitude = atof(cont);
		      //printf("%f\n", newWay->longitude);
		    }
		    else if(strcmp(attrName, "lat") == 0){
		      newWay->latitude = atof(cont);
		      //printf("%f\n", newWay->latitude);
		    }
		  }
			getName(cur_node->children, newWay->name);
			getAttributes(cur_node->children, newWay->attributes);
			insertBack(waypoints, newWay);
		}
		getWaypointRoutes(cur_node->children, waypoints);
	}
}
//helper function to get a list of waypoints for GPXdocs
void getWaypointsGPX(xmlNode * a_node, List* waypoints)
{
	xmlNode *cur_node = NULL;
	//char* name;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		/*if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "name") == 0){
			printf("node type: Element, name: %s\n", cur_node->name);
			name = (char *)cur_node->name;
			printf("%s\n", name);
			return;
		}*/
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "wpt") == 0){
			//printf("node type: Element, name: %s\n", cur_node->name);
			Waypoint *newWay;
			newWay = malloc(sizeof(Waypoint));
			newWay->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
			newWay->name = malloc(1000);
			strcpy(newWay->name, "");
			xmlAttr *attr;
		  for (attr = cur_node->properties; attr != NULL; attr = attr->next){
		    xmlNode *value = attr->children;
		    char *attrName = (char *)attr->name;
		    char *cont = (char *)(value->content);
		    //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
		    if(strcmp(attrName, "lon") == 0){
		      newWay->longitude = atof(cont);
		      //printf("%f\n", newWay->longitude);
		    }
		    else if(strcmp(attrName, "lat") == 0){
		      newWay->latitude = atof(cont);
		      //printf("%f\n", newWay->latitude);
		    }
		  }
			getName(cur_node->children, newWay->name);
			//printf("%s\n", newWay->name);
			getAttributes(cur_node->children, newWay->attributes);
			insertBack(waypoints, newWay);
		}
		getWaypointsGPX(cur_node->children, waypoints);
	}
}

//helper function to get the name of an object if it exists
void getName(xmlNode *a_node, char* name)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;

		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "name") == 0){
			//printf("node type: Element, name: %s\n", (char*)xmlNodeGetContent(cur_node));//xml getcontent
			char *cont;
			cont = (char *)xmlNodeGetContent(cur_node);
			strcpy(name, cont);
			free(cont);
			return;
		 }
	 }
	if(name == NULL){
		getName(cur_node->children, NULL);
	}
	strcpy(name, "");
}

//helper function to get a list of attributes
void getAttributes(xmlNode * a_node, List* attributes)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "ele") == 0) {
			//printf("node type: Element, name: %s\n", cur_node->name);
			Attribute *newAtt;
			newAtt = malloc(sizeof(Attribute));
			newAtt->name = malloc(1000);
			strcpy(newAtt->name, nodeName);
			xmlNode *value = cur_node->children;
			char *cont = malloc(256);
			strcpy(cont, (char *)value->content);
			newAtt->value = cont;
			//printf("%s\n", 	newAtt->value);
			insertBack(attributes, newAtt);
		}
		getAttributes(cur_node->children, attributes);
	}

}

//get attributes for a route or track
void getAttributesRT(xmlNode * a_node, List* attributes)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
		char *nodeName = (char *)cur_node->name;
		if (cur_node->type == XML_ELEMENT_NODE && strcmp(nodeName, "desc") == 0) {
			//printf("node type: Element, name: %s\n", cur_node->name);
			Attribute *newAtt;
			newAtt = malloc(sizeof(Attribute));
			newAtt->name = malloc(1000);
			strcpy(newAtt->name, nodeName);
			xmlNode *value = cur_node->children;
			char *cont = malloc(256);
			strcpy(cont, (char *)value->content);
			newAtt->value = cont;
			//printf("%s\n", 	newAtt->value);
			insertBack(attributes, newAtt);
		}
		getAttributesRT(cur_node->children, attributes);
	}

}

char* GPXdocToString(GPXdoc* doc)
{
	char* tempStr;
	char *version;
	tempStr = malloc(1000000);

	strcpy(tempStr,"Name: ");
	strcat(tempStr, doc->namespace);
	strcat(tempStr, "\nVersion: ");
	version = malloc(318);
	snprintf(version, 318, "%lf", doc->version);
	strcat(tempStr, version);
	strcat(tempStr, "\ncreator: ");
	//strcat(tempStr, doc->creator);

	free(version);
  return tempStr;
}

//Number of waypoints in the GPX file - i.e. objects in the waypoint list
int getNumWaypoints(const GPXdoc* doc)
{
	if(doc == NULL){
		return 0;
	}
	int num = 0;
	num = getLength(doc->waypoints);
	return num;
}

//Number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc)
{
	if(doc == NULL){
		return 0;
	}
	int num = 0;
	num = getLength(doc->routes);
	return num;
}

//Number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc)
{
	if(doc == NULL){
		return 0;
	}
	int num = 0;
	num = getLength(doc->tracks);
	return num;
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc)
{
	if(doc == NULL){
		return 0;
	}
	int num = 0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->tracks);
	Track *temptrk;
	for(int i = 0; i < getLength(doc->tracks); i++){
		temptrk = (Track*)listpoint->current->data;
		num += getLength(temptrk->segments);
		nextElement(listpoint);
	}
	free(listpoint);
	return num;
}

//Total number of attributes in the document
int getNumAttributes(const GPXdoc* doc)
{
	if(doc == NULL){
		return 0;
	}
	int num = 0;
	ListIterator *listpoint;
	ListIterator *point2;
	ListIterator *point3;
	listpoint = malloc(sizeof(ListIterator));
	point2 = malloc(sizeof(ListIterator));
	point3 = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->tracks);
	Track *temptrk;
	for(int i = 0; i < getLength(doc->tracks); i++){
		/*if(strcmp(temptrk->name, "") != 0){
			num++;
		}*/
		temptrk = (Track*)listpoint->current->data;
		if(strcmp(temptrk->name, "") != 0){
			num++;
		}
		num += getLength(temptrk->attributes);
		nextElement(listpoint);

		*point3 = createIterator(temptrk->segments);
		TrackSegment *tempseg;
		for(int i = 0; i < getLength(temptrk->segments); i++){
			tempseg = (TrackSegment*)point3->current->data;
			nextElement(point3);
			*point2 = createIterator(tempseg->waypoints);
			Waypoint *tempway;
			for(int i = 0; i < getLength(tempseg->waypoints); i++){
				tempway = (Waypoint*)point2->current->data;
				if(strcmp(tempway->name, "") != 0){
					num++;
				}
				num += getLength(tempway->attributes);
				nextElement(point2);
			}
		}
	}

	*listpoint = createIterator(doc->routes);
	Route *temproute;
	for(int i = 0; i < getLength(doc->routes); i++){
		temproute = (Route*)listpoint->current->data;
		if(strcmp(temproute->name, "") != 0){
			num++;
		}
		num += getLength(temproute->attributes);
		nextElement(listpoint);
		*point2 = createIterator(temproute->waypoints);
		Waypoint *tempway;
		for(int i = 0; i < getLength(temproute->waypoints); i++){
			tempway = (Waypoint*)point2->current->data;
			if(strcmp(tempway->name, "") != 0){
				num++;
			}
			num += getLength(tempway->attributes);
			nextElement(point2);
		}
	}

	*listpoint = createIterator(doc->waypoints);
	Waypoint *tempway;
	for(int i = 0; i < getLength(doc->waypoints); i++){
		tempway = (Waypoint*)listpoint->current->data;
		if(strcmp(tempway->name, "") != 0){
			num++;
		}
		num += getLength(tempway->attributes);
		nextElement(listpoint);
	}
	free(point3);
	free(listpoint);
	free(point2);
	return num;
}

Waypoint* getWaypoint(const GPXdoc* doc, char* name)
{
	if(doc == NULL || name == NULL || strcmp(name, "") == 0){
		return NULL;
	}
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->waypoints);
	Waypoint *tempway;
	for(int i = 0; i < getLength(doc->waypoints); i++){
		tempway = (Waypoint*)listpoint->current->data;
		if(strcmp(tempway->name, name) == 0){
			free(listpoint);
			return tempway;
		}
		nextElement(listpoint);
	}
	free(listpoint);
	return NULL;
}
// Function that returns a track with the given name.  If more than one exists, return the first one.
Track* getTrack(const GPXdoc* doc, char* name)
{
	if(doc == NULL || name == NULL || strcmp(name, "") == 0){
		return NULL;
	}
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->tracks);
	Track *temptrk;
	for(int i = 0; i < getLength(doc->tracks); i++){
		temptrk = (Track*)listpoint->current->data;
		if(strcmp(temptrk->name, name) == 0){
			free(listpoint);
			return temptrk;
		}
		nextElement(listpoint);
	}
	free(listpoint);
	return NULL;
}
// Function that returns a route with the given name.  If more than one exists, return the first one.
Route* getRoute(const GPXdoc* doc, char* name)
{
	if(doc == NULL || name == NULL || strcmp(name, "") == 0){
		return NULL;
	}
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->routes);
	Route *temproute;
	for(int i = 0; i < getLength(doc->routes); i++){
		temproute = (Route*)listpoint->current->data;
		if(strcmp(temproute->name, name) == 0){
			free(listpoint);
			return temproute;
		}
		nextElement(listpoint);
	}
	free(listpoint);
	return NULL;
}


//free's the data in a gpx doc
void deleteGPXdoc(GPXdoc* doc)
{
  //valgrind --leak-check=yes bin/runMe -v --track-origins=yes
	if(doc == NULL){
		return;
	}
  freeList(doc->waypoints);
  freeList(doc->routes);
  freeList(doc->tracks);
	free(doc->creator);
	free(doc);
}

//helper functions attributes
void deleteAttribute( void* data)
{
	Attribute *tempAtt;
	if(data == NULL){
		return;
	}
	tempAtt = (Attribute*)data;
	free(tempAtt->name);
	free(tempAtt->value);
	free(data);
}

char* attributeToString( void* data){
  return "";
}

int compareAttributes(const void *first, const void *second)
{
  return 1;
}

//helper functions waypoints
void deleteWaypoint(void* data)
{
	Waypoint *tempwp;
	if(data == NULL){
		return;
	}
	tempwp = (Waypoint*)data;
	freeList(tempwp->attributes);
	free(tempwp->name);
	free(data);
}

char* waypointToString( void* data){
  return "";
}

int compareWaypoints(const void *first, const void *second)
{
  return 1;
}

//helper functions routes
void deleteRoute(void* data)
{
	Route *temproute;
	if(data == NULL){
		return;
	}
	temproute = (Route*)data;
	freeList(temproute->waypoints);
	freeList(temproute->attributes);
	free(temproute->name);
	free(data);
}

char* routeToString(void* data)
{
  return "";
}

int compareRoutes(const void *first, const void *second)
{
  return 1;
}

//helper functions track segments
void deleteTrackSegment(void* data)
{
	TrackSegment *tempsgmt;
	if(data == NULL){
		return;
	}
	tempsgmt = (TrackSegment*)data;
	freeList(tempsgmt->waypoints);
	free(data);
}

char* trackSegmentToString(void* data)
{
  return "";
}

int compareTrackSegments(const void *first, const void *second)
{
  return 1;
}

//helper functions tracks
void deleteTrack(void* data)
{
	Track *temptrk;
	if(data == NULL){
		return;
	}
	temptrk = (Track*)data;
	freeList(temptrk->segments);
	freeList(temptrk->attributes);
	free(temptrk->name);
	free(data);
}

char* trackToString(void* data)
{
  return "";
}

int compareTracks(const void *first, const void *second)
{
  return 1;
}

//A2 functions

//code for xml validation taken from http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
int validateGPXDoc(char* doc, char* gpxSchemaFile)
{
	if(doc == NULL){
		return 0;
	}
	//writeGPXdoc(doc, "writeFile");
	xmlDocPtr xmldoc;
	xmlSchemaPtr schema = NULL;
	xmlSchemaParserCtxtPtr ctxt;
	//char *XMLFileName = doc;
	//char *XSDFileName = "test.xsd";

	xmlLineNumbersDefault(1);

	ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

	xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
	schema = xmlSchemaParse(ctxt);
	xmlSchemaFreeParserCtxt(ctxt);
	//xmlSchemaDump(stdout, schema); //To print schema dump
	xmldoc = xmlReadFile(doc, NULL, 0);//GPXtoXml(doc);//xmlReadFile(XMLFileName, NULL, 0);

	if (xmldoc == NULL)
	{
		//fprintf(stderr, "Could not parse %s\n", XMLFileName);
	}
	else
	{
		xmlSchemaValidCtxtPtr ctxt;
		int ret;

		ctxt = xmlSchemaNewValidCtxt(schema);
		xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
		ret = xmlSchemaValidateDoc(ctxt, xmldoc);

		if (ret == 0)
		{
			xmlSchemaFreeValidCtxt(ctxt);
			xmlFreeDoc(xmldoc);
			xmlSchemaFree(schema);

			xmlSchemaCleanupTypes();
			xmlCleanupParser();
			xmlMemoryDump();
			//printf("%s validates\n", XMLFileName);
			return 1;
		}
		else if (ret > 0)
		{
			xmlSchemaFreeValidCtxt(ctxt);
			xmlFreeDoc(xmldoc);
			xmlSchemaFree(schema);

			xmlSchemaCleanupTypes();
			xmlCleanupParser();
			xmlMemoryDump();
			return 0;
			//printf("%s fails to validate\n", XMLFileName);
		}
		else
		{
			xmlSchemaFreeValidCtxt(ctxt);
			xmlFreeDoc(xmldoc);
			xmlSchemaFree(schema);

			xmlSchemaCleanupTypes();
			xmlCleanupParser();
			xmlMemoryDump();
			return 0;
			//printf("%s validation generated an internal error\n", XMLFileName);
		}
		xmlSchemaFreeValidCtxt(ctxt);
		xmlFreeDoc(xmldoc);

	}

	// free the resource
	if(schema != NULL){
		xmlSchemaFree(schema);
		xmlSchemaCleanupTypes();
		xmlCleanupParser();
		xmlMemoryDump();
	}

	return 0;
}


bool writeGPXdoc(GPXdoc* doc, char* fileName)
{
	/*if(validateGPXDoc(doc, "gpx.xsd") == false){
		//printf("false\n");
		return false;
	}*/
	xmlDoc *tree = GPXtoXml(doc);

	//save tree to file and free contents
	xmlSaveFormatFileEnc(fileName, tree, "UTF-8", 1);
	//xmlFreeNs(tree->oldNs);
  xmlFreeDoc(tree);
  xmlCleanupParser();
	return true;
}

xmlDoc* GPXtoXml(GPXdoc* doc)
{
	if(doc == NULL){
		return NULL;
	}
	xmlDoc *xmldoc = NULL;
	xmlNode *root = NULL;
	xmlNode *node = NULL;
	xmlNode *node2 = NULL;
	xmlNode *node3 = NULL;
	//xmlNode *node4 = NULL;
	char *version;
	version = malloc(10);

	sprintf(version, "%.1f", doc->version);
	//printf("%s\n", version);

	xmldoc = xmlNewDoc(BAD_CAST "1.0");
  root = xmlNewNode(NULL, BAD_CAST "gpx");
  xmlDocSetRootElement(xmldoc, root);
	//xmlCreateIntSubset(xmldoc, BAD_CAST "root", NULL, BAD_CAST "tree2.dtd");
	//creates an attibute for a node
  //xmlNewProp(node1, BAD_CAST "attribute", BAD_CAST "yes");

	xmlNewProp(root, BAD_CAST "version", BAD_CAST version);
	//printf("%s\n", doc->creator);
	xmlNewProp(root, BAD_CAST "creator", BAD_CAST doc->creator);
	xmlNewNs(root,  BAD_CAST "http://www.topografix.com/GPX/1/1", NULL);
	//xmlSetNs(root, ns);

	//creates a child node
	//xmlNewChild(root, NULL, BAD_CAST "node1", BAD_CAST "content of node 1");
	//waypoints
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->waypoints);
	Waypoint *tempway;
	for(int i = 0; i < getLength(doc->waypoints); i++){
		tempway = (Waypoint*)listpoint->current->data;
		char* lat = malloc(256);
		char* lon = malloc(256);
		sprintf(lat, "%f", tempway->latitude);
		sprintf(lon, "%f", tempway->longitude);
		node = xmlNewChild(root, NULL, BAD_CAST "wpt", NULL);
		xmlNewProp(node, BAD_CAST "lat", BAD_CAST lat);
		xmlNewProp(node, BAD_CAST "lon", BAD_CAST lon);
		xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tempway->name);
		free(lat);
		free(lon);

		ListIterator *listpoint4;
		listpoint4 = malloc(sizeof(ListIterator));
		*listpoint4 = createIterator(tempway->attributes);
		Attribute *att;
		for(int i = 0; i < getLength(tempway->attributes); i++){
			att = (Attribute*)listpoint4->current->data;
			xmlNewChild(node, NULL, BAD_CAST att->name, BAD_CAST att->value);
			nextElement(listpoint4);
		}
		free(listpoint4);

		nextElement(listpoint);
	}
	//routes
	*listpoint = createIterator(doc->routes);
	Route *temproute;
	for(int i = 0; i < getLength(doc->routes); i++){
		temproute = (Route*)listpoint->current->data;
		node = xmlNewChild(root, NULL, BAD_CAST "rte", NULL);
		//xmlNewProp(node, BAD_CAST "lat", BAD_CAST lat);
		xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST temproute->name);

		ListIterator *listpoint4;
		listpoint4 = malloc(sizeof(ListIterator));
		*listpoint4 = createIterator(temproute->attributes);
		Attribute *att;
		for(int i = 0; i < getLength(temproute->attributes); i++){
			att = (Attribute*)listpoint4->current->data;
			//printf("attribute: %s\n", att->name);
			xmlNewChild(node, NULL, BAD_CAST att->name, BAD_CAST att->value);
			nextElement(listpoint4);
		}
		free(listpoint4);

		ListIterator *listpoint2;
		listpoint2 = malloc(sizeof(ListIterator));
		*listpoint2 = createIterator(temproute->waypoints);
		Waypoint *way;
		for(int i = 0; i < getLength(temproute->waypoints); i++){
			way = (Waypoint*)listpoint2->current->data;
			char* lat = malloc(256);
			char* lon = malloc(256);
			sprintf(lat, "%f", way->latitude);
			sprintf(lon, "%f", way->longitude);
			node2 = xmlNewChild(node, NULL, BAD_CAST "rtept", NULL);
			xmlNewProp(node2, BAD_CAST "lat", BAD_CAST lat);
			xmlNewProp(node2, BAD_CAST "lon", BAD_CAST lon);
			xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST way->name);
			free(lat);
			free(lon);
			ListIterator *listpoint5;
			listpoint5 = malloc(sizeof(ListIterator));
			*listpoint5 = createIterator(way->attributes);
			Attribute *att;
			for(int i = 0; i < getLength(way->attributes); i++){
				att = (Attribute*)listpoint5->current->data;
				xmlNewChild(node2, NULL, BAD_CAST att->name, BAD_CAST att->value);
				nextElement(listpoint5);
			}
			free(listpoint5);
			nextElement(listpoint2);
		}
		free(listpoint2);
		nextElement(listpoint);
	}
	//tracks
	*listpoint = createIterator(doc->tracks);
	Track *temptrack;
	for(int i = 0; i < getLength(doc->tracks); i++){
		temptrack = (Track*)listpoint->current->data;
		node = xmlNewChild(root, NULL, BAD_CAST "trk", NULL);
		//xmlNewProp(node, BAD_CAST "lat", BAD_CAST lat);
		xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST temptrack->name);

		ListIterator *listpoint4;
		listpoint4 = malloc(sizeof(ListIterator));
		*listpoint4 = createIterator(temptrack->attributes);
		Attribute *att;
		for(int i = 0; i < getLength(temptrack->attributes); i++){
			att = (Attribute*)listpoint4->current->data;
			xmlNewChild(node, NULL, BAD_CAST att->name, BAD_CAST att->value);
			nextElement(listpoint4);
		}
		free(listpoint4);

		ListIterator *listpoint3;
		listpoint3 = malloc(sizeof(ListIterator));
		*listpoint3 = createIterator(temptrack->segments);
		TrackSegment *seg;
		for(int i = 0; i < getLength(temptrack->segments); i++){
			seg = (TrackSegment*)listpoint3->current->data;
			node3 = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
			//xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST seg->name);
			ListIterator *listpoint2;
			listpoint2 = malloc(sizeof(ListIterator));
			*listpoint2 = createIterator(seg->waypoints);
			Waypoint *way;
			for(int i = 0; i < getLength(seg->waypoints); i++){
				way = (Waypoint*)listpoint2->current->data;
				char* lat = malloc(256);
				char* lon = malloc(256);
				sprintf(lat, "%f", way->latitude);
				sprintf(lon, "%f", way->longitude);
				node2 = xmlNewChild(node3, NULL, BAD_CAST "trkpt", NULL);
				xmlNewProp(node2, BAD_CAST "lat", BAD_CAST lat);
				xmlNewProp(node2, BAD_CAST "lon", BAD_CAST lon);
				xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST way->name);
				free(lat);
				free(lon);
				ListIterator *listpoint5;
				listpoint5 = malloc(sizeof(ListIterator));
				*listpoint5 = createIterator(way->attributes);
				Attribute *att;
				for(int i = 0; i < getLength(way->attributes); i++){
					att = (Attribute*)listpoint5->current->data;
					xmlNewChild(node2, NULL, BAD_CAST att->name, BAD_CAST att->value);
					nextElement(listpoint5);
				}
				free(listpoint5);
				nextElement(listpoint2);
			}
			free(listpoint2);
			nextElement(listpoint3);
		}
		free(listpoint3);
		nextElement(listpoint);
	}

	free(listpoint);
	free(version);
	return xmldoc;
}

float round10(float len)
{
	len += 5;
	len = (int)len / 10;
	len = len* 10;
	return len;
}

//calaculate haversine formula to get the distance between 2 points
float haversine(float lon1, float lat1, float lon2, float lat2)
{
	float x = (lat2 - lat1) * (M_PI / 180);
	float y = (lon2 - lon1) * (M_PI / 180);
	float a = 0;
	float c = 0;
	float d = 0;

	a = sin(x/2) * sin(x/2) + sin(y/2) * sin(y/2) *cos(lat1 * (M_PI / 180)) * cos(lat2 * (M_PI / 180));

	c = 2 * atan2(sqrt(a), sqrt(1-a));

	d = c * 6371000;

	return d;
}

float getRouteLen(const Route *rt)
{
	if(rt == NULL){
		return 0;
	}
	if(getLength(rt->waypoints) == 0){
		return 0;
	}
	float total = 0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(rt->waypoints);
	Waypoint *tempway;
	Waypoint *previous;
	//get the len between first 2 waypoints
	previous = (Waypoint*)listpoint->current->data;
	nextElement(listpoint);
	tempway = (Waypoint*)listpoint->current->data;

	total = haversine(previous->longitude, previous->latitude, tempway->longitude, tempway->latitude);
	previous = (Waypoint*)listpoint->current->data;
	//do the rest of the waypoints
	for(int i = 1; i < getLength(rt->waypoints); i++){
		tempway = (Waypoint*)listpoint->current->data;
		total = total + haversine(previous->longitude, previous->latitude, tempway->longitude, tempway->latitude);
		previous = (Waypoint*)listpoint->current->data;
		nextElement(listpoint);
	}
	free(listpoint);
	return total;
}


float getTrackLen(const Track *tr)
{
	if(tr == NULL){
		return 0;
	}
	if(getLength(tr->segments) == 0){
		return 0;
	}
	/*else{
		return 10;
	}*/
	float total = 0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(tr->segments);
	TrackSegment *tempseg;
	for(int i = 0; i < getLength(tr->segments); i++){
		tempseg = (TrackSegment*)listpoint;
		ListIterator* listpoint2;
		listpoint2 = malloc(sizeof(ListIterator));
		*listpoint2 = createIterator(tempseg->waypoints);
		Waypoint* tempway;
		Waypoint *previous;
		if(i == 0){
			previous = (Waypoint*)listpoint2->current->data;
		}
		for(int j = 0; j < getLength(tempseg->waypoints); j++){
			tempway = (Waypoint*)listpoint2->current->data;
			total = total + haversine(previous->longitude, previous->latitude, tempway->longitude, tempway->latitude);
			previous = (Waypoint*)listpoint2->current->data;
			nextElement(listpoint2);
		}
		nextElement(listpoint);
		free(listpoint2);
	}
	free(listpoint);
	return total;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta)
{
	if(doc == NULL || len < 0 || delta < 0){
		return 0;
	}
	float total = 0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->routes);
	Route *temp;
	//do the rest of the waypoints
	for(int i = 0; i < getLength(doc->routes); i++){
		temp = (Route*)listpoint->current->data;
		if(getRouteLen(temp) >= len && getRouteLen(temp) + delta <= len){
			total++;
		}
		else if(getRouteLen(temp) <= len && getRouteLen(temp) + delta >= len){
			total++;
		}
		nextElement(listpoint);
	}
	free(listpoint);
	return total;
}



int numTracksWithLength(const GPXdoc* doc, float len, float delta)
{
	if(doc == NULL || len < 0 || delta < 0){
		return 0;
	}
	float total = 0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->tracks);
	Track *temp;
	//do the rest of the waypoints
	for(int i = 0; i < getLength(doc->tracks); i++){
		temp = (Track*)listpoint->current->data;
		if(getTrackLen(temp) >= len && getTrackLen(temp) + delta <= len){
			total++;
		}
		else if(getTrackLen(temp) <= len && getTrackLen(temp) + delta >= len){
			total++;
		}
		nextElement(listpoint);
	}
	free(listpoint);
	return total;
}


bool isLoopRoute(const Route* route, float delta)
{
	if(route == NULL || delta < 0){
		return false;
	}
	if(getLength(route->waypoints) < 4){
		return false;
	}
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(route->waypoints);
	Waypoint *temp;
	Waypoint *end;
	temp = (Waypoint*)listpoint->current->data;
	for(int i = 0; i < getLength(route->waypoints); i++){
		end = (Waypoint*)listpoint->current->data;
		nextElement(listpoint);
	}
	if(haversine(temp->longitude, temp->latitude, end->longitude, end->latitude) < delta){
		free(listpoint);
		return true;
	}
	free(listpoint);
	return false;
}


bool isLoopTrack(const Track *tr, float delta)
{
	if(tr == NULL || delta < 0){
		return false;
	}
	int count = 0;
	ListIterator *listpoint2;
	listpoint2 = malloc(sizeof(ListIterator));
	*listpoint2 = createIterator(tr->segments);
	TrackSegment *curr;
	Waypoint *temp;
	Waypoint *end;
	curr = (TrackSegment*)listpoint2->current->data;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	for(int j = 0; j < getLength(tr->segments); j++){
		*listpoint = createIterator(curr->waypoints);
		temp = (Waypoint*)listpoint->current->data;
		for(int i = 0; i < getLength(curr->waypoints); i++){
			end = (Waypoint*)listpoint->current->data;
			nextElement(listpoint);
			count++;
		}
		nextElement(listpoint2);
	}
	if(haversine(temp->longitude, temp->latitude, end->longitude, end->latitude) < delta && count > 3){
		free(listpoint);
		free(listpoint2);
		return true;
	}
	free(listpoint);
	free(listpoint2);
	return false;
}



List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
	if(doc == NULL){
		return NULL;
	}
	int start = 0;
	int end =  0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->routes);
	List * routes = initializeList(routeToString, deleteRoute, compareRoutes);
	Route *temp;
	for(int i = 0; i < getLength(doc->routes); i++){
		temp = (Route*)listpoint->current->data;
		ListIterator *listpoint2;
		listpoint2 = malloc(sizeof(ListIterator));
		*listpoint2 = createIterator(temp->waypoints);
		Waypoint *way;
		for(int j = 0; j < getLength(temp->waypoints); j++){
			way = (Waypoint*)listpoint2->current->data;
			//delta is 15
			//printf("%f\n", sourceLat);
			//printf("%f\n", sourceLong);
			//printf("%f \n", haversine(sourceLong, sourceLat, way->longitude, way->latitude));
			//printf("%f \n", haversine(destLong, destLat, way->longitude, way->latitude));
			//printf("%f \n", delta);
			if(haversine(sourceLong, sourceLat, way->longitude, way->latitude) <= delta){
				start = 1;
			}
			if(haversine(destLong, destLat, way->longitude, way->latitude) <= delta){
				end = 1;
			}
			nextElement(listpoint2);
		}
		if(start == 1 && end == 1){
			insertBack(routes, temp);
		}
		start = 0;
		end = 0;
		nextElement(listpoint);
		free(listpoint2);
	}
	free(listpoint);
	if(getLength(routes) <= 0){
		return NULL;
	}
	return routes;
}


List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
	if(doc == NULL){
		return NULL;
	}
	List * tracks = initializeList(trackToString, deleteTrack, compareTracks);
	int start = 0;
	int end =  0;
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(doc->tracks);
	Track *temp;
	for(int i = 0; i < getLength(doc->tracks); i++){
		temp = (Track*)listpoint->current->data;
		ListIterator *listpoint2;
		listpoint2 = malloc(sizeof(ListIterator));
		*listpoint2 = createIterator(temp->segments);
		TrackSegment *seg;
		for(int i = 0; i < getLength(temp->segments); i++){
			seg = (TrackSegment*)listpoint->current->data;
			ListIterator *listpoint3;
			listpoint3 = malloc(sizeof(ListIterator));
			*listpoint3 = createIterator(seg->waypoints);
			Waypoint *way;
			for(int j = 0; j < getLength(seg->waypoints); j++){
				way = (Waypoint*)listpoint3->current->data;
				if(haversine(sourceLong, sourceLat, way->longitude, way->latitude) <= delta){
					start = 1;
				}
				if(haversine(destLong, destLat, way->longitude, way->latitude) <= delta){
					end = 1;
				}
				nextElement(listpoint3);
			}
			nextElement(listpoint2);
			free(listpoint3);
		}
		if(start == 1 && end == 1){
			insertBack(tracks, temp);
		}
		start = 0;
		end = 0;
		nextElement(listpoint);
		free(listpoint2);
	}
	free(listpoint);
	if(getLength(tracks) <= 0){
		return NULL;
	}
	return tracks;
}

char* trackToJSON(const Track *tr)
{
	char *tempStr;
	tempStr = malloc(1000);
	if(tr == NULL){
		sprintf(tempStr, "{}");
		return tempStr;
	}
	if(strcmp(tr->name, "") == 0){
		if(isLoopTrack(tr, 15) == true){
			sprintf(tempStr, "{\"name\":\"None\",\"len\":%.1f,\"loop\":true}", round10(getTrackLen(tr)));
		}
		else{
			sprintf(tempStr, "{\"name\":\"None\",\"len\":%.1f,\"loop\":false}", round10(getTrackLen(tr)));
		}
	}
	else{
		if(isLoopTrack(tr, 15) == true){
			sprintf(tempStr, "{\"name\":\"%s\",\"len\":%.1f,\"loop\":true}", tr->name, round10(getTrackLen(tr)));
		}
		else{
			sprintf(tempStr, "{\"name\":\"%s\",\"len\":%.1f,\"loop\":false}", tr->name, round10(getTrackLen(tr)));
		}
	}
	return tempStr;
	return NULL;
}


char* routeToJSON(const Route *rt)
{
	char *tempStr;
	tempStr = malloc(1000);
	if(rt == NULL){
		sprintf(tempStr, "{}");
		return tempStr;
	}
	//printf("%s\n", rt->name);
	if(strcmp(rt->name, "") == 0){
		if(isLoopRoute(rt, 15) == true){
			sprintf(tempStr, "{\"name\":\"None\",\"numPoints\":%d,\"len\":%.1f,\"loop\":true}", getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
		else{
			sprintf(tempStr, "{\"name\":\"None\",\"numPoints\":%d,\"len\":%.1f,\"loop\":false}", getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
	}
	else{
		if(isLoopRoute(rt, 15) == true){
			sprintf(tempStr, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":true}", rt->name, getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
		else{
			sprintf(tempStr, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":false}", rt->name, getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
	}
	return tempStr;
	return NULL;
}


char* routeListToJSON(const List *list){
	char *tempStr;
	tempStr = malloc(1000);
	if(list == NULL){
		sprintf(tempStr, "[]");
		return tempStr;
	}

	strcpy(tempStr, "[");
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	listpoint->current = list->head;
	Route *temp;
	temp = (Route*)listpoint->current->data;
	int i = 0;
	while((temp = nextElement(listpoint)) != NULL){
		i++;
	}
	listpoint->current = list->head;
	int j = 0;
	while((temp = nextElement(listpoint)) != NULL){
		strcat(tempStr, routeToJSON(temp));
		j++;
		if(j < i){
			strcat(tempStr, ",");
		}
	}
	strcat(tempStr, "]");

	free(listpoint);
	return tempStr;
}


char* trackListToJSON(const List *list)
{
	char *tempStr;
	tempStr = malloc(1000);
	if(list == NULL){
		sprintf(tempStr, "[]");
		return tempStr;
	}

	strcpy(tempStr, "[");
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	listpoint->current = list->head;
	Track *temp;
	temp = (Track*)listpoint->current->data;
	int i = 0;
	while((temp = nextElement(listpoint)) != NULL){
		i++;
	}
	listpoint->current = list->head;
	int j = 0;
	while((temp = nextElement(listpoint)) != NULL){
		strcat(tempStr, trackToJSON(temp));
		j++;
		if(j < i){
			strcat(tempStr, ",");
		}
	}
	strcat(tempStr, "]");

	free(listpoint);
	return tempStr;
}


char* GPXtoJSON(const GPXdoc* gpx)
{
	char *tempStr;
	tempStr = malloc(1000);
	if(gpx == NULL){
		sprintf(tempStr, "{}");
		return tempStr;
	}
	char *version;
	version = malloc(10);
	sprintf(version, "%.1f", gpx->version);

	sprintf(tempStr, "{\"version\":%s,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", version, gpx->creator, getLength(gpx->waypoints), getLength(gpx->routes), getLength(gpx->tracks));
	free(version);
	return tempStr;
}

//bonus fnctions
void addWaypoint(Route *rt, Waypoint *pt)
{
	if(rt == NULL || pt == NULL){
		return;
	}
	insertBack(rt->waypoints, pt);
	return;
}


void addRoute(GPXdoc* doc, Route* rt)
{
	if(rt == NULL || doc == NULL){
		return;
	}
	insertBack(doc->routes, rt);
	return;
}


GPXdoc* JSONtoGPX(const char* gpxString)
{
	if(gpxString == NULL){
		return NULL;
	}
	GPXdoc *newDoc;
	newDoc = malloc(sizeof(GPXdoc));
	newDoc->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
	newDoc->routes = initializeList(routeToString, deleteRoute, compareRoutes);
  newDoc->tracks = initializeList(trackToString, deleteTrack, compareTracks);
	strcpy(newDoc->namespace, "http://www.topografix.com/GPX/1/1");
	newDoc->creator = malloc(256);
	char *tempStr;
	tempStr = malloc(256);
	strcpy(tempStr, gpxString);
	char *ptr = strtok(tempStr, ":");
	ptr = strtok(NULL, ",");
	newDoc->version = atof(ptr);
	ptr = strtok(NULL, ":");
	ptr = strtok(ptr, "}");
	strcpy(newDoc->creator, ptr);

	return newDoc;
}


Waypoint* JSONtoWaypoint(const char* gpxString)
{
	if(gpxString == NULL){
		return NULL;
	}
	Waypoint *way;
	way = malloc(sizeof(Waypoint));
	way->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
	char *tempStr;
	tempStr = malloc(256);
	strcpy(tempStr, gpxString);
	char *ptr = strtok(tempStr, ":");
	ptr = strtok(NULL, ",");
	way->latitude = atof(ptr);
	ptr = strtok(NULL, ":");
	ptr = strtok(NULL, "}");
	way->longitude = atof(ptr);
	return way;
}


Route* JSONtoRoute(const char* gpxString)
{
	if(gpxString == NULL){
		return NULL;
	}
	Route *rt;
	rt = malloc(sizeof(Route));
	rt->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
	rt->attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
	rt->name = malloc(256);
	char *tempStr;
	tempStr = malloc(256);
	strcpy(tempStr, gpxString);
	char *ptr = strtok(tempStr, ":");
	ptr = strtok(NULL, "\"");
	ptr = strtok(ptr, "\"");
	strcpy(rt->name, ptr);
	return rt;
}

char* fileNametoGPXJSON(char* filename)
{
	GPXdoc *doc;
	doc = createGPXdoc(filename);
	char* tempstr = GPXtoJSON(doc);
	//printf("%s\n", GPXtoJSON(createGPXdoc(filename)));
	return tempstr;
}

char* fileNametoRouteListJSON(char* filename){
	GPXdoc *doc;
	doc = createGPXdoc(filename);
	char* tempStr = routeListToJSON(doc->routes);
	return tempStr;
}

char* fileNametoTrackListJSON(char* filename){
	GPXdoc *doc;
	doc = createGPXdoc(filename);
	char* tempStr = trackListToJSON(doc->tracks);
	return tempStr;
}

char* routeToAttJSONlist(char *routeName, char *fileName){
	char *tempStr;
	tempStr = malloc(1000);
	GPXdoc *doc;
	doc = createGPXdoc(fileName);
	Route * rt = getRoute(doc, routeName);
	if(rt == NULL){
		sprintf(tempStr, "[]");
		return tempStr;
	}

	strcpy(tempStr, "[");
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(rt->attributes);
	Attribute *temp;
	temp = (Attribute*)listpoint->current->data;
	int i = 0;
	while((temp = nextElement(listpoint)) != NULL){
		i++;
	}
	listpoint->current = rt->attributes->head;
	int j = 0;
	while((temp = nextElement(listpoint)) != NULL){
		char* tempstr;
		tempstr = malloc(256);
		temp = (Attribute*)listpoint->current->data;
		sprintf(tempstr,  "{\"name\":\"%s\",\"value\":\"%s\"}", temp->name, temp->value);
		strcat(tempStr, tempstr);
		//strcat(tempStr, routeToJSON(temp));
		j++;
		if(j < i){
			strcat(tempStr, ",");
		}
		free(tempstr);
	}
	strcat(tempStr, "]");

	free(listpoint);
	return "[{\"name\":\"some attribute\",\"value\":\"value of attribte\"}]";
}

char* trackToAttJSONlist(char *trackName, char *fileName){
	char *tempStr;
	tempStr = malloc(1000);
	GPXdoc *doc;
	doc = createGPXdoc(fileName);
	Track *tr = getTrack(doc, trackName);
	if(tr == NULL){
		sprintf(tempStr, "[]");
		return tempStr;
	}

	strcpy(tempStr, "[");
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	*listpoint = createIterator(tr->attributes);
	Attribute *temp;
	temp = (Attribute*)listpoint->current->data;
	int i = 0;
	while((temp = nextElement(listpoint)) != NULL){
		i++;
	}
	listpoint->current = tr->attributes->head;
	int j = 0;
	while((temp = nextElement(listpoint)) != NULL){
		char* tempstr;
		tempstr = malloc(256);
		temp = (Attribute*)listpoint->current->data;
		sprintf(tempstr,  "{\"Name\":\"%s\",\"Value\":\"%s\"}", temp->name, temp->value);
		strcat(tempStr, tempstr);
		//strcat(tempStr, routeToJSON(temp));
		j++;
		if(j < i){
			strcat(tempStr, ",");
		}
		free(tempstr);
	}
	strcat(tempStr, "]");

	free(listpoint);
	return tempStr;
}

void writefromJSON(char* JSON, char* fileName){
	GPXdoc *doc;
	doc = JSONtoGPX(JSON);
	writeGPXdoc(doc, fileName);
}
