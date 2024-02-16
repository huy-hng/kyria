$fn = 300;

top_plate = "./kicad/dxf/kicad_top_plate-Edge_Cuts.dxf";
outline = "./kicad/dxf/kicad_top_plate-Margin.dxf";
tolerance = "./kicad/dxf/kicad_top_plate-User_1.dxf";
thickness = 1.5;
outline_height = 10;

module dxf(file, thickness) {
	linear_extrude(height = thickness)
		import(file = file);
}

module v1() {
	difference() {
		// add border
		dxf(outline, outline_height);

		// extrude top plate and remove from border
		dxf(top_plate, outline_height);

		// get thinner border, extrude and remove from border
		translate([0,0,thickness])
			dxf(tolerance, outline_height);

		// cutout for controller and display
		translate([-63,-20,5])
			cube([34,67,10]);
	}
}

module v2() {
	difference() {
		// add border

		dxf(outline, outline_height);

		// extrude top plate and remove from border
		// dxf(top_plate, outline_height);

		// // get thinner border, extrude and remove from border
		// translate([0,0,thickness])
		// 	dxf(tolerance, outline_height);

		// // cutout for controller and display
		// translate([-63,-20,5])
		// 	cube([34,67,10]);
	}
}

module main() {
	dxf(top_plate, thickness);
	#v2();
}

render()
main();
