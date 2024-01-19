$fn = 300;

top_plate = "./kicad/dxf/kicad_top_plate-Edge_Cuts.dxf";
outline = "./kicad/dxf/kicad_top_plate-Margin.dxf";
tolerance = "./kicad/dxf/kicad_top_plate-User_1.dxf";
height = 1.5;
outline_height = 10;

module dxf(file, height) {
	linear_extrude(height = height)
		import(file = file);
}

dxf(top_plate, height);
difference() {
	// add border
	dxf(outline, outline_height);

	// extrude top plate and remove from border
	dxf(top_plate, outline_height);

	// get thinner border, extrude and remove from border
	translate([0,0,height])
		dxf(tolerance, outline_height);

	// cutout for controller and display
	translate([-63,-20,5])
		cube([34,67,10]);
}
