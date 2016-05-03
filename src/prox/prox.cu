/**
* This file is part of prost.
*
* Copyright 2016 Thomas Möllenhoff <thomas dot moellenhoff at in dot tum dot de> 
* and Emanuel Laude <emanuel dot laude at in dot tum dot de> (Technical University of Munich)
*
* prost is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* prost is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with prost. If not, see <http://www.gnu.org/licenses/>.
*/

#include "prost/prox/prox.hpp"
#include <ctime>

namespace prost {

template<typename T>
void Prox<T>::Eval(
  thrust::device_vector<T>& result, 
  const thrust::device_vector<T>& arg, 
  const thrust::device_vector<T>& tau_diag, 
  T tau,
  bool invert_tau)
{
  EvalLocal(
    result.begin() + index_,
    result.begin() + index_ + size_,
    arg.cbegin() + index_,
    arg.cbegin() + index_ + size_,
    tau_diag.cbegin() + index_,
    tau_diag.cbegin() + index_ + size_,
    tau,
    invert_tau);
}

template<typename T>
double Prox<T>::Eval(
  std::vector<T>& result, 
  const std::vector<T>& arg, 
  const std::vector<T>& tau_diag, 
  T tau) 
{
  const int repeats = 1;

  const thrust::device_vector<T> d_arg(arg.begin(), arg.end());
  thrust::device_vector<T> d_res;
  d_res.resize(arg.size());
  const thrust::device_vector<T> d_tau(tau_diag.begin(), tau_diag.end());

  const clock_t begin_time = clock();
  for(int i = 0; i < repeats; i++)
  {
    Eval(d_res, d_arg, d_tau, tau);
    cudaDeviceSynchronize();
  }
  double s = (double)(clock() - begin_time) / CLOCKS_PER_SEC;

  result.resize(arg.size());
  thrust::copy(d_res.begin(), d_res.end(), result.begin());

  return (s * 1000 / (double)repeats);
}

template <typename T>
void Prox<T>::get_separable_structure(
    vector<std::tuple<size_t, size_t, size_t> >& sep)
{
  sep.push_back( std::tuple<size_t, size_t, size_t> (index_, size_, 1) );
}


// Explicit template instantiation
template class Prox<float>;
template class Prox<double>;

} // namespace prost

