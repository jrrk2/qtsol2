// AcceleratedMatrix.hpp - High-performance matrix class using Accelerate Framework
#ifndef ACCELERATEDMATRIX_HPP
#define ACCELERATEDMATRIX_HPP

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>

class AcceleratedMatrix {
private:
    std::vector<double> data;  // Column-major storage for BLAS compatibility
    size_t rows, cols;
    
    // Convert (row, col) to linear index in column-major order
    size_t getIndex(size_t r, size_t c) const {
        return c * rows + r;
    }

public:
    // Constructors
    AcceleratedMatrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows * cols, 0.0);
    }
    
    AcceleratedMatrix(const std::vector<std::vector<double>>& input) {
        rows = input.size();
        cols = rows > 0 ? input[0].size() : 0;
        data.resize(rows * cols);
        
        // Convert to column-major storage
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[getIndex(i, j)] = input[i][j];
            }
        }
    }

    // Accessors
    double get(size_t r, size_t c) const {
        if (r >= rows || c >= cols) throw std::out_of_range("Matrix index out of range");
        return data[getIndex(r, c)];
    }
    
    void set(size_t r, size_t c, double value) {
        if (r >= rows || c >= cols) throw std::out_of_range("Matrix index out of range");
        data[getIndex(r, c)] = value;
    }
    
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }
    
    // Raw data access for BLAS operations
    double* getData() { return data.data(); }
    const double* getData() const { return data.data(); }

