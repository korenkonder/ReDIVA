/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sort_array.h"

void sort_array(size_t* array, size_t length) {
    size_t i, j, l, m;

    if (length <= 0)
        return;
    else if (length == 1) {
        *array = 0;
        return;
    }

    array[0] = 0;
    i = 1;
    j = 1;

    m = 1;
    while (m < length)
        m *= 10;
    l = 0;
    while (i < length) {
        if (j * 10 < m) {
            array[i++] = j;
            j *= 10;
        }
        else if (j >= length) {
            m /= 10;
            j /= 10;
            array[i++] = ++j;
            if (j * 10 >= length)
                j++;
        }
        else if (j % 10 != 9)
            array[i++] = j++;

        if (i < length)
            if (j == length && j % 10 != 9) {
                m /= 10;
                j /= 10;
                while (j % 10 == 9) j /= 10;
                j++;
            }
            else if (j < length && j % 10 == 9) {
                array[i++] = j;
                while (j % 10 == 9) j /= 10;
                j++;
            }
    }
}
