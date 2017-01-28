template <typename T>
Histogram2D<T>::Histogram2D() {
    //Empty constructor to prevent compiler errors.
}

template <typename T>
Histogram2D<T>::Histogram2D(std::vector<T> &X1_values, std::vector<T> &X2_values) {
    // Get the size of each argument vector
    int X1_size = X1_values.size(); int X2_size = X2_values.size();

    if (X1_size != X2_size) throw std::runtime_error("Value deques differ in size!");

    this->num_data_ = X1_size;

    // get the range of the values
    T X1_min = *std::min_element(X1_values.begin(), X1_values.end());
    T X1_max = *std::max_element(X1_values.begin(), X1_values.end());
    T X1_range = X1_max - X1_min;

    T X2_min = *std::min_element(X2_values.begin(), X2_values.end());
    T X2_max = *std::max_element(X2_values.begin(), X2_values.end());
    T X2_range = X2_max - X2_min;

    // Get the number of bins for each Variable
    int X1_bin_num = (sqrt(sqrt(X1_size)) > X1_range) ? sqrt(sqrt(X1_size)) : X1_range;
    int X2_bin_num = (sqrt(sqrt(X2_size)) > X2_range) ? sqrt(sqrt(X2_size)) : X2_range;

    // Initialize member vectors
    try {
        this->X1_bins_ = std::vector<double>(X1_bin_num);
        this->X2_bins_ = std::vector<double>(X2_bin_num);
        this->density_ = std::vector<std::vector<double> >(X1_bin_num);
        this->counts_ = std::vector<std::vector<int> >(X1_bin_num);

        for (int i = 0; i < X1_bin_num; i++) {
            this->density_.at(i) = (std::vector<double>(X2_bin_num));
            this->counts_.at(i) = (std::vector<int>(X2_bin_num));
        }

    } catch (std::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    }

    //this->X1_bin_num_ = X1_bin_num; this->X2_bin_num_ = X2_bin_num;

    // calculate the interval between each bin
    double X1_interval = static_cast<double>(X1_range) /static_cast<double>(X1_bin_num);
    double X2_interval = static_cast<double>(X2_range) /static_cast<double>(X2_bin_num);

    // iterate through the bin intervals. Set each element of bins_
    // to be the upper bound of the bin.
    double bin_ubound = X1_min + X1_interval;
    for (int bin_cnt = 0; bin_cnt < X1_bin_num; bin_cnt++) {
        this->X1_bins_.at(bin_cnt) = bin_ubound;
        bin_ubound += X1_interval;
    }

    bin_ubound = X2_min + X2_interval;
    for (int bin_cnt = 0; bin_cnt < X2_bin_num; bin_cnt++) {
        this->X2_bins_.at(bin_cnt) = bin_ubound;
        bin_ubound += X2_interval;
    }

    // calculate how much each data point contributes to the density
    double density_incr = 1.0/static_cast<double>(this->num_data_);

    // iterate through data points and increment counts_ and density_ appropriately
    for (int i = 0; i < this->num_data_; i++) {
        T X1_val = X1_values.at(i); T X2_val = X2_values.at(i);
        int X1_bin_pos = static_cast<int>((X1_val - X1_min)/X1_interval);
        int X2_bin_pos = static_cast<int>((X2_val - X2_min)/X2_interval);
        if (X1_bin_pos == X1_bin_num) X1_bin_pos--;
        if (X2_bin_pos == X2_bin_num) X2_bin_pos--;

        try {
            this->density_.at(X1_bin_pos).at(X2_bin_pos) += density_incr;
            this->counts_.at(X1_bin_pos).at(X2_bin_pos)++;
        } catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
            throw;
        }
    }

    this->filtered_ = false;
}

