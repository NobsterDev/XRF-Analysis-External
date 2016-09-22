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



#ifndef Optimizer_H
#define Optimizer_H

#include "fit_parameters.h"
#include "calibration_standard.h"
#include "base_model.h"


namespace fitting
{
namespace optimizers
{

using namespace std;
using namespace data_struct::xrf;
using namespace fitting::models;


enum OPTIMIZER_INFO { IMPROPER_INPUT, MOST_TOL, EXCEED_CALL, TOL_TOO_SMALL, NO_PROGRESS };

/**
 * @brief The User_Data struct : Structure used by minimize function for optimizers
 */
struct User_Data
{
    Base_Model* fit_model;
    Spectra *spectra;
    std::valarray<real_t> *weights;
    Fit_Parameters *fit_parameters;
    Calibration_Standard *calibration;
    Fit_Element_Map_Dict *elements;
    Range *energy_range;
    //Fit_Counts_Array* counts_arr;
};

/**
 * @brief The Optimizer class : Base class for error minimization to find optimal specta model
 */
class DLL_EXPORT Optimizer
{
public:
    Optimizer(){}

    ~Optimizer(){}

    virtual void minimize(Fit_Parameters *fit_params,
                          const Spectra * const spectra,
                          const Calibration_Standard * const calibration,
                          const Fit_Element_Map_Dict * const elements_to_fit,
                          Base_Model* model) = 0;

private:


};

} //namespace optimizers

} //namespace fitting

#endif // Optimizer