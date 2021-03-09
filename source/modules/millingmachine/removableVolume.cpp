#include "removableVolume.h"

std::vector<Mesh> computeRemovableVolumes(std::map<std::string, std::vector<Polygon_with_holes_ptr_vector>> offsets, Mesh workingPieceGeometry, Mesh toolGeometry, Mesh convexHull) {
	// Transformieren zu AABB tree f�r Intersection Computation
	Tree workingPieceTree(faces(workingPieceGeometry).first, faces(workingPieceGeometry).second, workingPieceGeometry);
	std::map<double, Mesh> result; // Zwischenspeicher f�r Planes 
	std::vector<Mesh> output;
	// Ermitteln der Z-Koordinate -> Auf Pr�fpunkt wird Ray geschossen und Intersection Punkt ermittelt (Sichtbarkeitsanalyse)
	for (std::pair<std::string, std::vector<Polygon_with_holes_ptr_vector>> offset_polygons : offsets) {
		double z = std::stod(offset_polygons.first);
		std::vector<Polygon_with_holes_ptr_vector> off_poly = offset_polygons.second;
		for each (Polygon_with_holes_ptr_vector poly_ptr in off_poly)
		{
			for (int i = 0; i < poly_ptr.size(); ++i) {
				Polygon_with_holes poly = *poly_ptr[i].get();
				std::vector<Point_3> points;
				bool valid = true;
				for (std::vector<Point>::const_iterator it = poly.outer_boundary().begin(); it != poly.outer_boundary().end(); ++it) {
					Point xycord = *it;
					double x = xycord.exact().x().to_double();
					double y = xycord.exact().y().to_double();
					Ray ray(Point_3(x, y, 2), Point_3(x, y, 0));
					Ray_intersection intersection = workingPieceTree.first_intersection(ray);
					double z_intersection;
					if (intersection) {
						if (boost::get<Point_3>(&(intersection->first))) {
							const Point_3* p = boost::get<Point_3>(&(intersection->first));
							z_intersection = p->exact().z().to_double();
							Point_3 checkPoint = Point_3(x, y, z_intersection);
							if (z < z_intersection) valid = false; // Falls z-Position nicht sichtbar, nicht aufnehmen
							Mesh toolTemp = positioningMillingTool(toolGeometry, Point(x, y), z_intersection);
							checkPosition(workingPieceGeometry, toolTemp, checkPoint);
							points.push_back(checkPoint);
						}
					};
				};
				if (valid) {
					Mesh sm;
					CGAL::convex_hull_3(points.begin(), points.end(), sm);
					CGAL::Bbox_3 bboxSm = CGAL::Polygon_mesh_processing::bbox(sm);

					double area = CGAL::Polygon_mesh_processing::area(sm).exact().to_double();
					double res_key = bboxSm.xmax() + bboxSm.xmin() + bboxSm.ymax() + bboxSm.ymin() + bboxSm.zmax() + bboxSm.zmin();

				// TODO: Herausfinden wie ein eindeutiger Key erstellt wird, um Doppelungen zu vermeiden.
					if (result.find(res_key) == result.end()) {
						result.insert(std::pair<double, Mesh>(res_key, sm));
						std::string zPosition = "plane_" + std::to_string(i) + "_(" + std::to_string(res_key) + "_" + std::to_string(std::rand()) + ")";
						std::string eps_name = zPosition + ".off";
						std::ofstream out;
						out.open(eps_name.c_str());
						CGAL::write_off(out, sm);
						output.push_back(computeMinkowskiSum(sm, toolGeometry));
					};
				};
			};
		};
	};
	return output;
};

Mesh computeMinkowskiSum(Mesh plane, Mesh toolGeometry) {
	Nef_polyhedron planeNef(plane);
	Nef_polyhedron toolNef(toolGeometry);
	Nef_polyhedron result = CGAL::minkowski_sum_3(planeNef, toolNef);
	Mesh output;
	CGAL::convert_nef_polyhedron_to_polygon_mesh(result, output);
	std::string fileName = "minkowskisum" + std::to_string(rand()) + ".off";
	std::ofstream out;
	out.open(fileName.c_str());
	out << output;
	out.close();
	return output;
};


Mesh computeLeftover(std::vector<Mesh> minkowskiSums, Mesh convexHull){
	Nef_polyhedron convexHullNef(convexHull);
	for (int i = 0; i < minkowskiSums.size(); ++i) {
		Nef_polyhedron msNef(minkowskiSums[i]);
		convexHullNef = convexHullNef - msNef;
	};
	Mesh output;
	CGAL::convert_nef_polyhedron_to_polygon_mesh(convexHullNef, output);
	std::string filename = "FinalGeometry.off";
	std::ofstream out;
	out.open(filename.c_str());
	out << output;
	out.close();
	return output;
}
