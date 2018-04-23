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


//== INCLUDES =================================================================

#include "IO.h"
#include <stdio.h>


//== NAMESPACES ===============================================================


namespace graphene {
	namespace surface_mesh {

		bool read_clr(Surface_mesh& mesh, const std::string& filename)
		{

  			mesh.vsa_info.clear();
			//if (mesh.vertices_size() == 0) return false;
			//== IMPLEMENTATION ===========================================================
			FILE* in = fopen(filename.c_str(), "r");
			if (!in) return false;
			int id1,id2, patchnum,faces,face_id;
			char line[200];
			fscanf(in, "%d %d %d %d\n", &id1, &id1, &patchnum, &id1);
			fgets(line, 200, in);

			Surface_mesh::FaceInfo tPatch;
			auto points = mesh.get_vertex_property<Point>("v:point");
			auto VSA_segmetation = mesh.face_property<int>("f:segmentation");
			for (auto i = 0; i < patchnum; i++)
			{
				fscanf(in, "%d %d %d", &id1, &id2, &faces);
				for (int j = 0; j < faces; ++j)
				{
					fscanf(in, "%d", &face_id);
					VSA_segmetation[Surface_mesh::Face(face_id)] = i;
				}	
				double x, y, z;
				fscanf(in, "%lf %lf %lf", &x, &y, &z);
				tPatch.r = x / 255;
				tPatch.g = y / 255;
				tPatch.b = z / 255;
				mesh.vsa_info.push_back(tPatch);
			}
			//mesh.remove_face_property(VSA_segmetation);
			return true;
		}
		//=============================================================================
	} // namespace surface_mesh
} // namespace graphene
  //=============================================================================