#ifdef __APPLE__
    // High-performance matrix multiplication using Accelerate BLAS
    AcceleratedMatrix multiplyAccelerate(const AcceleratedMatrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions incompatible for multiplication");
        }
        
        AcceleratedMatrix result(rows, other.cols);
        
        // BLAS parameters
        const double alpha = 1.0, beta = 0.0;
        const int m = static_cast<int>(rows);
        const int n = static_cast<int>(other.cols);
        const int k = static_cast<int>(cols);
        const int lda = m, ldb = static_cast<int>(other.rows), ldc = m;
        
        // Call Accelerate BLAS dgemm
        cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans,
                    m, n, k, alpha,
                    getData(), lda,
                    other.getData(), ldb,
                    beta, result.getData(), ldc);
        
        return result;
    }
    
    // Matrix-vector multiplication using Accelerate BLAS
    std::vector<double> multiplyVector(const std::vector<double>& vec) const {
        if (cols != vec.size()) {
            throw std::invalid_argument("Vector size incompatible with matrix columns");
        }
        
        std::vector<double> result(rows, 0.0);
        
        const double alpha = 1.0, beta = 0.0;
        const int m = static_cast<int>(rows);
        const int n = static_cast<int>(cols);
        const int incx = 1, incy = 1;
        
        cblas_dgemv(CblasColMajor, CblasNoTrans,
                    m, n, alpha,
                    getData(), m,
                    vec.data(), incx,
                    beta, result.data(), incy);
        
        return result;
    }
    
    // LU factorization using Accelerate LAPACK
    std::pair<AcceleratedMatrix, std::vector<int>> luFactorization() const {
        if (rows != cols) throw std::invalid_argument("LU factorization requires square matrix");
        
        AcceleratedMatrix lu_matrix = *this;  // Copy
        std::vector<int> pivots(std::min(rows, cols));
        
        int m = static_cast<int>(rows);
        int n = static_cast<int>(cols);
        int lda = m;
        int info;
        
        // Call LAPACK LU factorization
        dgetrf_(&m, &n, lu_matrix.getData(), &lda, pivots.data(), &info);
        
        if (info < 0) {
            throw std::runtime_error("LAPACK dgetrf: illegal parameter at position " + std::to_string(-info));
        } else if (info > 0) {
            throw std::runtime_error("Matrix is singular: U[" + std::to_string(info-1) + "," + std::to_string(info-1) + "] = 0");
        }
        
        return {lu_matrix, pivots};
    }
    
    // Matrix inversion using Accelerate LAPACK
    AcceleratedMatrix inverse() const {
        if (rows != cols) throw std::invalid_argument("Only square matrices can be inverted");
        
        // First, perform LU factorization
        auto [lu_matrix, pivots] = luFactorization();
        
        int n = static_cast<int>(rows);
        int lda = n;
        int info;
        
        // Query optimal workspace size
        double work_query;
        int lwork = -1;
        dgetri_(&n, lu_matrix.getData(), &lda, pivots.data(), &work_query, &lwork, &info);
        
        // Allocate workspace and compute inverse
        lwork = static_cast<int>(work_query);
        std::vector<double> work(lwork);
        
        dgetri_(&n, lu_matrix.getData(), &lda, pivots.data(), work.data(), &lwork, &info);
        
        if (info < 0) {
            throw std::runtime_error("LAPACK dgetri: illegal parameter at position " + std::to_string(-info));
        } else if (info > 0) {
            throw std::runtime_error("Matrix is singular and cannot be inverted");
        }
        
        return lu_matrix;
    }
    
    // Eigenvalue computation using Accelerate LAPACK
    std::pair<std::vector<double>, std::vector<double>> eigenvalues() const {
        if (rows != cols) throw std::invalid_argument("Eigenvalues only defined for square matrices");
        
        AcceleratedMatrix a_copy = *this;  // LAPACK modifies input
        
        int n = static_cast<int>(rows);
        char jobvl = 'N', jobvr = 'N';  // Don't compute eigenvectors
        int lda = n, ldvl = 1, ldvr = 1;
        
        std::vector<double> wr(n), wi(n);  // Real and imaginary parts
        double* vl = nullptr;
        double* vr = nullptr;
        
        // Query optimal workspace size
        double work_query;
        int lwork = -1;
        int info;
        
        dgeev_(&jobvl, &jobvr, &n, a_copy.getData(), &lda,
               wr.data(), wi.data(), vl, &ldvl, vr, &ldvr,
               &work_query, &lwork, &info);
        
        // Compute eigenvalues
        lwork = static_cast<int>(work_query);
        std::vector<double> work(lwork);
        
        dgeev_(&jobvl, &jobvr, &n, a_copy.getData(), &lda,
               wr.data(), wi.data(), vl, &ldvl, vr, &ldvr,
               work.data(), &lwork, &info);
        
        if (info < 0) {
            throw std::runtime_error("LAPACK dgeev: illegal parameter at position " + std::to_string(-info));
        } else if (info > 0) {
            throw std::runtime_error("Eigenvalue computation failed to converge");
        }
        
        return {wr, wi};
    }
    
    // Solve linear system Ax = b using LAPACK
    std::vector<double> solve(const std::vector<double>& b) const {
        if (rows != cols) throw std::invalid_argument("Coefficient matrix must be square");
        if (b.size() != rows) throw std::invalid_argument("Right-hand side vector size mismatch");
        
        AcceleratedMatrix a_copy = *this;
        std::vector<double> x = b;  // Solution overwrites RHS
        std::vector<int> pivots(rows);
        
        int n = static_cast<int>(rows);
        int nrhs = 1;
        int lda = n, ldb = n;
        int info;
        
        // Solve using LU factorization
        dgesv_(&n, &nrhs, a_copy.getData(), &lda, pivots.data(),
               x.data(), &ldb, &info);
        
        if (info < 0) {
            throw std::runtime_error("LAPACK dgesv: illegal parameter at position " + std::to_string(-info));
        } else if (info > 0) {
            throw std::runtime_error("Matrix is singular: cannot solve system");
        }
        
        return x;
    }
    
    // QR decomposition using Accelerate LAPACK
    std::pair<AcceleratedMatrix, AcceleratedMatrix> qrDecomposition() const {
        AcceleratedMatrix a_copy = *this;
        
        int m = static_cast<int>(rows);
        int n = static_cast<int>(cols);
        int lda = m;
        int min_mn = std::min(m, n);
        
        std::vector<double> tau(min_mn);
        
        // Query optimal workspace size
        double work_query;
        int lwork = -1;
        int info;
        
        dgeqrf_(&m, &n, a_copy.getData(), &lda, tau.data(), &work_query, &lwork, &info);
        
        // Perform QR factorization
        lwork = static_cast<int>(work_query);
        std::vector<double> work(lwork);
        
        dgeqrf_(&m, &n, a_copy.getData(), &lda, tau.data(), work.data(), &lwork, &info);
        
        if (info != 0) {
            throw std::runtime_error("QR decomposition failed");
        }
        
        // Extract R matrix (upper triangular part)
        AcceleratedMatrix R(min_mn, cols);
        for (size_t i = 0; i < min_mn; ++i) {
            for (size_t j = i; j < cols; ++j) {
                R.set(i, j, a_copy.get(i, j));
            }
        }
        
        // Generate Q matrix
        AcceleratedMatrix Q = a_copy;  // Start with the factored form
        
        dorgqr_(&m, &min_mn, &min_mn, Q.getData(), &lda, tau.data(),
                work.data(), &lwork, &info);
        
        if (info != 0) {
            throw std::runtime_error("Q matrix generation failed");
        }
        
        return {Q, R};
    }
    
  //  SVDResult svd() const;
  
#endif // __APPLE__
    
    // Basic operations (fallback when Accelerate not available)
    AcceleratedMatrix multiply(const AcceleratedMatrix& other) const {
#ifdef __APPLE__
        return multiplyAccelerate(other);
#else
        // Fallback to basic implementation
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions incompatible for multiplication");
        }
        
        AcceleratedMatrix result(rows, other.cols);
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < cols; ++k) {
                    sum += get(i, k) * other.get(k, j);
                }
                result.set(i, j, sum);
            }
        }
        return result;
