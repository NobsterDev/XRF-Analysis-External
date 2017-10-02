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


#ifndef HDF5_IO_H
#define HDF5_IO_H


#include <list>
#include <mutex>
#include <queue>
#include <future>
#include "io/file/base_file_io.h"
#include "data_struct/base_dataset.h"
#include "data_struct/xrf/spectra_volume.h"
#include "data_struct/xrf/fit_element_map.h"
#include "hdf5.h"

//Include mda data structures to save scalers
#include "io/file/mda_io.h"

#include "data_struct/xrf/quantification_standard.h"
#include "data_struct/xrf/params_override.h"

namespace io
{
namespace file
{

enum H5_SPECTRA_LAYOUTS {MAPS_RAW, MAPS_V9, MAPS_V10, XSPRESS, APS_SEC20};

struct H5_Spectra_Layout
{

    H5_Spectra_Layout() {}

    size_t detector_num;

    std::string spectra_path;

    std::string elt_path;
    std::string ert_path;
    std::string incnt_path;
    std::string outcnt_path;

    bool is_line;
};

H5_Spectra_Layout Generate_Layout(H5_SPECTRA_LAYOUTS layout_def, size_t detector_num);

class DLL_EXPORT HDF5_IO : public Base_File_IO
{
public:

    static HDF5_IO* inst();

    ~HDF5_IO();

    /**
     * @brief lazy_load : Only load in the meta info, not the actual datasets
     * @param filename
     */
    virtual void lazy_load();

    /**
     * @brief load : Load the full dataset
     * @param filename
     */
    virtual bool load_dataset(std::string path, Base_Dataset* dset);

    bool load_spectra_volume(std::string path, size_t detector_num, data_struct::xrf::Spectra_Volume* spec_vol);

    bool load_spectra_volume_with_callback(std::string path,
                                           size_t detector_start,
                                           size_t detector_end,
                                           IO_Callback_Func_Def callback_func,
                                           void* user_data);

    bool load_spectra_line_xspress3(std::string path, size_t detector_num, data_struct::xrf::Spectra_Line* spec_row);

    //bool load_spectra_volume_confocal();

    bool load_and_integrate_spectra_volume(std::string path, size_t detector_num, data_struct::xrf::Spectra* spectra);

    bool load_spectra_vol_analyzed_h5(std::string path,
                                      size_t detector_num,
                                      data_struct::xrf::Spectra_Volume* spectra_volume,
                                      int row_idx_start = 0,
                                      int row_idx_end = -1,
                                      int col_idx_start = 0,
                                      int col_idx_end = -1);

    bool load_integrated_spectra_analyzed_h5(std::string path, size_t detector_num, data_struct::xrf::Spectra* spectra);

    bool load_integrated_spectra_analyzed_h5(hid_t file_id, data_struct::xrf::Spectra* spectra);

    bool generate_avg(std::string avg_filename, std::vector<std::string> files_to_avg);

    bool generate_stream_dataset(std::string dataset_directory,
                                 std::string dataset_name,
                                 int detector_num,
                                 size_t height,
                                 size_t width);

    bool save_stream_row(size_t d_hash,
                         size_t detector_num,
                         size_t row,
                         std::vector< data_struct::xrf::Spectra* >  *spectra_row);


    bool save_itegrade_spectra(data_struct::xrf::Spectra * spectra);

    bool close_dataset(size_t d_hash);

    bool start_save_seq(const std::string filename, bool force_new_file=false);

    bool start_save_seq(bool force_new_file=false){ return start_save_seq(_cur_filename, force_new_file);}

    void set_filename(std::string fname) {_cur_filename = fname;}

    bool save_spectra_volume(const std::string path,
                             data_struct::xrf::Spectra_Volume * spectra_volume,
                             real_t energy_offset,
                             real_t energy_slope,
                             real_t energy_quad,
                             size_t row_idx_start=0,
                             int row_idx_end=-1,
                             size_t col_idx_start=0,
                             int col_idx_end=-1);

    bool save_element_fits(const std::string path,
                           const data_struct::xrf::Fit_Count_Dict * const element_counts,
                           size_t row_idx_start=0,
                           int row_idx_end=-1,
                           size_t col_idx_start=0,
                           int col_idx_end=-1);

    bool save_quantification(data_struct::xrf::Quantification_Standard * quantification_standard,
                             size_t row_idx_start=0,
                             int row_idx_end=-1,
                             size_t col_idx_start=0,
                             int col_idx_end=-1);

    bool save_scan_scalers(size_t detector_num,
                           struct mda_file *mda_scalers,
                           data_struct::xrf::Params_Override * params_override,
                           bool hasNetcdf,
                           size_t row_idx_start=0,
                           int row_idx_end=-1,
                           size_t col_idx_start=0,
                           int col_idx_end=-1);

    bool end_save_seq();

private:

    HDF5_IO();

    static HDF5_IO *_this_inst;

    static std::mutex _mutex;

	bool _save_scan_meta_data(hid_t scan_grp_id, struct mda_file *mda_scalers);
	bool _save_extras(hid_t scan_grp_id, struct mda_file *mda_scalers);
    bool _save_scalers(hid_t maps_grp_id, struct mda_file *mda_scalers, size_t detector_num, data_struct::xrf::Params_Override * params_override, bool hasNetcdf);
    void _save_amps(hid_t scalers_grp_id, struct mda_file *mda_scalers, data_struct::xrf::Params_Override * params_override);

    void _gen_average(std::string full_hdf5_path, std::string dataset_name, hid_t src_analyzed_grp_id, hid_t dst_fit_grp_id, hid_t ocpypl_id, std::vector<hid_t> &hdf5_file_ids, bool avg=true);
    void _generate_avg_analysis(hid_t src_maps_grp_id, hid_t dst_maps_grp_id, std::string group_name, hid_t ocpypl_id, std::vector<hid_t> &hdf5_file_ids);
    void _generate_avg_integrated_spectra(hid_t src_analyzed_grp_id, hid_t dst_fit_grp_id, std::string group_name, hid_t ocpypl_id, std::vector<hid_t> &hdf5_file_ids);

    //bool save_scalar(const hid_t group_id,  mda_scan *mda_scalers)

    struct scaler_struct
    {
        scaler_struct(std::string name, int mda_idx_, int hdf_idx_, bool normalize_by_time_)
        {
             hdf_name = name;
             mda_idx = mda_idx_;
             hdf_idx = hdf_idx_;
             normalize_by_time = normalize_by_time_;
        }
        int mda_idx;
        int hdf_idx;
        std::string hdf_name;
        bool normalize_by_time;
    };

    hid_t _cur_file_id;
    std::string _cur_filename;

    //void _parse_group_info(hid_t h5file, hid_t id);
    //void _parse_dataset_info(hid_t h5file, hid_t id);
    //void _parse_attr_info(hid_t h5file, hid_t id);

};

}// end namespace file
}// end namespace io

#endif // HDF5_IO_H
