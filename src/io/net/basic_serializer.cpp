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

/// Initial Author <2017>: Arthur Glowacki


#include "io/net/basic_serializer.h"

#include <iostream>
#include <string>


namespace io
{
namespace net
{

//-----------------------------------------------------------------------------

Basic_Serializer::Basic_Serializer()
{

}

Basic_Serializer::~Basic_Serializer()
{

}

//-----------------------------------------------------------------------------

std::string Basic_Serializer::encode_counts(data_struct::Stream_Block* stream_block)
{

    std::string raw_msg = "";
    char *tmp_real = new char[sizeof(real_t)];
    char *tmp_ushort = new char[sizeof(unsigned short)];
    char *tmp_uint = new char[sizeof(unsigned int)];
    char *tmp_size_t = new char[sizeof(size_t)];

    //TODO:
    // add something to distinguish between counts and spectra
    // add something to tell if 4 or 8 byte real

    _convert_var_to_bytes(&raw_msg, tmp_uint, stream_block->detector_number, sizeof(unsigned int));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->row(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->col(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->height(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->width(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_real, stream_block->theta, sizeof(real_t));


    _convert_var_to_bytes(&raw_msg, tmp_uint, stream_block->fitting_blocks.size(), 4);

    // iterate through fitting routine
    for( auto& itr : stream_block->fitting_blocks)
    {
		_convert_var_to_bytes(&raw_msg, tmp_uint, itr.first, 4);
        _convert_var_to_bytes(&raw_msg, tmp_uint, itr.second.fit_counts.size(), 4);
        // iterate through elements counts
        for(auto &itr2 : itr.second.fit_counts)
        {
            raw_msg += itr2.first;
            raw_msg += '\0';
            _convert_var_to_bytes(&raw_msg, tmp_real, itr2.second, sizeof(real_t));
        }
    }

    delete [] tmp_real;
    delete [] tmp_ushort;
    delete [] tmp_uint;
    delete [] tmp_size_t;
	return raw_msg;
}

//-----------------------------------------------------------------------------

data_struct::Stream_Block* Basic_Serializer::decode_counts(char* message, size_t message_len)
{

    size_t idx = 0;
    size_t idx2 = 0;
	int detector_number = 0;
	size_t row = 0;
	size_t col = 0;
	size_t height = 0;
	size_t width = 0;
    float theta = 0;
	size_t proc_type_count = 0;
	data_struct::Fitting_Routines proc_type;
	size_t fit_block_size = 0;
	char name[256] = { 0 };
	real_t val = 0.0;

    memcpy(&detector_number, message + idx, sizeof(unsigned int));
    idx += sizeof(unsigned int);
    memcpy(&row, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&col, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&height, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&width, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&theta, message + idx, sizeof(real_t));
    idx += sizeof(real_t);

	data_struct::Stream_Block* out_stream_block = new data_struct::Stream_Block(row, col, height, width);
	out_stream_block->detector_number = detector_number;


	memcpy(&proc_type_count, message + idx, 4);
	idx += 4;
    for (size_t proc_type_itr = 0; proc_type_itr < proc_type_count; proc_type_itr++)
	{
		memcpy(&proc_type, message + idx, 4);
		idx += 4;
		out_stream_block->fitting_blocks[proc_type] = data_struct::Stream_Fitting_Block();

		//get fit_block[proc_type] size
		memcpy(&fit_block_size, message + idx, 4);
		idx += 4;
        for (size_t i = 0; i < fit_block_size; i++)
		{
			idx2 = idx;
			// find null term
			while (idx2 < message_len && message[idx2] != '\0')
			{
				idx2++;
			}

            if (idx2 >= message_len)
                break;

			memset(&name[0], 0, 255);
			if (idx2 - idx > 255)
			{
				memcpy(&name[0], message + idx, 255);
			}
			else
			{
				memcpy(&name[0], message + idx, idx2 - idx);
			}
			idx = idx2 + 1;
			memcpy(&val, message + idx, sizeof(real_t));
			idx += sizeof(real_t);
			out_stream_block->fitting_blocks[proc_type].fit_counts[std::string(name)] = val;
		}
	}
	return out_stream_block;

}

//-----------------------------------------------------------------------------

std::string Basic_Serializer::encode_spectra(data_struct::Stream_Block* stream_block)
{

    std::string raw_msg = "";
    char *tmp_real = new char[sizeof(real_t)];
    char *tmp_ushort = new char[sizeof(unsigned short)];
    char *tmp_uint = new char[sizeof(unsigned int)];
    char *tmp_size_t = new char[sizeof(size_t)];

    short send_cnt = 0;

    std::map<unsigned short, real_t> spec_indx_values;

    //TODO:
    // add something to distinguish between counts and spectra
    // add something to tell if 4 or 8 byte real

    _convert_var_to_bytes(&raw_msg, tmp_uint, stream_block->detector_number, sizeof(unsigned int));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->row(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->col(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->height(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_size_t, stream_block->width(), sizeof(size_t));
    _convert_var_to_bytes(&raw_msg, tmp_real, stream_block->theta, sizeof(real_t));


    _convert_var_to_bytes(&raw_msg, tmp_real, stream_block->spectra->elapsed_livetime(), sizeof(real_t));
    _convert_var_to_bytes(&raw_msg, tmp_real, stream_block->spectra->elapsed_realtime(), sizeof(real_t));
    _convert_var_to_bytes(&raw_msg, tmp_real, stream_block->spectra->input_counts(), sizeof(real_t));
    _convert_var_to_bytes(&raw_msg, tmp_real, stream_block->spectra->output_counts(), sizeof(real_t));
    _convert_var_to_bytes(&raw_msg, tmp_uint, stream_block->spectra->rows(), 4);

    //first get count of how many
    for(unsigned short i=0; i<stream_block->spectra->rows(); i++)
    {

        real_t spec_val = (*(stream_block->spectra))[i];
        if(spec_val > 0.0)
        {
            send_cnt++;
            spec_indx_values.emplace(i, spec_val);
        }
    }

    //if we have more than half the spec it doesn't help to to send index/values pairs
//    if(send_cnt < (stream_block->spectra->rows() / 2))
//    {

    _convert_var_to_bytes(&raw_msg, tmp_ushort, send_cnt, sizeof(unsigned short));

    //encode values
    for(auto const & itr : spec_indx_values)
    {
        _convert_var_to_bytes(&raw_msg, tmp_ushort, itr.first, sizeof(unsigned short));
        _convert_var_to_bytes(&raw_msg, tmp_real, itr.second, sizeof(real_t));
    }

//    }

    delete [] tmp_real;
    delete [] tmp_ushort;
    delete [] tmp_uint;
    delete [] tmp_size_t;
    return raw_msg;
}

//-----------------------------------------------------------------------------

data_struct::Stream_Block* Basic_Serializer::decode_spectra(char* message, size_t message_len)
{

    size_t idx = 0;
    int detector_number = 0;
    size_t row = 0;
    size_t col = 0;
    size_t height = 0;
    size_t width = 0;
    float theta = 0;
    real_t elt = 0;
    real_t ert = 0;
    real_t incnt = 0;
    real_t outcnt = 0;
    size_t spectra_size = 0;
    unsigned short recv_cnt = 0;
    unsigned short spec_index = 0;
    real_t spec_value = 0.0f;

    memcpy(&detector_number, message + idx, sizeof(unsigned int));
    idx += sizeof(unsigned int);
    memcpy(&row, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&col, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&height, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&width, message + idx, sizeof(size_t));
    idx += sizeof(size_t);
    memcpy(&theta, message + idx, sizeof(real_t));
    idx += sizeof(real_t);

    data_struct::Stream_Block* out_stream_block = new data_struct::Stream_Block(row, col, height, width);
    out_stream_block->detector_number = detector_number;


    memcpy(&elt, message + idx, sizeof(real_t));
    idx += sizeof(real_t);
    memcpy(&ert, message + idx, sizeof(real_t));
    idx += sizeof(real_t);
    memcpy(&incnt, message + idx, sizeof(real_t));
    idx += sizeof(real_t);
    memcpy(&outcnt, message + idx, sizeof(real_t));
    idx += sizeof(real_t);

    memcpy(&spectra_size, message + idx, 4);
    idx += 4;
    out_stream_block->spectra = new data_struct::Spectra(spectra_size);
    out_stream_block->spectra->elapsed_livetime(elt);
    out_stream_block->spectra->elapsed_realtime(ert);
    out_stream_block->spectra->input_counts(incnt);
    out_stream_block->spectra->output_counts(outcnt);

    memcpy(&recv_cnt, message + idx, sizeof(unsigned short));
    idx += sizeof(unsigned short);

    for (unsigned short i = 0; i < recv_cnt; i++)
    {
        memcpy(&spec_index, message + idx, sizeof(unsigned short));
        idx += sizeof(unsigned short);
        memcpy(&spec_value, message + idx, sizeof(real_t));
        idx += sizeof(real_t);
        (*(out_stream_block->spectra))[spec_index] = spec_value;
    }
    return out_stream_block;

}


//-----------------------------------------------------------------------------

} //end namespace net
}// end namespace io
