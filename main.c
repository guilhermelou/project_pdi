#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

enum read_state{
    MAGIC_NMB = 0,
    WIDTH,
    HEIGHT,
    MAX_COLOR_RANGE,
    PIXEL
};
typedef enum read_state READ_STATE;

typedef struct PGM {
    char magic_nmb[3];
    int width;
    int height;
    int color_range;
    int **matrix;
} PGM;

typedef struct PPM {
    char magic_nmb[3];
    int width;
    int height;
    int color_range;
    int ***matrix;
} PPM;

// Function to build a PGM from a char buffer.
void PGMConstFromBuffer(PGM *image, char *buffer);

// Function to build a PGM from a file
void PGMConstFromFile(PGM *image, char *filename);

// Function to alloc a new matrix based on width and height
void PGMConstMatrixFromWidthHeight(PGM *image);

// Function to alloc a PGM, allocatting the matrix by width and height
PGM * PGMConst(char *magic_nmb, int width, int height, int color_range);

// Function to clone a image allocatting a new one
PGM * PGMCloneImage(PGM *image);

// Function to alloc and return a filter c matrix
int ** PGMConstFilterC();

// Function to alloc and return a filter d matrix
int ** PGMConstFilterD();

// Function to clone a matrix allocatting a new one
void PGMCloneMatrix(PGM *dest, PGM *source);

// Function to print image on terminal
void PGMPrint(PGM *image);

// Function to print image on file
void PGMPrintToFile(PGM *image, char *filename);

// Function to apply the bright effect with sum
void PGMBrighterSum(PGM *image, int bright_sum);

// Function to apply the bright effect with multiply
void PGMBrighterMult(PGM *image, float bright_mult);

// Function to apply the dark effect with subtraction
void PGMDarkerSub(PGM *image, int dark_sub);

// Function to apply the dark effect with division
void PGMDarkerDiv(PGM *image, float dark_div);

// Function to subtract the image by the minus_image. The result will be on
// the image
void PGMSubImages(PGM *image, PGM *minus_image);

// Function to sum the image by the sum_image. The result will be on
// the image
void PGMSumImages(PGM *image, PGM *sum_image);

// Rotate a image in 90 degrees
void PGMRotate90(PGM *image);

// Function to apply the dark effect with division
void PGMRotate(PGM *image, int degree);

// Function to apply the negative
void PGMNegative(PGM *image);

// Function to apply the average
void PGMAverage(PGM *image, int row_col_near);

// Function to get the average of the region of rows and cols near to [i][j]
int PGMLocalAverage(PGM *image, int i, int j, int row_col_near);

// Function to get the standard deviation of a local region of rows and cols
// near to [i][j]
double PGMLocalStandardDeviation(PGM *image, int i, int j, int row_col_near);

// Function to get the global average of image
int PGMGlobalAverage(PGM *image);

// Function to get the global standard deviation of image
double PGMGlobalStandardDeviation(PGM *image);

// Function to apply the average
void PGMLaplace(PGM *image, int **matrix_filter);

// Function to apply the mirrored effect horizontally
void PGMMirroredH(PGM *image);

// Function to apply the mirrored effect vertically
void PGMMirroredV(PGM *image);

// Function to change the color range to a new one
void PGMChangeColorRange(PGM *image, int new_range);

// Function to return a histogram
int * PGMHistogram(PGM *image);

// Function to save the histogram of an image into a file
void PGMHistogramToFileParam(PGM *image, int* histogram, char *file_name);

// Function to save the histogram of an image into a file
void PGMHistogramToFile(PGM *image, char *file_name);

// Function to get the histogram equalized
int * PGMHistogramEqualized(PGM *image);

// Function to apply a determinated histogram globally
void PGMHistogramGlobal(PGM *image, int * histogram);

// Function to apply a histogram equalized on image globally
void PGMEqualizedHistogramGlobal(PGM *image);

// Function to apply a histogram equalized on image globally
void PGMEqualizedHistogramLocal(PGM *image, int size);

// Function to apply a histogram statistic on image
void PGMHistogramStatistics(
        PGM *image, double E, double k0, double k1, double k2, int row_col_near
    );

// Function to cut part of image into a new one
PGM * PGMCutImage(PGM *image, int ini_x, int ini_y, int width, int height);

// Function to apply the histogram binary
void PGMGraphBinary(PGM *image, int center);

// Function to apply the histogram binary inside and outside a interval [A-B]
void PGMGraphBinaryAB(PGM *image, int a, int b, int color_in, int color_out);

// Function to apply the color inside a interval [A-B]
void PGMGraphAB(PGM *image, int a, int b, int color_in);

// Function to apply the color inside a interval [A-B] by the equation of the
// line AB
// y=ax+b
void PGMGraphLineAB(PGM *image, int a, int b);

// Function to apply the inverse log
void PGMGraphInverseLog(PGM *image);

// Function to apply the gamma pow
void PGMGraphPow(PGM *image, double a);

// Function to amplify the image multiplying the number of pixels
void PGMZoomIn(PGM *image, double factor);

// Function to reduce the image multiplying the number of pixels
void PGMZoomOut(PGM *image, double factor);

// Function to seek for negative values and change their to zero
void PGMNegativeValToZero(PGM *image);

// Function to seek for Too high values and change for the max value
void PGMTooHighToMax(PGM *image);

// Function that seek for both, higher and lower values to stabilize them
void PGMStabilizeValues(PGM* image);

