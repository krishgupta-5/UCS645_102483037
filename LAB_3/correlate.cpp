#include "correlate.h"
#include <cmath>
#include <vector>
#include <algorithm>

#ifdef _OPENMP
#include <omp.h>
#endif

static inline double safe_sqrt(double x) {
    return (x <= 0.0) ? 0.0 : std::sqrt(x);
}

static void correlate_seq(int ny, int nx, const float* data, float* result) {
    for (int i = 0; i < ny; ++i) {
        // compute mean and variance for row i (double precision)
        double mean_i = 0.0;
        for (int x = 0; x < nx; ++x) mean_i += (double)data[x + i * nx];
        mean_i /= (double)nx;

        double var_i = 0.0;
        for (int x = 0; x < nx; ++x) {
            double di = (double)data[x + i * nx] - mean_i;
            var_i += di * di;
        }

        for (int j = 0; j <= i; ++j) {
            double mean_j = 0.0;
            for (int x = 0; x < nx; ++x) mean_j += (double)data[x + j * nx];
            mean_j /= (double)nx;

            double var_j = 0.0;
            double cov = 0.0;

            for (int x = 0; x < nx; ++x) {
                double ai = (double)data[x + i * nx] - mean_i;
                double bj = (double)data[x + j * nx] - mean_j;
                cov += ai * bj;
                var_j += bj * bj;
            }

            double denom = safe_sqrt(var_i * var_j);
            double corr = (denom == 0.0) ? 0.0 : (cov / denom);
            result[i + j * ny] = (float)corr;
        }
    }
}
static void correlate_omp(int ny, int nx, const float* data, float* result) {
   
    std::vector<double> mean(ny, 0.0), var(ny, 0.0);

    #pragma omp parallel for schedule(static)
    for (int r = 0; r < ny; ++r) {
        double m = 0.0;
        for (int x = 0; x < nx; ++x) m += (double)data[x + r * nx];
        m /= (double)nx;
        mean[r] = m;

        double v = 0.0;
        for (int x = 0; x < nx; ++x) {
            double d = (double)data[x + r * nx] - m;
            v += d * d;
        }
        var[r] = v;
    }
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < ny; ++i) {
        for (int j = 0; j <= i; ++j) {
            double cov = 0.0;
            const double mi = mean[i];
            const double mj = mean[j];

            // compute covariance
            for (int x = 0; x < nx; ++x) {
                double ai = (double)data[x + i * nx] - mi;
                double bj = (double)data[x + j * nx] - mj;
                cov += ai * bj;
            }

            double denom = safe_sqrt(var[i] * var[j]);
            double corr = (denom == 0.0) ? 0.0 : (cov / denom);
            result[i + j * ny] = (float)corr;
        }
    }
}

static void correlate_opt(int ny, int nx, const float* data, float* result) {
    std::vector<double> Z((size_t)ny * (size_t)nx);
    std::vector<double> norm(ny, 0.0);

    #pragma omp parallel for schedule(static)
    for (int r = 0; r < ny; ++r) {
        double m = 0.0;
        for (int x = 0; x < nx; ++x) m += (double)data[x + r * nx];
        m /= (double)nx;

        double ss = 0.0;
        double* zr = &Z[(size_t)r * (size_t)nx];

        for (int x = 0; x < nx; ++x) {
            double v = (double)data[x + r * nx] - m;
            zr[x] = v;
            ss += v * v;
        }

        double n = safe_sqrt(ss);
        norm[r] = n;

        if (n != 0.0) {
            double inv = 1.0 / n;
            for (int x = 0; x < nx; ++x) zr[x] *= inv;
        } else {
            for (int x = 0; x < nx; ++x) zr[x] = 0.0;
        }
    }

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < ny; ++i) {
        const double* zi = &Z[(size_t)i * (size_t)nx];
        for (int j = 0; j <= i; ++j) {
            const double* zj = &Z[(size_t)j * (size_t)nx];

            double sum0 = 0.0, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
            int x = 0;
            for (; x + 3 < nx; x += 4) {
                sum0 += zi[x] * zj[x];
                sum1 += zi[x + 1] * zj[x + 1];
                sum2 += zi[x + 2] * zj[x + 2];
                sum3 += zi[x + 3] * zj[x + 3];
            }
            double sum = (sum0 + sum1) + (sum2 + sum3);
            for (; x < nx; ++x) sum += zi[x] * zj[x];

            result[i + j * ny] = (float)sum;
        }
    }
}

void correlate(int ny, int nx, const float* data, float* result, int mode) {
    if (mode == 0) correlate_seq(ny, nx, data, result);
    else if (mode == 1) correlate_omp(ny, nx, data, result);
    else correlate_opt(ny, nx, data, result);
}