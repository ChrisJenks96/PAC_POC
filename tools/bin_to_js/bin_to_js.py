import sys

#program params - [width] [height] [bin_name] [output_name]
#e.g. 16 16 PLAYER.BIN PLAYER.JS

#array for storing all the bytes loaded in from the bin file
bin_arr = [];

def bin_load(arg1, arg2, arg3):
	#e.g (16 * 16) 256 / 2 (div 2 is because we store 2 colours per byte)
	elem_size = (arg1 * arg2) >> 1;
	f_bin = open(arg3, "rb");
	#loop through the width, height
	for x in range(0, elem_size):
		#remove unicode encoding and make the byte hex
		bin_num = f_bin.read(1).hex();
		#write the leading 0x for hex with the values in uppercase
		#append to the array to access in the writing function
		bin_arr.append("0x" + bin_num.upper());
	f_bin.close();
	return elem_size;

def bin_to_js_write(arg4, arg3, size):
	f_js = open(arg4, "w");
	#remove the .bin off the end of the str
	new_arg3 = "";
	for c in range(0, len(arg3) - 4):
		new_arg3 += arg3[c];
	f_js.write("var " + new_arg3 + "_buffer_size = " + str(size) + ";\n");
	f_js.write("var " + new_arg3 + "_buffer = [");
	#size is the same as len(bin_arr)
	#write each byte to the file with correct formatting
	for x in range(0, len(bin_arr)):
		f_js.write(bin_arr[x]);
		if (x < len(bin_arr)-1):
			f_js.write(", ");
	f_js.write("];");
	f_js.close();

#main entry point function for execution
def main():
	elem_size = bin_load(int(sys.argv[1]), int(sys.argv[2]), sys.argv[3]);
	bin_to_js_write(sys.argv[4], sys.argv[3], elem_size);

main();
exit(); 