void PGMConstFromBuffer(PGM *image, char *buffer){
    char magic_nmb[3];
    int width = 0, height = 0, row_nmb = 0, col_nmb = 0, color_range = 0;
    int col = 0, row = 0;
    int **matrix;
    bool start_matrix = false, is_comment = false, token_started=false;
    READ_STATE reading = MAGIC_NMB;
    // Iterate over buffer to get the tokens
    // Token var
    char *token = NULL;
    // Token iterator
    int token_i = 0;
    for (int i=0; buffer[i]!= '\0'; i++){
        // Current char
        char c = buffer[i];
        // Some delimiter showed up
        if (c == '\n' || c == ' ' || c == '\t' || c == '#'){
            // The delimiter is a break-line
            if (c == '\n'){
                // if this char is a break-line we need to end the comment
                if (is_comment){
                    is_comment = false;
                }
            }
            else if (c == '#'){
                // Starting the comment
                is_comment = true;
            }
            // Ending the Token
            if (token_started){
                token_started = false;
                // Choosing what to do with a valid token
                switch(reading){
                    // The first stage of reading is for the magic number
                    case MAGIC_NMB:
                        strcpy(image->magic_nmb, token);
                        reading = WIDTH;
                        break;
                    // The second stage of reading is for get the width
                    case WIDTH:
                        image->width = atoi(token);
                        reading = HEIGHT;
                        break;
                    // The third stage of reading is for get the height
                    case HEIGHT:
                        image->height = atoi(token);
                        // Allocating the matrix
                        PGMConstMatrixFromWidthHeight(image);
                        reading = MAX_COLOR_RANGE;
                        break;
                    // The fourth stage of reading is for get the max color
                    // range
                    case MAX_COLOR_RANGE:
                        image->color_range = atoi(token);
                        reading = PIXEL;
                        break;
                    // Filling the matrix is the last stage
                    case PIXEL:
                        if (col>=image->width){
                            col=0;
                            row++;
                        }
                        int value = atoi(token);
                        image->matrix[row][col] = value;
                        col++;
                        break;
                    default:
                        break;
                }
            }
        }
        // No delimiter showed up and assemblying the token
        else{
            // This is part of a valid token outside of a comment
            if (!is_comment){
                // Token is already initiated
                if (token_started){
                    token_i++;
                    // Coppying the current token content into the auxiliar
                    // string
                    char *str_aux = (char *)malloc((token_i+1)+1);
                    strcpy(str_aux, token);
                    // Adding the new char on auxiliar string
                    str_aux[token_i] = c;
                    str_aux[token_i+1] = '\0';
                    // Free the old token memory
                    free(token);
                    // Creating the new token to receive the token +
                    // new char
                    token = (char *)malloc((token_i+1)+1);
                    strcpy(token, str_aux);
                    // Free the auxiliar string
                    free(str_aux);
                }
                // Starting a new token
                else{
                    token_started = true;
                    token_i = 0;
                    token = (char *)malloc((token_i+1)+1);
                    token[token_i] = c;
                    token[token_i+1] = '\0';
                }
            }
        }
    }
}
void PGMConstFromFile(PGM *image, char *filename){
	FILE *file;
	char *buffer = 0;
    long length;
    // Loding file on a char buffer
	file = fopen(filename, "r");
    if (file)
	{
		fseek (file, 0, SEEK_END);
		length = ftell(file);
		fseek (file, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer)
		{
			fread (buffer, 1, length, file);
		}
		fclose (file);
	}
	if (buffer)
	{
        PGMConstFromBuffer(image, buffer);
	}

}

// Function to create a new image with matrix allocated by width and height
PGM * PGMConst(char *magic_nmb, int width, int height, int color_range){
    PGM * image = malloc(sizeof(struct PGM));
    strcpy(image->magic_nmb, magic_nmb);
    image->width = width;
    image->height = height;
    image->color_range = color_range;
    PGMConstMatrixFromWidthHeight(image);
    return image;
}

// Function to allocate a matrix by width and height
void PGMConstMatrixFromWidthHeight(PGM *image){
    // Allocating the matrix with the proper
    // row & col size
    image->matrix = malloc(image->height * sizeof(int *));
    for (int i = 0; i < image->height; i++) {
        image->matrix[i] = malloc(image->width * sizeof(int));
    }
}

// Function to alloc and return a filter c matrix
int ** PGMConstFilterC(){
    int ** filter;
    int size = 3;
    filter = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        filter[i] = malloc(size * sizeof(int));
    }
    filter[0][0] = 0;
    filter[0][1] = -1;
    filter[0][2] = 0;
    filter[1][0] = -1;
    filter[1][1] = 4;
    filter[1][2] = -1;
    filter[2][0] = 0;
    filter[2][1] = -1;
    filter[2][2] = 0;

    return filter;
}

// Function to alloc and return a filter d matrix
int ** PGMConstFilterD(){
    int ** filter;
    int size = 3;
    filter = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        filter[i] = malloc(size * sizeof(int));
    }
    filter[0][0] = -1;
    filter[0][1] = -1;
    filter[0][2] = -1;
    filter[1][0] = -1;
    filter[1][1] = 8;
    filter[1][2] = -1;
    filter[2][0] = -1;
    filter[2][1] = -1;
    filter[2][2] = -1;

    return filter;
}

// Function to clone a image allocatting a new one
PGM * PGMCloneImage(PGM *image){
    PGM *new_image;
    new_image = malloc(sizeof(struct PGM));
    strcpy(new_image->magic_nmb, image->magic_nmb);
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->color_range = image->color_range;
    PGMCloneMatrix(new_image, image);
    return new_image;
}

// Function to clone a matrix allocatting a new one
void PGMCloneMatrix(PGM *dest, PGM *source){
    PGMConstMatrixFromWidthHeight(dest);
    for(int i=0; i<source->height; i++){
        for(int j=0; j<source->width; j++){
            // Adding a bright sum to the present value
            dest->matrix[i][j] = source->matrix[i][j];
        }
    }
}

void PGMPrint(PGM *image){
    char *magic_nmb = image->magic_nmb;
    int col_nmb = image->width, row_nmb = image->height;
    int width = image->width, height = image->height;
    int color_range = image->color_range;
    int **matrix = image->matrix;
	printf("%s\n", magic_nmb);
	printf("%d %d\n", width, height);
	printf("%d\n", color_range);
	for(int i = 0; i < row_nmb; i++) {
		for(int j = 0; j < col_nmb; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}


void PGMPrintToFile(PGM *image, char *filename){
	FILE *file;
    char *magic_nmb = image->magic_nmb;
    int col_nmb = image->width, row_nmb = image->height;
    int width = image->width, height = image->height;
    int color_range = image->color_range;
    int **matrix = image->matrix;
	file = fopen(filename, "w");
	fprintf(file, "%s\n", magic_nmb);
	fprintf(file, "%d %d\n", width, height);
	fprintf(file, "%d\n", color_range);
	for(int i = 0; i < row_nmb; i++) {
		for(int j = 0; j < col_nmb; j++) {
			fprintf(file, "%d ", matrix[i][j]);
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n");
    fclose (file);
}

// Function to apply the bright effect with sum
void PGMBrighterSum(PGM *image, int bright_sum){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Adding a bright sum to the present value
            image->matrix[i][j] = image->matrix[i][j] + bright_sum;
        }
    }
}

// Function to apply the bright effect with multiply
void PGMBrighterMult(PGM *image, float bright_mult){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Multiplying bright with the present value
            image->matrix[i][j] = (int)((float)image->matrix[i][j]*bright_mult);
        }
    }
}
// Function to apply the dark effect with subtraction
void PGMDarkerSub(PGM *image, int dark_sub){
    PGMBrighterSum(image, -dark_sub);
}

