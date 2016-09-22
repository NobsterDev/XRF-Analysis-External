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



#ifndef Gauss_Tails_Model_H
#define Gauss_Tails_Model_H

#include "base_model.h"
#include "optimizer.h"
#include "fit_parameters.h"
#include <valarray>

namespace fitting
{
namespace models
{

using namespace std;

const string STR_FWHM_OFFSET            = "FWHM_OFFSET";
const string STR_FWHM_FANOPRIME         = "FWHM_FANOPRIME";

const string STR_COHERENT_SCT_ENERGY    = "COHERENT_SCT_ENERGY";
const string STR_COHERENT_SCT_AMPLITUDE = "COHERENT_SCT_AMPLITUDE";

const string STR_COMPTON_ANGLE          = "COMPTON_ANGLE";
const string STR_COMPTON_FWHM_CORR      = "COMPTON_FWHM_CORR";
const string STR_COMPTON_AMPLITUDE      = "COMPTON_AMPLITUDE";
const string STR_COMPTON_F_STEP         = "COMPTON_F_STEP";
const string STR_COMPTON_F_TAIL         = "COMPTON_F_TAIL";
const string STR_COMPTON_GAMMA          = "COMPTON_GAMMA";
const string STR_COMPTON_HI_F_TAIL      = "COMPTON_HI_F_TAIL";
const string STR_COMPTON_HI_GAMMA       = "COMPTON_HI_GAMMA";

const string STR_SNIP_WIDTH             = "SNIP_WIDTH";

const string STR_F_STEP_OFFSET          = "F_STEP_OFFSET";
const string STR_F_STEP_LINEAR          = "F_STEP_LINEAR";
const string STR_F_STEP_QUADRATIC       = "F_STEP_QUADRATIC";

const string STR_F_TAIL_OFFSET          = "F_TAIL_OFFSET";
const string STR_F_TAIL_LINEAR          = "F_TAIL_LINEAR";
const string STR_F_TAIL_QUADRATIC       = "F_TAIL_QUADRATIC";

const string STR_GAMMA_OFFSET           = "GAMMA_OFFSET";
const string STR_GAMMA_LINEAR           = "GAMMA_LINEAR";
const string STR_GAMMA_QUADRATIC        = "GAMMA_QUADRATIC";

const string STR_KB_F_TAIL_OFFSET       = "KB_F_TAIL_OFFSET";
const string STR_KB_F_TAIL_LINEAR       = "KB_F_TAIL_LINEAR";
const string STR_KB_F_TAIL_QUADRATIC    = "KB_F_TAIL_QUADRATIC";


using namespace data_struct::xrf;
using namespace fitting::optimizers;

/**
 * @brief gauss_peak :  models a gaussian fluorescence peak, see also van espen, spectrum evaluation,
                        in van grieken, handbook of x-ray spectrometry, 2nd ed, page 182 ff
                        gain / (sigma * sqrt( 2.0 * M_PI) ) * exp( -0.5 * ( (delta_energy / sigma) ** 2 )
 * @param gain
 * @param sigma
 * @param delta_energy
 * @return
 */
DLL_EXPORT valarray<real_t> gauss_peak(real_t gain, real_t sigma, valarray<real_t>& delta_energy);

/**
 * @brief gauss_step : gain / 2.0 /  peak_E * Faddeeva::erfc(delta_energy/(M_SQRT2 * sigma));
 * @param gain
 * @param sigma
 * @param delta_energy
 * @param peak_E
 * @return
 */
DLL_EXPORT valarray<real_t> gauss_step(real_t gain, real_t sigma, valarray<real_t>& delta_energy, real_t peak_E);

DLL_EXPORT valarray<real_t> gauss_tail(real_t gain, real_t sigma, valarray<real_t>& delta_energy, real_t gamma);

DLL_EXPORT valarray<real_t> elastic_peak(const Fit_Parameters * const fitp, valarray<real_t> ev, real_t gain);

DLL_EXPORT valarray<real_t> compton_peak(const Fit_Parameters * const fitp, valarray<real_t> ev, real_t gain);


class DLL_EXPORT Gauss_Tails_Model: public Base_Model
{
public:
    Gauss_Tails_Model();

    ~Gauss_Tails_Model();

    virtual Fit_Parameters get_fit_parameters();

    virtual Spectra model_spectrum(const Fit_Parameters * const fit_params,
                                   const Spectra * const spectra,
                                   const Calibration_Standard * const calibration,
                                   const Fit_Element_Map_Dict * const elements_to_fit,
                                   const struct Range energy_range);

    Spectra model_spectrum_element(const Fit_Parameters * const fitp,
                                   const Fit_Element_Map * const element_to_fit,
                                   const Calibration_Standard * const calibration,
                                   valarray<real_t> energy);

    void set_optimizer(Optimizer *optimizer);

protected:

    void _calc_and_update_coherent_amplitude(Fit_Parameters *fitp,
                                             const Spectra * const spectra,
                                             const Calibration_Standard * const calibration);

    virtual void _pre_process(Fit_Parameters *fit_params,
                              const Spectra * const spectra,
                              const Calibration_Standard * const calibration,
                              const Fit_Element_Map_Dict * const elements_to_fit);

    virtual void _fit_spectra(Fit_Parameters *fit_params,
                              const Spectra * const spectra,
                              const Calibration_Standard * const calibration,
                              const Fit_Element_Map_Dict * const elements_to_fit);

    virtual void _post_process(Fit_Parameters *fit_params,
                               const Spectra * const spectra,
                               const Calibration_Standard * const calibration,
                               const Fit_Element_Map_Dict * const elements_to_fit,
                               Fit_Count_Dict *out_counts_dic,
                               size_t row_idx,
                               size_t col_idx);

    bool _snip_background;

    valarray<real_t> _background_counts;

private:

    Optimizer *_optimizer;

};

} //namespace models

} //namespace fitting

#endif // Gauss_Tails_Model_H