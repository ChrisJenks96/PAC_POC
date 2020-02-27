#include <stdio.h>
#include <malloc.h>
#include <string>

/*
	X.JS
	var X_buffer = [254, 245, 245...];
*/

static inline void write_bin_to_js(char* buffer, char* argv2, int size)
{
	FILE* f_out = fopen(argv2, "wb");
	int c = 0;
	// WRITE THE BUFFER SIZE FIRST THEN THE BUFFER
	//var
	fwrite("var ", 4, 1, f_out);
	//var example (-3 because we dont want the JS extension)
	fwrite(argv2, strlen(argv2) - 3, 1, f_out);
	fwrite("_buffer_size = ", 15, 1, f_out);
	fprintf(f_out, "%i", size);
	fwrite(";\n", 2, 1, f_out);

	fwrite("var ", 4, 1, f_out);
	//var example (-3 because we dont want the JS extension)
	fwrite(argv2, strlen(argv2) - 3, 1, f_out);
	//var example_buffer = [
	fwrite("_buffer = [", 11, 1, f_out);
	while (c < size)
	{
		char buf[10];
		//some values printf 0xFFFFFFXX
		sprintf(&buf[0], "%02X", buffer[c]);
		fwrite("0x", 2, 1, f_out);
		fwrite(&buf[strlen(buf)-2], 2, 1, f_out);
		//dont write the last coma out, no next element
		if (c != size-1)
			fwrite(", ", 2, 1, f_out);
		c++;
	}

	fwrite("];", 2, 1, f_out);
	fclose(f_out);
	return;
}

int main(int argc, char** argv)
{
	//make sure we have 
	if (argc > 1)
	{
		FILE* f = fopen(argv[1], "rb");
		if (!f)
			return -1;
		else
		{
			//get file size and return to start of file
			int s = ftell(f), s2;
			char* buffer;
			fseek(f, 0, SEEK_END);
			s2 = ftell(f) - s;
			fseek(f, 0, SEEK_SET);
			//alloc size of buffer (char = 1 byte) * filesize
			buffer = (char*)malloc(s2);
			fread(&buffer[0], s2, 1, f);
			write_bin_to_js(buffer, argv[2], s2);
			fclose(f);
		}
	}

	return 0;
}