// Function to apply the dark effect with division
void PGMDarkerDiv(PGM *image, float dark_div){
    PGMBrighterMult(image, 1/dark_div);
}

// Function to subtract the image by the minus_image. The result will be on
// the image
void PGMSubImages(PGM *image, PGM *minus_image){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // subtract the image value by the minus value
            image->matrix[i][j] = (
                    image->matrix[i][j] -
                    minus_image->matrix[i][j]
                );
        }
    }
}

// Function to sum the image by the sum_image. The result will be on
// the image
void PGMSumImages(PGM *image, PGM *sum_image){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // subtract the image value by the minus value
            image->matrix[i][j] = (
                    image->matrix[i][j] +
                    sum_image->matrix[i][j]
                );
        }
    }
}

// Function to apply the dark effect with division
void PGMRotate(PGM *image, int degree){
    switch(degree){
        case 90:
            PGMRotate90(image);
            break;
        case 180:
            PGMRotate90(image);
            PGMRotate90(image);
            break;
        case -90:
            PGMRotate90(image);
            PGMRotate90(image);
            PGMRotate90(image);
            break;
        default:
            break;
    }
}

void PGMRotate90(PGM *image){
    PGM * new_image;
    new_image = PGMConst(image->magic_nmb, image->height, image->width,
            image->color_range);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // inverting the points
            new_image->matrix[new_image->height-j-1][i] = image->matrix[i][j];
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->width = new_image->width;
    image->height = new_image->height;
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}


void PGMNegative(PGM *image){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Inverting the values by the color range
            // value = ColorRange - value
            image->matrix[i][j] = image->color_range - image->matrix[i][j];
        }
    }
}

// Function to apply the average
void PGMAverage(PGM *image, int row_col_near){
    // Calculating the number of lines and col around the pixel
    row_col_near = (int)((float)row_col_near/2);
    // Preparing a cloned image for save the data
    PGM *new_image = PGMCloneImage(image);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Initiating average and elements that are reachable to count the
            // divisor
            int average = 0;
            int elements_reachable = 0;
            for(int k=i-row_col_near; k<=i+row_col_near; k++){
                for (int l=j-row_col_near; l<=j+row_col_near; l++){
                    // If pixel is inside the limits they will be counted.
                    if((k>=0 && k<image->height) && (l>=0 && l<image->width)){
                        elements_reachable++;
                        average += image->matrix[k][l];
                    }
                }
            }
            // Getting the final average rounded not truncated
            average = (int)(((float)average/elements_reachable)+0.5);
            new_image->matrix[i][j] = average;
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}

// Function to get the average of the region of rows and cols near to [i][j]
int PGMLocalAverage(PGM *image, int i, int j, int row_col_near){
    int average = 0;
    int elements_reachable = 0;
    for(int k=i-row_col_near; k<=i+row_col_near; k++){
        for (int l=j-row_col_near; l<=j+row_col_near; l++){
            // If pixel is inside the limits they will be counted.
            if((k>=0 && k<image->height) && (l>=0 && l<image->width)){
                elements_reachable++;
                average += image->matrix[k][l];
            }
        }
    }
    // Getting the final average rounded not truncated
    average = (int)(((float)average/elements_reachable)+0.5);
    return average;
}

// Function to get the standard deviation of a local region of rows and cols
// near to [i][j]
double PGMLocalStandardDeviation(PGM *image, int i, int j, int row_col_near){
    double standard_deviation = 0.0;
    double variance = 0.0;
    int elements_reachable = 0;
    int average = PGMLocalAverage(image, i, j, row_col_near);
    for(int k=i-row_col_near; k<=i+row_col_near; k++){
        for (int l=j-row_col_near; l<=j+row_col_near; l++){
            // If pixel is inside the limits they will be counted.
            if((k>=0 && k<image->height) && (l>=0 && l<image->width)){
                elements_reachable++;
                variance += pow(image->matrix[k][l] - average, 2);
            }
        }
    }
    // Getting the final average rounded not truncated
    variance = (variance/(double)elements_reachable);
    standard_deviation = sqrt(variance);
    return standard_deviation;
}

// Function to get the global average of image
int PGMGlobalAverage(PGM *image){
    int average = 0;
    int elements_reachable = 0;
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            elements_reachable++;
            average += image->matrix[i][j];
        }
    }
    // Getting the final average rounded not truncated
    average = (int)(((float)average/elements_reachable)+0.5);
    return average;
}

// Function to get the global standard deviation of image
double PGMGlobalStandardDeviation(PGM *image){
    double standard_deviation = 0.0;
    double variance = 0.0;
    int elements_reachable = 0;
    int average = PGMGlobalAverage(image);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            elements_reachable++;
            variance += pow(image->matrix[i][j] - average, 2);
        }
    }
    // Getting the final average rounded not truncated
    variance = (variance/(double)elements_reachable);
    standard_deviation = sqrt(variance);
    return standard_deviation;
}

// Function to apply the laplace with filter
void PGMLaplace(PGM *image, int **filter){
    // Calculating the number of lines and col around the pixel
    int row_col_near = 1;
    // Preparing a cloned image for save the data
    PGM *new_image = PGMCloneImage(image);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Initiating amount
            int amount = 0;
            // Index r for rows of filter
            int r=0;
            for(int k=i-row_col_near; k<=i+row_col_near; k++){
                // Index s for rows of filter
                int s=0;
                for (int l=j-row_col_near; l<=j+row_col_near; l++){
                    // If pixel is inside the limits they will be counted.
                    if((k>=0 && k<image->height) && (l>=0 && l<image->width)){
                        amount += (image->matrix[k][l]*filter[r][s]);
                    }
                    s++;
                }
                r++;
            }
            // Getting the final average rounded not truncated
            new_image->matrix[i][j] = amount;
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);

}

