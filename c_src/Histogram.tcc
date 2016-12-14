
template <typename T>
Histogram<T>::Histogram() {
}

template <typename T>
Histogram<T>::Histogram(std::vector<T> &values) {
    // get the number of data points (length of values)
    int data_points = values.size();

    // get the range of the values
    T min = *std::min_element(values.begin(), values.end());
    T max = *std::max_element(values.begin(), values.end());
    T range = max - min;

    // The number of bins will be equal to the floor of the range (for now)
    int bin_num = sqrt(data_points);

    // Initialize member vectors
    try {
        this->bins_ = std::vector<double>(bin_num);
        this->density_ = std::vector<double>(bin_num);
        this->counts_ = std::vector<int>(bin_num);
    } catch (std::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    }

    this->size_ = bin_num;

    // Fill density with zeroes
    //std::fill(this->density_.begin(), this->density_.end(), 0);

    // calculate the interval between each bin
    double interval = static_cast<double>(range) /static_cast<double>(bin_num);

    // iterate through the bin intervals. Set each element of bins_
    // to be the upper bound of the bin.
    // For example if you have 1,2,3,4,5; 5 bins of interval 1
    // bins_[0] = 2; bins_[1] = 3; ... bins_[4] = 6;
    double bin_ubound = min + interval;
    for (int bin_cnt = 0; bin_cnt < bin_num; bin_cnt++) {
        this->bins_[bin_cnt] = bin_ubound;
        bin_ubound += interval;
    }

    // calculate how much each data point contributes to the density
    double density_incr = 1.0/static_cast<double>(data_points);

    // iterate through data points and increment counts_ and density_ appropriately
    for (typename std::vector<T>::iterator it = values.begin(); it != values.end(); it++) {
        int bin_pos = static_cast<int>((*it - min)/interval);
        if (bin_pos == bin_num) bin_pos--;
        try {
            this->density_.at(bin_pos) += density_incr;
            this->counts_.at(bin_pos)++;
        } catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
            std::cout << "Bin_pos = " << bin_pos << std::endl;
            throw;
        }
    }
}

template <typename T>
Histogram<T>::Histogram(std::deque<T> &values) {
    // get the number of data points (length of values)
    int data_points = values.size();

    // get the range of the values
    T min = *std::min_element(values.begin(), values.end());
    T max = *std::max_element(values.begin(), values.end());
    T range = max - min;

    // The number of bins will be equal to the floor of the range (for now)
    int bin_num = sqrt(data_points);

    // Initialize member vectors
    try {
        this->bins_ = std::vector<double>(bin_num);
        this->density_ = std::vector<double>(bin_num);
        this->counts_ = std::vector<int>(bin_num);
    } catch (std::bad_alloc &ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    }

    this->size_ = bin_num;

    // Fill density with zeroes
    //std::fill(this->density_.begin(), this->density_.end(), 0);

    // calculate the interval between each bin
    double interval = static_cast<double>(range) /static_cast<double>(bin_num);

    // iterate through the bin intervals. Set each element of bins_
    // to be the upper bound of the bin.
    // For example if you have 1,2,3,4,5; 5 bins of interval 1
    // bins_[0] = 2; bins_[1] = 3; ... bins_[4] = 6;
    double bin_ubound = min + interval;
    for (int bin_cnt = 0; bin_cnt < bin_num; bin_cnt++) {
        this->bins_[bin_cnt] = bin_ubound;
        bin_ubound += interval;
    }

    // calculate how much each data point contributes to the density
    double density_incr = 1.0/static_cast<double>(data_points);

    // iterate through data points and increment counts_ and density_ appropriately
    for (typename std::deque<T>::iterator it = values.begin(); it != values.end(); it++) {
        int bin_pos = static_cast<int>((*it - min)/interval);
        if (bin_pos == bin_num) bin_pos--;
        try {
            this->density_.at(bin_pos) += density_incr;
            this->counts_.at(bin_pos)++;
        } catch (std::exception &ex) {
            std::cout << ex.what() << std::endl;
            std::cout << "Bin_pos = " << bin_pos << std::endl;
            throw;
        }
    }
}

template <typename T>
Histogram<T>::Histogram(const Histogram &obj) {
    std::vector<double> bins(obj.bins_);
    std::vector<double> density(obj.density_);
    std::vector<int> counts(obj.counts_);
    this->bins_ = bins;
    this->density_ = density;
    this->counts_ = counts;
    this->size_ = obj.size_;
}

template <typename T>
Histogram<T>& Histogram<T>::operator=(const Histogram& obj) {
    if (this != &obj) {
        std::vector<double> bins(obj.bins_);
        std::vector<double> density(obj.density_);
        std::vector<int> counts(obj.counts_);
        this->bins_ = bins;
        this->density_ = density;
        this->counts_ = counts;
        this->size_ = obj.size_;
    }
    return *this;
}

template <typename T>
Histogram<T>::~Histogram() {
}

template <typename T>
void Histogram<T>::showHistogram() {
    for (int i = 0; i < this->size_; i++) {
        std::cout << "Bin maximum: " << this->bins_[i] << "; Bin Density: " << this->density_[i] <<
                  "; Bin Count: " << this->counts_[i] << std::endl;
    }
}

template <typename T>
void Histogram<T>::getPeakRange(double threshold, T &minRange, T &maxRange) {
    // Initialize min and max, max index as -1 (to mean None)
    T min = -1; T max = -1;
    int min_index = -1; int max_index = -1;

    int cnt = 0;

    // Loop through each bin
    while (cnt < this->size_) {
        // If the density of the bin is greater than the threshold
        // set mininum to be that bin if min doesn't exist (== -1)
        // If both a min and a max exist (ie, a peak has already been found),
        // And the current bin has a greater density than the first bin
        // of the previously detected peak, then this marks the new peak

        // If the density is smaller than the threshold and we have a min, then set the
        // current bin to max, ending the peak range.
        if (this->density_[cnt] > threshold) {
            if (min == -1) {
                min = this->bins_[cnt];
                min_index = cnt;
            } else if (max != -1 && this->density_[cnt] > this->density_[min_index]) {
                min = this->bins_[cnt] - 3;
                min_index = cnt;
                max = -1;
                max_index = -1;
            }
        } else {
            if (min != -1 && max == -1) {
                max = this->bins_[cnt];
                max_index = cnt;
            }
        }
        cnt++;
    }
    //std::cout << min << " " << max << std::endl;
    // If for some reason either peak bound has not been detected, set it to a reasonable range
    // for green chromaticity values
    // If this is used for anything else, will need to change this to something more general
    if (max == -1) {
        std::cerr << "Max peak not found, defaulting to 120..." << std::endl;
        max = 120;
    }
    if (min == -1) {
        std::cerr << "Min peak not found, defaulting to 90..." << std::endl;
        min = 90;
    }

    minRange = min; maxRange = max;
}
