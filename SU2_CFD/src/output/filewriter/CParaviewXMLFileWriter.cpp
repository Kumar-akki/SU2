/*!
 * \file CParaviewXMLFileWriter.cpp
 * \brief Filewriter class for Paraview binary format.
 * \author T. Albring
 * \version 7.0.0 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2019, SU2 Contributors (cf. AUTHORS.md)
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

#include "../../../include/output/filewriter/CParaviewXMLFileWriter.hpp"
#include "../../../../Common/include/toolboxes/printing_toolbox.hpp"

const string CParaviewXMLFileWriter::fileExt = ".vtu";

CParaviewXMLFileWriter::CParaviewXMLFileWriter(string valFileName, CParallelDataSorter *valDataSorter) :
  CFileWriter(std::move(valFileName), valDataSorter, fileExt){

  /* Check for big endian. We have to swap bytes otherwise.
   * Since size of character is 1 byte when the character pointer
   *  is de-referenced it will contain only first byte of integer. ---*/

  bigEndian = false;
  unsigned int i = 1;
  char *c = (char*)&i;
  if (*c) bigEndian = false;
  else bigEndian = true;

}


CParaviewXMLFileWriter::~CParaviewXMLFileWriter(){

}

void CParaviewXMLFileWriter::Write_Data(){

  if (!dataSorter->GetConnectivitySorted()){
    SU2_MPI::Error("Connectivity must be sorted.", CURRENT_FUNCTION);
  }

  /*--- We always have 3 coords, independent of the actual value of nDim ---*/

  const int NCOORDS = 3;
  const unsigned short nDim = dataSorter->GetnDim();
  unsigned short iDim = 0;

  /*--- Array containing the field names we want to output ---*/

  const vector<string>& fieldNames = dataSorter->GetFieldNames();

  unsigned long iPoint, iElem;

  char str_buf[255];

  fileSize = 0.0;
  dataOffset = 0;

  /*--- Set a timer for the file writing. ---*/

#ifndef HAVE_MPI
  startTime = su2double(clock())/su2double(CLOCKS_PER_SEC);
#else
  startTime = MPI_Wtime();
#endif


  /*--- Parallel binary output using MPI I/O. ---*/

  int ierr;

#ifdef HAVE_MPI
  /*--- All ranks open the file using MPI. Here, we try to open the file with
   exclusive so that an error is generated if the file exists. We always want
   to write a fresh output file, so we delete any existing files and create
   a new one. ---*/

  ierr = MPI_File_open(MPI_COMM_WORLD, fileName.c_str(),
                       MPI_MODE_CREATE|MPI_MODE_EXCL|MPI_MODE_WRONLY,
                       MPI_INFO_NULL, &fhw);
  if (ierr != MPI_SUCCESS)  {
    MPI_File_close(&fhw);
    if (rank == 0)
      MPI_File_delete(fileName.c_str(), MPI_INFO_NULL);
    ierr = MPI_File_open(MPI_COMM_WORLD, fileName.c_str(),
                         MPI_MODE_CREATE|MPI_MODE_EXCL|MPI_MODE_WRONLY,
                         MPI_INFO_NULL, &fhw);
  }

  /*--- Error check opening the file. ---*/

  if (ierr) {
    SU2_MPI::Error(string("Unable to open VTK binary legacy file ") +
                   fileName, CURRENT_FUNCTION);
  }
#else
  fhw = fopen(fileName.c_str(), "wb");
  /*--- Error check for opening the file. ---*/

  if (!fhw) {
    SU2_MPI::Error(string("Unable to open VTK binary legacy file ") +
                   fileName, CURRENT_FUNCTION);
  }
#endif

  /*--- Write the initial strings to the file. Only the master will
   write the header lines, but all ranks will store the offsets. ---*/

  disp = 0;

  /*--- Communicate the number of total points that will be
   written by each rank. After this communication, each proc knows how
   many poinnts will be written before its location in the file and the
   offsets can be correctly set. ---*/

  unsigned long myPoint, GlobalPoint;

  GlobalPoint = dataSorter->GetnPointsGlobal();
  myPoint     = dataSorter->GetnPoints();

  /*--- Compute our local number of elements, the required storage,
   and reduce the total number of elements and storage globally. ---*/

  unsigned long nTot_Line;
  unsigned long nTot_Tria, nTot_Quad;
  unsigned long nTot_Tetr, nTot_Hexa, nTot_Pris, nTot_Pyra;
  unsigned long myElem, myElemStorage, GlobalElem, GlobalElemStorage;

  unsigned long nParallel_Line = dataSorter->GetnElem(LINE),
                nParallel_Tria = dataSorter->GetnElem(TRIANGLE),
                nParallel_Quad = dataSorter->GetnElem(QUADRILATERAL),
                nParallel_Tetr = dataSorter->GetnElem(TETRAHEDRON),
                nParallel_Hexa = dataSorter->GetnElem(HEXAHEDRON),
                nParallel_Pris = dataSorter->GetnElem(PRISM),
                nParallel_Pyra = dataSorter->GetnElem(PYRAMID);

  SU2_MPI::Allreduce(&nParallel_Line, &nTot_Line, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  SU2_MPI::Allreduce(&nParallel_Tria, &nTot_Tria, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  SU2_MPI::Allreduce(&nParallel_Quad, &nTot_Quad, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  SU2_MPI::Allreduce(&nParallel_Tetr, &nTot_Tetr, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  SU2_MPI::Allreduce(&nParallel_Hexa, &nTot_Hexa, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  SU2_MPI::Allreduce(&nParallel_Pris, &nTot_Pris, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
  SU2_MPI::Allreduce(&nParallel_Pyra, &nTot_Pyra, 1,
                     MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);

  myElem        = (nParallel_Line + nParallel_Tria + nParallel_Quad + nParallel_Tetr +
                   nParallel_Hexa + nParallel_Pris + nParallel_Pyra);
  myElemStorage = (nParallel_Line*2 + nParallel_Tria*3 + nParallel_Quad*4 + nParallel_Tetr*4 +
                   nParallel_Hexa*8 + nParallel_Pris*6 + nParallel_Pyra*5);

  GlobalElem        = (nTot_Line + nTot_Tria   + nTot_Quad   + nTot_Tetr   +
                       nTot_Hexa   + nTot_Pris   + nTot_Pyra);
  GlobalElemStorage = (nTot_Line*2 + nTot_Tria*3 + nTot_Quad*4 + nTot_Tetr*4 +
                       nTot_Hexa*8 + nTot_Pris*6 + nTot_Pyra*5);

  /* Write the ASCII XML header. Note that we use the appended format for the data,
  * which means that all data is appended at the end of the file in one binary blob.
  */

  if (!bigEndian){
    WriteString("<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\">\n", MASTER_NODE);
  } else {
    WriteString("<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"BigEndian\">\n", MASTER_NODE);
  }

  WriteString("<UnstructuredGrid>\n", MASTER_NODE);

  SPRINTF(str_buf, "<Piece NumberOfPoints=\"%i\" NumberOfCells=\"%i\">\n",
          SU2_TYPE::Int(GlobalPoint), SU2_TYPE::Int(GlobalElem));

  WriteString(std::string(str_buf), MASTER_NODE);
  WriteString("<Points>\n", MASTER_NODE);
  AddDataArray(VTKDatatype::FLOAT32, "", NCOORDS, myPoint*NCOORDS, GlobalPoint*NCOORDS);
  WriteString("</Points>\n", MASTER_NODE);
  WriteString("<Cells>\n", MASTER_NODE);
  AddDataArray(VTKDatatype::INT32, "connectivity", 1, myElemStorage, GlobalElemStorage);
  AddDataArray(VTKDatatype::INT32, "offsets", 1, myElem, GlobalElem);
  AddDataArray(VTKDatatype::UINT8, "types", 1, myElem, GlobalElem);
  WriteString("</Cells>\n", MASTER_NODE);

  WriteString("<PointData>\n", MASTER_NODE);

  /*--- Adjust container start location to avoid point coords. ---*/

  unsigned short varStart = 2;
  if (nDim == 3) varStart++;

  /*--- Loop over all variables that have been registered in the output. ---*/

  unsigned short iField, VarCounter = varStart;
  for (iField = varStart; iField < fieldNames.size(); iField++) {

    string fieldname = fieldNames[iField];
    fieldname.erase(remove(fieldname.begin(), fieldname.end(), '"'),
                    fieldname.end());

    /*--- Check whether this field is a vector or scalar. ---*/

    bool output_variable = true, isVector = false;
    size_t found = fieldNames[iField].find("_x");
    if (found!=string::npos) {
      output_variable = true;
      isVector        = true;
    }
    found = fieldNames[iField].find("_y");
    if (found!=string::npos) {
      /*--- We have found a vector, so skip the Y component. ---*/
      output_variable = false;
      VarCounter++;
    }
    found = fieldNames[iField].find("_z");
    if (found!=string::npos) {
      /*--- We have found a vector, so skip the Z component. ---*/
      output_variable = false;
      VarCounter++;
    }

    /*--- Write the point data as an <X,Y,Z> vector or a scalar. ---*/

    if (output_variable && isVector) {

      /*--- Adjust the string name to remove the leading "X-" ---*/

      fieldname.erase(fieldname.end()-2,fieldname.end());

      AddDataArray(VTKDatatype::FLOAT32, fieldname, NCOORDS, myPoint*NCOORDS, GlobalPoint*NCOORDS);

    } else if (output_variable) {

      AddDataArray(VTKDatatype::FLOAT32, fieldname, 1, myPoint, GlobalPoint);

    }

  }
  WriteString("</PointData>\n", MASTER_NODE);
  WriteString("</Piece>\n", MASTER_NODE);
  WriteString("</UnstructuredGrid>\n", MASTER_NODE);

  int *nPoint_Snd = new int[size+1];
  int *nPoint_Cum = new int[size+1];

  nPoint_Snd[0] = 0; nPoint_Cum[0] = 0;
  for (int ii=1; ii < size; ii++) {
    nPoint_Snd[ii] = int(myPoint); nPoint_Cum[ii] = 0;
  }
  nPoint_Snd[size] = int(myPoint); nPoint_Cum[size] = 0;

  /*--- Communicate the local counts to all ranks for building offsets. ---*/

  SU2_MPI::Alltoall(&(nPoint_Snd[1]), 1, MPI_INT,
                    &(nPoint_Cum[1]), 1, MPI_INT, MPI_COMM_WORLD);

  /*--- Put the counters into cumulative storage format. ---*/

  for (int ii = 0; ii < size; ii++) {
    nPoint_Cum[ii+1] += nPoint_Cum[ii];
  }

  /*--- Now write all the data we have previously defined into the binary section of the file ---*/

  WriteString("<AppendedData encoding=\"raw\">\n_", MASTER_NODE);

  /*--- Load/write the 1D buffer of point coordinates. Note that we
   always have 3 coordinate dimensions, even for 2D problems. ---*/

  vector<float> dataBufferFloat(myPoint*NCOORDS);
  for (iPoint = 0; iPoint < myPoint; iPoint++) {
    for (iDim = 0; iDim < NCOORDS; iDim++) {
      if (nDim == 2 && iDim == 2) {
        dataBufferFloat[iPoint*NCOORDS + iDim] = 0.0;
      } else {
        float val = (float)dataSorter->GetData(iDim, iPoint);
        dataBufferFloat[iPoint*NCOORDS + iDim] = val;
      }
    }
  }

  WriteDataArray(dataBufferFloat.data(), VTKDatatype::FLOAT32, NCOORDS*myPoint, GlobalPoint*NCOORDS, nPoint_Cum[rank]*NCOORDS);

  /*--- Communicate the number of total cells/storage that will be
   written by each rank. After this communication, each proc knows how
   many cells will be written before its location in the file and the
   offsets can be correctly set. ---*/

  int *nElem_Snd = new int[size+1]; int *nElemStorage_Snd = new int[size+1];
  int *nElem_Cum = new int[size+1]; int *nElemStorage_Cum = new int[size+1];

  nElem_Snd[0] = 0; nElemStorage_Snd[0] = 0;
  nElem_Cum[0] = 0; nElemStorage_Cum[0] = 0;
  for (int ii=1; ii < size; ii++) {
    nElem_Snd[ii] = int(myElem); nElemStorage_Snd[ii] = int(myElemStorage);
    nElem_Cum[ii] = 0;      nElemStorage_Cum[ii] = 0;
  }
  nElem_Snd[size] = int(myElem); nElemStorage_Snd[size] = int(myElemStorage);
  nElem_Cum[size] = 0;      nElemStorage_Cum[size] = 0;

  /*--- Communicate the local counts to all ranks for building offsets. ---*/

  SU2_MPI::Alltoall(&(nElem_Snd[1]), 1, MPI_INT,
                    &(nElem_Cum[1]), 1, MPI_INT, MPI_COMM_WORLD);

  SU2_MPI::Alltoall(&(nElemStorage_Snd[1]), 1, MPI_INT,
                    &(nElemStorage_Cum[1]), 1, MPI_INT, MPI_COMM_WORLD);

  /*--- Put the counters into cumulative storage format. ---*/

  for (int ii = 0; ii < size; ii++) {
    nElem_Cum[ii+1]        += nElem_Cum[ii];
    nElemStorage_Cum[ii+1] += nElemStorage_Cum[ii];
  }

  /*--- Load/write 1D buffers for the connectivity of each element type. ---*/

  vector<int> connBuf(myElemStorage);
  vector<int> offsetBuf(myElem);
  unsigned long iStorage = 0, iElemID = 0;
  unsigned short iNode = 0;

  auto copyToBuffer = [&](GEO_TYPE type, unsigned long nElem, unsigned short nPoints){
    for (iElem = 0; iElem < nElem; iElem++) {
      for (iNode = 0; iNode < nPoints; iNode++){
        connBuf[iStorage+iNode] = int(dataSorter->GetElem_Connectivity(type, iElem, iNode)-1);
      }
      iStorage += nPoints;
      offsetBuf[iElemID++] = int(iStorage + nElemStorage_Cum[rank]);
    }
  };

  copyToBuffer(LINE,          nParallel_Line, N_POINTS_LINE);
  copyToBuffer(TRIANGLE,      nParallel_Tria, N_POINTS_TRIANGLE);
  copyToBuffer(QUADRILATERAL, nParallel_Quad, N_POINTS_QUADRILATERAL);
  copyToBuffer(TETRAHEDRON,   nParallel_Tetr, N_POINTS_TETRAHEDRON);
  copyToBuffer(HEXAHEDRON,    nParallel_Hexa, N_POINTS_HEXAHEDRON);
  copyToBuffer(PRISM,         nParallel_Pris, N_POINTS_PRISM);
  copyToBuffer(PYRAMID,       nParallel_Pyra, N_POINTS_PYRAMID);

  WriteDataArray(connBuf.data(), VTKDatatype::INT32, myElemStorage, GlobalElemStorage, nElemStorage_Cum[rank]);
  WriteDataArray(offsetBuf.data(), VTKDatatype::INT32, myElem, GlobalElem, nElem_Cum[rank]);

  /*--- Load/write the cell type for all elements in the file. ---*/

  vector<uint8_t> typeBuf(myElem);
  vector<uint8_t>::iterator typeIter = typeBuf.begin();

  std::fill(typeIter, typeIter+nParallel_Line, LINE);          typeIter += nParallel_Line;
  std::fill(typeIter, typeIter+nParallel_Tria, TRIANGLE);      typeIter += nParallel_Tria;
  std::fill(typeIter, typeIter+nParallel_Quad, QUADRILATERAL); typeIter += nParallel_Quad;
  std::fill(typeIter, typeIter+nParallel_Tetr, TETRAHEDRON);   typeIter += nParallel_Tetr;
  std::fill(typeIter, typeIter+nParallel_Hexa, HEXAHEDRON);    typeIter += nParallel_Hexa;
  std::fill(typeIter, typeIter+nParallel_Pris, PRISM);         typeIter += nParallel_Pris;
  std::fill(typeIter, typeIter+nParallel_Pyra, PYRAMID);       typeIter += nParallel_Pyra;

  WriteDataArray(typeBuf.data(), VTKDatatype::UINT8, myElem, GlobalElem, nElem_Cum[rank]);

  /*--- Loop over all variables that have been registered in the output. ---*/

  VarCounter = varStart;
  for (iField = varStart; iField < fieldNames.size(); iField++) {

    /*--- Check whether this field is a vector or scalar. ---*/

    bool output_variable = true, isVector = false;
    size_t found = fieldNames[iField].find("_x");
    if (found!=string::npos) {
      output_variable = true;
      isVector        = true;
    }
    found = fieldNames[iField].find("_y");
    if (found!=string::npos) {
      /*--- We have found a vector, so skip the Y component. ---*/
      output_variable = false;
      VarCounter++;
    }
    found = fieldNames[iField].find("_z");
    if (found!=string::npos) {
      /*--- We have found a vector, so skip the Z component. ---*/
      output_variable = false;
      VarCounter++;
    }

    /*--- Write the point data as an <X,Y,Z> vector or a scalar. ---*/

    if (output_variable && isVector) {

      /*--- Load up the buffer for writing this rank's vector data. ---*/

      float val = 0.0;
      for (iPoint = 0; iPoint < myPoint; iPoint++) {
        for (iDim = 0; iDim < NCOORDS; iDim++) {
          if (nDim == 2 && iDim == 2) {
            dataBufferFloat[iPoint*NCOORDS + iDim] = 0.0;
          } else {
            val = (float)dataSorter->GetData(VarCounter+iDim,iPoint);
            dataBufferFloat[iPoint*NCOORDS + iDim] = val;
          }
        }
      }

      WriteDataArray(dataBufferFloat.data(), VTKDatatype::FLOAT32, myPoint*NCOORDS, GlobalPoint*NCOORDS, nPoint_Cum[rank]*NCOORDS);

      VarCounter++;

    } else if (output_variable) {


      /*--- For now, create a temp 1D buffer to load up the data for writing.
       This will be replaced with a derived data type most likely. ---*/

      for (iPoint = 0; iPoint < myPoint; iPoint++) {
        float val = (float)dataSorter->GetData(VarCounter,iPoint);
        dataBufferFloat[iPoint] = val;
      }

      WriteDataArray(dataBufferFloat.data(), VTKDatatype::FLOAT32, myPoint, GlobalPoint, nPoint_Cum[rank]);

      VarCounter++;
    }

  }

  WriteString("</AppendedData>\n", MASTER_NODE);
  WriteString("</VTKFile>\n", MASTER_NODE);

#ifdef HAVE_MPI
  /*--- All ranks close the file after writing. ---*/

  MPI_File_close(&fhw);
#else
  fclose(fhw);
#endif

  /*--- Compute and store the write time. ---*/

#ifndef HAVE_MPI
  stopTime = su2double(clock())/su2double(CLOCKS_PER_SEC);
#else
  stopTime = MPI_Wtime();
#endif
  usedTime = stopTime-startTime;

  /*--- Communicate the total file size for the restart ---*/

#ifdef HAVE_MPI
  su2double my_fileSize = fileSize;
  SU2_MPI::Allreduce(&my_fileSize, &fileSize, 1,
                     MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#endif

  /*--- Compute and store the bandwidth ---*/

  bandwidth = fileSize/(1.0e6)/usedTime;

  /*--- Delete the offset counters that we needed for MPI IO. ---*/

  delete [] nElem_Snd;        delete [] nElem_Cum;
  delete [] nElemStorage_Snd; delete [] nElemStorage_Cum;
  delete [] nPoint_Snd;       delete [] nPoint_Cum;

}


void CParaviewXMLFileWriter::WriteString(std::string str, int rankOut){

#ifdef HAVE_MPI
  /*--- Reset the file view before writing the next ASCII line for cells. ---*/

  MPI_File_set_view(fhw, 0, MPI_BYTE, MPI_BYTE,
                    "native", MPI_INFO_NULL);

  if (SU2_MPI::GetRank() == rankOut)
    MPI_File_write_at(fhw, disp, str.c_str(), int(strlen(str.c_str())),
                      MPI_CHAR, MPI_STATUS_IGNORE);
  disp += strlen( str.c_str())*sizeof(char);
  fileSize += sizeof(char)*strlen( str.c_str());
#else
  char str_buf[255];
  strcpy(str_buf, str.c_str());
  fwrite(str_buf, sizeof(char), strlen(str_buf), fhw);
  fileSize += sizeof(char)*strlen(str_buf);
#endif

}

void CParaviewXMLFileWriter::WriteDataArray(void* data, VTKDatatype type, unsigned long arraySize,
                                            unsigned long globalSize, unsigned long offset){


  unsigned long totalByteSize, byteSize;

  std::string typeStr;
  unsigned long typeSize;

  GetTypeInfo(type, typeStr, typeSize);

  /*--- Compute the size of the data to write in bytes ---*/

  byteSize = arraySize*typeSize;

  /*--- The total data size ---*/

  totalByteSize = globalSize*typeSize;

#ifdef HAVE_MPI

  MPI_Datatype filetype;
  MPI_Status status;

  /*--- Write the total size in bytes at the beginning of the binary data blob ---*/

  MPI_File_set_view(fhw, 0, MPI_BYTE, MPI_BYTE,
                    (char*)"native", MPI_INFO_NULL);

  if (SU2_MPI::GetRank() == MASTER_NODE)
    MPI_File_write_at(fhw, disp, &totalByteSize, sizeof(int),
                      MPI_BYTE, MPI_STATUS_IGNORE);

  disp += sizeof(int);

  /*--- Prepare to write the actual data ---*/

  MPI_Type_contiguous(int(byteSize), MPI_BYTE, &filetype);
  MPI_Type_commit(&filetype);

  /*--- Set the view for the MPI file write, i.e., describe the
 location in the file that this rank "sees" for writing its
 piece of the file. ---*/

  MPI_File_set_view(fhw, disp + offset*typeSize, MPI_BYTE, filetype,
                    (char*)"native", MPI_INFO_NULL);

  /*--- Collective call for all ranks to write simultaneously. ---*/

  MPI_File_write_all(fhw, data, int(byteSize), MPI_BYTE, &status);

  MPI_Type_free(&filetype);

  disp      += totalByteSize;
  fileSize  += byteSize;
#else
  /*--- Write the total size in bytes at the beginning of the binary data blob ---*/

  fwrite(&totalByteSize, sizeof(int),1, fhw);

  /*--- Write binary data ---*/

  fwrite(data, sizeof(char), byteSize, fhw);
  fileSize += byteSize;
#endif
}

void CParaviewXMLFileWriter::AddDataArray(VTKDatatype type, string name,
                                          unsigned short nComponents, unsigned long arraySize, unsigned long globalSize){

  /*--- Add quotation marks around the arguments ---*/

  name = "\"" + name + "\"";

  string nComp ="\"" + PrintingToolbox::to_string(nComponents) + "\"";

  /*--- Full precision ASCII output of offset for 32 bit integer ---*/

  stringstream ss;
  ss.precision(10); ss.setf(std::ios::fixed, std::ios::floatfield);
  ss <<  "\"" << dataOffset <<  "\"";
  string offsetStr = ss.str();

  std::string typeStr;
  unsigned long typeSize, byteSize, totalByteSize;

  GetTypeInfo(type, typeStr, typeSize);

  /*--- Compute the size of the data to write in bytes ---*/

  byteSize = arraySize*typeSize;

  /*--- Total data size ---*/

  totalByteSize = globalSize*typeSize;

  /*--- Write the ASCII XML header information for this array ---*/

  WriteString(string("<DataArray type=") + typeStr +
              string(" Name=") + name +
              string(" NumberOfComponents= ") + nComp +
              string(" offset=") + offsetStr +
              string(" format=\"appended\"/>\n"), MASTER_NODE);

  dataOffset += totalByteSize + sizeof(int);

}