// Function to apply the mirrored effect horizontally
void PGMMirroredH(PGM *image){
    PGM *new_image = PGMCloneImage(image);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Inverting the values on start col to end col
            new_image->matrix[i][j] = image->matrix[i][image->width-j-1];
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}

// Function to apply the mirrored effect vertically
void PGMMirroredV(PGM *image){
    PGM *new_image = PGMCloneImage(image);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Inverting the values on start col to end col
            new_image->matrix[i][j] = image->matrix[image->height-i-1][j];
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}

// Function to change the color range to a new one
// color_range - color
// new_color_range - x
// x*color_range = color*new_color_range
// x = (color*new_color_range)/color_range
void PGMChangeColorRange(PGM *image, int new_range){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            image->matrix[i][j]= (int)(
                    (
                        (float)image->matrix[i][j] *
                        (float)new_range /
                        (float)image->color_range
                    ) +
                    0.5
                );
        }
    }
    image->color_range = new_range;
}

// Function to save the histogram of an image into a file
int * PGMHistogram(PGM *image){
    int *histogram;
    // Allocating the histogram vector
    histogram = malloc((image->color_range+1)*sizeof(int));
    for (int i=0; i < image->color_range+1; i++){
        histogram[i]=0;
    }
    // Calculating values of image on each index of histogram
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            histogram[image->matrix[i][j]]++;
        }
    }
    return histogram;
}

// Function to save the histogram of an image into a file
void PGMHistogramToFileParam(PGM *image, int* histogram, char *file_name){
	FILE *file;
	file = fopen(file_name, "w");
    // Saving on file
    for (int i=0; i < image->color_range+1; i++){
        fprintf(file, "%d;", histogram[i]);
    }
    free(histogram);
    fclose (file);
}

// Function to save the histogram of an image into a file
void PGMHistogramToFile(PGM *image, char *file_name){
    int * histogram = PGMHistogram(image);
    PGMHistogramToFileParam(image, histogram, file_name);
}

// Function to get the histogram equalized
int * PGMHistogramEqualized(PGM *image){
    int *histogram;
    // Allocating the histogram vector
    histogram = PGMHistogram(image);
    float pr_sum = 0;
    for (int i=0; i < image->color_range; i++){
        float pr = 0.0;
        pr = (float)histogram[i]/(float)(image->width*image->height);
        pr_sum += pr;
        histogram[i] = (int)((image->color_range*pr_sum) + 0.5);
    }
    return histogram;
}

// Function to apply a determinated histogram globally
void PGMHistogramGlobal(PGM *image, int * histogram){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Find the histogram value for this color
            image->matrix[i][j] = histogram[image->matrix[i][j]];
        }
    }
}

// Function to apply a histogram equalized on image globally
void PGMEqualizedHistogramGlobal(PGM *image){
    PGMHistogramGlobal(image, PGMHistogramEqualized(image));
}

// Function to apply a determinated histogram locally
void PGMEqualizedHistogramLocal(PGM *image, int size){
    // Preparing a cloned image for save the data
    PGM *new_image = PGMCloneImage(image);
    for(int i=0; i<image->height; i+=size){
        for(int j=0; j<image->width; j+=size){
            // Getting a new histogram with the values calculated by the local
            // The local is obtained by a cutted image from original with
            // size x size
            int * histogram = PGMHistogramEqualized(
                        PGMCutImage(image, j, i, size, size)
                    );
            for(int k=i; k < (size+i); k++){
                for(int l=j; l < (size+j); l++){
                    if(new_image->height > k && new_image->width > l){
                        new_image->matrix[k][l] = (
                                histogram[new_image->matrix[k][l]]);
                    }
                }
            }
            free(histogram);
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}

// Function to apply a histogram statistic on image
void PGMHistogramStatistics(
        PGM *image, double E, double k0, double k1, double k2, int row_col_near
    ){
    // Preparing a cloned image for save the data
    PGM *new_image = PGMCloneImage(image);
    int global_average = PGMGlobalAverage(image);
    double global_standard_deviation = PGMGlobalStandardDeviation(image);
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            int local_average = PGMLocalAverage(image, i, j, row_col_near);
            double local_standard_deviation = PGMLocalStandardDeviation(
                    image, i, j, row_col_near);
            // If the local average is minor than k0*global_average and
            // local_standard_deviation is between k1*global_standard_deviation
            // and k2*global_standard_deviation
            if (
                    local_average <= round(k0*(double)global_average) &&
                    (
                     k1*global_standard_deviation <= local_standard_deviation &&
                     local_standard_deviation <= k2*global_standard_deviation
                     )
                ){
                new_image->matrix[i][j] = (int)((E*image->matrix[i][j])+0.5);
            }
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}
// Function to cut part of image into a new one
PGM * PGMCutImage(PGM *image, int ini_x, int ini_y, int width, int height){
    PGM * new_image = PGMConst(
            image->magic_nmb, width, height, image->color_range);
    for(int i=ini_y; i < (ini_y+height); i++){
        for(int j=ini_x; j < (ini_x+width); j++){
            new_image->matrix[i-ini_y][j-ini_x] = (
                        (
                            i < image->height &&
                            j < image->width
                        )?
                        image->matrix[i][j]:
                        0
                    );
        }
    }
    return new_image;
}

// Function to apply the histogram binary
void PGMGraphBinary(PGM *image, int center){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // If the value is higher than the center it will be changed to max
            // otherwise the value will be changed to 0
            image->matrix[i][j] = (image->matrix[i][j] > center?
                                     image->color_range-1:
                                     0
                );
        }
    }
}

// Function to apply the histogram binary inside and outside a interval [A-B]
void PGMGraphBinaryAB(PGM *image, int a, int b,
        int color_in, int color_out){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // If the value is inside the interval [a-b] we will use the
            // color_in, otherwise we will use the color_out
            image->matrix[i][j] = (
                    (
                        image->matrix[i][j] >= a &&
                        image->matrix[i][j] <= b
                     )?
                        color_in:
                        color_out
                );
        }
    }
}

// Function to apply the color inside a interval [A-B]
void PGMGraphAB(PGM *image, int a, int b, int color_in){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // If the value is inside the interval [a-b] we will use the
            // color_in, otherwise we will use the current_color
            image->matrix[i][j] = (
                    (
                        image->matrix[i][j] >= a &&
                        image->matrix[i][j] <= b
                     )?
                        color_in:
                        image->matrix[i][j]
                );
        }
    }
}

