#include<bits/stdc++.h>
#define  READ_BY_TYPE(a)  (char *)a, sizeof(a)    
using namespace std;
struct BMPHeader{
  //Bitmap-File-Header
	unsigned char file_type[2];
	unsigned int file_size;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int offset_data;
  //BITMAPINFOHEADER
	unsigned int size;
	int width;
	int height;
	unsigned short planes;
	unsigned short bit_count;
	unsigned int compression_type;
	unsigned int image_size;
	int x_res;
	int y_res;
	unsigned int colors_used;
	unsigned int colors_imp;
};

/// Color table for saving grayscale image
struct color_table{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char reserved;
};

/** 
 * The BitMap class
 *
 * A class to load, store, write, and display header information
 * of .bmp files
 *
 * Public Member Functions:
 *   - read: Reads pixel values and header information from .bmp file
 *   - save: Writes the current bitmap image to a .bmp file
 *   - display_header_information: Prints the header information in a
 *         structured way
 *
 * Public Member Variables:
 *   - bm_head: Structure to store the header information
 *   - bm_pixelValues: A 3D array of pixel values of the loaded
 *         .bmp image
 */
class BitMap
{
	private:
		string bm_filename;
		BMPHeader bm_head;
		unsigned char*** bm_pixelValues;
    template<typename T>
    void extract(T& to_store ,unsigned char * &header_ptr){
      to_store = *(T*)header_ptr;
      header_ptr += sizeof(T);
    }
    template<typename T>
    void write_it(T to_write, ofstream& bm_stream){
      bm_stream.write((char *)&to_write, sizeof(T));
    }
    template<typename T>
    T*** get3Dmat(int row, int col, int depth) {
      T*** mat = new T** [row];
      for (int i = 0; i < row; i++){
        mat[i] = new T* [col];
        for (int j = 0; j < col; j++)
          mat[i][j] = new T [depth];
			}
      return mat;
    }
	public:
    //constructor
    BitMap(){}
    BitMap(string filename){
      read(filename);
    }
		/**
		 * A function to read the pixel values and header information
		 * of the specified .bmp file
		 *
		 * @param filename: Path to the .bmp file to be read
		 */
		void read(string filename){
			// Store the filename for future reference
			bm_filename = filename;

			// Open new binary stream from the input file
			ifstream bm_stream(filename, ios::binary);

			// Store the first 54 bytes (header)
			unsigned char bm_header[54];

			bm_stream.read((char *)bm_head.file_type, 2);
			// Add '\0' to avoid printing garbage values
			bm_head.file_type[2] = 0;
      bm_stream.read((char *)bm_header,52);
      unsigned char * header_ptr = bm_header; 

			// Store the appropriate fields of file header
			extract(bm_head.file_size,header_ptr);
      extract(bm_head.reserved1,header_ptr);
      extract(bm_head.reserved2,header_ptr);
      extract(bm_head.offset_data,header_ptr);
      extract(bm_head.size,header_ptr);
      extract(bm_head.width,header_ptr);
      extract(bm_head.height,header_ptr);
      extract(bm_head.planes,header_ptr);
      extract(bm_head.bit_count,header_ptr);
      extract(bm_head.compression_type,header_ptr);
      extract(bm_head.image_size,header_ptr);
      extract(bm_head.x_res,header_ptr);
      extract(bm_head.y_res,header_ptr);
      extract(bm_head.colors_used,header_ptr);
      extract(bm_head.colors_imp,header_ptr);

			// This block is for bit-width of 24, i.e., color images
			if(bm_head.bit_count == 24){
				// Compute size of data stream in bytes, including padding
				int dataSize = ((bm_head.width * 3 + 3) & (~3)) * bm_head.height;
        char* img = new char[dataSize];
				// Read the pixel values into the array
				bm_stream.read(img, dataSize);

				// Allocate a 3D array to store the pixel values
        bm_pixelValues = get3Dmat<unsigned char>(bm_head.height,bm_head.width,3);

				// Copy pixel values to the 3D array in RGB ordering and flipping vertical axis
				for (int i = 0; i < bm_head.height; i++){
					for (int j = 0; j < bm_head.width; j++){
						unsigned char a = img[(bm_head.width*3*i)+j*3] & 0xff;
						bm_pixelValues[bm_head.height-1-i][j][2] = a;
						a = img[(bm_head.width*3*i)+j*3+1] & 0xff;
						bm_pixelValues[bm_head.height-1-i][j][1] = a;
						a = img[(bm_head.width*3*i)+j*3+2] & 0xff;
						bm_pixelValues[bm_head.height-1-i][j][0] = a;	
					}
				}
			}

			// Close the input stream
			bm_stream.close();
		}