template <typename T>
Histogram2D<T>::Histogram2D(std::deque<T> &X1_values, std::deque<T> &X2_values) {
    // Get the size of each argument vector
    int X1_size = X1_values.size(); int X2_size = X2_values.size();

    if (X1_size != X2_size) throw std::runtime_error("Value deques differ in size!");

    this->num_data_ = X1_size;

    // get the range of the values
    T X1_min = *std::min_element(X1_values.begin(), X1_values.end());
    T X1_max = *std::max_element(X1_values.begin(), X1_values.end());
    T X1_range = X1_max - X1_min;

    T X2_min = *std::min_element(X2_values.begin(), X2_values.end());
    T X2_max = *std::max_element(X2_values.begin(), X2_values.end());
    T X2_range = X2_max - X2_min;

    // Get the number of bins for each Variable
    int X1_bin_num = (sqrt(sqrt(X1_size)) > X1_range) ? sqrt(sqrt(X1_size)) : X1_range;
    int X2_bin_num = (sqrt(sqrt(X2_size)) > X2_range) ? sqrt(sqrt(X2_size)) : X2_range;

    // Initialize member vectors
    try {
        this->X1_bins_ = std::vector<double>(X1_bin_num);
        this->X2_bins_ = std::vector<double>(X2_bin_num);
        this->density_ = std::vector<std::vector<double> >(X1_bin_num);
        this->counts_ = std::vector<std::vector<int> >(X1_bin_num);

        for (int i = 0; i < X1_bin_num; i++) {
            this->density_.at(i) = (std::vector<double>(X2_bin_num));
            this->counts_.at(i) = (std::vector<int>(X2_bin_num));
        }

    } catch (std::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    }

    //this->X1_bin_num_ = X1_bin_num; this->X2_bin_num_ = X2_bin_num;

    // calculate the interval between each bin
    double X1_interval = static_cast<double>(X1_range) /static_cast<double>(X1_bin_num);
    double X2_interval = static_cast<double>(X2_range) /static_cast<double>(X2_bin_num);

    // iterate through the bin intervals. Set each element of bins_
    // to be the upper bound of the bin.
    double bin_ubound = X1_min + X1_interval;
    for (int bin_cnt = 0; bin_cnt < X1_bin_num; bin_cnt++) {
        this->X1_bins_.at(bin_cnt) = bin_ubound;
        bin_ubound += X1_interval;
    }

    bin_ubound = X2_min + X2_interval;
    for (int bin_cnt = 0; bin_cnt < X2_bin_num; bin_cnt++) {
        this->X2_bins_.at(bin_cnt) = bin_ubound;
        bin_ubound += X2_interval;
    }

    // calculate how much each data point contributes to the density
    double density_incr = 1.0/static_cast<double>(this->num_data_);

    // iterate through data points and increment counts_ and density_ appropriately
    for (int i = 0; i < this->num_data_; i++) {
        T X1_val = X1_values.at(i); T X2_val = X2_values.at(i);
        int X1_bin_pos = static_cast<int>((X1_val - X1_min)/X1_interval);
        int X2_bin_pos = static_cast<int>((X2_val - X2_min)/X2_interval);
        if (X1_bin_pos == X1_bin_num) X1_bin_pos--;
        if (X2_bin_pos == X2_bin_num) X2_bin_pos--;

        try {
            this->density_.at(X1_bin_pos).at(X2_bin_pos) += density_incr;
            this->counts_.at(X1_bin_pos).at(X2_bin_pos)++;
        } catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
            throw;
        }
    }

    this->filtered_ = false;
}

template <typename T>
Histogram2D<T>::Histogram2D(const Histogram2D &obj) {
    // TODO: Don't shallow copy the 2d vectors
    std::vector<double> X1_bins(obj.X1_bins_);
    std::vector<double> X2_bins(obj.X2_bins_);
    std::vector<std::vector<double> > density(obj.density_);
    std::vector<std::vector<int> > counts(obj.counts_);
    this->X1_bins_ = X1_bins;
    this->X2_bins_ = X2_bins;
    this->density_ = density;
    this->counts_ = counts;
    this->num_data_ = obj.num_data_;
}

template <typename T>
Histogram2D<T>& Histogram2D<T>::operator=(const Histogram2D& obj) {
    // TODO: Don't shallow copy the 2d vectors
    if (this != &obj) {
        std::vector<double> X1_bins(obj.X1_bins_);
        std::vector<double> X2_bins(obj.X2_bins_);
        std::vector<std::vector<double> > density(obj.density_);
        std::vector<std::vector<int> > counts(obj.counts_);
        this->X1_bins_ = X1_bins;
        this->X2_bins_ = X2_bins;
        this->density_ = density;
        this->counts_ = counts;
        this->num_data_ = obj.num_data_;
    }
}