// Function to apply the color inside a interval [A-B] by the equation of the
// line AB
// y=(x - a)*(255/b-a);
void PGMGraphLineAB(PGM *image, int a, int b){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // 0 if the value is under a, 255 if is higher than b, inner
            // interval is the value given by the line equation
            if(image->matrix[i][j] < a){
                image->matrix[i][j] = 0;
            }else if(image->matrix[i][j] > b){
                image->matrix[i][j] = image->color_range;
            }else{
                image->matrix[i][j] = (
                        (image->matrix[i][j] - a)*(image->color_range/(b-a))
                    );
            }
        }
    }
}

// Function to apply the inverse log
void PGMGraphInverseLog(PGM *image){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Transform the current value into the interval 0..1 then exec the
            // function
            image->matrix[i][j] = (
                    (int)(
                        fabs(
                            image->color_range *(
                                1*log(
                                        (double)image->matrix[i][j] /
                                        (double)image->color_range
                                )
                            )
                        )
                    )
                );
        }
    }
}

// Function to apply the gamma pow
void PGMGraphPow(PGM *image, double a){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // Transform the current value into the interval 0..1 then exec the
            // function
            image->matrix[i][j] = (
                    (int)(
                        image->color_range *(
                            1*pow(
                                (
                                    (double)image->matrix[i][j] /
                                    (double)image->color_range
                                ),
                                a
                            )
                        )
                    )
                );
        }
    }
}

// Function to amplify the image multiplying the number of pixels
void PGMZoomIn(PGM *image, double factor){
    PGM * new_image;
    // Creating a new image multiplying the size by the factor
    new_image = PGMConst(
            image->magic_nmb,
            image->width*factor,
            image->height*factor,
            image->color_range
        );
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            for (int k=i*factor; k<factor*(i+1); k++){
                for (int l=j*factor; l<factor*(j+1); l++){
                    new_image->matrix[k][l] = image->matrix[i][j];
                }
            }
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->width = new_image->width;
    image->height = new_image->height;
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);

}

// Function to reduce the image multiplying the number of pixels
void PGMZoomOut(PGM *image, double factor){
    PGM * new_image;
    // Creating a new image multiplying the size by the factor
    new_image = PGMConst(
            image->magic_nmb,
            image->width/factor,
            image->height/factor,
            image->color_range
        );
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            for (int k=i*factor; k<factor*(i+1); k++){
                for (int l=j*factor; l<factor*(j+1); l++){
                    new_image->matrix[k][l] = image->matrix[i][j];
                }
            }
        }
    }
    // free the old matrix
    free(image->matrix);
    // pass values from the old image
    image->width = new_image->width;
    image->height = new_image->height;
    image->matrix = new_image->matrix;
    // free the auxiliar struct
    free(new_image);
}

// Function to seek for negative values and change their to zero
void PGMNegativeValToZero(PGM *image){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // If value is minor than 0 the value will be changed to 0
            if (image->matrix[i][j]<0){
                image->matrix[i][j] = 0;
            }
        }
    }
}

// Function to seek for too high values and change their to the max
void PGMTooHighToMax(PGM *image){
    for(int i=0; i<image->height; i++){
        for(int j=0; j<image->width; j++){
            // If value is higher than the color range the value will be
            // changed to the color range
            if (image->matrix[i][j]>image->color_range){
                image->matrix[i][j] = image->color_range;
            }
        }
    }
}

// Function that seek for both, higher and lower values to stabilize them
void PGMStabilizeValues(PGM* image){
    PGMNegativeValToZero(image);
    PGMTooHighToMax(image);
}

//********************PPM******************************//
//********************PPM******************************//
//********************PPM******************************//
//********************PPM******************************//

// Function to build a PPM from a char buffer.
void PPMConstFromBuffer(PPM *image, char *buffer);

// Function to build a PPM from a file
void PPMConstFromFile(PPM *image, char *filename);

// Function to alloc a new matrix based on width and height
void PPMConstMatrixFromWidthHeight(PPM *image);

// Function to alloc a PPM, allocatting the matrix by width and height
PPM * PPMConst(char *magic_nmb, int width, int height, int color_range);

// Function to clone a matrix allocatting a new one
void PPMCloneMatrix(PPM *dest, PPM *source);

// Function to clone a image allocatting a new one
PPM * PPMCloneImage(PPM *image);

// Function to print image on terminal
void PPMPrint(PPM *image);

// Function to print image on file
void PPMPrintToFile(PPM *image, char *filename);

// Function to extract the channel from image R=0; G=0; B=0
void PPMExtractChannel(PPM *image, int channel);

// Function to extract the gray channel from image R=0; G=0; B=0
void PPMExtractGrayChannel(PPM *image, int channel);

// Function to remove channel from image R=0; G=0; B=0
void PPMRemoveChannel(PPM *image, int channel);

// Function to add a value to channel on image R=0; G=0; B=0
void PPMSumChannel(PPM *image, int channel, int value);

// Function to mix the channels on image R=0; G=0; B=0
void PPMMixChannel(PPM *image, int red, int green, int blue);

// Function to replace the choosen channel from src to destiny
void PPMSwapFromImgChannel(PPM *dest, PPM *src, int channel);

// Function to use the 3 images into the dest
void PPMMountChannelsFromimages(PPM *dest, PPM *src_red, PPM *src_green, PPM *src_blue);

