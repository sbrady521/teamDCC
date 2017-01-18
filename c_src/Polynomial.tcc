inline Polynomial1V::Polynomial1V(Eigen::VectorXf v) {
    this->polynomial_ = v;
    this->degree_ = v.size() - 1;
}

inline void Polynomial1V::showPolynomial() {
    std::cout << this->polynomial_ << std::endl;
}

inline Polynomial1V Polynomial1V::integrate() {
    Eigen::VectorXf integral = Eigen::VectorXf(this->degree_ + 2);
    integral(0) = 0; // We don't care about the constant term.
    // Integrate each term separately
    for (int i = 0; i <= this->degree_; i++) {
        integral(i+1) = this->polynomial_(i)/(i+1);
    }

    return Polynomial1V(integral);
}

inline double Polynomial1V::eval(double x) {
    double ans = 0;
    for (int i = 0; i <= this->degree_; i++) {
        ans += this->polynomial_(i)*pow(x, i);
    }
    return ans;
}

inline void Polynomial1V::maxAreaWindow(double x_start, double x_end, double window_range, double &min, double &max) {
    if (window_range > x_end - x_start) throw std::runtime_error("maxArea : Area range is greater than search range!");

    // Integrate the Coordinate vector
    Polynomial1V integral = this->integrate();

    // Calculate the interval between each area we evaluate
    // Note that areas should overlap, like sliding window
    double interval = (window_range/(x_end - x_start) < 1) ? 1 : (window_range/(x_end - x_start) < 1);

    // Initialise best_window_area, and its min, max x_value
    double best_window_area = 0;
    double best_window_min; double best_window_max;

    for (double x = x_start; x < x_end; x += interval) {
        // Calculate upper and lower bounds for this window
        double l_bound = x;
        double u_bound = (x + window_range > x_end) ? x_end : x + window_range;

        // Calculate the area of this window
        double area = integral.eval(u_bound) - integral.eval(l_bound);

        // If the area is better than best windows area, this is the new best window
        if (area > best_window_area) {
            best_window_area = area;
            best_window_min = l_bound;
            best_window_max = u_bound;
        }
    }

    if (best_window_area == 0) {
        min = -1; max = -1;
    } else {
        min = best_window_min; max = best_window_max;
    }
}

template <typename T>
Polynomial1V polyFit(std::vector<T> x_data, std::vector<T> y_data, int degree) {
    // We want to solve the equation
    // y=aX for a.
    if (degree <= 0) throw std::runtime_error("Invalid degree size!");
    // Create matrices
    if (x_data.size() != y_data.size()) throw std::runtime_error("Size of vector x not equal to Size of Vector y!");
    int num_data = x_data.size();

    Eigen::VectorXf y = Eigen::VectorXf(num_data);
    Eigen::MatrixXf X = Eigen::MatrixXf(num_data, degree + 1);

    // Fill matrices with Data.
    for (int row_cnt = 0; row_cnt < num_data; row_cnt++) {
        y(row_cnt) = y_data.at(row_cnt);
    }

    for (int row_cnt = 0; row_cnt < num_data; row_cnt++) {
        for (int col_cnt = 0; col_cnt <= degree; col_cnt++) {
            X(row_cnt, col_cnt) = pow(x_data.at(row_cnt), col_cnt);
        }
    }


    //std::cerr << "y" << std::endl << y << std::endl;
    //std::cerr << "X" << std::endl << X << std::endl;


    // Solve for a
    Eigen::VectorXf a = X.colPivHouseholderQr().solve(y);
    return Polynomial1V(a);
}

template <typename T>
Polynomial1V polyFit(std::vector<T> x_data, std::vector<T> y_data, int degree, double &residualAvg) {
    residualAvg = 0;
    Polynomial1V model = polyFit(x_data, y_data, degree);
    for (int i = 0; i < x_data.size(); i++) {
        residualAvg += abs(model.eval(x_data.at(i)) - y_data.at(i));
    }
    residualAvg = residualAvg / x_data.size();
    return model;
}