template <typename T>
Histogram2D<T>::~Histogram2D() {
}

template <typename T>
void Histogram2D<T>::showHistogram() {
    std::cout << "X2 \\ X1  ";
    for (int i = 0; i < this->X1_bins_.size(); i++) {
        std::cout << X1_bins_.at(i) << " ";
    }
    std::cout << std::endl;
    std::cout << "       --------------" << std::endl;
    for (int i = 0; i < this->X2_bins_.size(); i++) {
        std::cout << "       " << X2_bins_.at(i) << "|";
        for (int j = 0; j < this->X1_bins_.size(); j++) {
            std::cout << this->counts_.at(j).at(i) << " ";
        }
        std::cout << std::endl;
    }
}

template <typename T>
double Histogram2D<T>::getDensity(T X1_val, T X2_val) {
    // Figure out which bin each Value belongs to.
    int X1_bin_pos;
    int X2_bin_pos;

    if (this->X1_bins_.size() == 1) X1_bin_pos = 0;
    else {
        double X1_interval = this->X1_bins_.at(1) - X1_bins_.at(0);
        X1_bin_pos = static_cast<int>((X1_val - (this->X1_bins_.at(0) - X1_interval) )/X1_interval);
    }

    if (this->X2_bins_.size() == 1) X2_bin_pos = 0;
    else {
        double X2_interval = this->X2_bins_.at(1) - X2_bins_.at(0);
        X2_bin_pos = static_cast<int>((X2_val - (this->X2_bins_.at(0) - X2_interval) )/X2_interval);
    }

    if (X1_bin_pos == this->X1_bins_.size()) X1_bin_pos--;
    if (X2_bin_pos == this->X2_bins_.size()) X2_bin_pos--;

    if (X1_bin_pos < 0 || X2_bin_pos < 0) return 0; // This means the value is so small that it isn't even in the histogram.
    if (X1_bin_pos > this->X1_bins_.size() || X2_bin_pos > this->X2_bins_.size()) return 0; // Similar, but too large.
    return this->density_.at(X1_bin_pos).at(X2_bin_pos);
}

template <typename T>
std::vector<std::pair<int, int> > Histogram2D<T>::getBinNeighbours(std::pair<int, int> bin) {
    std::vector<std::pair<int, int> > neighbours = std::vector<std::pair<int, int> >();

    int X1_bin_num = this->X1_bins_.size();
    int X2_bin_num = this->X2_bins_.size();

    int X1_bin = bin.first; int X2_bin = bin.second;

    // Add north bin
    if (bin.first != 0)
        neighbours.push_back(std::make_pair(X1_bin - 1, X2_bin));

    // Add northeast bin
    if (bin.first != 0 && bin.second != X2_bin_num)
        neighbours.push_back(std::make_pair(X1_bin - 1, X2_bin + 1));

    // Add east bin
    if (bin.second != X2_bin_num)
        neighbours.push_back(std::make_pair(X1_bin, X2_bin + 1));

    // Add southeast bin
    if (bin.first != X1_bin_num && bin.second != X2_bin_num)
        neighbours.push_back(std::make_pair(X1_bin + 1, X2_bin + 1));

    // Add south bin
    if (bin.first != X1_bin_num)
        neighbours.push_back(std::make_pair(X1_bin + 1, X2_bin));

    // Add southwest bin
    if (bin.first != X1_bin_num && bin.second != 0)
        neighbours.push_back(std::make_pair(X1_bin + 1, X2_bin - 1));

    // Add west bin
    if (bin.second != 0)
        neighbours.push_back(std::make_pair(X1_bin, X2_bin - 1));

    // Add northwest bin
    if (bin.first != 0 && bin.second != 0)
        neighbours.push_back(std::make_pair(X1_bin - 1, X2_bin - 1));

    return neighbours;
};

template <typename T>
void Histogram2D<T>::filterBins(int max_bins, const std::string method) {
    // Initialize this->filtered_bins_
    this->filtered_bins_ = std::vector<std::vector<bool> >(this->X1_bins_.size());
    for (int i = 0; i < this->X1_bins_.size(); i++) {
        this->filtered_bins_.at(i) = std::vector<bool>(this->X2_bins_.size());
    }

    if (method == "vertical_peak")
        this->filterVerticalPeaks(max_bins);
    else
        throw std::runtime_error("Invalid filtering method!");

    this->filtered_ = true;
}