// Function to build a PPM from a char buffer.
void PPMConstFromBuffer(PPM *image, char *buffer){
    char magic_nmb[3];
    int width = 0, height = 0, row_nmb = 0, col_nmb = 0, color_range = 0;
    int col = 0, row = 0, rgb = 0;
    int **matrix;
    bool start_matrix = false, is_comment = false, token_started=false;
    READ_STATE reading = MAGIC_NMB;
    // Iterate over buffer to get the tokens
    // Token var
    char *token = NULL;
    // Token iterator
    int token_i = 0;
    for (int i=0; buffer[i]!= '\0'; i++){
        // Current char
        char c = buffer[i];
        // Some delimiter showed up
        if (c == '\n' || c == ' ' || c == '\t' || c == '#'){
            // The delimiter is a break-line
            if (c == '\n'){
                // if this char is a break-line we need to end the comment
                if (is_comment){
                    is_comment = false;
                }
            }
            else if (c == '#'){
                // Starting the comment
                is_comment = true;
            }
            // Ending the Token
            if (token_started){
                token_started = false;
                // Choosing what to do with a valid token
                switch(reading){
                    // The first stage of reading is for the magic number
                    case MAGIC_NMB:
                        strcpy(image->magic_nmb, token);
                        reading = WIDTH;
                        break;
                    // The second stage of reading is for get the width
                    case WIDTH:
                        image->width = atoi(token);
                        reading = HEIGHT;
                        break;
                    // The third stage of reading is for get the height
                    case HEIGHT:
                        image->height = atoi(token);
                        // Allocating the matrix
                        PPMConstMatrixFromWidthHeight(image);
                        reading = MAX_COLOR_RANGE;
                        break;
                    // The fourth stage of reading is for get the max color
                    // range
                    case MAX_COLOR_RANGE:
                        image->color_range = atoi(token);
                        reading = PIXEL;
                        break;
                    // Filling the matrix is the last stage
                    case PIXEL:
                        if (col>=image->width){
                            col=0;
                            row++;
                        }
                        int value = atoi(token);
                        image->matrix[row][col][rgb] = value;
                        rgb++;
                        if (rgb>=3){
                            rgb=0;
                            col++;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        // No delimiter showed up and assemblying the token
        else{
            // This is part of a valid token outside of a comment
            if (!is_comment){
                // Token is already initiated
                if (token_started){
                    token_i++;
                    // Coppying the current token content into the auxiliar
                    // string
                    char *str_aux = (char *)malloc((token_i+1)+1);
                    strcpy(str_aux, token);
                    // Adding the new char on auxiliar string
                    str_aux[token_i] = c;
                    str_aux[token_i+1] = '\0';
                    // Free the old token memory
                    free(token);
                    // Creating the new token to receive the token +
                    // new char
                    token = (char *)malloc((token_i+1)+1);
                    strcpy(token, str_aux);
                    // Free the auxiliar string
                    free(str_aux);
                }
                // Starting a new token
                else{
                    token_started = true;
                    token_i = 0;
                    token = (char *)malloc((token_i+1)+1);
                    token[token_i] = c;
                    token[token_i+1] = '\0';
                }
            }
        }
    }
}

// Function to build a PPM from a file
void PPMConstFromFile(PPM *image, char *filename){
	FILE *file;
	char *buffer = 0;
    long length;
    // Loding file on a char buffer
	file = fopen(filename, "r");
    if (file)
	{
		fseek (file, 0, SEEK_END);
		length = ftell(file);
		fseek (file, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer)
		{
			fread (buffer, 1, length, file);
		}
		fclose (file);
	}
	if (buffer)
	{
        PPMConstFromBuffer(image, buffer);
	}
}

// Function to alloc a new matrix based on width and height
void PPMConstMatrixFromWidthHeight(PPM *image){
    // Allocating the matrix with the proper
    // row & col size
    image->matrix = malloc(image->height * sizeof(int *));
    for (int i = 0; i < image->height; i++) {
        image->matrix[i] = malloc(image->width * sizeof(int *));
        for (int j = 0; j < image->width; j++){
            image->matrix[i][j] = malloc(3*sizeof(int));
        }
    }
}

// Function to alloc a PPM, allocatting the matrix by width and height
PPM * PPMConst(char *magic_nmb, int width, int height, int color_range){
    PPM * image = malloc(sizeof(struct PPM));
    strcpy(image->magic_nmb, magic_nmb);
    image->width = width;
    image->height = height;
    image->color_range = color_range;
    PPMConstMatrixFromWidthHeight(image);
    return image;
}

// Function to clone a matrix allocatting a new one
void PPMCloneMatrix(PPM *dest, PPM *source){
    PPMConstMatrixFromWidthHeight(dest);
    for(int i=0; i<source->height; i++){
        for(int j=0; j<source->width; j++){
            // Adding a bright sum to the present value
            for (int rgb=0; rgb<3; rgb++){
                dest->matrix[i][j][rgb] = source->matrix[i][j][rgb];
            }
        }
    }
}

// Function to clone a image allocatting a new one
PPM * PPMCloneImage(PPM *image){
    PPM *new_image;
    new_image = malloc(sizeof(struct PPM));
    strcpy(new_image->magic_nmb, image->magic_nmb);
    new_image->width = image->width;
    new_image->height = image->height;
    new_image->color_range = image->color_range;
    PPMCloneMatrix(new_image, image);
    return new_image;

}

// Function to print image on terminal
void PPMPrint(PPM *image){
    char *magic_nmb = image->magic_nmb;
    int col_nmb = image->width, row_nmb = image->height;
    int width = image->width, height = image->height;
    int color_range = image->color_range;
    int ***matrix = image->matrix;
	printf("%s\n", magic_nmb);
	printf("%d %d\n", width, height);
	printf("%d\n", color_range);
	for(int i = 0; i < row_nmb; i++) {
		for(int j = 0; j < col_nmb; j++) {
            for (int rgb=0; rgb<3; rgb++){
                printf("%d ", matrix[i][j][rgb]);
            }
		}
		printf("\n");
	}
	printf("\n");
}

// Function to print image on file
void PPMPrintToFile(PPM *image, char *filename){
	FILE *file;
    char *magic_nmb = image->magic_nmb;
    int col_nmb = image->width, row_nmb = image->height;
    int width = image->width, height = image->height;
    int color_range = image->color_range;
    int ***matrix = image->matrix;
	file = fopen(filename, "w");
	fprintf(file, "%s\n", magic_nmb);
	fprintf(file, "%d %d\n", width, height);
	fprintf(file, "%d\n", color_range);
	for(int i = 0; i < row_nmb; i++) {
		for(int j = 0; j < col_nmb; j++) {
            for(int rgb=0; rgb<3; rgb++){
                fprintf(file, "%d ", matrix[i][j][rgb]);
            }
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n");
    fclose (file);
}

// Function to extract the channel from image R=0; G=0; B=0
void PPMExtractChannel(PPM *image, int channel){
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++){
            for(int rgb=0; rgb<3; rgb++){
                // If the channel is equals to rgb index it will be saved
                // otherwise will be changed to zero
                image->matrix[i][j][rgb]=(
                        channel==rgb? image->matrix[i][j][rgb]: 0);
            }
		}
	}
}

// Function to extract the gray channel from image R=0; G=0; B=0
void PPMExtractGrayChannel(PPM *image, int channel){
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++){
            for(int rgb=0; rgb<3; rgb++){
                // Let the all rgb equals to channel choosed
                image->matrix[i][j][rgb]=image->matrix[i][j][channel];
            }
		}
	}
}

// Function to remove channel from image R=0; G=0; B=0
void PPMRemoveChannel(PPM *image, int channel){
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++){
            for(int rgb=0; rgb<3; rgb++){
                // If the channel is equals to rgb index it will be changed to
                // zero, otherwise will be saved
                image->matrix[i][j][rgb]=(
                        channel==rgb? 0: image->matrix[i][j][rgb]);
            }
		}
	}
}

