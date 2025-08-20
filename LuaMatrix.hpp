// Matrix Processing Acceleration Options for Sol2 + Qt

// Option 1: Basic C++ Matrix Class with Sol2 Binding
// Add to your project - works immediately with current setup

#ifndef LUAMATRIX_HPP
#define LUAMATRIX_HPP

#include <vector>
#include <stdexcept>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

class LuaMatrix {
private:
    std::vector<std::vector<double>> data;
    size_t rows, cols;

public:
    // Constructors
    LuaMatrix(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }
    
    LuaMatrix(const std::vector<std::vector<double>>& input) {
        data = input;
        rows = data.size();
        cols = rows > 0 ? data[0].size() : 0;
    }

    // Basic accessors
    double get(size_t r, size_t c) const {
        if (r >= rows || c >= cols) throw std::out_of_range("Matrix index out of range");
        return data[r][c];
    }
    
    void set(size_t r, size_t c, double value) {
        if (r >= rows || c >= cols) throw std::out_of_range("Matrix index out of range");
        data[r][c] = value;
    }
    
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    // Matrix operations
    LuaMatrix multiply(const LuaMatrix& other) const {
        if (cols != other.rows) throw std::invalid_argument("Matrix dimensions don't match for multiplication");
        
        LuaMatrix result(rows, other.cols);
        
        // Basic matrix multiplication - can be optimized with BLAS
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.cols; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < cols; ++k) {
                    sum += data[i][k] * other.data[k][j];
                }
                result.data[i][j] = sum;
            }
        }
        return result;
    }
    
    LuaMatrix add(const LuaMatrix& other) const {
        if (rows != other.rows || cols != other.cols) 
            throw std::invalid_argument("Matrix dimensions don't match for addition");
        
        LuaMatrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }
    
    LuaMatrix subtract(const LuaMatrix& other) const {
        if (rows != other.rows || cols != other.cols) 
            throw std::invalid_argument("Matrix dimensions don't match for subtraction");
        
        LuaMatrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] - other.data[i][j];
            }
        }
        return result;
    }
    
    LuaMatrix transpose() const {
        LuaMatrix result(cols, rows);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[j][i] = data[i][j];
            }
        }
        return result;
    }
    
    LuaMatrix scale(double factor) const {
        LuaMatrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.data[i][j] = data[i][j] * factor;
            }
        }
        return result;
    }
    
    // Determinant (for small matrices)
    double determinant() const {
        if (rows != cols) throw std::invalid_argument("Determinant only defined for square matrices");
        
        if (rows == 1) return data[0][0];
        if (rows == 2) return data[0][0] * data[1][1] - data[0][1] * data[1][0];
        
        // For larger matrices, use LU decomposition or other methods
        // This is a simple recursive implementation (not efficient for large matrices)
        double det = 0.0;
        for (size_t c = 0; c < cols; ++c) {
            LuaMatrix minor = getMinor(0, c);
            double cofactor = (c % 2 == 0 ? 1 : -1) * data[0][c] * minor.determinant();
            det += cofactor;
        }
        return det;
    }
    
    // Utility methods
    void fillRandom(double min = 0.0, double max = 1.0) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = min + (max - min) * (rand() / double(RAND_MAX));
            }
        }
    }
    
    void fillIdentity() {
        if (rows != cols) throw std::invalid_argument("Identity matrix must be square");
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                data[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Matrix " << rows << "x" << cols << ":\n";
        for (size_t i = 0; i < rows; ++i) {
            ss << "[";
            for (size_t j = 0; j < cols; ++j) {
                ss << " " << std::fixed << std::setprecision(3) << data[i][j];
                if (j < cols - 1) ss << ",";
            }
            ss << " ]\n";
        }
        return ss.str();
    }
    
    // Get row/column as vector
    std::vector<double> getRow(size_t r) const {
        if (r >= rows) throw std::out_of_range("Row index out of range");
        return data[r];
    }
    
    std::vector<double> getCol(size_t c) const {
        if (c >= cols) throw std::out_of_range("Column index out of range");
        std::vector<double> result(rows);
        for (size_t i = 0; i < rows; ++i) {
            result[i] = data[i][c];
        }
        return result;
    }

private:
    LuaMatrix getMinor(size_t row, size_t col) const {
        LuaMatrix minor(rows - 1, cols - 1);
        size_t minor_row = 0;
        
        for (size_t i = 0; i < rows; ++i) {
            if (i == row) continue;
            size_t minor_col = 0;
            
            for (size_t j = 0; j < cols; ++j) {
                if (j == col) continue;
                minor.data[minor_row][minor_col] = data[i][j];
                minor_col++;
            }
            minor_row++;
        }
        return minor;
    }
};

