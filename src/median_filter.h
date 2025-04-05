#ifndef median_filter_h
#define median_filter_h

#define SIZE 10

class MedianFilter
{

public:
  MedianFilter();

  void insertValue(float newVal);

  float getValue();

protected:
  float bufferRing[SIZE]; /**< Array of the ellemnts in their FIFO order */
  int bufferPtrs[SIZE]; /**< Array of pointers to the next element in ascending order */
  int head,             /**< Position to insert the next element */
      first;            /**< Index of the first pointer */

  void removeOldestValue(int oldestIndex),
      insertNewValue(float newVal);
};
#endif // median_filter_h