// Function to add a value to channel on image R=0; G=0; B=0
void PPMSumChannel(PPM *image, int channel, int value){
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++){
            for(int rgb=0; rgb<3; rgb++){
                // If the channel is equals to rgb index it will added with
                // value, otherwise will be saved
                image->matrix[i][j][rgb]=
                    (
                        channel==rgb?
                            image->matrix[i][j][rgb] + value:
                            image->matrix[i][j][rgb]
                    );
            }
		}
	}
}

// Function to mix the channels on image R=0; G=0; B=0
void PPMMixChannel(PPM *image, int red, int green, int blue){
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++){
            // Change channels
            int red_color = image->matrix[i][j][red];
            int green_color = image->matrix[i][j][green];
            int blue_color = image->matrix[i][j][blue];
            image->matrix[i][j][0] = red_color;
            image->matrix[i][j][1] = green_color;
            image->matrix[i][j][2] = blue_color;
		}
	}
}

// Function to replace the choosen channel from src to destiny
void PPMSwapFromImgChannel(PPM *dest, PPM *src, int channel){
    for (int i = 0; i < dest->height; i++) {
        for (int j = 0; j < dest->width; j++){
            dest->matrix[i][j][channel]= src->matrix[i][j][channel];
		}
	}
}

// Function to use the 3 images into the dest
void PPMMountChannelsFromimages(PPM *dest, PPM *src_red, PPM *src_green, PPM *src_blue){
    for (int i = 0; i < dest->height; i++) {
        for (int j = 0; j < dest->width; j++){
            dest->matrix[i][j][0]= src_red->matrix[i][j][0];
            dest->matrix[i][j][1]= src_green->matrix[i][j][1];
            dest->matrix[i][j][2]= src_blue->matrix[i][j][2];
		}
	}
}

