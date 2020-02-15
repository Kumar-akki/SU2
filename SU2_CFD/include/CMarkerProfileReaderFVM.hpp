/*!
 * \file CMarkerProfileReaderFVM.hpp
 * \brief Header file for the class CMarkerProfileReaderFVM.
 *        The implementations are in the <i>CMarkerProfileReaderFVM.cpp</i> file.
 * \author T. Economon
 * \version 6.2.0 "Falcon"
 *
 * The current SU2 release has been coordinated by the
 * SU2 International Developers Society <www.su2devsociety.org>
 * with selected contributions from the open-source community.
 *
 * The main research teams contributing to the current release are:
 *  - Prof. Juan J. Alonso's group at Stanford University.
 *  - Prof. Piero Colonna's group at Delft University of Technology.
 *  - Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *  - Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *  - Prof. Rafael Palacios' group at Imperial College London.
 *  - Prof. Vincent Terrapon's group at the University of Liege.
 *  - Prof. Edwin van der Weide's group at the University of Twente.
 *  - Lab. of New Concepts in Aeronautics at Tech. Institute of Aeronautics.
 *
 * Copyright 2012-2019, Francisco D. Palacios, Thomas D. Economon,
 *                      Tim Albring, and the SU2 contributors.
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#include "../../Common/include/mpi_structure.hpp"
#include "../../Common/include/CConfig.hpp"
#include "../../Common/include/geometry/CGeometry.hpp"

using namespace std;

/*!
 * \class CMarkerProfileReaderFVM
 * \brief Class for the marker profile reader of the finite volume solver (FVM).
 * \author: T. Economon
 */
class CMarkerProfileReaderFVM {
  
protected:
  
  int rank;  /*!< \brief MPI Rank. */
  int size;  /*!< \brief MPI Size. */
  
  CConfig *config;  /*!< \brief Local pointer to the config parameter object. */
  
  CGeometry *geometry;  /*!< \brief Local pointer to the geometry object. */

  unsigned short dimension;  /*!< \brief Dimension of the problem (2 or 3). */
  unsigned short markerType;  /*!< \brief Type of marker where the profiles are being applied. */

  unsigned short numberOfVars;  /*!< \brief Number of variables added to the number of coordinates to write each line in the template profile file. */

  unsigned long numberOfProfiles;  /*!< \brief Auxiliary structure for holding the number of markers in a profile file. */

  string filename;  /*!< \brief File name of the marker profile file. */
  
  vector<string> profileTags;  /*!< \brief Auxiliary structure for holding the string names of the markers in a profile file. */
  
  vector<unsigned long> numberOfRowsInProfile;  /*!< \brief Auxiliary structure for holding the number of rows for a particular marker in a profile file. */
  vector<unsigned long> numberOfColumnsInProfile;  /*!< \brief Auxiliary structure for holding the number of columns for a particular marker in a profile file. */
  
  vector<vector<passivedouble> > profileData;  /*!< \brief Auxiliary structure for holding the data values from a profile file. */
  vector<su2double> ColumnData;  /*!< \brief Auxiliary structure for holding the column values from a profile file. */
  vector<vector<vector<su2double> > > profileCoords;  /*!< \brief Data structure for holding the merged inlet boundary coordinates from all ranks. */
  
private:
  
  /*!
   * \brief Read a native SU2 marker profile file in ASCII format.
   */
  void ReadMarkerProfile();
  
  /*!
   * \brief Merge the node coordinates of all profile-type boundaries from all processors.
   */
  void MergeProfileMarkers();
  
  /*!
   * \brief Write a template profile file if the requested file is not found.
   */
  void WriteMarkerProfileTemplate();
  
public:
  
  /*!
   * \brief Constructor of the CMarkerProfileReaderFVM class.
   * \param[in] val_geometry    - Pointer to the current geometry
   * \param[in] val_config      - Pointer to the current config structure
   * \param[in] val_filename    - Name of the profile file to be read
   * \param[in] val_kind_marker - Type of marker where profile will be applied
   * \param[in] val_number_vars - Number of columns of profile data to be written to template file (excluding coordinates)
   */
  CMarkerProfileReaderFVM(CGeometry      *val_geometry,
                          CConfig        *val_config,
                          string         val_filename,
                          unsigned short val_kind_marker,
                          unsigned short val_number_vars);
  
  /*!
   * \brief Destructor of the CMeshReaderFVM class.
   */
  ~CMarkerProfileReaderFVM(void);
  
  /*!
   * \brief Get the number of profiles found within the input file.
   * \returns Number of profiles found within the input file.
   */
  inline unsigned long GetNumberOfProfiles() {
    return numberOfProfiles;
  }
  
  /*!
   * \brief Get the string tag for the marker where the profile is applied.
   * \param[in] val_iProfile - current profile index.
   * \returns String tag for the marker where the profile is applied.
   */
  inline const string &GetTagForProfile(int val_iProfile) const {
    return profileTags[val_iProfile];
  }
  
  /*!
   * \brief Get the number of rows of data in a profile.
   * \param[in] val_iProfile - current profile index.
   * \returns Number of rows of data in a profile.
   */
  inline unsigned long GetNumberOfRowsInProfile(int val_iProfile) {
    return numberOfRowsInProfile[val_iProfile];
  }

  /*!
   * \brief Get the number of columns of data in a profile.
   * \param[in] val_iProfile - current profile index.
   * \returns Number of columns of data in a profile.
   */
  inline unsigned long GetNumberOfColumnsInProfile(int val_iProfile) {
    return numberOfColumnsInProfile[val_iProfile];
  }
  
    /*!
   * \brief Get the number of columns of data in a profile if an interpolation method was used (multigrid).
   * \param[in] val_iProfile - current profile index.
   * \param[in] Interpolate - bool value for Interpolation
   * \returns Number of columns of data in the (if) interpolated profile.
   */
  inline unsigned long GetNumberOfColumnsInProfile(int val_iProfile, bool Interpolate) {
    if (Interpolate == true) {return numberOfColumnsInProfile[val_iProfile]+dimension;}
    else {return numberOfColumnsInProfile[val_iProfile];}
  }

  /*!
   * \brief Get the 1D vector of data for a profile from the input file.
   * \param[in] val_iProfile - current profile index.
   * \returns 1D vector of data for a profile from the input file.
   */
  inline const vector<passivedouble> &GetDataForProfile(int val_iProfile) const {
    return profileData[val_iProfile];
  }
  
      /*!
   * \brief Get the data for the specific column if interpolation being done.
   * \param[in] val_iProfile - current profile index.
   * \param[in] iCol - the column whose data is required
   * \returns the specific column data.
   */
  inline vector<su2double> &GetColumnForProfile(int val_iProfile, unsigned short iCol) {
    this->ColumnData.resize(this->numberOfRowsInProfile[val_iProfile]);
  for (unsigned long iRow = 0; iRow < this->numberOfRowsInProfile[val_iProfile]; iRow++)
    this->ColumnData[iRow]=this->profileData[val_iProfile][iRow*this->numberOfColumnsInProfile[val_iProfile]+iCol];
  return this->ColumnData;
  }

};