#endif
    }
    
    AcceleratedMatrix add(const AcceleratedMatrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions must match for addition");
        }
        
        AcceleratedMatrix result(rows, cols);
        
#ifdef __APPLE__
        // Use Accelerate vDSP for vector addition
        vDSP_vaddD(getData(), 1, other.getData(), 1, result.getData(), 1, rows * cols);
#else
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] + other.data[i];
        }
#endif
        return result;
    }
    
    AcceleratedMatrix subtract(const AcceleratedMatrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions must match for subtraction");
        }
        
        AcceleratedMatrix result(rows, cols);
        
#ifdef __APPLE__
        // Use Accelerate vDSP for vector subtraction
        vDSP_vsubD(other.getData(), 1, getData(), 1, result.getData(), 1, rows * cols);
#else
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] - other.data[i];
        }
#endif
        return result;
    }
    
    AcceleratedMatrix scale(double factor) const {
        AcceleratedMatrix result(rows, cols);
        
#ifdef __APPLE__
        // Use Accelerate vDSP for scalar multiplication
        vDSP_vsmulD(getData(), 1, &factor, result.getData(), 1, rows * cols);
#else
        for (size_t i = 0; i < data.size(); ++i) {
            result.data[i] = data[i] * factor;
        }
#endif
        return result;
    }
    
    AcceleratedMatrix transpose() const {
        AcceleratedMatrix result(cols, rows);
        
#ifdef __APPLE__
        // Use Accelerate vDSP for matrix transpose
        vDSP_mtransD(getData(), 1, result.getData(), 1, cols, rows);
#else
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.set(j, i, get(i, j));
            }
        }
#endif
        return result;
    }
    
    double determinant() const {
        if (rows != cols) throw std::invalid_argument("Determinant only defined for square matrices");
        
#ifdef __APPLE__
        try {
            auto [lu_matrix, pivots] = luFactorization();
            
            // Determinant is product of diagonal elements, adjusted for pivoting
            double det = 1.0;
            int sign_changes = 0;
            
            for (size_t i = 0; i < rows; ++i) {
                det *= lu_matrix.get(i, i);
                if (pivots[i] != static_cast<int>(i + 1)) {  // LAPACK uses 1-based indexing
                    sign_changes++;
                }
            }
            
            return (sign_changes % 2 == 0) ? det : -det;
        } catch (...) {
            return 0.0;  // Singular matrix
        }
#else
        // Fallback: simple recursive determinant (not efficient for large matrices)
        if (rows == 1) return get(0, 0);
        if (rows == 2) return get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0);
        
        double det = 0.0;
        for (size_t c = 0; c < cols; ++c) {
            AcceleratedMatrix minor = getMinor(0, c);
            double cofactor = (c % 2 == 0 ? 1 : -1) * get(0, c) * minor.determinant();
            det += cofactor;
        }
        return det;
#endif
    }

    // Utility methods
    void fillRandom(double min = 0.0, double max = 1.0) {
        // Seed only once
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }
        
        for (double& val : data) {
            double random_01 = static_cast<double>(std::rand()) / RAND_MAX;
            val = min + (max - min) * random_01;
        }
    }

    void fillIdentity() {
        if (rows != cols) throw std::invalid_argument("Identity matrix must be square");
        
        std::fill(data.begin(), data.end(), 0.0);
        for (size_t i = 0; i < rows; ++i) {
            set(i, i, 1.0);
        }
    }
    
    double norm() const {
#ifdef __APPLE__
        // Use Accelerate vDSP for efficient norm calculation
        double result;
        vDSP_svesqD(getData(), 1, &result, data.size());
        return std::sqrt(result);
#else
        double sum = 0.0;
        for (double val : data) {
            sum += val * val;
        }
        return std::sqrt(sum);
#endif
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "AcceleratedMatrix " << rows << "x" << cols << ":\n";
        ss << std::fixed << std::setprecision(6);
        
        for (size_t i = 0; i < rows; ++i) {
            ss << "[";
            for (size_t j = 0; j < cols; ++j) {
                ss << std::setw(10) << get(i, j);
                if (j < cols - 1) ss << " ";
            }
            ss << "]\n";
        }
        return ss.str();
    }

private:
    AcceleratedMatrix getMinor(size_t row, size_t col) const {
        AcceleratedMatrix minor(rows - 1, cols - 1);
        size_t minor_row = 0;
        
        for (size_t i = 0; i < rows; ++i) {
            if (i == row) continue;
            size_t minor_col = 0;
            
            for (size_t j = 0; j < cols; ++j) {
                if (j == col) continue;
                minor.set(minor_row, minor_col, get(i, j));
                minor_col++;
            }
            minor_row++;
        }
        return minor;
    }
};

#endif // ACCELERATEDMATRIX_HPP