		/**
		 * A function to save the pixel values and header information
		 * to the specified file
		 *
		 * @param filename: Path to the file to be saved
		 */
		int save(const char* filename){
			// Open new output binary stream
			ofstream bm_stream(filename, ios::binary);

			// Write the header information
			bm_stream << bm_head.file_type[0];
			bm_stream << bm_head.file_type[1];
      write_it(bm_head.file_size, bm_stream);
      write_it(bm_head.reserved1,bm_stream);
      write_it(bm_head.reserved2,bm_stream);
      write_it(bm_head.offset_data,bm_stream);
      write_it(bm_head.size,bm_stream);
      write_it(bm_head.width,bm_stream);
      write_it(bm_head.height,bm_stream);
      write_it(bm_head.planes,bm_stream);
      write_it(bm_head.bit_count,bm_stream);
      write_it(bm_head.compression_type,bm_stream);
      write_it(bm_head.image_size,bm_stream);
      write_it(bm_head.x_res,bm_stream);
      write_it(bm_head.y_res,bm_stream);
      write_it(bm_head.colors_used,bm_stream);
      write_it(bm_head.colors_imp,bm_stream);

			// Write the pixel values to file for color images
			if (bm_head.bit_count == 24){
				int pad;
				for (int i = 0; i < bm_head.height; i++){
					for (int j = 0; j < bm_head.width; j++){
						bm_stream << bm_pixelValues[bm_head.height-1-i][j][2];
						bm_stream << bm_pixelValues[bm_head.height-1-i][j][1];
						bm_stream << bm_pixelValues[bm_head.height-1-i][j][0];
					}
					// Compute the number of padding bytes necessary
					pad = (4 - (bm_head.width*3) % 4) % 4;
					// Write zeros for padding
					for (int i = 0; i < pad; i++)
						bm_stream << 0x00;
				}
			}

			// Write the pixel values to file for grayscale images
			if (bm_head.bit_count == 8){
				// Define the color table
				color_table clr_tbl[256];

				// Populate the color table with corresponding RGB values
				for (int i = 0; i < 256; i++){
					clr_tbl[i].B = i;
					clr_tbl[i].G = i;
					clr_tbl[i].R = i;
				}

				// Write the color table to file
				bm_stream.write((char*)(&clr_tbl), 256*sizeof(color_table));

				// Write the pixel values to file
				for (int i = 0; i < bm_head.height; i++)
					for (int j = 0; j < bm_head.width; j++){
						bm_stream << bm_pixelValues[bm_head.height-1-i][j][0];
					}
			}

			// Close the output stream
			bm_stream.close();

			// Return 1 on success
			return 1;
		}
    /**
 * A function to flip the image diagonally
 *
 * @param inp: Input bitmap
 * @result out: The output bitmap object
 */
  BitMap flip_bitmap(){
    // Create new empty bitmap for storing output
    BitMap out;

    // Copy the header information from input
    out.bm_head = bm_head;

    // Flip the height and width
    int temp_h = out.bm_head.height;
    out.bm_head.height = out.bm_head.width;
    out.bm_head.width = temp_h;

    // Allocate memory to pixelValues
    out.bm_pixelValues = get3Dmat<unsigned char>(out.bm_head.height, out.bm_head.width, 3);

    // Flip and copy pixel values
    for (int i = 0; i < out.bm_head.height; i++)
      for (int j = 0; j < out.bm_head.width; j++)
        for (int k = 0; k < 3; k++)
          out.bm_pixelValues[i][j][k] = bm_pixelValues[out.bm_head.width - 1 - j][out.bm_head.height - 1 - i][k];

    // Return the flipped image
    return out;
  }

