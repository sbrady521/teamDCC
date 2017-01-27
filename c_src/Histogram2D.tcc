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

        for (int i = 0; i < X2_bin_num; i++) {
            this->density_.push_back(std::vector<double>(X2_bin_num));
            this->counts_.push_back(std::vector<int>(X2_bin_num));
        }

    } catch (std::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    }

    this->X1_bin_num_ = X1_bin_num; this->X2_bin_num_ = X2_bin_num;

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
}

template <typename T>
Histogram2D<T>::Histogram2D(const Histogram2D &obj) {
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