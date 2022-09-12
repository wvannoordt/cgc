#include "PTL.h"
#include "geolytical.h"

struct chan_spec_t
{
	double xmin, xmax, ymin, ymax, zmin, zmax;
	double buffer;
	int nx, nz;
	bool is3d;
	std::string filename;
	void assign(PTL::PropertyTree& input)
	{
		input["xmin"].MapTo(&xmin)         = new PTL::PTLDouble(0.0,       "Lower streamwise extent bound");
		input["xmax"].MapTo(&xmax)         = new PTL::PTLDouble(12.8,      "Upper streamwise extent bound");
		input["ymin"].MapTo(&ymin)         = new PTL::PTLDouble(-1.0,      "Lower wall-normal extent bound");
		input["ymax"].MapTo(&ymax)         = new PTL::PTLDouble( 1.0,      "Upper wall-normal extent bound");
		input["zmin"].MapTo(&zmin)         = new PTL::PTLDouble(0.0,       "Lower spanwise extent bound");
		input["zmax"].MapTo(&zmax)         = new PTL::PTLDouble(6.28,      "Upper spanwise extent bound");
		input["buffer"].MapTo(&buffer)     = new PTL::PTLDouble(0.2,       "Buffer size");
		input["nx"].MapTo(&nx)             = new PTL::PTLInteger(512,      "Number of cells in the streamwise direction");
		input["nz"].MapTo(&nz)             = new PTL::PTLInteger(64,       "Number of cells in the spanwise direction");
		input["is3d"].MapTo(&is3d)         = new PTL::PTLBoolean(true,     "Output a 3D geometry");
		input["filename"].MapTo(&filename) = new PTL::PTLString("out.vtk", "Output file name");
	}
};

void help()
{
	std::cout << "Usage:\n";
	std::cout << ">>Initialization: cgc --init        (generates an input file with default options)\n";
	std::cout << ">>Help:           cgc --help        (shows this help message)\n";
	std::cout << ">>General use:    cgc [input file]  (reads [input file] and generates the channel geometry)";
	std::cout << std::endl;
}

void create_geom_2d(const chan_spec_t& spec, const bbox& b1)
{
	geolytical::FlatLine f1(spec.nx, b1);
	geolytical::FlatLine f2(spec.nx, b1);
	geolytical::v3d<double> axis(0.0,0.0,1.0);
	geolytical::v3d<double> point(0.5*(spec.xmin+spec.xmax), 0.5*(spec.ymin+spec.ymax), 0.0);
	double theta = geolytical::pi;
	f2.Rotate(axis, point, theta);
	f1 += f2;
	f1.OutputToVtk(spec.filename);
}

void create_geom_3d(const chan_spec_t& spec, const bbox& b1)
{
	geolytical::FlatPlate f1(spec.nx, spec.nz, b1);
	geolytical::FlatPlate f2(spec.nx, spec.nz, b1);
	geolytical::v3d<double> axis(0.0,0.0,1.0);
	geolytical::v3d<double> point(0.5*(spec.xmin+spec.xmax), 0.5*(spec.ymin+spec.ymax), 0.0);
	double theta = geolytical::pi;
	f2.Rotate(axis, point, theta);
	f1 += f2;
	f1.OutputToVtk(spec.filename);
}

int main(int argc, char** argv)
{
	chan_spec_t spec;
	PTL::PropertyTree input;
	spec.assign(input);
	std::vector<std::string> args;
	for (int i = 0; i < argc; ++i) args.push_back(std::string(argv[i]));
	if (args.size()==1 || args.size()>2)
	{
		help();
		return 1;
	}
	bool init = false;
	std::string input_filename;
	for (const auto& p: args)
	{
		if (p=="--init")
		{
			input.CreateDefaultValuesFile("input.ptl");
			return 0;
		}
		if (p=="--help")
		{
			help();
			return 1;
		}
		input_filename = p;
	}
	std::cout << "Reading " << input_filename << std::endl;
	input.Read(input_filename);
	input.StrictParse();
	
	
	bbox b1;
	b1.xmin = spec.xmin - spec.buffer;
	b1.xmax = spec.xmax + spec.buffer;
	b1.zmin = spec.zmin - spec.buffer;
	b1.zmax = spec.zmax + spec.buffer;
	b1.ymin = spec.ymin - spec.buffer;
	b1.ymax = spec.ymin;
	
	if (spec.is3d)
	{
		create_geom_3d(spec, b1);
	}
	else
	{
		create_geom_2d(spec, b1);
	}
	
	return 0;
}
