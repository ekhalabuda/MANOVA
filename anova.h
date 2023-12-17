#pragma once
#include <iostream>
#include <vector>
#include <boost/math/distributions/fisher_f.hpp>
#include <fstream>

class ANOVA {
    double alpha = 0; // significance level
    size_t N = 0;//total data number
    size_t k = 0;//  of groups
    std::vector<size_t> ni;// number of data in each group
    std::vector<std::vector<double>> matrix_of_data;
    double Q = 0;//general deviation
    double Q1 = 0;//deviations in the group
    double Q2 = 0;//deviations between groups

    double get_Fisher_value() { // returns the value of the Fisher distribution
        boost::math::fisher_f dist(k - 1, N - k);
        double Fc = quantile(dist, 1 - alpha);
        return Fc;
    }

    double average_in_the_group(size_t i) {//the average value in the i-th group
        double M = 0;
        for (size_t j = 0; j < ni[i]; j++) {
            M += matrix_of_data[i][j];
        }
        M /= ni[i];
        return M;
    }

    double general_averange() {//total average value
        double M = 0;
        for (size_t i = 0; i < k; i++) {
            for (size_t j = 0; j < ni[i]; j++) {
                M += matrix_of_data[i][j];
            }
        }
        M /= N;
        return M;
    }

    void get_Q(double General_M) {
        for (size_t i = 0; i < k; i++) {
            for (size_t j = 0; j < ni[i]; j++) {
                Q += pow((matrix_of_data[i][j] - General_M), 2);
            }
        }
    }
    void get_Q1(std::vector<double> M) {
        for (size_t i = 0; i < k; i++) {
            for (size_t j = 0; j < ni[i]; j++) {
                Q1 += pow((matrix_of_data[i][j] - M[i]), 2);
            }
        }
    }

    void get_Q2(double General_M, std::vector<double> M) {
        for (size_t i = 0; i < k; i++) {
            Q2 += ni[i] * pow((General_M - M[i]), 2);
        }
    }
    void get_Qs(double General_M, std::vector<double> M) {
        get_Q(General_M);
        get_Q1(M);
        get_Q2(General_M, M);
    }

    void Tukey_Kramer( std::vector<double> M, double s2_1, double Fc, std::ofstream & anova_res) {
        for (size_t i = 0; i < k; i++) {
            for (size_t j = i + 1; j < k; j++) {
                // consider the modules of differences
                double d = abs(M[i] - M[j]);
                anova_res << "Модуль разности:" << d << '\n';
                // let's compare the critical scope
                double mu = Fc * pow(s2_1 / 2 * ((double)1 / ni[i] + (double)1 / ni[j]), 0.5);
                if (d < mu) {
                    anova_res<< " Группы " << i + 1 << " и " << j + 1 << " не различаются!" << '\n';

                }
                else {
                    anova_res << " Группы " << i + 1 << " и " << j + 1 << " различаются!" << '\n';

                }
            }
        }
        return;
    }
public:
    ANOVA(std::vector<std::vector<double>> matrix, double al = 0.05) : alpha(al), matrix_of_data(matrix) {
        ni.resize(matrix_of_data.size());
        for (size_t i = 0; i < matrix_of_data.size(); i++) {
            ni[i] = matrix_of_data[i].size();
            N += ni[i];
        }
        k = ni.size();

    }
    void analyse() {
        std::ofstream anova_res( "Anova_res.txt" );
        std::vector<double> M(k);
        for (size_t i = 0; i < k; i++) {
            M[i] = average_in_the_group(i);
            anova_res << "Среднее значение в " << i+1 << " группе:" << M[i] << '\n';
        }
        double General_M = general_averange();
        anova_res << "Общее среднее значение:" << General_M << '\n';
        get_Qs(General_M, M);
        double s2_1 = Q1 / (N - k);
        double s2_2 = Q2 / (k - 1);
        double Fs = s2_2 / s2_1;
        double Fc = get_Fisher_value();
        anova_res << "Q1:" << Q1 << '\n' << "Q2:" << Q2 << '\n'
                  << "s2_2/s2_1:" << Fs << "  k-1:" << k - 1 << "   N-k:" << N - k << '\n'
                  << "Табличное критическое значение распределения Фишера:" << Fc << '\n';
        // We check the hypothesis of uniformity H0
        if (Fs < Fc) {
            anova_res << "Гипотеза об однородности Ho принимается!" << '\n';
            return;
        }
        else {
            anova_res << "Гипотеза об однородности Ho отвергается!" << '\n' << '\n'
                      << "Применим процедуру Тьюки-Крамера!" << '\n';
            // The Tukey-Kramer procedure
            Tukey_Kramer( M, s2_1, Fc, anova_res);
            return;
        }
        anova_res.close();
    }

};

