#include "CircularArray.h"

CircularArray::CircularArray(int max_size, int allocated_size) {
	start = 0;
	size = 0;
	index = 0;
	maxSize = max_size;
	
	if (allocated_size == -1)
		allocated_size = maxSize;
		
	data = new float[allocated_size];
	
	for (int i = 0; i < allocated_size; i++)
		data[i] = 0;
}

CircularArray::~CircularArray() {
	delete [] data;
}

void CircularArray::addValue(float value) {	
	data[index] = value;
	
	if (size == maxSize)
		start = (start + 1) % maxSize;
	else
		size ++;
	
	index = (index + 1) % maxSize;
}

int CircularArray::getSize() {
	return size;
}

int CircularArray::getMaxSize() {
	return maxSize;
}

int CircularArray::getStart() {
	return start;
}

int CircularArray::getIndex() {
	return index;
}

float CircularArray::getValue(int offset) {
	return data[(start + offset) % maxSize];
}
