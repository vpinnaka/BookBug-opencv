// Name : Vinay Datta Pinnaka
// ID : A03963118
// Class :COSC 5590- Digital Image Processing and Computer Vision
// Semester : Fall 2015
// Instructor : Dr. Maryam Rahnemoonfar
// Final Prpject
// Due Date:  Nov 30th 11:59pm

//*********************************************************
//
//                    Includes and Defines
//
//*********************************************************
#include <opencv2/opencv.hpp>
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#define EXIT_FLAG -1


using namespace cv;
using namespace std;

//*********************************************************
//
//                    Global variables
//
//*********************************************************

Mat BookCover;
Mat PageNumber;
int frameSelect;

//*********************************************************
//
//                    Function Prototypes
//
//*********************************************************

int bookMark_a_book();
int findTheBook();
void MouseEvent(int, int, int, int, void*);

void ProcessBookCover();
void ProcessPageNumber();
void exitFunction(int);

//********************************************************
//
//  main Function
//
//	Runs a infinite while loop
//	takes a choice from the user input
//	Based on the choice selects an option in the switch case
//	calls the corresponding function
//
//  Return Value
//  ------------  
//  int
//	
// 	Functions called
//	----------------
//	int bookMark_a_book();
//	int findTheBook();
//  void ProcessBookCover();
//	void ProcessPageNumber();
//	void exitFunction(int);
//
//  Local Variables  
//  ---------------
//
// 	choice,flag1 	   int
//********************************************************
int main()															// main function starts here
{
	int choice, flag1 = 0;
	while (true)													// Infinite loop starts here
	{
		system("CLS");

		cout << "\n\t\t\t\tBOOK BUG\n\nSelect a coice for the type of operation" << endl;
		cout << "1. BOOKMARK A BOOK\n2. EXIT APPLICATION" << endl;
		cout << "Enter the choice here :";							//Getting the choice among three functionalities from the user
		cin >> choice;
		cout << endl;

		switch (choice)												//Switch case to execute the selected choice 
		{
		case 1:	flag1 = bookMark_a_book();							//Calling the function 'bookMark_a_book'
			exitFunction(flag1);
			break;
		case 2:flag1 = findTheBook();								//Calling the function 'findTheBook'
			exitFunction(flag1);
			break;
		case 3:	exitFunction(EXIT_FLAG);							//Calling the function 'exitFunction'
			break;
		default:cout << "Invalid choice select the correct one" << endl;
		}
	}
	
	return 0;

}

/*******************		User selects this option if he wants to bookmark a new book   *********************/


int bookMark_a_book()
{
	
//Commented code is use the default web camera to take the images of the text book both the book cover and the pagenumber
//If you are willing to use your web camera please uncomment this code and comment the code in which hardcoding is done

	/*frameSelect = 2;
	
	VideoCapture frameCapture;
	frameCapture.open(0);
	if (!frameCapture.isOpened())
		cout << "Error in loading camera" << endl;
	namedWindow("Capture Book Cover", CV_WINDOW_NORMAL);
	cout << "MOUSE CLICK 1 -  Book Cover Image\nMOUSE CLICK 2 - Page Number Image" << endl;
	while (frameSelect > 0)
	{
		Mat frame;
		frameCapture >> frame;
		if (!frame.data)
		{
			cerr << "Cannot capture frame from camera" << endl;
			break;
		}
		//rectangle(frame, Point(frame.rows/5, frame.cols/8), Point(4*frame.rows / 5, 7*frame.cols / 8), Scalar(255, 0, 0));
		imshow("Capture Book Cover", frame);
		setMouseCallback("Capture Book Cover", MouseEvent, &frame);
			
		if (waitKey(30) == 27)
			break;
		
	}
	destroyWindow("Capture Book Cover");

	ProcessBookCover();
	ProcessPageNumber();
	*/

// The following part of code has to be commented to obtain the images of the book dynamicaly from web camera 
// Sample images are hard coded to obtain the valid result
	
	Mat readImg;
	readImg = imread("bookCover.jpeg", CV_LOAD_IMAGE_UNCHANGED); // Read the book cover image 
	readImg.copyTo(BookCover);
	ProcessBookCover();											// Calling the function 'ProcessBookCover'

	readImg = imread("pageNumber.jpeg", CV_LOAD_IMAGE_UNCHANGED);// Read the Page number  image 
	readImg.copyTo(PageNumber);
	ProcessPageNumber();										// Calling the function "ProcessPageNumber"

//Here is the end of hardcoded code
	return 0;
}

int findTheBook()
{
	
	return 0;
}

