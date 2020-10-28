#pragma once

#include <array>

#include "image.h"

template <typename T, std::size_t N>
class Mask {
	static_assert(N % 2 == 1, "Size of mask must be odd!");

public:
	Mask();
	Mask(const T (&values)[N][N]);

	// Convolution
	Image operator*(const Image& image) const;

	T values[N][N];
	// A sum of all the positive and negative values in the mask, respectively
	// Used for remapping to [0,255] after convolution
	const T& posSum = _posSum;
	const T& negSum = _negSum;

private:
	T _posSum;
	T _negSum;
};

template <typename T, std::size_t N>
Mask<T, N>::Mask() {}

template <typename T, std::size_t N>
Mask<T, N>::Mask(const T (&values)[N][N]) {
	_posSum = _negSum = 0;
	for (unsigned i = 0; i < N; i++) {
		for (unsigned j = 0; j < N; j++) {
			Mask<T, N>::values[i][j] = values[i][j];
			(values[i][j] < 0 ? _negSum : _posSum) += values[i][j];
		}
	}
}

template <typename T, std::size_t N>
Image Mask<T, N>::operator*(const Image& image) const {
	Image re(image.rows, image.cols, image.maxVal);

	// x and y (and u and v) are interchanged from normal for cache locality purposes
#pragma omp parallel for collapse(2)
	for (unsigned y = 0; y < re.rows; y++) {
		for (unsigned x = 0; x < re.cols; x++) {
			// Keep track of a running sum in a temporary variable instead of in the
			// return image, since we probably have higher precision in T than in
			// pixelT.
			T sum = 0;

			for (unsigned v = 0; v < N; v++) {
				// Clamp the convolution to the image, "extending" the image by
				// its border pixels
				unsigned v_mod = (y + N / 2 < v) ?
				                     0 :
				                     ((y + N / 2 >= v + re.rows) ? (re.rows - 1) :
				                                                   (y - v + N / 2));
				for (unsigned u = 0; u < N; u++) {
					// Clamp u too
					unsigned u_mod = (x + N / 2 < u) ? 0 :
					                                   ((x + N / 2 >= u + re.cols) ?
					                                        (re.cols - 1) :
					                                        (x - u + N / 2));

					sum += values[v][u] * image[v_mod][u_mod];
				}
			}

			// re-map to [0,255]
			re[y][x] = (sum - _negSum * re.maxVal) / (_posSum - _negSum);
		}
	}

	return re;
}