int main(int argc, char **argv){
    char * pgm_result = "result.pgm";
    char * ppm_result = "result.ppm";
    char * cmd = argv[1];
    // You can call like:
    // gcc main.c -o mypdi
    // ./mypdi PGMNegative lena.pgm
    if (strcmp(cmd, "PGMNegative")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMNegative(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMDarkerSub lena.pgm 50
    else if(strcmp(cmd, "PGMDarkerSub")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMDarkerSub(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMDarkerDiv lena.pgm 2
    else if(strcmp(cmd, "PGMDarkerDiv")==0){
        char *src_img_file_name = argv[2];
        float val = atof(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMDarkerDiv(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMBrighterSum lena.pgm 50
    else if(strcmp(cmd, "PGMBrighterSum")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMBrighterSum(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMBrighterMult lena.pgm 0.5
    else if(strcmp(cmd, "PGMBrighterMult")==0){
        char *src_img_file_name = argv[2];
        float val = atof(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMBrighterMult(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMGraphBinary lena.pgm 100
    else if(strcmp(cmd, "PGMGraphBinary")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMGraphBinary(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMMirroredH lena.pgm
    else if (strcmp(cmd, "PGMMirroredH")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMMirroredH(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMMirroredV lena.pgm
    else if (strcmp(cmd, "PGMMirroredV")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMMirroredV(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMRotate lena.pgm -90
    else if(strcmp(cmd, "PGMRotate")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMRotate(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMChangeColorRange lena.pgm 10
    else if(strcmp(cmd, "PGMChangeColorRange")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMChangeColorRange(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMGraphBinaryAB lena.pgm 100 150 50 100
    else if(strcmp(cmd, "PGMGraphBinaryAB")==0){
        char *src_img_file_name = argv[2];
        int A = atoi(argv[3]);
        int B = atoi(argv[4]);
        int in = atoi(argv[5]);
        int out = atoi(argv[6]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMGraphBinaryAB(image, A, B, in, out);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMGraphAB lena.pgm 100 150 70
    else if(strcmp(cmd, "PGMGraphAB")==0){
        char *src_img_file_name = argv[2];
        int A = atoi(argv[3]);
        int B = atoi(argv[4]);
        int in = atoi(argv[5]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMGraphAB(image, A, B, in);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMGraphLineAB lena.pgm 100 150
    else if(strcmp(cmd, "PGMGraphLineAB")==0){
        char *src_img_file_name = argv[2];
        int A = atoi(argv[3]);
        int B = atoi(argv[4]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMGraphLineAB(image, A, B);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMGraphInverseLog lena.pgm
    else if(strcmp(cmd, "PGMGraphInverseLog")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMGraphInverseLog(image);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMSubImages lena.pgm my_lena.pgm
    else if(strcmp(cmd, "PGMSubImages")==0){
        char *src_img_1_file_name = argv[2];
        char *src_img_2_file_name = argv[3];

        PGM *image1 = malloc(sizeof(struct PGM));
        PGMConstFromFile(image1, src_img_1_file_name);

        PGM *image2 = malloc(sizeof(struct PGM));
        PGMConstFromFile(image2, src_img_2_file_name);

        PGMSubImages(image1, image2);
        PGMStabilizeValues(image1);
        PGMPrint(image1);
        PGMPrintToFile(image1, pgm_result);
    }
    // ./mypdi PGMSumImages lena.pgm my_lena1.pgm
    else if(strcmp(cmd, "PGMSumImages")==0){
        char *src_img_1_file_name = argv[2];
        char *src_img_2_file_name = argv[3];

        PGM *image1 = malloc(sizeof(struct PGM));
        PGMConstFromFile(image1, src_img_1_file_name);

        PGM *image2 = malloc(sizeof(struct PGM));
        PGMConstFromFile(image2, src_img_2_file_name);

        PGMSumImages(image1, image2);
        PGMStabilizeValues(image1);
        PGMPrint(image1);
        PGMPrintToFile(image1, pgm_result);
    }
    // ./mypdi PGMZoomIn lena.pgm 2
    else if(strcmp(cmd, "PGMZoomIn")==0){
        char *src_img_file_name = argv[2];
        float val = atof(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMZoomIn(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMAverage lena.pgm 5
    else if(strcmp(cmd, "PGMAverage")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMAverage(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMLaplaceC lena.pgm
    else if(strcmp(cmd, "PGMLaplaceC")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        int **filter = PGMConstFilterC();
        PGMConstFromFile(image, src_img_file_name);
        PGMLaplace(image, filter);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMLaplaceD lena.pgm
    else if(strcmp(cmd, "PGMLaplaceD")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        int **filter = PGMConstFilterD();
        PGMConstFromFile(image, src_img_file_name);
        PGMLaplace(image, filter);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMEqualizedHistogramGlobal lena.pgm
    else if(strcmp(cmd, "PGMEqualizedHistogramGlobal")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMEqualizedHistogramGlobal(image);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMEqualizedHistogramLocal lena.pgm 3
    else if(strcmp(cmd, "PGMEqualizedHistogramLocal")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMEqualizedHistogramLocal(image, val);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMHistogramStatistics lena.pgm 4.0 0.4 0.02 0.4 1
    else if(strcmp(cmd, "PGMHistogramStatistics")==0){
        char *src_img_file_name = argv[2];
        double E = atof(argv[3]);
        double k0 = atof(argv[4]);
        double k1 = atof(argv[5]);
        double k2 = atof(argv[6]);
        int row_col_near = atoi(argv[7]);
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMHistogramStatistics(image, E, k0, k1, k2, row_col_near);
        PGMStabilizeValues(image);
        PGMPrint(image);
        PGMPrintToFile(image, pgm_result);
    }
    // ./mypdi PGMHistogramToFile lena.pgm
    else if(strcmp(cmd, "PGMHistogramToFile")==0){
        char *src_img_file_name = argv[2];
        PGM *image = malloc(sizeof(struct PGM));
        PGMConstFromFile(image, src_img_file_name);
        PGMHistogramToFile(image, "result.csv");
    }
    // INITING PPM METHODS!!!
    // ./mypdi PPMExtractChannel lenna.ppm 1
    else if(strcmp(cmd, "PPMExtractChannel")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PPM *image = malloc(sizeof(struct PPM));
        PPMConstFromFile(image, src_img_file_name);
        PPMExtractChannel(image, val);
        PPMPrint(image);
        PPMPrintToFile(image, ppm_result);
    }
    // ./mypdi PPMExtractGrayChannel lenna.ppm 1
    else if(strcmp(cmd, "PPMExtractGrayChannel")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PPM *image = malloc(sizeof(struct PPM));
        PPMConstFromFile(image, src_img_file_name);
        PPMExtractGrayChannel(image, val);
        PPMPrint(image);
        PPMPrintToFile(image, ppm_result);
    }
    // ./mypdi PPMRemoveChannel lenna.ppm 1
    else if(strcmp(cmd, "PPMRemoveChannel")==0){
        char *src_img_file_name = argv[2];
        int val = atoi(argv[3]);
        PPM *image = malloc(sizeof(struct PPM));
        PPMConstFromFile(image, src_img_file_name);
        PPMRemoveChannel(image, val);
        PPMPrint(image);
        PPMPrintToFile(image, ppm_result);
    }
    // ./mypdi PPMSumChannel lenna.ppm 1 50
    else if(strcmp(cmd, "PPMSumChannel")==0){
        char *src_img_file_name = argv[2];
        int channel = atoi(argv[3]);
        int val = atoi(argv[4]);
        PPM *image = malloc(sizeof(struct PPM));
        PPMConstFromFile(image, src_img_file_name);
        PPMSumChannel(image, channel, val);
        PPMPrint(image);
        PPMPrintToFile(image, ppm_result);
    }
    // ./mypdi PPMMixChannel lenna.ppm 1 0 1
    else if(strcmp(cmd, "PPMMixChannel")==0){
        char *src_img_file_name = argv[2];
        int red = atoi(argv[3]);
        int green = atoi(argv[4]);
        int blue = atoi(argv[5]);
        PPM *image = malloc(sizeof(struct PPM));
        PPMConstFromFile(image, src_img_file_name);
        PPMMixChannel(image, red, green, blue);
        PPMPrint(image);
        PPMPrintToFile(image, ppm_result);
    }
    // ./mypdi PPMSwapFromImgChannel lenna.ppm lenna_green.ppm 1
    else if(strcmp(cmd, "PPMSwapFromImgChannel")==0){
        char *src_img_1_file_name = argv[2];
        char *src_img_2_file_name = argv[3];

        PPM *image1 = malloc(sizeof(struct PPM));
        PPMConstFromFile(image1, src_img_1_file_name);

        PPM *image2 = malloc(sizeof(struct PPM));
        PPMConstFromFile(image2, src_img_2_file_name);

        int channel = atoi(argv[4]);

        PPMSwapFromImgChannel(image1, image2, channel);
        PPMPrint(image1);
        PPMPrintToFile(image1, ppm_result);
    }
    // ./mypdi PPMMountChannelsFromimages lenna_red.ppm lenna_green.ppm
    // lenna_blue.ppm
    else if(strcmp(cmd, "PPMMountChannelsFromimages")==0){
        char *src_img_1_file_name = argv[2];
        char *src_img_2_file_name = argv[3];
        char *src_img_3_file_name = argv[4];

        PPM *image1 = malloc(sizeof(struct PPM));
        PPMConstFromFile(image1, src_img_1_file_name);

        PPM *image2 = malloc(sizeof(struct PPM));
        PPMConstFromFile(image2, src_img_2_file_name);

        PPM *image3 = malloc(sizeof(struct PPM));
        PPMConstFromFile(image3, src_img_3_file_name);

        PPMMountChannelsFromimages(image1, image1, image2, image3);
        PPMPrint(image1);
        PPMPrintToFile(image1, ppm_result);
    }
    // Showing the arguments used
    printf("Argumenst Used: \n\n");
    for (int i=0; i<argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    return 0;
}
