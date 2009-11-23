#ifndef _CIRCULARARRAY_H
#define _CIRCULARARRAY_H

// Circular array allows values to be added and simply replace older values if the maximum size is reached.
class CircularArray {
private:		
	float* data;
	
	int size;			// Current size of the array.
	int maxSize;		// Maximum size before wrapping to the front of the array.
	int allocatedSize;	// Total space allocated for the array (>= maxSize), for zero-padding.
	int start;			// Pointer to the start of the array. Not always zero.
	int index;			// Pointer to the last element of the array.
	
public:
	CircularArray(int max_size = 1, int allocated_size = -1);
	~CircularArray();
	
	void addValue(float value);
	
	int getSize();
	int getMaxSize();
	int getStart();
	int getIndex();
			
	float getUnorderedValue(int offset);
	float getValue(int offset);
	
	float* getData() {
		return data;
	}
};

#endif
