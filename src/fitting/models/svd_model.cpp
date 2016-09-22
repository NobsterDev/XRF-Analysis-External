/***
Copyright (c) 2016, UChicago Argonne, LLC. All rights reserved.

Copyright 2016. UChicago Argonne, LLC. This software was produced
under U.S. Government contract DE-AC02-06CH11357 for Argonne National
Laboratory (ANL), which is operated by UChicago Argonne, LLC for the
U.S. Department of Energy. The U.S. Government has rights to use,
reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR
UChicago Argonne, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is
modified to produce derivative works, such modified software should
be clearly marked, so as not to confuse it with the version available
from ANL.

Additionally, redistribution and use in source and binary forms, with
or without modification, are permitted provided that the following
conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

    * Neither the name of UChicago Argonne, LLC, Argonne National
      Laboratory, ANL, the U.S. Government, nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY UChicago Argonne, LLC AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL UChicago
Argonne, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
***/

/// Initial Author <2016>: Arthur Glowacki



#include "svd_model.h"

#include <Eigen/SVD>

//debug
#include <iostream>

namespace fitting
{
namespace models
{

SVD_Model::SVD_Model() : Gauss_Matrix_Model()
{

    _update_element_guess_value = false;
    _counts_log_10 = false;
    _snip_background = false;

}

// ----------------------------------------------------------------------------

SVD_Model::~SVD_Model()
{

}

// ----------------------------------------------------------------------------

void SVD_Model::initialize(Fit_Parameters *fit_params,
                               const Calibration_Standard * const calibration,
                               const Fit_Element_Map_Dict * const elements_to_fit,
                               const struct Range energy_range)
{

    Base_Model::initialize(fit_params, calibration, elements_to_fit, energy_range);

    unordered_map<string, Spectra> element_models = _generate_element_models(fit_params, calibration, elements_to_fit, energy_range);

    _generate_fitmatrix(fit_params, &element_models, energy_range);

}

// ----------------------------------------------------------------------------

void SVD_Model::_generate_fitmatrix(Fit_Parameters *fit_params,
                                        const unordered_map<string, Spectra> * const element_models,
                                        struct Range energy_range)
{

    _fitmatrix.resize(energy_range.count(), element_models->size());
    int i = 0;
    for(const auto& itr : *element_models)
    {
        //Spectra element_model = itr.second;
        for (int j=0; j<itr.second.size(); j++)
        {
            _fitmatrix(j,i) = itr.second[j];
        }
        //save element index for later
        (*fit_params)[itr.first].opt_array_index = i;
        i++;
    }

}

// ----------------------------------------------------------------------------

Spectra SVD_Model::model_spectrum(const Fit_Parameters * const fit_params,
                                      const Spectra * const spectra,
                                      const Calibration_Standard * const calibration,
                                      const Fit_Element_Map_Dict * const elements_to_fit,
                                      const struct Range energy_range)
{
    //dummy function
    return *spectra;
}

// ----------------------------------------------------------------------------

void SVD_Model::_fit_spectra(Fit_Parameters *fit_params,
                                 const Spectra * const spectra,
                                 const Calibration_Standard * const calibration,
                                 const Fit_Element_Map_Dict * const elements_to_fit)
{

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(_fitmatrix, Eigen::ComputeThinU | Eigen::ComputeThinV );
    Eigen::VectorXd rhs;
    rhs.resize(spectra->size());
    //assert _fitmatrix.rows() == spectra->size()

    for(size_t i=0; i<spectra->size(); i++)
    {
        rhs[i] = (*spectra)[i];
    }

    Eigen::VectorXd result = svd.solve(rhs);
    //std::cout << "SVD Result : "<<std::endl<< result <<std::endl;

    for(const auto& itr : *elements_to_fit)
    {
        Fit_Param param = (*fit_params)[itr.first];
        (*fit_params)[itr.first].value = result[param.opt_array_index];
    }

}

} //namespace models
} //namespace fitting