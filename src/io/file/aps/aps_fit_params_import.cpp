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



#include "aps_fit_params_import.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

namespace io
{
namespace file
{
namespace aps
{

/***
 * Translation map from APS file tags to internal tags
 */
const std::unordered_map<std::string, std::string> FILE_TAGS_TRANSLATION = {
    std::pair<std::string, std::string>("CAL_OFFSET_[E_OFFSET]", data_struct::xrf::STR_E_OFFSET),
    std::pair<std::string, std::string>("CAL_OFFSET_[E_OFFSET]_MAX", data_struct::xrf::STR_E_OFFSET),
    std::pair<std::string, std::string>("CAL_OFFSET_[E_OFFSET]_MIN", data_struct::xrf::STR_E_OFFSET),
    std::pair<std::string, std::string>("CAL_SLOPE_[E_LINEAR]", data_struct::xrf::STR_E_LINEAR),
    std::pair<std::string, std::string>("CAL_SLOPE_[E_LINEAR]_MAX", data_struct::xrf::STR_E_LINEAR),
    std::pair<std::string, std::string>("CAL_SLOPE_[E_LINEAR]_MIN", data_struct::xrf::STR_E_LINEAR),
    std::pair<std::string, std::string>("CAL_QUAD_[E_QUADRATIC]", data_struct::xrf::STR_E_QUADRATIC),
    std::pair<std::string, std::string>("CAL_QUAD_[E_QUADRATIC]_MAX", data_struct::xrf::STR_E_QUADRATIC),
    std::pair<std::string, std::string>("CAL_QUAD_[E_QUADRATIC]_MIN", data_struct::xrf::STR_E_QUADRATIC),
    std::pair<std::string, std::string>("FWHM_OFFSET", data_struct::xrf::STR_FWHM_OFFSET),
    std::pair<std::string, std::string>("FWHM_FANOPRIME", data_struct::xrf::STR_FWHM_FANOPRIME),
    std::pair<std::string, std::string>("COHERENT_SCT_ENERGY", data_struct::xrf::STR_COHERENT_SCT_ENERGY),
    std::pair<std::string, std::string>("COHERENT_SCT_ENERGY_MAX", data_struct::xrf::STR_COHERENT_SCT_ENERGY),
    std::pair<std::string, std::string>("COHERENT_SCT_ENERGY_MIN", data_struct::xrf::STR_COHERENT_SCT_ENERGY),
    std::pair<std::string, std::string>("COMPTON_ANGLE", data_struct::xrf::STR_COMPTON_ANGLE),
    std::pair<std::string, std::string>("COMPTON_ANGLE_MAX", data_struct::xrf::STR_COMPTON_ANGLE),
    std::pair<std::string, std::string>("COMPTON_ANGLE_MIN", data_struct::xrf::STR_COMPTON_ANGLE),
    std::pair<std::string, std::string>("COMPTON_FWHM_CORR", data_struct::xrf::STR_COMPTON_FWHM_CORR),
    std::pair<std::string, std::string>("COMPTON_STEP", data_struct::xrf::STR_COMPTON_F_STEP),
    std::pair<std::string, std::string>("COMPTON_F_TAIL", data_struct::xrf::STR_COMPTON_F_TAIL),
    std::pair<std::string, std::string>("COMPTON_GAMMA", data_struct::xrf::STR_COMPTON_GAMMA),
    std::pair<std::string, std::string>("COMPTON_HI_F_TAIL", data_struct::xrf::STR_COMPTON_HI_F_TAIL),
    std::pair<std::string, std::string>("COMPTON_HI_GAMMA", data_struct::xrf::STR_COMPTON_HI_GAMMA),
    std::pair<std::string, std::string>("STEP_OFFSET", data_struct::xrf::STR_F_STEP_OFFSET),
    std::pair<std::string, std::string>("STEP_LINEAR", data_struct::xrf::STR_F_STEP_LINEAR),
    std::pair<std::string, std::string>("STEP_QUADRATIC", data_struct::xrf::STR_F_STEP_QUADRATIC),
    std::pair<std::string, std::string>("F_TAIL_OFFSET", data_struct::xrf::STR_F_TAIL_OFFSET),
    std::pair<std::string, std::string>("F_TAIL_LINEAR", data_struct::xrf::STR_F_TAIL_LINEAR),
    std::pair<std::string, std::string>("F_TAIL_QUADRATIC", data_struct::xrf::STR_F_TAIL_QUADRATIC),
    std::pair<std::string, std::string>("KB_F_TAIL_OFFSET", data_struct::xrf::STR_KB_F_TAIL_OFFSET),
    std::pair<std::string, std::string>("KB_F_TAIL_LINEAR", data_struct::xrf::STR_KB_F_TAIL_LINEAR),
    std::pair<std::string, std::string>("KB_F_TAIL_QUADRATIC", data_struct::xrf::STR_KB_F_TAIL_QUADRATIC),
    std::pair<std::string, std::string>("GAMMA_OFFSET", data_struct::xrf::STR_GAMMA_OFFSET),
    std::pair<std::string, std::string>("GAMMA_LINEAR", data_struct::xrf::STR_GAMMA_LINEAR),
    std::pair<std::string, std::string>("GAMMA_QUADRATIC", data_struct::xrf::STR_GAMMA_QUADRATIC),
    std::pair<std::string, std::string>("SNIP_WIDTH", data_struct::xrf::STR_SNIP_WIDTH),
    std::pair<std::string, std::string>("SI_ESCAPE_FACTOR", data_struct::xrf::STR_SI_ESCAPE),
    std::pair<std::string, std::string>("GE_ESCAPE_FACTOR", data_struct::xrf::STR_GE_ESCAPE),
    std::pair<std::string, std::string>("ESCAPE_LINEAR", data_struct::xrf::STR_ESCAPE_LINEAR)
};

APS_Fit_Params_Import::APS_Fit_Params_Import()
{



}

APS_Fit_Params_Import::~APS_Fit_Params_Import()
{


}

bool APS_Fit_Params_Import::load(std::string path,
                                 data_struct::xrf::Element_Info_Map *element_info_map,
                                 data_struct::xrf::Fit_Parameters* out_fit_params,
                                 std::unordered_map<std::string, data_struct::xrf::Fit_Element_Map*>* out_elements_to_fit)
{

    std::ifstream paramFileStream(path);


    if (paramFileStream.is_open() )
    {
        //paramFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        paramFileStream.exceptions(std::ifstream::failbit);
        //std::string line;
        std::string tag;
        try
        {
            for (std::string line; std::getline(paramFileStream, line); )
            //while(std::getline(paramFileStream, line))
            {
                std::istringstream strstream(line);
                std::getline(strstream, tag, ':');
                //std::cout<<"tag : "<<tag<<std::endl;
                if (tag == "VERSION" || tag == "DATE")
                {
                    std::cout << line << std::endl;
                }
                else if (tag == "DETECTOR_ELEMENTS")
                {

                }
                else if (tag == "ELEMENTS_TO_FIT")
                {

                    std::string element_symb;
                    while(std::getline(strstream, element_symb, ','))
                    {
                        element_symb.erase(std::remove_if(element_symb.begin(), element_symb.end(), ::isspace), element_symb.end());

                        // check if element_symb contains '_'
                        std::string base_element_symb = element_symb.substr(0, element_symb.find_last_of("_"));

                        //std::cout<<"Element : "<<element_symb<<" : "<<base_element_symb<<std::endl;

                        data_struct::xrf::Element_Info* e_info = element_info_map->get_element(base_element_symb);
                        if(e_info == nullptr)
                        {
                            std::cout<<"Can not find element "<<base_element_symb<<std::endl;
                        }
                        else
                        {
                            data_struct::xrf::Fit_Element_Map* fit_map;
                            if(out_elements_to_fit->count(element_symb) > 0)
                            {
                                fit_map = (*out_elements_to_fit)[element_symb];
                            }
                            else
                            {
                                fit_map = new data_struct::xrf::Fit_Element_Map(element_symb, e_info);
                                (*out_elements_to_fit)[element_symb] = fit_map;
                            }
                        }
                    }

                }
                else if (tag == "ELEMENTS_WITH_PILEUP")
                {
                    /* compile, need to update logic
                    std::string element_symb;
                    while(std::getline(strstream, element_symb, ','))
                    {
                        element_symb.erase(std::remove_if(element_symb.begin(), element_symb.end(), ::isspace), element_symb.end());
                        std::cout<<"Element with pileup : "<<element_symb<<std::endl;
                        //data_struct::xrf::Element_Param* element_param = new data_struct::xrf::Element_Param();
                        //element_param->name = element_symb;
                        //out_fit_params->append_element(element_param);
                    }
                    */
                }
                else if(FILE_TAGS_TRANSLATION.count(tag)> 0)
                {
                    std::string tag_name = FILE_TAGS_TRANSLATION.at(tag);
                    if( false == out_fit_params->contains(tag_name) )
                    {
                        out_fit_params->add_parameter(tag_name, data_struct::xrf::Fit_Param(tag_name));
                    }

                    std::string str_value;
                    std::getline(strstream, str_value, ':');
                    float fvalue = std::stof(str_value);

                    if (tag.find("_MAX") != std::string::npos)
                    {
                        (*out_fit_params)[tag_name].max_val = fvalue;
                    }
                    else if (tag.find("_MIN") != std::string::npos)
                    {
                        (*out_fit_params)[tag_name].min_val = fvalue;
                    }
                    else
                    {
                        (*out_fit_params)[tag_name].value = fvalue;
                    }
                }
                else if (tag == "BRANCHING_RATIO_ADJUSTMENT_L" || tag == "BRANCHING_RATIO_ADJUSTMENT_K")
                {
                    unsigned int cnt = 0;

                    if (tag == "BRANCHING_FAMILY_ADJUSTMENT_L")
                    {
                        cnt = 3;
                    }
                    else if (tag == "BRANCHING_RATIO_ADJUSTMENT_K")
                    {
                        cnt = 4;
                    }
                    else if (tag == "BRANCHING_RATIO_ADJUSTMENT_L")
                    {
                        cnt = 12;
                    }

                    std::string element_symb;
                    std::string str_value;

                    std::getline(strstream, element_symb, ',');
                    element_symb.erase(std::remove_if(element_symb.begin(), element_symb.end(), ::isspace), element_symb.end());

                    data_struct::xrf::Fit_Element_Map* fit_map;
                    if(out_elements_to_fit->count(element_symb) > 0)
                    {
                        fit_map = (*out_elements_to_fit)[element_symb];
                        for (unsigned int i = 0; i<cnt; i++)
                        {
                            float factor = 1.0;
                            std::getline(strstream, str_value, ',');
                            factor = std::stof(str_value);
                            fit_map->set_custom_multiply_ratio(i, factor);
                        }
                    }
                }
                else if (tag == "FIT_SNIP_WIDTH")
                {
                    //TODO add to fit params
                    /*
                    data_struct::xrf::Fit_Param* fit_param = out_fit_params->get_fit_param(data_struct::xrf::STR_SNIP_WIDTH);
                    if (fit_param == nullptr)
                    {
                        fit_param = new data_struct::xrf::Fit_Param(data_struct::xrf::STR_SNIP_WIDTH);
                        out_fit_params->append_fit_param(fit_param);
                    }
                    fit_param->bound_type = data_struct::xrf::LIMITED_LO_HI;
                    */
                }
                else if (tag == "DS_AMP_SENS_UNIT")
                {
                    std::cout<<"break "<<std::endl;
                }
                //todo
                //FIT_SNIP_WIDTH = use
                //DETECTOR_MATERIAL =  0= Germanium, 1 = Si
                //BE_WINDOW_THICKNESS = general
                //DET_CHIP_THICKNESS = general
                //GE_DEAD_LAYER = general
                //MAX_ENERGY_TO_FIT = general
                //MIN_ENERGY_TO_FIT = general
                //TAIL_FRACTION_ADJUST_SI
                //TAIL_WIDTH_ADJUST_SI
                //SI_ESCAPE_ENABLE = use and batch
                //GE_ESCAPE_ENABLE = use and batch
            }
        }
        catch(std::exception e)
        {
            if (paramFileStream.eof() == 0 && (paramFileStream.bad() || paramFileStream.fail()) )
            {
                std::cerr << "ios Exception happened: " << e.what() << "\n"
                    << "Error bits are: "
                    << "\nfailbit: " << paramFileStream.fail()
                    << "\neofbit: " << paramFileStream.eof()
                    << "\nbadbit: " << paramFileStream.bad() << std::endl;
            }
        }

        paramFileStream.close();
        return true;
    }
    return false;

}


} //end namespace aps
} //end namespace file
}// end namespace io