/// Holds the available calculation mode functions. 
class CalculationModes
{
public:
    /// Returns the average of the given buffer, the size parameter indicates the size of the float pointer array.
    static float Average(float *buffer, int size);
    /// Returns the mode of the given buffer, the size parameter indicates the size of the float pointer array.
    static float Mode(float *buffer, int size);
    /// Returns the median of the given buffer, the size parameter indicates the size of the float pointer array.
    static float Median(float *buffer, int size);
};
