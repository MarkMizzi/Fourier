#ifndef __KERNEL_H_
#define __KERNEL_H_

#include <vector>
#include <cstdlib>
#include <cmath>

class GaussianKernel : public std::vector<std::vector<float>> {
    public:
        GaussianKernel(float std_dev, ssize_t kern_size_f) :
            std::vector<std::vector<float>>(2 * kern_size_f + 1,
                                            std::vector<float>(2 * kern_size_f + 1,
                                                               0)) {
            float p = 2 * std_dev * std_dev;
            float q = 2 * M_PI * std_dev * std_dev;
            for (ssize_t i = 0; i < 2 * kern_size_f + 1; i++)
                for (ssize_t j = 0; j < 2 * kern_size_f + 1; j++)
                    (*this)[j][i] = exp(-((i - kern_size_f) * (i - kern_size_f) +
                                          (j - kern_size_f) * (j - kern_size_f)) / p) / q;
        }

        void normalize() {
            float sum = 0;
            for (auto it1 = begin(); it1 != end(); ++it1)
                for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
                    sum += *it2;
            for (auto it1 = begin(); it1 != end(); ++it1)
                for (auto it2 = it1->begin(); it2 != it1->end(); ++it2)
                    (*it2) /= sum;
        }

};

class GaussianRow : public std::vector<std::vector<float>> {
    public:
        GaussianRow(float std_dev, ssize_t kern_size_f) :
            std::vector<std::vector<float>>(1,
                                            std::vector<float>(2 * kern_size_f + 1,
                                                               0)) {
            float p = 2 * std_dev * std_dev;
            float q = sqrt(2 * M_PI * std_dev * std_dev);

            for (ssize_t i = 0; i < 2 * kern_size_f + 1; i++)
                (*this)[0][i] = exp(-(i - kern_size_f) / p) / q;
        }

        void normalize() {
            float sum = 0;
            for (auto it = (*this)[0].begin(); it != (*this)[0].end(); ++it)
                sum += *it;
            for (auto it = (*this)[0].begin(); it != (*this)[0].end(); ++it)
                (*it) /= sum;
        }

};

class GaussianColumn : public std::vector<std::vector<float>> {
    public:
        GaussianColumn(float std_dev, ssize_t kern_size_f) :
            std::vector<std::vector<float>>(2 * kern_size_f + 1,
                                            std::vector<float>(1,
                                                               0)) {
            float p = 2 * std_dev * std_dev;
            float q = sqrt(2 * M_PI * std_dev * std_dev);

            for (ssize_t i = 0; i < 2 * kern_size_f + 1; i++)
                (*this)[i][0] = exp(-(i - kern_size_f) / p) / q;
        }

        void normalize() {
            float sum = 0;
            for (auto it = begin(); it != end(); ++it)
                sum += (*it)[0];
            for (auto it = begin(); it != end(); ++it)
                (*it)[0] /= sum;
        }
};

class GaussianXDerivativeKernel : public std::vector<std::vector<float>> {
    public:
        GaussianXDerivativeKernel(float std_dev,
                                  ssize_t kern_size_f) :
            std::vector<std::vector<float>>(2 * kern_size_f + 1,
                                            std::vector<float>(2 * kern_size_f + 1,
                                                               0)) {

            float p = 2 * std_dev * std_dev;

#define NORMAL_FACTOR (sqrt(2) / sqrt(M_PI) * std_dev)
            float q = 2 * M_PI * pow(std_dev, 4) * NORMAL_FACTOR;
#undef NORMAL_FACTOR

            for (ssize_t i = 0; i < 2 * kern_size_f + 1; i++)
                for (ssize_t j = 0; j < 2 * kern_size_f + 1; j++)
                    (*this)[j][i] = (kern_size_f - i) *
                        exp(-((i - kern_size_f) * (i - kern_size_f) +
                              (j - kern_size_f) * (j - kern_size_f)) / p) / q;
        }
};

class GaussianYDerivativeKernel : public std::vector<std::vector<float>> {
    public:
        GaussianYDerivativeKernel(float std_dev,
                                  ssize_t kern_size_f) :
            std::vector<std::vector<float>>(2 * kern_size_f + 1,
                                            std::vector<float>(2 * kern_size_f + 1,
                                                               0)) {

            float p = 2 * std_dev * std_dev;

#define NORMAL_FACTOR (sqrt(2) / sqrt(M_PI) * std_dev)
           float q = 2 * M_PI * pow(std_dev, 4) * NORMAL_FACTOR;
#undef NORMAL_FACTOR

            for (ssize_t i = 0; i < 2 * kern_size_f + 1; i++)
                for (ssize_t j = 0; j < 2 * kern_size_f + 1; j++)
                    (*this)[j][i] = (kern_size_f - j) *
                        exp(-((i - kern_size_f) * (i - kern_size_f) +
                              (j - kern_size_f) * (j - kern_size_f)) / p) / q;
        }
};

#endif // __KERNEL_H_
