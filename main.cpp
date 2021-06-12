#include <complex>
#include <numeric>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

auto scaler(int min_from, int max_from, double min_to, double max_to) {
    double w_from = double(max_from - min_from);
    double w_to = max_to - min_to;
    double mid_from = double(w_from / 2);
    double mid_to = w_to / 2;

    return [w_from, w_to, mid_from, mid_to, min_to](int from) { return double(from - mid_from) / w_from * w_to + mid_to + min_to; };
}

template <typename A, typename B>
static auto scaled_cmplx(A scaler_x, B scaler_y) {
   return [=](int x, int y) {
       double scaled_x = scaler_x(x);
       double scaled_y = scaler_y(y);

       return std::complex<double>(scaled_x, scaled_y);
   }; 
}

const int max_iterations = 1000;
static auto mandelbrot_iterations(std::complex<double> c) {
    auto z = c;
    int iteration = 0;
    while (iteration < max_iterations) {
        iteration ++;
        z = std::pow(z, 2) + c;
        if (std::abs(z) > 2) {
            break;
        }
    }

    return iteration;
}



int main() {
    const int width = 100, high = 40;
    auto scale(scaled_cmplx(scaler(0,width,-2.0,1.0), scaler(0,high,-1.0,1.0)));
    auto i_to_xy ([&](int i){
        int y = i / width;
        int x = i % width;
        return scale(x, y);
    });
    auto to_iteration_count ([&](int i){
        return mandelbrot_iterations(i_to_xy(i));
    });

    std::vector<int> result(width*high);
    std::iota(result.begin(), result.end(), 0);
    std::transform(result.begin(), result.end(), result.begin(), to_iteration_count);

    auto binfunc([width,n{0}] (auto output_it, int x) mutable{
        *++output_it = (x > 50? '*':' ');
        if(++n %width ==0){++output_it = '\n';}
        return output_it;
    });

    std::ostream_iterator<char> out_it (std::cout,"");
    std::accumulate(result.begin(), result.end(), out_it, binfunc);
}