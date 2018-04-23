//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011 by Graphics & Geometry Group, Bielefeld University
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================


#ifndef GRAPHENE_SURFACE_MESH_IO_H
#define GRAPHENE_SURFACE_MESH_IO_H


//== INCLUDES =================================================================

#include <string>
#include "Surface_mesh.h"


//== NAMESPACE ================================================================


namespace graphene {
namespace surface_mesh {


//=============================================================================


bool read_mesh(Surface_mesh& mesh, const std::string& filename,const std::string& ext);
bool read_feature(Surface_mesh& mesh, const std::string& filename);
bool read_segmentation(Surface_mesh& mesh, const std::string& filename);
bool read_off(Surface_mesh& mesh, const std::string& filename);
bool read_obj(Surface_mesh& mesh, const std::string& filename);
bool read_stl(Surface_mesh& mesh, const std::string& filename);
bool read_fld(Surface_mesh& mesh, const std::string& filename);
bool read_clr(Surface_mesh& mesh, const std::string& filename);

bool write_mesh(const Surface_mesh& mesh, const std::string& filename);
bool write_off(const Surface_mesh& mesh,
               const std::string& filename,
               const bool write_normals = false,
               const bool write_texcoords = false);
bool write_obj(const Surface_mesh& mesh, const std::string& filename);


//=============================================================================
} // namespace surface_mesh
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_SURFACE_MESH_IO_H
//=============================================================================
