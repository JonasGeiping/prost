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

#ifndef PROST_BACKEND_ADMM_HPP_
#define PROST_BACKEND_ADMM_HPP_

#include <thrust/device_vector.h>
#include <cublas_v2.h>

#include "prost/backend/backend.hpp"
#include "prost/common.hpp"

namespace prost {

template<typename T> class Prox;

template<typename T>
class BackendADMM : public Backend<T>
{
public:
  struct Options
  {
    /// \brief initial step size
    double rho0;

    // \brief over-relaxation factor
    double alpha;

    /// \brief factor for increasing cg tolerance every iteration
    double cg_tol_pow;

    /// \brief initial cg tolerance
    double cg_tol_min;

    /// \brief maximal cg tolerance
    double cg_tol_max;

    /// \brief maximum number of cg iterations
    int cg_max_iter;

    /// \brief Every how many iterations to compute the residuals?
    int residual_iter;

    /// \brief Parameters for residual converging step size scheme.
    T arb_delta, arb_tau, arb_gamma;
  };

  BackendADMM(const typename BackendADMM<T>::Options& opts);
  virtual ~BackendADMM();

  virtual void Initialize();
  virtual void PerformIteration();
  virtual void Release();

  virtual void current_solution(vector<T>& primal, vector<T>& dual);

  virtual void current_solution(vector<T>& primal_x,
                                vector<T>& primal_z,
                                vector<T>& dual_y,
                                vector<T>& dual_w);

  /// \brief Returns amount of gpu memory required in bytes.
  virtual size_t gpu_mem_amount() const;

private:
  thrust::device_vector<T> x_half_, z_half_;
  thrust::device_vector<T> x_proj_, z_proj_;
  thrust::device_vector<T> x_dual_, z_dual_;
  thrust::device_vector<T> temp1_, temp2_, temp3_;
  
  /// \brief ADMM-specific options.
  typename BackendADMM<T>::Options opts_;

  // Current step size parameters
  T rho_;
  T delta_;
  int arb_u_;
  int arb_l_;

  /// \brief Internal iteration counter.
  size_t iteration_;

  /// \brief Internal prox_g
  vector< shared_ptr<Prox<T> > > prox_g_;

  /// \brief Internal prox_f
  vector< shared_ptr<Prox<T> > > prox_f_;

  /// \brief cuBLAS handle
  cublasHandle_t hdl_;
};

} // namespace prost

#endif // PROST_BACKEND_ADMM_HPP