// Option 2: BLAS/LAPACK Integration (requires additional libraries)
/*
// Add to CMakeLists.txt:
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

target_link_libraries(Sol2QtApp PRIVATE
    Qt6::Core Qt6::Widgets
    ${LUA_LIBRARY}
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
)
*/

#ifdef USE_BLAS_LAPACK
extern "C" {
    // BLAS Level 1 (vector operations)
    double ddot_(const int* n, const double* x, const int* incx, const double* y, const int* incy);
    void daxpy_(const int* n, const double* alpha, const double* x, const int* incx, double* y, const int* incy);
    void dscal_(const int* n, const double* alpha, double* x, const int* incx);
    
    // BLAS Level 2 (matrix-vector operations)
    void dgemv_(const char* trans, const int* m, const int* n, const double* alpha,
                const double* a, const int* lda, const double* x, const int* incx,
                const double* beta, double* y, const int* incy);
    
    // BLAS Level 3 (matrix-matrix operations)
    void dgemm_(const char* transa, const char* transb, const int* m, const int* n, const int* k,
                const double* alpha, const double* a, const int* lda, const double* b, const int* ldb,
                const double* beta, double* c, const int* ldc);
    
    // LAPACK routines
    void dgetrf_(const int* m, const int* n, double* a, const int* lda, int* ipiv, int* info);
    void dgetri_(const int* n, double* a, const int* lda, const int* ipiv, double* work, const int* lwork, int* info);
    void dgeev_(const char* jobvl, const char* jobvr, const int* n, double* a, const int* lda,
                double* wr, double* wi, double* vl, const int* ldvl, double* vr, const int* ldvr,
                double* work, const int* lwork, int* info);
}

class AcceleratedMatrix : public LuaMatrix {
public:
    AcceleratedMatrix(size_t r, size_t c) : LuaMatrix(r, c) {}
    
    // High-performance matrix multiplication using BLAS
    AcceleratedMatrix multiplyBLAS(const AcceleratedMatrix& other) const {
        if (getCols() != other.getRows()) 
            throw std::invalid_argument("Matrix dimensions don't match for multiplication");
        
        AcceleratedMatrix result(getRows(), other.getCols());
        
        // Flatten matrices for BLAS
        std::vector<double> a_flat, b_flat, c_flat(getRows() * other.getCols(), 0.0);
        
        // Convert to column-major order (FORTRAN convention)
        for (size_t j = 0; j < getCols(); ++j) {
            for (size_t i = 0; i < getRows(); ++i) {
                a_flat.push_back(get(i, j));
            }
        }
        
        for (size_t j = 0; j < other.getCols(); ++j) {
            for (size_t i = 0; i < other.getRows(); ++i) {
                b_flat.push_back(other.get(i, j));
            }
        }
        
        // Call BLAS DGEMM
        char trans = 'N';
        int m = static_cast<int>(getRows());
        int n = static_cast<int>(other.getCols());
        int k = static_cast<int>(getCols());
        double alpha = 1.0, beta = 0.0;
        int lda = m, ldb = k, ldc = m;
        
        dgemm_(&trans, &trans, &m, &n, &k, &alpha, 
               a_flat.data(), &lda, b_flat.data(), &ldb, 
               &beta, c_flat.data(), &ldc);
        
        // Convert back to matrix format
        for (size_t i = 0; i < getRows(); ++i) {
            for (size_t j = 0; j < other.getCols(); ++j) {
                result.set(i, j, c_flat[j * getRows() + i]);
            }
        }
        
        return result;
    }
    
    // Matrix inversion using LAPACK
    AcceleratedMatrix inverse() const {
        if (getRows() != getCols()) throw std::invalid_argument("Only square matrices can be inverted");
        
        int n = static_cast<int>(getRows());
        std::vector<double> a_copy;
        
        // Copy matrix data in column-major order
        for (size_t j = 0; j < getCols(); ++j) {
            for (size_t i = 0; i < getRows(); ++i) {
                a_copy.push_back(get(i, j));
            }
        }
        
        std::vector<int> ipiv(n);
        int info;
        
        // LU factorization
        dgetrf_(&n, &n, a_copy.data(), &n, ipiv.data(), &info);
        
        if (info != 0) throw std::runtime_error("Matrix is singular and cannot be inverted");
        
        // Query optimal work array size
        int lwork = -1;
        double work_query;
        dgetri_(&n, a_copy.data(), &n, ipiv.data(), &work_query, &lwork, &info);
        
        // Perform inversion
        lwork = static_cast<int>(work_query);
        std::vector<double> work(lwork);
        dgetri_(&n, a_copy.data(), &n, ipiv.data(), work.data(), &lwork, &info);
        
        if (info != 0) throw std::runtime_error("Matrix inversion failed");
        
        // Convert back to matrix format
        AcceleratedMatrix result(getRows(), getCols());
        for (size_t i = 0; i < getRows(); ++i) {
            for (size_t j = 0; j < getCols(); ++j) {
                result.set(i, j, a_copy[j * getRows() + i]);
            }
        }
        
        return result;
    }
    