void ProcessPageNumber()										// Function definition for 'ProcessPageNumber'
{
	Mat originalImg, grayImg, binaryImg, connectedImg, gradientImg; // declaration of Mat objects
	PageNumber.copyTo(originalImg);
	cvtColor(originalImg, grayImg, CV_RGB2GRAY);				//Convert original image to gray scale
	//imwrite("grayImg.jpeg", grayImg);
	Mat ellipseSE = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));	// generate a structuring elment of type ELLIPSE size 3x3

	morphologyEx(grayImg, gradientImg, MORPH_GRADIENT, ellipseSE);	// Morphological gradient on the grayscale image with ellipse SE

	//imwrite("gradientImg.jpeg", gradientImg);

	threshold(gradientImg, binaryImg, 0.0, 255.0, THRESH_BINARY| THRESH_OTSU);// Apply threshold using OTSU method
	
	//imwrite("binaryImg.jpeg", binaryImg);
	
	vector<vector<Point>> contours;								// Declarion of vector named contours
	vector<Vec4i> hierarchy;
	findContours(binaryImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0)); 	// Find all the contours of the binary image and store the contour point in a vector of points 
	int max_idx = 0;
	Mat mask = Mat::zeros(binaryImg.size(), CV_8UC1);			// Define a Mat object with all zeros
	double max_area = 0;
	for (int i = 0; i < contours.size(); i++)					//	condition to draw the contours in the image
	{
		double area = contourArea(contours[i]);
		if(area > 10)											// If area of contour is grater than 10 then draw the contour
		{
			drawContours(mask, contours, i, cv::Scalar(255), -1);// draw the contour and fill it completly
			
		}
		//imwrite("mask.jpeg", mask);
	}
	
	Mat output = ((~grayImg) & mask)>115;						// Create a output image which can be passed to tessract.
	
	//imwrite("output.jpeg", output);
	tesseract::TessBaseAPI tess;								// Declaration of TessBaseAPI
	tess.Init("", "eng", tesseract::OEM_DEFAULT);				// Intialization of TessBaseAPI
	tess.SetVariable("tessedit_char_whitelist", "0123456789");	// Set the variables that are to be extracted from the image
	tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(7);// Set the tesseract page segment mode to single page
	tess.SetImage((uchar*)output.data, output.cols, output.rows, 1, output.cols);// Set the image ready and pass to tesseract

	char* out = tess.GetUTF8Text();								// Get the extracted data from tesseract and place it in out variable
	string line(out);											//	define a string and copy the data to string variable.
	cout << "Page Number:" << line << endl;
	ofstream outputfile;										// Declare the file handler
	outputfile.open("out.dat", ios::out | ios::app);			// open file in append mode
	
	outputfile << line;											// write page number to the file 


}

void ProcessBookCover()											// Function definition for 'ProcessBookCover'
{
	Mat originalImg, smallImg, grayImg, gradientImg, binaryImg, connectedImg;// declaration of Mat objects

	BookCover.copyTo(originalImg);
	smallImg.create(Size(originalImg.cols, originalImg.rows), originalImg.depth());
	
	cvtColor(originalImg, grayImg, CV_RGB2GRAY);				//Convert original image to gray scale
	//imwrite("grayImg.jpeg", grayImg);

	Mat ellipseSE = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));	// generate a structuring elment of type ELLIPSE size 3x3
	morphologyEx(grayImg, gradientImg, MORPH_GRADIENT, ellipseSE);	// Morphological gradient on the grayscale image with ellipse SE

	//imwrite("gradientImg.jpeg", gradientImg);

	threshold(gradientImg, binaryImg, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU); // Apply threshold using OTSU method
