bin_to_js converts the 4 bits per colour binary file generate by the bmp_dos_strip
into a javascript include file containing raw pixel bytes.

- Written in Python 3.8.2
- Program input [width] [height] [binary name] [output name]
- width and height must be divisable by 8 and the same 