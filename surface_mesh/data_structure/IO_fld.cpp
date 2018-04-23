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
#include <cmath>

//== NAMESPACES ===============================================================


namespace graphene {
	namespace surface_mesh {


		//== IMPLEMENTATION ===========================================================
		bool read_fld(Surface_mesh& mesh, const std::string& filename)
		{
			// open file (in ASCII mode)
			FILE* in = fopen(filename.c_str(), "r");
			if (!in) return false;

			char                 line[200], *lp;
			int                  nc, ID, faceid, crest_v, nE, nfE, ridges_n, ravines_n;
			Scalar               k1, k2, ridge_len, ravine_len;
			unsigned int         i, j, items,idx0, idx1, num1,num2;
			Vec3f                p, t1,t2;
			Normal               n;
			Vec2f                t;
			Surface_mesh::FeatureVertex v;
		
			// properties
			Surface_mesh::Vertex_property<Direction>              maxdir;
			Surface_mesh::Vertex_property<Direction>              mindir;
			Surface_mesh::Vertex_property<Scalar>                 k_max;
			Surface_mesh::Vertex_property<Scalar>                 k_min;
			Surface_mesh::FeatureEdge_property<int>          face_id;
			Surface_mesh::FeatureEdge_property<int>            line_id;
			Surface_mesh::FeatureEdge_property<bool>              is_ridge;
			Surface_mesh::FeatureVertex_property<bool>            visited;

			maxdir = mesh.vertex_property<Direction>("v:max direction");
		    mindir = mesh.vertex_property<Direction>("v:min direction");
			k_max = mesh.vertex_property<Scalar>("v:max curvature");
			k_min = mesh.vertex_property<Scalar>("v:min curvature");
			face_id = mesh.feature_e_property<int>("e:feature id");
			line_id = mesh.feature_e_property<int>("e:feature line id");
			is_ridge = mesh.add_feature_e_property<bool>("e:is ridge", false);
			visited = mesh.add_feature_v_property<bool>("v:is visited", false);
            
			// read vertices: pos [normal] [color] [texcoord]
			for (auto v : mesh.vertices())
			{
				// read line
				lp = fgets(line, 200, in);
				lp = line;
				// position
 				items = sscanf(lp,"%f %f %f %f %f %f %f %f%n", &k1,&k2,&t1[0],&t1[1],&t1[2],&t2[0],&t2[1],&t2[2],&nc);
				assert(items == 8);
				k_max[v] = k1;
				k_min[v] = k2;
				maxdir[v] = t1;
				mindir[v] = t2;
				lp += nc;
			}

			items = fscanf(in, "%d %d\n", &crest_v,&nfE);
 			mesh.reserve(crest_v, nfE);
			//read feature(ridges) point vector
			for (i = 0; i < crest_v && !feof(in); ++i)
			{
				// read line
				lp = fgets(line, 200, in);
				lp = line;

				// position
				items = sscanf(lp, "%f %f %f%n", &p[0], &p[1], &p[2], &nc);
				assert(items == 3);
				v = mesh.add_feature_vertex((Point)p);
				lp += nc;
			}
			//Feature #Vertices #Edges
			std::vector<Surface_mesh::FeatureVertex> vertices;
			std::vector<int>  face_id_cache;
			std::vector<float> Cestlength;

			items = fscanf(in, "%d\n", &ridges_n);
			for (i = 0; i < ridges_n; ++i)
			{
				items = fscanf(in, "%d %d %f\n", &ID, &num1,&ridge_len);
				// read edges: #N v[1] v[2] ... v[n-1]
				vertices.reserve(num1 + 1);
				face_id_cache.reserve(num1);
				items = fscanf(in, "%d\n", &idx0);
				vertices.push_back(Surface_mesh::FeatureVertex(idx0));
				Cestlength.push_back(ridge_len);
				for (j = 0; j < num1; ++j)
				{
					// read line
					lp = fgets(line, 200, in);
					lp = line;
					// #vertices
					items = sscanf(lp, "%d %d%n", &idx1, &faceid, &nc);
					assert(items == 2);
					visited[Surface_mesh::FeatureVertex(idx1)] = true;
					vertices.push_back(Surface_mesh::FeatureVertex(idx1));
					face_id_cache.push_back(faceid);
					lp += nc;
					
				}
				mesh.add_feature_line(vertices, face_id_cache,ridge_len);
				vertices.clear();
				face_id_cache.clear();
			}

			items = fscanf(in, "%d\n", &ravines_n);
			for (i = 0; i < ravines_n; ++i)
			{
				items = fscanf(in, "%d %d %f\n", &ID, &num2,&ravine_len);
				// read edges: #N v[1] v[2] ... v[n-1]
				vertices.reserve(num2 + 1);
				face_id_cache.reserve(num2);
				items = fscanf(in, "%d\n", &idx0);
				vertices.push_back(Surface_mesh::FeatureVertex(idx0));
				Cestlength.push_back(ravine_len);
				for (j = 0; j < num2; ++j)
				{
					// read line
					lp = fgets(line, 200, in);
					lp = line;
					// #vertices
					items = sscanf(lp, "%d %d%n", &idx1, &faceid, &nc);
					assert(items == 2);
					vertices.push_back(Surface_mesh::FeatureVertex(idx1)) ;
					face_id_cache.push_back(faceid);
					lp += nc;
				}
				mesh.add_feature_line(vertices,face_id_cache,ravine_len,false);
				vertices.clear();
				face_id_cache.clear();
			}
			fclose(in);
			std::sort(Cestlength.begin(), Cestlength.end());
			int  k = Cestlength.size();
			mesh.average = Cestlength.at(Cestlength.size()/20);
			mesh.remove_feature_e_property(is_ridge);
			mesh.remove_feature_v_property(visited);
			return true;
		}
		//=============================================================================
	} // namespace surface_mesh
} // namespace graphene
  //=============================================================================