//	imwrite("binaryImg.jpeg", binaryImg);

	Mat closingK = getStructuringElement(MORPH_RECT, Size(9, 1));	// generate a structuring elment of type RECT size 9x1  line SE
	morphologyEx(binaryImg, connectedImg, MORPH_CLOSE, closingK);	// Morphological CLOSE on the binaryImg image with Line SE
	//imwrite("connectedImg.jpeg", connectedImg);

	vector<vector<Point>> contours;									// Declarion of vector named contours
	vector<Vec4i> hierarchy;
	findContours(connectedImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));	// Find all the contours of the binary image and store the contour point in a vector of points 
	//imwrite("contours.jpeg", connectedImg);

	Mat mask = Mat::zeros(binaryImg.size(), CV_8UC1);				// Define a Mat object with all zeros
	
	double max_area = 0;
	int max_idx = 0;
	int* idx_arry;
	int arr_size = contours.size();
	idx_arry = new int[arr_size];
	for (int i = 0; i < contours.size(); i++)		//	condition to itterate among all the contours in the image
	{
		double area = contourArea(contours[i]);		// Find area of each contour
		max_idx = area > max_area ? i : max_idx;	// Find the index of the contour with maximum size
		max_area = area > max_area ? area : max_area;	// Find the maximum area of the contour
	}
	idx_arry[0] = max_idx;

	for (int i = 1; i < contours.size(); i++)		//	condition to itterate among all the contours in the image
	{
		double next_area = 0;
		for (int i = 0; i < contours.size(); i++)	//	condition to itterate among all the contours in the image
		{
			double area = contourArea(contours[i]);

			if ((area > next_area) && (area < max_area)) // condition to compare present contour area and last contour area
			{
				next_area = area;
				max_idx = i;
			}
			
		}
		max_area = next_area;					//Finding the max area of contour 
		idx_arry[i] = max_idx;					// Array which holds the index of contours sorted on their size
	}
	
	vector<Mat> Mask_array;						// Declare a vector of type Mat this is used to store the masks of each contour
	


	for (int i = 0; i < contours.size(); i++)	//	condition to itterate among all the contours in the image
	{
		Mat single_mask = Mat::zeros(binaryImg.size(), CV_8UC1);
		Rect rect = boundingRect(contours[idx_arry[i]]);  // draw the bounding rectangle for each contour
		Mat maskROI(single_mask, rect);					// MArk the region of intereast
		//imwrite("ROI.jpeg", maskROI);
		maskROI = Scalar(0, 0, 0);
		drawContours(single_mask, contours, idx_arry[i], cv::Scalar(255), -1); // draw the contour on the mask image
		double r = (double)countNonZero(maskROI) / (rect.width*rect.height);// Find the ratio of non zero pixels
		if (r > .50 && (rect.height > 8  && rect.width > 10) && (rect.width > 2* rect.height) ) // Condition that specifies that the contour drawn is valid or not
		{
			
			Mask_array.push_back(single_mask.clone()); // If condition satisify store the mask in the array
			//cout << Mask_array.size() << endl;
			//imwrite("mask.jpeg", single_mask);
		}

		
	}
	
	delete[] idx_arry;
	
	vector<Mat> Final_array;			// Declare a vector of type Mat this is used to store the final array
	//cout << Mask_array.size() << endl;
	for (vector<Mat>::iterator i = Mask_array.begin(); i != Mask_array.end(); i++)//	condition to itterate among all the Mask array vector
	{
		Mat and_out = (*i & (grayImg>200));				// Create a output image which can be passed to tessract.		
		Final_array.push_back(and_out.clone());			// push back final array 
		//imwrite("out.jpeg", and_out);
	}
	
	tesseract::TessBaseAPI tess;						// Declaration of TessBaseAPI
	tess.Init("", "eng", tesseract::OEM_DEFAULT);		// Intialization of TessBaseAPI
	tess.SetVariable("tessedit_char_whitelist", "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,");  // Set the variables that are to be extracted from the image
	tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(7);// Set the tesseract page segment mode to single page

	ofstream outputfile;
	outputfile.open("out.dat", ios::out | ios::app);
	int title = 1;
	for (vector<Mat>::iterator i = Final_array.begin(); (i != Final_array.end()) ; i++,title--)//	condition to itterate among all the final array vector
	{
		Mat final = *i;
		//imwrite("final.jpeg", final);
		tess.SetImage((uchar*)final.data, final.cols, final.rows, 1, final.cols);// Set the image ready and pass to tesseract

		char* out = tess.GetUTF8Text();			// Get the extracted data from tesseract and place it in out variable
		string line(out);						//	define a string and copy the data to string variable.
		
		line.erase(remove(line.begin(), line.end(), '\n'), line.end()); // remove '\n' in the text
		if(title>0)
		{
			cout << "Book Title : ";
		}
		else
		{
			cout << "Atributes : ";
		}
		cout << line << endl;
		outputfile << line << ";";			// write title, atributes to the file  with a delimiter";"
	}



	
}

void MouseEvent(int event, int x, int y, int flags, void *Imgsave) // Mouse event function definition
{
	Mat *Img = (Mat *)Imgsave;
	//static int count;
	if (event == EVENT_RBUTTONDOWN && frameSelect == 2)		// If right button event is happened for first time write that flame to bookcover
	{
		BookCover = *Img;
		frameSelect--;
		imwrite("bookCover.jpeg", BookCover);
	}
	else if(event == EVENT_RBUTTONDOWN && frameSelect == 1)	// If right button is happened for second time then page number is stored
	{
		PageNumber = *Img;
		frameSelect--;
		imwrite("pageNumber.jpeg", PageNumber);
	}
	
}


void exitFunction(int flag1)								//Exit function definiton									
{
	char next;


	if (flag1 == EXIT_FLAG)									//If exit flag is equal to -1 execute the exit function to completly exit the program
	{
		cout << "\nI'm Shutting down Thank You...." << endl;
		exit(0);
	}
	else
	{
		cout << "\nWant to perform next operation press y/n [y-for yes n-for no] : ";
		cin >> next;
		flag1 = (next == 'y') ? 0 : EXIT_FLAG;
		if (flag1 == EXIT_FLAG)									//If exit flag is equal to -1 execute the exit function to completly exit the program
		{
			cout << "\nI'm Shutting down Thank You...." << endl;
			exit(0);
		}
		else
			cout << "Your next operation starting" << endl;

	}

}

