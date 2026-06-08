#!/bin/python3
import math
def incr(oldtemp, oldres, depth, out):
    if depth > 70:
        return out
    r = oldres - oldres * 4 / 100
    floored = math.floor(r)
    t = oldtemp + 1
    out += f"\t{floored},\t// {t}'C\n"
    return incr(t, r, depth + 1, out)

def decr(oldtemp, oldres, depth, out):
    if depth < 1:
        return out
    r = oldres + oldres * 4 / 100
    floored = math.floor(r)
    t = oldtemp - 1
    out = decr(t, r, depth - 1, out)
    ret = out + f"\t{floored},\t// {t}'C\n"
    return ret

def incr_float(oldtemp, oldres, depth, out):
    if depth > 70:
        return out
    r = oldres - oldres * 4 / 100
    floored = math.floor(r * 1000) / 1000.0
    t = oldtemp + 1
    out += f"\t{floored}, // {t}'C\n"
    return incr_float(t, r, depth + 1, out)   # note: call incr_float, not incr

def decr_float(oldtemp, oldres, depth, out):
    if depth < 1:
        return out
    r = oldres + oldres * 4 / 100
    floored = math.floor(r * 1000) / 1000.0
    t = oldtemp - 1
    out = decr_float(t, r, depth - 1, out)   # call decr_float recursively
    ret = out + f"\t{floored}, // {t}'C\n"
    return ret

def get_function_string():
    func = """/**
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
}"""

    return func

def get_header_string():
    header = """#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdint.h>

uint16_t GetTemperature(uint32_t value);

#endif // LOOKUP_H"""
    return header

def main():
    start_temp = 25
    start_res = 10000
    out = '#include "lookup.h"\n\n'
    out += "const float f_temp_lookup[] = {\n"
    out = decr_float(start_temp, start_res, start_temp, out)      # goes down to 0
    out += f"\t{start_res}.0,\t// {start_temp}'C\n"
    out = incr_float(start_temp, start_res, start_temp + 0, out)  # grows up to 40
    out += "};"

    out += "\n\nconst uint32_t u_temp_lookup[] = {\n"
    out = decr(start_temp, start_res, start_temp, out)      # goes down to 0
    out += f"\t{start_res},\t// {start_temp}'C\n"
    out = incr(start_temp, start_res, start_temp + 0, out)  # grows up to 40
    out += "};\n\n\n"

    out += get_function_string()

    return out



if __name__ == "__main__":
    resultc = main()
    filec = open("Src/lookup.c", 'w')
    filec.write(resultc)
    filec.close()

    resulth = get_header_string()
    fileh = open("Inc/lookup.h", 'w')
    fileh.write(resulth)
    fileh.close()