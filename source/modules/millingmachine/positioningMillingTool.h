#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Vector_3.h>
#include <CGAL/Aff_transformation_3.h>
#include <CGAL/aff_transformation_tags.h>

#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_halfedge_graph_segment_primitive.h>



typedef CGAL::Exact_predicates_exact_constructions_kernel   K;
typedef K::Point_3                                          Point_3;
typedef K::Point_2                                          Point;
typedef CGAL::Surface_mesh<K::Point_3>                      Mesh;
typedef K::Vector_3                                         Vector_3;
typedef CGAL::Aff_transformation_3<K>                       Aff_transformation_3;

typedef CGAL::AABB_face_graph_triangle_primitive<Mesh>		Primitive;
typedef CGAL::AABB_traits<K, Primitive>						Traits;
typedef CGAL::AABB_tree<Traits>								Tree;

Mesh positioningMillingTool(Mesh toolGeometry, Point xy, double z, bool saveToolPositions, std::filesystem::path out);
Mesh positioningMillingTool(std::ofstream& debuger, Mesh toolGeometry, Point_3 point);
bool checkPosition(std::ofstream& debuger, Mesh inputGeometry, Mesh toolgeometry, double tooldiameter, Point_3& checkPoint, bool saveIntersections, std::filesystem::path out);
bool computeIntersection(std::ofstream& debuger, Mesh inputGeometry, Mesh toolgeometry, double tooldiameter, Point_3& checkPoint);
//Point_3 checkPosition(Mesh inputGeometry, Mesh toolgeometry, Point_3 checkPoint, bool saveIntersections, std::filesystem::path out);
