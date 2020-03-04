import sys

#program params - [bmp_name] [output_name]
#e.g. 16 16 PLAYER.BIN PLAYER.JS

#original array from the BMP
pix = [];
#the new rectified array to push out to BIN
final_pix = [];

#we must extract and swap from XBGR to XRGB with the 4 bit colour
#X = intensity, this colour is taken from the 16 color CGA palette
#0000 XBGR -> 0000 XRGB
def byte_swap_rb(col):
	#use bitwise and to extract singular bits
	x = col & 0x08; #0000 X000
	r = col & 0x01; #0000 000R
	#if r isn't 0 (if it is, we don't need to move it), then bit shift left twice 
	#0000 0R00
	if (r != 0):
		r <<= 2;
	g = col & 0x02; #0000 00G0
	b = col & 0x04; #0000 0B00
	#same as r, we bit shift right twice
	#0000 000B
	if (b != 0):
		b >>= 2;
	return x + r + g + b;

#col1 = 0000 XXXX col2 = 0000 YYYY
#return 0000 XXXX + (0000 YYYY << 4 = YYYY 0000) = YYYY 0000
def merge_4bit_col(col1, col2):
	return col2+(col1 << 4);

def bmp_load(filename, output):
	f_bmp = open(filename, "rb");
	#seek 10 bytes from a relative seek position
	f_bmp.seek(10);
	#read 4 bytes (C unsigned int)
	pix_offset = int(f_bmp.read(1).hex(), 16);
	#offset to the width and height and save them
	f_bmp.seek(18);
	width = int(f_bmp.read(1).hex(), 16);
	print("width: " + str(width));
	f_bmp.seek(22);
	height = int(f_bmp.read(1).hex(), 16);
	print("height: " + str(height));
	#python 3 only does absolute, so minus the 10 byte seek at the start
	#offset file pointer pixel offset absolute
	f_bmp.seek(pix_offset, 0);
	pix_size = (width * height) >> 1;
	print("pix_size: " + str(pix_size));
	pix = f_bmp.read(pix_size);

	#loop through the pixels and correct them BGR -> RGB
	for c in range(0, len(pix)):
		#e.g. XXXX YYYY -> YYYY 0000 -> 0000 YYYY
		first_col = (pix[c] << 4) >> 4;
		first_col = byte_swap_rb(first_col);
		#e.g. XXXX YYYY -> 0000 XXXX
		second_col = pix[c] >> 4;
		second_col = byte_swap_rb(second_col);
		#append to array and write directly to file
		final_pix.append(merge_4bit_col(second_col, first_col));

	open(output, "wb").write(bytearray(final_pix));
	f_bmp.close();

#main entry point function for execution
def main():
	bmp_load(sys.argv[1], sys.argv[2]);

main();
exit(); 

