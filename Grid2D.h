/************************ Grid2D.h ************************************
Author: Rusty Sammon
Purpose: This class is useful if you need to store a 2D array of booleans, ints,
floats, or any other data type for that matter.
I find that I often need to do this for edge tracing, and am sick of writing
the same x*row_width*y code over and over again.  It's pretty basic.

*************************************************************************/
#pragma once

/***********************************************************************/
template <class datatype>
class Grid2D {
public:
    //member variables
    datatype *data;
    int width;
    int height;

    //Constructors / Destructors
    Grid2D();
    Grid2D(const Grid2D &source);
    Grid2D(const int newWidth, const int newHeight);
    Grid2D(const int newWidth, const int newHeight, const datatype initValue);
    ~Grid2D();

    //Initialization
    void SetSize(const int newWidth, const int newHeight);
    void Initialize(datatype initValue);
    
    //overloaded operators
    datatype & operator() (const int x, const int y) const;
    const Grid2D<datatype> & operator= (const Grid2D<datatype> &source);

    //other helpful functions
    const datatype & GetValue (const int x, const int y) const;
    bool InGrid (const int x, const int y) const;
    void QuickSwap( Grid2D &grid2);
};

/***********************************************************************/
template <class datatype>
Grid2D<datatype>::Grid2D()
{
    width = 0;
    height = 0;
    data = NULL;
}

/***********************************************************************/
template <class datatype>
Grid2D<datatype>::Grid2D(const Grid2D<datatype> &source)
{
    width = 0;
    height = 0;
    data = NULL;
    (*this) = source;
}

/***********************************************************************/
template <class datatype>
Grid2D<datatype>::Grid2D(const int newWidth, const int newHeight)
{
    width = newWidth;
    height = newHeight;
    data = new datatype [width*height];
}

/***********************************************************************/
template <class datatype>
Grid2D<datatype>::Grid2D (const int newWidth, 
                          const int newHeight, 
                          const datatype initValue)
{
    width = newWidth;
    height = newHeight;
    data = new datatype [width*height];
    Initialize (initValue);
}

/***********************************************************************/
template <class datatype>
Grid2D<datatype>::~Grid2D()
{
    delete [] data;
    data = NULL;
    width = 0;
    height = 0;
}

/***********************************************************************/
template <class datatype>
datatype & Grid2D<datatype>::operator() (const int x, const int y) const
{
    return (data[x + y*width]);
}

/***********************************************************************/
template <class datatype>
const datatype & Grid2D<datatype>::GetValue (const int x, const int y) const
{
    return (data[x + y*width]);
}

/***********************************************************************/
template <class datatype>
void Grid2D<datatype>::SetSize(const int newWidth, const int newHeight)
{
    if (width != newWidth || height != newHeight)
	{
        if (data != NULL) delete [] data;
        width = newWidth;
        height = newHeight;
        data = new datatype [width*height];
    }
}

/***********************************************************************/
template <class datatype>
void Grid2D<datatype>::Initialize(const datatype initValue)
{
    for (int i=0; i<width; i++)
        for (int j=0; j<height; j++) 
        {
            (*this)(i,j) = initValue;
        }
}

/***********************************************************************/
template <class datatype>
const Grid2D<datatype> & Grid2D<datatype>::operator= (const Grid2D<datatype> &source)
{
	if (&source != this)
	{
        //reallocate memory if necessary
        SetSize(source.width, source.height);

        //copy the data from the source grid
        //note that memcopy might not work if we have a class as a datatype
        for (int i=0; i<width; i++)
            for (int j=0; j<height; j++)
            {
                (*this)(i,j) = source(i,j);
            }
    }
    return *this;
}

/***********************************************************************/
template <class datatype>
bool Grid2D<datatype>::InGrid (const int x, const int y) const
{
    return ((x>=0) && (y>=0) && (x<width) && (y<height));
}

/***********************************************************************/
template <class datatype>
void Grid2D<datatype>::QuickSwap( Grid2D<datatype> &grid2 )
/* Swaps the data and sizes of the current grid with the second grid.
   This is significantly faster than using the = operator, which performs
   a deep copy.
 */
{   
    //make temp copies of the member variables for this grid
    int tempWidth = width;
    int tempHeight = height;        
    datatype *tempData = data;

    //set this grid's member variables to those of grid2
    width = grid2.width;
    height = grid2.height;
    data = grid2.data;

    //complete the swap by setting grid2's member variables to the temp values
    grid2.width = tempWidth;
    grid2.height = tempHeight;
    grid2.data = tempData;
}