template <typename T>
void Histogram2D<T>::filterVerticalPeaks(int max_bins) {
    /*
     * TODO:
     * Optimize this massively.
     * Instead of setting an arbitrary starting density we can look for the max
     * density in the histogram.
     * We then add all neighbours to a vector of to be considered vertices
     * Then we repeatedly decrement the density and search over all the to be considered vertices until we have enough
     */
    int validated_cnt = 0;

    int X1_bin_num = this->X1_bins_.size();
    int X2_bin_num = this->X2_bins_.size();

    std::pair<int, int> best_bin_loc;
    double best_bin_density = 0;

    // Get the densest bin in the Histogram
    for (int i = 0; i < X1_bin_num; i++) {
        for (int j = 0; j < X2_bin_num; j++) {
            if (this->density_.at(i).at(j) > best_bin_density) {
                best_bin_loc = std::make_pair(i, j);
                best_bin_density = this->density_.at(i).at(j);
            }
        }
    }

    // Create vector of candidate bins
    std::vector<std::pair<int, int> > bins_todo = std::vector<std::pair<int, int> >();
    bins_todo.push_back(best_bin_loc);

    for (double density = best_bin_density; density > 0; density -= 0.001) {
        // For this density, starting at the best bins density, iterate through all
        // of bins_todo
        for (int i = 0; i < bins_todo.size();) {
            // If this bin has greater than or equal density,
            // Mark it as valid, then add its neighbours to bins_todo
            std::pair<int, int> curr_bin = bins_todo.at(i);

            if (this->density_.at(curr_bin.first).at(curr_bin.second) >= density) {
                this->filtered_bins_.at(curr_bin.first).at(curr_bin.second) = true;
                validated_cnt++;
                std::vector<std::pair<int, int> > neighbours = getBinNeighbours(curr_bin);
                for (int j = 0; j < neighbours.size(); j++) {
                    if (this->filtered_bins_.at(neighbours.at(j).first).at(neighbours.at(j).second) == false)
                        bins_todo.push_back(neighbours.at(j));
                }
                std::swap(bins_todo.at(i), bins_todo.back());
                bins_todo.pop_back();
            } else {
                i++;
            }

            if (validated_cnt == max_bins) return;
        }
    }
}

template <typename T>
bool Histogram2D<T>::isFiltered(T X1_val, T X2_val) {
    if (!this->filtered_) throw std::runtime_error("Histogram has not been filtered!");

    // TODO refactor into getBinPos function!!
    // Figure out which bin each Value belongs to.
    std::pair<int, int> bin = getBinPos(X1_val, X2_val);

    return this->filtered_bins_.at(bin.first).at(bin.second);
}

template <typename T>
std::pair<int, int> Histogram2D<T>::getBinPos(T X1_val, T X2_val) {
    int X1_bin_pos;
    int X2_bin_pos;

    if (this->X1_bins_.size() == 1) X1_bin_pos = 0;
    else {
        double X1_interval = this->X1_bins_.at(1) - X1_bins_.at(0);
        X1_bin_pos = static_cast<int>((X1_val - (this->X1_bins_.at(0) - X1_interval) )/X1_interval);
    }

    if (this->X2_bins_.size() == 1) X2_bin_pos = 0;
    else {
        double X2_interval = this->X2_bins_.at(1) - X2_bins_.at(0);
        X2_bin_pos = static_cast<int>((X2_val - (this->X2_bins_.at(0) - X2_interval) )/X2_interval);
    }

    if (X1_bin_pos == this->X1_bins_.size()) X1_bin_pos--;
    if (X2_bin_pos == this->X2_bins_.size()) X2_bin_pos--;

    if (X1_bin_pos == 0) X1_bin_pos = 0;
    if (X2_bin_pos == 0) X2_bin_pos = 0;

    if (X1_bin_pos > this->X1_bins_.size()) X1_bin_pos = 0;
    if (X2_bin_pos > this->X2_bins_.size()) X2_bin_pos = 0;

    return std::make_pair(X1_bin_pos, X2_bin_pos);
};