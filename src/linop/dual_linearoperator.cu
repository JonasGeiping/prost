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

#include "prost/linop/dual_linearoperator.hpp"

namespace prost {

template<typename T>
DualLinearOperator<T>::DualLinearOperator(shared_ptr<LinearOperator<T>> child)
    : child_(child)
{
  this->nrows_ = child_->ncols();
  this->ncols_ = child_->nrows();
}

template<typename T>
DualLinearOperator<T>::~DualLinearOperator()
{
}

template<typename T>
void DualLinearOperator<T>::Eval(
    device_vector<T>& result, 
    const device_vector<T>& rhs,
    T beta)
{
  if(beta == 0)
  {
    thrust::fill(result.begin(), result.end(), 0);
  }
  else if(beta != 1)
  {
    thrust::transform(result.begin(), result.end(), result.begin(), -beta * thrust::placeholders::_1);
  }

  for(auto& block : child_->blocks_)
    block->EvalAdjointAdd(result, rhs);

  thrust::transform(result.begin(), result.end(), result.begin(),
                    thrust::negate<float>());
}

template<typename T>
void DualLinearOperator<T>::EvalAdjoint(
    device_vector<T>& result, 
    const device_vector<T>& rhs,
    T beta)
{
  if(beta == 0)
  {
    thrust::fill(result.begin(), result.end(), 0);
  }
  else if(beta != 1)
  {
    thrust::transform(result.begin(), result.end(), result.begin(), -beta * thrust::placeholders::_1);
  }

  for(auto& block : child_->blocks_)
    block->EvalAdd(result, rhs);

  thrust::transform(result.begin(), result.end(), result.begin(),
                    thrust::negate<float>());
}
  
template<typename T>
T DualLinearOperator<T>::row_sum(size_t row, T alpha) const
{
  return child_->col_sum(row, alpha);
}

template<typename T>
T DualLinearOperator<T>::col_sum(size_t col, T alpha) const
{
  return child_->row_sum(col, alpha);
}

template<typename T>
size_t DualLinearOperator<T>::nrows() const
{
  return child_->ncols();
}

template<typename T>
size_t DualLinearOperator<T>::ncols() const
{
  return child_->nrows();
}

// Explicit template instantiation
template class DualLinearOperator<float>;
template class DualLinearOperator<double>;

}