    // Eigenvalue computation using LAPACK
    std::pair<std::vector<double>, std::vector<double>> eigenvalues() const {
        if (getRows() != getCols()) throw std::invalid_argument("Eigenvalues only defined for square matrices");
        
        int n = static_cast<int>(getRows());
        std::vector<double> a_copy;
        
        // Copy matrix data
        for (size_t j = 0; j < getCols(); ++j) {
            for (size_t i = 0; i < getRows(); ++i) {
                a_copy.push_back(get(i, j));
            }
        }
        
        std::vector<double> wr(n), wi(n);  // Real and imaginary parts
        char jobvl = 'N', jobvr = 'N';    // Don't compute eigenvectors
        double* vl = nullptr, *vr = nullptr;
        int ldvl = 1, ldvr = 1;
        
        // Query optimal work array size
        int lwork = -1;
        double work_query;
        int info;
        
        dgeev_(&jobvl, &jobvr, &n, a_copy.data(), &n, wr.data(), wi.data(),
               vl, &ldvl, vr, &ldvr, &work_query, &lwork, &info);
        
        // Compute eigenvalues
        lwork = static_cast<int>(work_query);
        std::vector<double> work(lwork);
        
        dgeev_(&jobvl, &jobvr, &n, a_copy.data(), &n, wr.data(), wi.data(),
               vl, &ldvl, vr, &ldvr, work.data(), &lwork, &info);
        
        if (info != 0) throw std::runtime_error("Eigenvalue computation failed");
        
        return {wr, wi};
    }
};
#endif // USE_BLAS_LAPACK

// Option 3: Eigen Library Integration (header-only, easier to integrate)
/*
// Download Eigen library and add to CMakeLists.txt:
find_package(Eigen3 REQUIRED)
target_link_libraries(Sol2QtApp PRIVATE Eigen3::Eigen)
*/

#ifdef USE_EIGEN
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

class EigenMatrix {
private:
    Eigen::MatrixXd matrix;

public:
    EigenMatrix(size_t rows, size_t cols) : matrix(rows, cols) {
        matrix.setZero();
    }
    
    EigenMatrix(const Eigen::MatrixXd& m) : matrix(m) {}
    
    // Accessors
    double get(size_t r, size_t c) const { return matrix(r, c); }
    void set(size_t r, size_t c, double value) { matrix(r, c) = value; }
    size_t getRows() const { return matrix.rows(); }
    size_t getCols() const { return matrix.cols(); }
    
    // High-performance operations (automatically vectorized)
    EigenMatrix multiply(const EigenMatrix& other) const {
        return EigenMatrix(matrix * other.matrix);
    }
    
    EigenMatrix add(const EigenMatrix& other) const {
        return EigenMatrix(matrix + other.matrix);
    }
    
    EigenMatrix subtract(const EigenMatrix& other) const {
        return EigenMatrix(matrix - other.matrix);
    }
    
    EigenMatrix transpose() const {
        return EigenMatrix(matrix.transpose());
    }
    
    EigenMatrix inverse() const {
        return EigenMatrix(matrix.inverse());
    }
    
    double determinant() const {
        return matrix.determinant();
    }
    
    // Advanced operations
    EigenMatrix pseudoInverse() const {
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(matrix, Eigen::ComputeFullU | Eigen::ComputeFullV);
        return EigenMatrix(svd.pinv());
    }
    
    std::vector<double> eigenvalues() const {
        Eigen::EigenSolver<Eigen::MatrixXd> solver(matrix);
        Eigen::VectorXcd eigenvals = solver.eigenvalues();
        
        std::vector<double> result;
        for (int i = 0; i < eigenvals.size(); ++i) {
            result.push_back(eigenvals[i].real());  // Real part only
        }
        return result;
    }
    
    EigenMatrix solve(const EigenMatrix& b) const {
        return EigenMatrix(matrix.lu().solve(b.matrix));
    }
    
    // Decompositions
    std::pair<EigenMatrix, EigenMatrix> qrDecomposition() const {
        Eigen::HouseholderQR<Eigen::MatrixXd> qr(matrix);
        return {EigenMatrix(qr.householderQ()), EigenMatrix(qr.matrixQR().triangularView<Eigen::Upper>())};
    }
    
    void fillRandom() {
        matrix.setRandom();
    }
    
    void fillIdentity() {
        matrix.setIdentity();
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "EigenMatrix " << getRows() << "x" << getCols() << ":\n";
        ss << matrix;
        return ss.str();
    }
};
#endif // USE_EIGEN

#endif // LUAMATRIX_HPP