template <typename T>
struct histogramRange {
    T min; T max;
    int min_index; int max_index;
};

template <typename T>
Histogram<T>::Histogram() {
}

template <typename T>
Histogram<T>::Histogram(std::vector<T> &values) {
    // get the number of data points (length of values)
    int data_points = values.size();
    this->num_data_ = data_points;

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
    int bin_num = sqrt(sqrt(data_points));

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
    this->num_data_ = obj.num_data_;
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
        this->num_data_ = obj.num_data_;
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
    struct histogramRange<T> bestRange;
    struct histogramRange<T> currRange;
    currRange.min = -1; currRange.max = -1;
    currRange.min_index = -1; currRange.max_index = -1;

    bestRange.min = -1; bestRange.max = -1;
    bestRange.min_index = -1; bestRange.max_index = -1;

    int cnt = 0;

    // Loop through each bin
    while (cnt < this->size_) {
        // Loop through the histogram and look for ranges.
        // Ranges are defined by their start and end - the start is a
        // bin with density greater than threshold, and the end
        // is the first bin after the start with density less than the threshold
        // Compare each range to the best range - the "best range" is the widest range found so far
        // and if the new range is wider, set it to the bestRange.
        if (this->density_[cnt] > threshold) {
            if (currRange.min == -1) {
                currRange.min = this->bins_[cnt];
                currRange.min_index = cnt;
            }
        } else {
            if (currRange.min != -1 && currRange.max == -1) {
                currRange.max = this->bins_[cnt];
                currRange.max_index = cnt;

                if (currRange.max_index - currRange.min_index >= bestRange.max_index - bestRange.min_index) {
                    // memcpy won't compile here for some reason
                    bestRange.min = currRange.min; bestRange.max = currRange.max;
                    bestRange.min_index = currRange.min_index; bestRange.max_index = currRange.max_index;
                }

                currRange.min = -1; currRange.max = -1;
                currRange.min_index = -1; currRange.max_index = -1;
            }
        }
        cnt++;
    }
    //std::cout << min << " " << max << std::endl;
    // If for some reason either peak bound has not been detected, set it to a reasonable range
    // for green chromaticity values
    // If this is used for anything else, will need to change this to something more general
    if (bestRange.max == -1) {
        std::cerr << "Max peak not found, defaulting to 120..." << std::endl;
        bestRange.max = 120;
    }
    if (bestRange.min == -1) {
        std::cerr << "Min peak not found, defaulting to 90..." << std::endl;
        bestRange.min = 90;
    }

    minRange = bestRange.min; maxRange = bestRange.max;
}

template <typename T>
void Histogram<T>::appendData(std::vector<T> &values) {
    // Get the mininum and maximum values in the values to be appended
    T min_of_new_vals = *std::min_element(values.begin(), values.end());
    T max_of_new_vals = *std::max_element(values.begin(), values.end());

    double min_of_bins = this->bins_[0];
    double max_of_bins = this->bins_[this->size_ - 1];

    double bin_interval;

    if (this->size_ <= 1) bin_interval = 1;
    else bin_interval = this->bins_[1] - this->bins_[0];

    // Check if we need to add bins
    if (min_of_new_vals < min_of_bins) {
        for (double i = min_of_bins - bin_interval; min_of_new_vals < i; i -= bin_interval) {
            std::vector<double>::iterator bins_start = this->bins_.begin();
            std::vector<double>::iterator density_start = this->density_.begin();
            std::vector<int>::iterator counts_start = this->counts_.begin();

            this->bins_.insert(bins_start, i);
            this->density_.insert(density_start, 0);
            this->counts_.insert(counts_start, 0);
        }
    }

    if (max_of_new_vals > max_of_bins) {
        for (double i = max_of_bins + bin_interval; i <= max_of_new_vals + bin_interval; i += bin_interval) {
            std::vector<double>::iterator bins_end = this->bins_.end();
            std::vector<double>::iterator density_end = this->density_.end();
            std::vector<int>::iterator counts_end = this->counts_.end();

            this->bins_.insert(bins_end, i);
            this->density_.insert(density_end, 0);
            this->counts_.insert(counts_end, 0);
        }
    }

    // Multiply the density of each old value by a multiplier to adjust density correctly
    double density_multiplier = static_cast<double>(this->num_data_)/static_cast<double>(values.size() + this->num_data_);

    for (int i = 0; i < this->density_.size(); i++) {
        this->density_[i] *= density_multiplier;
    }

    // calculate how much each data point contributes to the density
    double density_incr = 1.0/static_cast<double>(this->num_data_ + values.size());

    double min = min_of_bins < min_of_new_vals ? min_of_bins : min_of_new_vals;

    // iterate through data points and increment and density_ appropriately
    for (typename std::vector<T>::iterator it = values.begin(); it != values.end(); it++) {
        int bin_pos = (*it - min)/bin_interval;
        if (bin_pos == this->bins_.size()) bin_pos--;
        try {
            this->density_.at(bin_pos) += density_incr;
            this->counts_.at(bin_pos)++;
        } catch (std::exception &ex) {
            std::cerr << ex.what() << std::endl;
            std::cout << "Bin_pos = " << bin_pos << std::endl;
            throw;
        }

    }

    // Update size variables
    this->size_ = this->bins_.size();
    this->num_data_ = this->num_data_ + values.size();
}
