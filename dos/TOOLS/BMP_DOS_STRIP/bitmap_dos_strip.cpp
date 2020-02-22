#include <stdio.h>
#include <stdint.h>

static uint8_t SWAP_RB(uint8_t col)
{
	//0000 XBGR
	uint8_t x = col & 0x08; //0000 X000
	uint8_t r = col & 0x01; //0000 000R
	r = r != 0 ? r << 2 : 0; //0000 0R00
	uint8_t g = col & 0x02; //0000 00G0
	uint8_t b = col & 0x04; //0000 0B00
	b = b != 0 ? b >> 2 : 0; //0000 000B
	return (x + r + g + b);
}

static uint8_t MERGE_4BIT_COL(uint8_t col1, uint8_t col2)
{
	//col2 0000 XXXX
	//col1 0000 YYYY
	return col2 + (col1 << 4); //YYYY XXXX
}

int main(int argc, char** argv)
{
	FILE* f, *fo;
	uint8_t pix[128] = {0};
	//uint8_t pix2[256] = {0};
	int i, x, y, pix_id = 0;
	unsigned int pix_offset;
	f = fopen(argv[1], "rb");
	if (f)
	{
		fseek(f, 10, SEEK_CUR);
		fread(&pix_offset, 4, 1, f);
		//find the pixel offset, forget any palette
		fseek(f, pix_offset, SEEK_SET);
		fread(&pix[0], 128, 1, f);
		fclose(f);
		fo = fopen(argv[2], "wb");
		for (i = 0; i <	128; i++)
		{
			//8 bits contain 2 4 bit (16 val) colours
			//we need to split and swap R & B
			uint8_t first_col = pix[i] << 4;
			first_col = first_col >> 4;
			//swap the R and B bits in the 4 bit col
			first_col = SWAP_RB(first_col);
			uint8_t second_col = pix[i] >> 4;
			second_col = SWAP_RB(second_col);

			//This is experimental 4 bit writing
			pix[pix_id] = MERGE_4BIT_COL(second_col, first_col);
			pix_id++;
		}

		//4 bit, upside down, remember to flip image
		fwrite(&pix[0], 128, 1, fo);
		fclose(fo);
	}

	return 0;
}