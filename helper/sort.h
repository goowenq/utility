/*
 * std::vector<double> x = {1.0, -34.6, 78.9}; 
 * std::vector<int> index = sortD(x);
 * for(auto ind: index) {
 *   std::cout << x.at(ind) << std::endl;
 * }
 */

std::vector<int> sortI(std::vector<int> x) {

    // std::vector<int> x = {15, 3, 0, 20};
    std::vector<int> y(x.size());
    std::size_t n(0);
    std::generate(std::begin(y), std::end(y), [&]{ return n++; });

    std::sort(  std::begin(y),
                std::end(y),
                [&](int i1, int i2) { return x[i1] < x[i2]; } );

    // for (auto v : y)
    //     std::cout << v << ' ';

    return y;
}

std::vector<int> sortD(std::vector<double> x) {

    // std::vector<int> x = {15, 3, 0, 20};
    std::vector<int> y(x.size());
    std::size_t n(0);
    std::generate(std::begin(y), std::end(y), [&]{ return n++; });

    std::sort(  std::begin(y),
                std::end(y),
                [&](int i1, int i2) { return x[i1] < x[i2]; } );

    // for (auto v : y)
    //     std::cout << v << ' ';

    return y;
}

