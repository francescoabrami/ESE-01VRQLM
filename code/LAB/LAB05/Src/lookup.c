#include "lookup.h"

const float f_temp_lookup[] = {
	26658.363, // 0'C
	25633.041, // 1'C
	24647.155, // 2'C
	23699.187, // 3'C
	22787.68, // 4'C
	21911.231, // 5'C
	21068.491, // 6'C
	20258.165, // 7'C
	19479.004, // 8'C
	18729.812, // 9'C
	18009.435, // 10'C
	17316.764, // 11'C
	16650.735, // 12'C
	16010.322, // 13'C
	15394.54, // 14'C
	14802.442, // 15'C
	14233.118, // 16'C
	13685.69, // 17'C
	13159.317, // 18'C
	12653.19, // 19'C
	12166.529, // 20'C
	11698.585, // 21'C
	11248.64, // 22'C
	10816.0, // 23'C
	10400.0, // 24'C
	10000.0,	// 25'C
	9600.0, // 26'C
	9216.0, // 27'C
	8847.36, // 28'C
	8493.465, // 29'C
	8153.726, // 30'C
	7827.577, // 31'C
	7514.474, // 32'C
	7213.895, // 33'C
	6925.339, // 34'C
	6648.326, // 35'C
	6382.393, // 36'C
	6127.097, // 37'C
	5882.013, // 38'C
	5646.733, // 39'C
	5420.863, // 40'C
	5204.029, // 41'C
};

const uint32_t u_temp_lookup[] = {
	26658,	// 0'C
	25633,	// 1'C
	24647,	// 2'C
	23699,	// 3'C
	22787,	// 4'C
	21911,	// 5'C
	21068,	// 6'C
	20258,	// 7'C
	19479,	// 8'C
	18729,	// 9'C
	18009,	// 10'C
	17316,	// 11'C
	16650,	// 12'C
	16010,	// 13'C
	15394,	// 14'C
	14802,	// 15'C
	14233,	// 16'C
	13685,	// 17'C
	13159,	// 18'C
	12653,	// 19'C
	12166,	// 20'C
	11698,	// 21'C
	11248,	// 22'C
	10816,	// 23'C
	10400,	// 24'C
	10000,	// 25'C
	9600,	// 26'C
	9216,	// 27'C
	8847,	// 28'C
	8493,	// 29'C
	8153,	// 30'C
	7827,	// 31'C
	7514,	// 32'C
	7213,	// 33'C
	6925,	// 34'C
	6648,	// 35'C
	6382,	// 36'C
	6127,	// 37'C
	5882,	// 38'C
	5646,	// 39'C
	5420,	// 40'C
	5204,	// 41'C
};


/**
 * Takes the value of Rntc32 and returns the corresponding temperature
 * @param value: the resistance of the NTC in ohms multiplied by 10000
 * @return: the temperature in Kelvin multiplied by 10
 */
uint16_t GetTemperature(uint32_t value) {
    // We can do a binary search on the lookup table to find the closest value
    int left = 0;
    int right = sizeof(u_temp_lookup) / sizeof(u_temp_lookup[0]) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (u_temp_lookup[mid] == value) {
            return mid * 10 +  2732; // Exact match
        } else if (u_temp_lookup[mid] > value) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    // After the loop, left is the index of the smallest element greater than value
    // and right is the index of the largest element less than value
	// we interpolate between the two values to find a temperature
	uint16_t temperatureC = 0;

	if (right < 0) {
		temperatureC = 0; // value is smaller than the smallest element in the table
	} else if (left >= sizeof(u_temp_lookup) / sizeof(u_temp_lookup[0])) {
		// value is larger than the largest element in the table
		temperatureC = (sizeof(u_temp_lookup) / sizeof(u_temp_lookup[0]) - 1) * 10;
	} else {
		// Interpolate between right and left all x10 to keep a decimal digit
		uint32_t x0 = u_temp_lookup[right];
		uint32_t x1 = u_temp_lookup[left];
		uint16_t y0 = right * 10;
		uint16_t y1 = left * 10;
		temperatureC = y1 - (value - x1) * (y1 - y0) / (x0 - x1);
	}

	return temperatureC + 2732; // convert to Kelvin and multiply by 10
}