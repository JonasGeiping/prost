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

#ifndef PROST_PROX_ELEM_OPERATION_HPP_
#define PROST_PROX_ELEM_OPERATION_HPP_

#include <array>
#include <thrust/device_vector.h>

#include "prost/prox/prox_separable_sum.hpp"
#include "prost/common.hpp"

namespace prost {

template<typename T, class ELEM_OPERATION, class ENABLE = void>
class ProxElemOperation { };

template<typename T, class ELEM_OPERATION>
class ProxElemOperation<T, ELEM_OPERATION, typename std::enable_if<ELEM_OPERATION::kCoeffsCount == 0>::type> : public ProxSeparableSum<T> 
{
public:    
  ProxElemOperation(
      size_t index,
      size_t count,
      size_t dim,
      bool interleaved,
      bool diagsteps) 
      : ProxSeparableSum<T>(index, count, (ELEM_OPERATION::kDim <= 0) ? dim : ELEM_OPERATION::kDim, interleaved, diagsteps) { }
  
  virtual size_t gpu_mem_amount() const { return 0; }

protected:
  virtual void EvalLocal(
    const typename thrust::device_vector<T>::iterator& result_beg,
    const typename thrust::device_vector<T>::iterator& result_end,
    const typename thrust::device_vector<T>::const_iterator& arg_beg,
    const typename thrust::device_vector<T>::const_iterator& arg_end,
    const typename thrust::device_vector<T>::const_iterator& tau_beg,
    const typename thrust::device_vector<T>::const_iterator& tau_end,
    T tau,
    bool invert_tau);
};

template<typename T, class ELEM_OPERATION>
class ProxElemOperation<T, ELEM_OPERATION, typename std::enable_if<ELEM_OPERATION::kCoeffsCount != 0>::type> : public ProxSeparableSum<T> 
{
public:    
  ProxElemOperation(
      size_t index, 
      size_t count, 
      size_t dim, 
      bool interleaved, 
      bool diagsteps, 
      std::array<std::vector<T>, ELEM_OPERATION::kCoeffsCount> coeffs)
      : ProxSeparableSum<T>(index, count, ELEM_OPERATION::kDim <= 0 ? dim : ELEM_OPERATION::kDim, interleaved, diagsteps), coeffs_(coeffs) { }

  virtual void Initialize();
  
  virtual size_t gpu_mem_amount() const
  {
    size_t mem = 0;
    for(size_t i = 0; i < ELEM_OPERATION::kCoeffsCount; i++)
    {
      if(coeffs_[i].size() > 1)
        mem += this->count_ * sizeof(T);
    }
    
    return mem;
  }
   
protected:

  virtual void EvalLocal(
    const typename thrust::device_vector<T>::iterator& result_beg,
    const typename thrust::device_vector<T>::iterator& result_end,
    const typename thrust::device_vector<T>::const_iterator& arg_beg,
    const typename thrust::device_vector<T>::const_iterator& arg_end,
    const typename thrust::device_vector<T>::const_iterator& tau_beg,
    const typename thrust::device_vector<T>::const_iterator& tau_end,
    T tau,
    bool invert_tau);
  
private:
  std::array<std::vector<T>, ELEM_OPERATION::kCoeffsCount> coeffs_;
  std::array<thrust::device_vector<T>, ELEM_OPERATION::kCoeffsCount> d_coeffs_;  
};

template<typename T, class ELEM_OPERATION>
struct ElemOpCoefficients 
{
  T* dev_p[ELEM_OPERATION::kCoeffsCount];
  T val[ELEM_OPERATION::kCoeffsCount];
};


} // namespace prost

#endif // PROST_PROX_ELEM_OPERATION_HPP_
