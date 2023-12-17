#pragma once
#include <iostream>
#include <vector>
#include <boost/math/distributions/fisher_f.hpp>
#include <fstream>


class MANOVA {
    double alpha = 0; // significance level
    size_t N = 0;//total data size
    size_t g = 0;// number of random variables
    size_t k = 0;// number of groups
    std::vector<size_t> ni;// number of data in each group
    std::vector<std::vector<std::vector<double>>> matrix_of_data;
    std::vector<std::vector<double>> Q1;
    std::vector<std::vector<double>> V;

    double get_Fisher_value() { // returns the value of the Fisher distribution

        boost::math::fisher_f dist(2 * (k - 1), 2 * (N - k - 1));
        double Fc = quantile(dist, 1 - alpha);
        return Fc;
    }

    std::vector<double>average_in_the_group(size_t i) {//the average value in the i-th group
        std::vector<double> M(g);
        for (size_t j = 0; j < ni[i]; j++) {
            for (size_t k = 0; k < g; k++) {
                M[k] += matrix_of_data[i][j][k];
            }

        }
        for (size_t k = 0; k < g; k++) {
            M[k] /= ni[i];
        }
        return M;
    }

    std::vector<double> general_averange() {//total average value
        std::vector<double> M(g);
        for (size_t i = 0; i < k; i++) {
            for (size_t j = 0; j < ni[i]; j++) {
                for (size_t k = 0; k < g; k++) {
                    M[k] += matrix_of_data[i][j][k];
                }
            }
        }
        for (size_t k = 0; k < g; k++) {
            M[k] /= N;
        }

        return M;
    }

    std::vector<std::vector<double>> product_of_vectors(const std::vector<double> &x) {
        std::vector<std::vector<double>> res;
        res.resize(g);
        for (size_t i = 0; i < g; i++) {
            res[i].resize(g);
        }

        for (size_t i = 0; i < g; i++) {
            for (size_t j = 0; j < g; j++) {
                res[i][j] = x[i] * x[j];
            }
        }
        return res;
    }

    std::vector<std::vector<double>> add_matrix(const std::vector<std::vector<double>> &x,
        const std::vector<std::vector<double>> &y) {
        std::vector<std::vector<double>> res;
        res.resize(x.size());
        for (size_t i = 0; i < x.size(); i++) {
            res[i].resize(x.size());
        }
        for (size_t i = 0; i < x.size(); i++) {
            for (size_t j = 0; j < x[i].size(); j++) {
                res[i][j] = x[i][j] + y[i][j];
            }
        }
        return res;
    }

    std::vector<double> vector_difference(const std::vector<double> &x, const std::vector<double> &y) {
        std::vector<double> res;
        for (size_t i = 0; i < x.size(); i++) {
            res.push_back(x[i]-y[i]);
        }
        return res;
    }

    void get_Q1(const std::vector<std::vector<double>>& M) {
        Q1.resize(g);
        for (size_t i = 0; i < g; i++) {
            Q1[i].resize(g);
        }
        
        for (size_t i = 0; i < k; i++) {
            for (size_t j = 0; j < ni[i]; j++) {
                std::vector<double> a = vector_difference(matrix_of_data[i][j], M[i]);//(yij-yi)
                std::vector<std::vector<double>> A = product_of_vectors(a);//(a)(a)t
                Q1 = add_matrix(Q1, A);//adding the matrix to Q1

            }
        }
        
    }

    void get_V(std::vector<double> General_M) {
        V.resize(g);
        for (size_t i = 0; i < g; i++) {
            V[i].resize(g);
        }

        for (size_t i = 0; i < k; i++) {
            for (size_t j = 0; j < ni[i]; j++) {
                std::vector<double> a = vector_difference(matrix_of_data[i][j], General_M);//(yij-yi)
                std::vector<std::vector<double>> A = product_of_vectors(a);//(a)(a)t
                V = add_matrix(V, A);//adding the matrix to Q1

            }
        }

    }

    double det(std::vector<std::vector<double>> matrix) {

        double res = 0;
        size_t n = matrix.size();
        if (n == 2) {
            res = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
            return res;
        }

        for (size_t i = 0; i < n; i++) {
            std::vector<std::vector<double>> submatrix(n - 1, std::vector<double>(n - 1, 0));
            for (size_t j = 1; j < n; j++) {
                int k = 0;
                for (size_t l = 0; l < n; l++) {
                    if (l == i) continue;
                    submatrix[j - 1][k++] = matrix[j][l];
                }
            }
            res += (i % 2 == 0 ? 1 : -1) * matrix[0][i] * det(submatrix);
        }
        return res;
    }


public:
    MANOVA(std::vector<std::vector<std::vector<double>>> matrix, double alp=0.05) : matrix_of_data(matrix), alpha(alp) {
        ni.resize(matrix_of_data.size());
        for (size_t i = 0; i < matrix_of_data.size(); i++) {
            ni[i] = matrix_of_data[i].size();
            N += ni[i];
        }
        k = ni.size();
        g = matrix_of_data[0][0].size();
    }


    void analyse() {
        std::ofstream manova_res("Manova_res.txt");
        std::vector<std::vector<double>> M(k);
        for (size_t i = 0; i < k; i++) {
            M[i] = average_in_the_group(i);
            manova_res<< "Среднее значение в " << i + 1 << " группе: {";
                        for (size_t j = 0; j < M[i].size(); j++) {
                            manova_res << M[i][j] << ' ';
                        }
                        manova_res << '}' << '\n';
        }

        std::vector<double> General_M = general_averange();
        manova_res << "Общее среднее значение : {";
        for (size_t j = 0; j < General_M.size(); j++) {
            manova_res << General_M[j] << ' ';
        }
        manova_res << '}' << '\n' << '\n';

        get_Q1(M);
        manova_res << "Матрица Q1 (Residual) (yij-yi.)" << '\n';
        for (size_t i = 0; i < Q1.size(); i++) {
            for (size_t j = 0; j < Q1[i].size(); j++) {
                manova_res << Q1[i][j] << ' ';
            }
            manova_res << '\n';
        }

        get_V(General_M);
        manova_res << "Матрица V (Total) (yij-y..)" << '\n';
        for (size_t i = 0; i < V.size(); i++) {
            for (size_t j = 0; j < V[i].size(); j++) {
                manova_res << V[i][j] << ' ';
            }
            manova_res << '\n';
        }
        manova_res << '\n' << "Определитель Q1:" << det(Q1) << "; Определитель V:" << det(V) << '\n';

        double t = det(Q1) / det(V);
        manova_res << "t:"<<t<<'\n';

        
        double Fs = (1 - pow(t, 0.5)) * (N - k - 1) / ((k - 1) * pow(t, 0.5));
        double Fc = get_Fisher_value();
        manova_res << "Fs:" << Fs << "  v1:" << 2 * (k - 1) << "   v2:" << 2 * (N - k - 1) << '\n'
                   << "Fc:" << Fc << '\n';
        // Проверяем гипотезу об однородности
        if (Fs < Fc) {
            manova_res << "Гипотеза об однородности Ho принимается!" << '\n';
            return;
        }
        else {
            manova_res << "Гипотеза об однородности Ho отвергается!" << '\n' << '\n';

            return;
        }
        manova_res.close();
       
    }

};