  BitMap bgr_to_gray(string mode){
    // Create new empty bitmap for storing output
    BitMap out;

    // Copy the header information from input
    out.bm_head = bm_head;

    // Change bit-width of pixels
    out.bm_head.bit_count = 8;

    // Modify file size
    out.bm_head.file_size = (out.bm_head.height * out.bm_head.width) + sizeof(BMPHeader) + 256*sizeof(color_table);
    
    // Shift offset
    out.bm_head.offset_data += 256 * sizeof(color_table);
    
    // Modify image size
    out.bm_head.image_size = out.bm_head.height * out.bm_head.width;

    // Allocate memory to pixels
    out.bm_pixelValues = get3Dmat<unsigned char>(out.bm_head.height,out.bm_head.width,1);
    float value;

    // Compute grayscale values according to mode and assign them to out
    for (int i = 0; i < out.bm_head.height; i++)
      for (int j = 0; j < out.bm_head.width; j++){
        if (mode == "avg")
          value = (int(bm_pixelValues[i][j][0]) + int(bm_pixelValues[i][j][1]) + int(bm_pixelValues[i][j][2])) / 3.0;
        
        if (mode == "min"){
          value = 255;
          for (int k = 0; k < 3; k++)
            if (int(bm_pixelValues[i][j][k]) < value)
              value = int(bm_pixelValues[i][j][k]);
        }
        
        if (mode == "max"){
          value = 0;
          for (int k = 0; k < 3; k++)
            if (int(bm_pixelValues[i][j][k]) > value)
              value = int(bm_pixelValues[i][j][k]);
        }

        out.bm_pixelValues[i][j][0] = int(value);
      }

    // Return the grayscale image
    return out;
  }
		/// Function to display header information in a structured manner
		void display_header_information(){
			cout << endl << "HEADER INFORMATION ";
			// Display the filename, if available
			if (bm_filename != "")
				cout << "for file '" << bm_filename << "'";
			cout << endl;
			// Separator
			cout << "__________________________________________________" << endl; 
			// Header information
			cout << "File Type: " << "\t\t\t | " << bm_head.file_type << "\t\t |" << endl;
			cout << "File Size (in bytes): " << "\t\t | " << bm_head.file_size << "\t |" << endl;
			cout << "Reserved 1: " << "\t\t\t | " << bm_head.reserved1 << "\t\t |" << endl;
			cout << "Reserved 2: " << "\t\t\t | " << bm_head.reserved2 << "\t\t |" << endl;
			cout << "Data Offset: " << "\t\t\t | " << bm_head.offset_data << "\t\t |" << endl;
			cout << "Header Size (in bytes): " << "\t | " << bm_head.size << "\t\t |" << endl;
			cout << "Image Width: " << "\t\t\t | " << bm_head.width << "\t\t |" << endl;
			cout << "Image Height: " << "\t\t\t | " << bm_head.height << "\t\t |" << endl;
			cout << "Number of Planes: " << "\t\t | " << bm_head.planes << "\t\t |" << endl;
			cout << "Bits per Pixel: " << "\t\t | " << bm_head.bit_count << "\t\t |" << endl;
			cout << "Compression Type: " << "\t\t | " << bm_head.compression_type << "\t\t |" << endl;
			cout << "Image Size (in bytes): " << "\t\t | " << bm_head.image_size << "\t |" << endl;
			cout << "Resolution in x-direction: " << "\t | " << bm_head.x_res << "\t\t |" << endl;
			cout << "Resolution in y-direction: " << "\t | " << bm_head.y_res << "\t\t |" << endl;
			cout << "Colors Used: " << "\t\t\t | " << bm_head.colors_used << "\t\t |" << endl;
			cout << "Colors Important: " << "\t\t | " << bm_head.colors_imp << "\t\t |" << endl;
		}
};




/**
 * A function to convert color image to grayscale
 *
 * @param inp: Input bitmap
 * @param mode: Which method to follow for conversion. Should be one of the following:
 *                  - "avg"
 *					- "min"
 *					- "max"	
 * @result out: The output bitmap object
 */

int main(int argc, char** argv)
{
	// Check if all the command line arguments are provided
	if (argc != 6){
		cout << "Please provide the path to ALL of the following, in that order: " << endl;
		cout << endl;
		cout << "- Input .bmp file" << endl;
		cout << "- Destination of flipped image" << endl;
		cout << "- Destination of grayscale image with averaging" << endl;
		cout << "- Destination of grayscale image with minimum" << endl;
		cout << "- Destination of grayscale image with maximum" << endl;
		cout << endl << "Exiting" << endl;
		return -1;
	}

	// Define and read the .bmp file
	BitMap lena;
	lena.read(argv[1]);

	// Display the header information
	lena.display_header_information();

	// Flip and save the image
	lena.flip_bitmap().save(argv[2]);

	// Convert the image to grayscale and save it
	lena.bgr_to_gray("avg").save(argv[3]);
	lena.bgr_to_gray("min").save(argv[4]);
	lena.bgr_to_gray("max").save(argv[5]);
	return 0;
}