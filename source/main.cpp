#include <iostream>
#include <chrono>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include "modules/io/fileio.h"
#include "modules/io/machineloader.h"
#include "modules/io/config.h"
#include "modules/millingmachine/millingMachine.h"
#include "modules/rawmaterial/rawMaterial.h"
#include "modules/rawmaterial/rawMaterialLoader.h"
#include "modules/rawmaterial/rawMaterialSelection.h"

typedef CGAL::Exact_predicates_exact_constructions_kernel   K;
typedef K::Point_3                                          Point_3;
typedef CGAL::Surface_mesh<Point_3>							Mesh;

int main(int argc, char* argv[])
{
	auto start = std::chrono::high_resolution_clock::now();

	Config config;

	const char* filename = (argc > 1) ? argv[1] : "testbauteil_nachz_ausgerichtet_negativz.off";
	std::cout << filename << std::endl;

	Mesh inputMesh = importerOff(filename, config.getPrintImportInformation());

	std::vector<RawMaterial> rawMaterials = loadRawMaterial(config);
	if (rawMaterials.empty())
	{
		std::cout << "No raw material found " << std::endl;
		std::cout << "check folder //data//rawmaterial" << std::endl;
		return 0;
	};

	RawMaterial selectedRawMaterial = selectRawMaterial(rawMaterials, inputMesh);

	std::vector<MillingMachine> millingmachines = loadMachines(config);
	if (millingmachines.empty())
	{
		std::cout << "No milling machine found " << std::endl;
		std::cout << "check folder //data//machines//millingmachines" << std::endl;
		return 0;
	};

	for (MillingMachine mm : millingmachines)
	{
		mm.checkManufacturability(inputMesh, selectedRawMaterial, config);
	};

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Time taken by program: " << duration.count() << " seconds" << std::endl;

	return 0;
}