
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Simple Octree Implementation 11/10/2020
// 
//  Copyright (c) by Kevin M. Smith
//  Copying or use without permission is prohibited by law. 
//


#include "Octree.h"



//draw a box from a "Box" class  
//
void Octree::drawBox(const Box& box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();

	ofNoFill(); // Added

	ofDrawBox(p, w, h, d);

	ofFill(); // Added
}

// return a Mesh Bounding Box for the entire Mesh
//
Box Octree::meshBounds(const ofMesh& mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}

	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//
int Octree::getMeshPointsInBox(const ofMesh& mesh, const vector<int>& points,
	Box& box, vector<int>& pointsRtn)
{
	int count = 0;
	for (int i = 0; i < points.size(); i++) {
		ofVec3f v = mesh.getVertex(points[i]);
		if (box.inside(Vector3(v.x, v.y, v.z))) {
			count++;
			pointsRtn.push_back(points[i]);
		}
	}
	return count;
}

// getMeshFacesInBox:  return an array of indices to Faces in mesh that are contained 
//                      inside the Box.  Return count of faces found;
//
int Octree::getMeshFacesInBox(const ofMesh& mesh, const vector<int>& faces,
	Box& box, vector<int>& facesRtn)
{
	int count = 0;
	for (int i = 0; i < faces.size(); i++) {
		ofMeshFace face = mesh.getFace(faces[i]);
		ofVec3f v[3];
		v[0] = face.getVertex(0);
		v[1] = face.getVertex(1);
		v[2] = face.getVertex(2);
		Vector3 p[3];
		p[0] = Vector3(v[0].x, v[0].y, v[0].z);
		p[1] = Vector3(v[1].x, v[1].y, v[1].z);
		p[2] = Vector3(v[2].x, v[2].y, v[2].z);
		if (box.inside(p, 3)) {
			count++;
			facesRtn.push_back(faces[i]);
		}
	}
	return count;
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box& box, vector<Box>& boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

void Octree::create(const ofMesh& geo, int numLevels) {
	
	// initialize octree structure
	//
	mesh = geo;
	int level = 0;
	root.box = meshBounds(mesh);
	if (!bUseFaces) {
		for (int i = 0; i < mesh.getNumVertices(); i++) {
			root.points.push_back(i);
		}
	}
	else {
		// need to load face vertices here
		//
	}

	// recursively buid octree
	//
	level++;
	subdivide(mesh, root, numLevels, level);
}


// Implement functions below for Homework project ************************************
//
//
// subdivide:  recursive function to perform octree subdivision on a mesh
//
//  subdivide(node) algorithm:
//     1) subdivide box in node into 8 equal side boxes - see helper function subDivideBox8().
//     2) For each child box
//            sort point data into each box  (see helper function getMeshFacesInBox())
//        if a child box contains at least 1 point
//            add child to tree
//            if child is not a leaf node (contains more than 1 point)
//               recursively call subdivide(child)
//         
//      

void Octree::subdivide(const ofMesh& mesh, TreeNode& node, int numLevels, int level) {
	if (level >= numLevels) return;

	vector<Box> boxList;
	subDivideBox8(node.box, boxList); // Subdivide box into 1/8ths

	for (int i = 0; i < boxList.size(); i++) { // For each child box (each 1/8th)
		TreeNode child;
		child.box = boxList[i];

		vector<int> indices;  // Get the points found in this child box
		int count = getMeshPointsInBox(mesh, node.points, child.box, indices);

		// Sort point data into each box
		if (count > 0) { // If it contains at least 1 point
			child.points = indices; // Assign that child it's points
			node.children.push_back(child); // Add the child to the tree

			// If child is not a leaf node
			if (count > 1) {
				// Recursively subdivide it 
				subdivide(mesh, node.children.back(), numLevels, level + 1);
			}
		}
	}
}

bool Octree::intersect(const Ray& ray, const TreeNode& node, TreeNode& nodeRtn) {

	// If ray misses this node�s box, return false
	if (!node.box.intersect(ray, 0, 1000)) {
		return false;
	}

	// If this node has no children
	if (node.children.empty()) {
		nodeRtn = node; // save the hit
		return true;
	}

	//  Otherwise, recurse into children
	for (TreeNode child : node.children) {
		if (intersect(ray, child, nodeRtn)) {
			return true; // Stop at the first hit
		}
	}

	// No hit :(
	return false;
}

bool Octree::intersect(const Box& box, TreeNode& node, vector<Box>& boxListRtn) {

	bool intersects = node.box.overlap(box);

	if (intersects) {
		if (node.children.empty()) {
			boxListRtn.push_back(node.box);
		}

		for (TreeNode& child : node.children) {
			intersect(box, child, boxListRtn);
		}
	}
	return intersects;
}

void Octree::draw(TreeNode& node, int numLevels, int level, const vector<ofColor>& levelColors) {
	if (level >= numLevels) return;

	ofSetColor(levelColors[level % levelColors.size()]);
	ofNoFill();
	drawBox(node.box);
	ofFill();

	for (auto& child : node.children) {
		draw(child, numLevels, level + 1, levelColors);
	}
}

// Optional
//
void Octree::drawLeafNodes(TreeNode& node) {


}