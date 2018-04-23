//=============================================================================
// Copyright (C) Graphics & Geometry Processing Group, Bielefeld University
//=============================================================================


#include <graphene/macros.h>
#include <graphene/gl/gl_includes.h>
#include <graphene/gl/surface_mesh_gl.h>
#include <graphene/surface_mesh/scene_graph/Surface_mesh_node.h>
#include <graphene/surface_mesh/scene_graph/mean_curvature_texture.h>
#include <graphene/surface_mesh/data_structure/IO.h>
#include <graphene/utility/Stop_watch.h>

#include <climits>
#include <sstream>


//=============================================================================


namespace graphene {
	namespace scene_graph {


		//=============================================================================


		Surface_mesh_node::
			Surface_mesh_node(Base_node* _parent, const std::string& _name)
			: Object_node(_parent, _name)
		{
			// my geometry object is the Surface_mesh
			this->object_ = &mesh_;


			// init draw modes
			clear_draw_modes();
			add_draw_mode("Wireframe");
			add_draw_mode("Solid Edge");
			add_draw_mode("Solid");
			add_draw_mode("Points");
			add_draw_mode("Vertex Curvature");
			add_draw_mode("VSA Segmentation");
			set_draw_mode(2);


			// initialize GL buffers to zero
			vertex_array_object_ = 0;
			//f_vertex_array_object_ = 0;
			vertex_buffer_ = 0;
			normal_buffer_ = 0;
			color_buffer_ = 0;
			tex_coord_buffer_ = 0;
			edge_index_buffer_ = 0;
			feature_index_buffer_ = 0;
			selection_index_buffer_ = 0;
			vsa_index_buffer_ = 0;
			lineVert_buffer_ = 0;
			line_index_buffer_ = 0;
			line_ravidx_buffer_ = 0;


			// initialize buffer sizes
			n_vertices_ = 0;
			n_edges_ = 0;
			n_triangles_ = 0;
			n_selected_ = 0;
			
			n_feature_ = 0;
			n_rav_lines_ = 0;
			n_ravVerts_ = 0;
			n_vsa_edges_ = 0;

			// initialize texture
			glGenTextures(1, &texture_);
			glBindTexture(GL_TEXTURE_1D, texture_);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, mean_curvature_texture);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


			// material parameters
			front_color_ = Vec3f(0.4, 0.425, 0.475);
			back_color_ = Vec3f(0.5, 0.3, 0.3);
			wire_color_ = Vec3f(0, 0, 0);
			ridge_color_ = Vec3f(0, 0, 1);
			ravine_color_ = Vec3f(1, 0, 0);
			material_ = Vec4f(0.1, 1.0, 1.0, 100.0);
			crease_angle_ = 0.0;
			is_visual = false;

			// selection modes
			clear_selection_modes();
			add_selection_mode("Lasso");
			add_selection_mode("Constraints");
			set_selection_mode(0);
		}


		//-----------------------------------------------------------------------------


		Surface_mesh_node::
			~Surface_mesh_node()
		{
			// delete texture
			glDeleteTextures(1, &texture_);

			// delete all OpenGL buffers
			delete_buffers();
		}


		//-----------------------------------------------------------------------------


		bool
			Surface_mesh_node::
			load(const std::string& filename)
		{
			// extract file extension
			std::string::size_type dot(filename.rfind("."));
			if (dot == std::string::npos) return false;
			std::string ext = filename.substr(dot + 1, filename.length() - dot - 1);
			std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
			bool read = false;
			fileinfo_ = filename;
			if (ext == "fld")
			{
				read = mesh_.read(filename, ext);
				LOG(Log_info) << mesh_.n_fvertices() << " Feature Vertices, "
					<< mesh_.n_lines() << " Feature Lines." << std::endl;
				update_mesh(true);
			}
			else if (ext == "clr")
			{
				read = mesh_.read(filename, ext);
				LOG(Log_info) << mesh_.vsa_info.size() << " segmentation patch" << std::endl;
				update_mesh();
			}
			else
			{
			mesh_.clear();
			read = mesh_.read(filename,ext);

			LOG(Log_info) << mesh_.n_vertices() << " Vertices, "
				<< mesh_.n_faces() << " Faces." << std::endl;

			update_mesh();
			}
			return read;
		}


		//-----------------------------------------------------------------------------


		bool
			Surface_mesh_node::
			save(const std::string& filename) const
		{
			return mesh_.write(filename);
		}


		//-----------------------------------------------------------------------------


		std::string
			Surface_mesh_node::
			info() const
		{
			std::ostringstream s;
			s << mesh_.n_vertices() << " vertices, "
				<< mesh_.n_edges() << " edges, "
				<< mesh_.n_faces() << " faces";
			return s.str();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			draw(gl::GL_state *gl)
		{
			if (!visible_)
				return;

			// upload mesh data, if not done already
			if (!vertex_array_object_)
			{
				initialize_buffers();
			}


			// compute matrices
			gl->update_matrices();


			// setup Phong shader
			gl::Shader* phong_shader = gl->set_active_shader(gl::PHONG_SHADER);
			phong_shader->use();
			phong_shader->set_uniform("use_lighting", false);
			phong_shader->set_uniform("use_vertexcolor", false);
			phong_shader->set_uniform("use_texture", false);
			phong_shader->set_uniform("material", material_);
			phong_shader->set_uniform("front_color", front_color_);
			phong_shader->set_uniform("back_color", back_color_);
			phong_shader->set_uniform("modelview_projection_matrix", gl->modelviewproj_);
			phong_shader->set_uniform("normal_matrix", gl->normal_);


			// get current draw mode
			std::string draw_mode = get_draw_mode();


			// VAO init
			glBindVertexArray(vertex_array_object_);
			//glBindVertexArray(f_vertex_array_object_);


			if (draw_mode == "Wireframe")
			{
				glLineWidth(10);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
				glEnableVertexAttribArray(gl::attrib_locations::NORMAL);
				glDisableVertexAttribArray(gl::attrib_locations::COLOR);
				glDisableVertexAttribArray(gl::attrib_locations::TEXCOORDS);

				phong_shader->set_uniform("use_lighting", false);
				phong_shader->set_uniform("use_vertexcolor", false);
				phong_shader->set_uniform("front_color", wire_color_);

				glDepthRange(0.001, 1.0);
				//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_index_buffer_);
				//glDrawElements(GL_LINES, n_edges_, GL_UNSIGNED_INT, NULL);

				for (auto fidx = 0; fidx < mesh_.vsa_info.size(); fidx++)
				{
					phong_shader->set_uniform("front_color", Vec3f(mesh_.vsa_info[fidx].r, mesh_.vsa_info[fidx].g, mesh_.vsa_info[fidx].b));
					glBindBuffer(GL_ARRAY_BUFFER, vsa_index_buffer_);
					glBufferData(GL_ARRAY_BUFFER, mesh_.vsa_info[fidx].idx.size() * sizeof(float), mesh_.vsa_info[fidx].idx.data(), GL_STATIC_DRAW);
					glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
					glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
					n_vsa_edges_ = mesh_.vsa_info[fidx].idx.size() / 3;
					glDrawArrays(GL_TRIANGLES, 0, n_vsa_edges_);
				}

				phong_shader->set_uniform("front_color", ridge_color_);

				glBindBuffer(GL_ARRAY_BUFFER, lineVert_buffer_);
				glBufferData(GL_ARRAY_BUFFER, mesh_.vertset.size() * 3 * sizeof(float), mesh_.vertset.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
				n_lineVerts_ = mesh_.vertset.size();

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_index_buffer_);
				glDrawElements(GL_LINES, n_lines_, GL_UNSIGNED_INT, NULL);

				phong_shader->set_uniform("front_color", ravine_color_);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ravidx_buffer_);
				glDrawElements(GL_LINES, n_rav_lines_, GL_UNSIGNED_INT, NULL);

				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(float), points.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);

				glLineWidth(1.0);
			}


			else if (draw_mode == "Solid Edge")
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				// solid
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
				glEnableVertexAttribArray(gl::attrib_locations::NORMAL);
				glDisableVertexAttribArray(gl::attrib_locations::COLOR);
				glDisableVertexAttribArray(gl::attrib_locations::TEXCOORDS);
				phong_shader->set_uniform("use_lighting", true);
				phong_shader->set_uniform("use_vertexcolor", has_colors());
				glDepthRange(0.002, 1.0);
				glDrawArrays(GL_TRIANGLES, 0, n_vertices_);


				// edges
				glDisableVertexAttribArray(gl::attrib_locations::NORMAL);
				glDisableVertexAttribArray(gl::attrib_locations::COLOR);
				glDisableVertexAttribArray(gl::attrib_locations::TEXCOORDS);
				phong_shader->set_uniform("use_lighting", false);
				phong_shader->set_uniform("front_color", wire_color_);
				phong_shader->set_uniform("use_vertexcolor", false);
				glDepthRange(0.001, 1.0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_index_buffer_);
				glDrawElements(GL_LINES, n_edges_, GL_UNSIGNED_INT, NULL);



				phong_shader->set_uniform("front_color", ridge_color_);

				glBindBuffer(GL_ARRAY_BUFFER, lineVert_buffer_);
				glBufferData(GL_ARRAY_BUFFER, mesh_.vertset.size() * 3 * sizeof(float), mesh_.vertset.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
				n_lineVerts_ = mesh_.vertset.size();

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_index_buffer_);
				glDrawElements(GL_LINES, n_lines_, GL_UNSIGNED_INT, NULL);

				phong_shader->set_uniform("front_color", ravine_color_);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ravidx_buffer_);
				glDrawElements(GL_LINES, n_rav_lines_, GL_UNSIGNED_INT, NULL);

				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(float), points.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);

				glLineWidth(1.0);
			}


			else if (draw_mode == "Solid")
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
				glEnableVertexAttribArray(gl::attrib_locations::NORMAL);
				glDisableVertexAttribArray(gl::attrib_locations::COLOR);
				glDisableVertexAttribArray(gl::attrib_locations::TEXCOORDS);

				phong_shader->set_uniform("use_lighting", true);
				phong_shader->set_uniform("use_vertexcolor", has_colors());
				glDepthRange(0.002, 1.0);

				glDrawArrays(GL_TRIANGLES, 0, n_vertices_);

				phong_shader->set_uniform("front_color", ridge_color_);

				glBindBuffer(GL_ARRAY_BUFFER, lineVert_buffer_);
				glBufferData(GL_ARRAY_BUFFER, mesh_.vertset.size() * 3 * sizeof(float), mesh_.vertset.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
				n_lineVerts_ = mesh_.vertset.size();

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_index_buffer_);
				glDrawElements(GL_LINES, n_lines_, GL_UNSIGNED_INT, NULL);

				phong_shader->set_uniform("front_color", ravine_color_);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ravidx_buffer_);
				glDrawElements(GL_LINES, n_rav_lines_, GL_UNSIGNED_INT, NULL);

				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
				glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(float), points.data(), GL_STATIC_DRAW);
				glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(gl::attrib_locations::VERTEX);

				glLineWidth(1.0);
			}


			else if (draw_mode == "Points")
			{
				phong_shader->set_uniform("use_lighting", true);
				glDepthRange(0.0, 1.0);
				glPointSize(3.0f);
				glDrawArrays(GL_POINTS, 0, n_vertices_);
			}


			else if (draw_mode == "Vertex Curvature")
			{
				glDepthRange(0.002, 1.0);
				phong_shader->set_uniform("use_texture", true);
				phong_shader->set_uniform("use_lighting", true);
				gl->bind_texture("texture1D", texture_, GL_TEXTURE_1D, 1);
				glDrawArrays(GL_TRIANGLES, 0, n_vertices_);
			}



			// draw selected vertices
			if (n_selected_)
			{
				glDepthRange(0.0, 1.0);
				glPointSize(5.0);

				phong_shader->set_uniform("use_lighting", true);
				phong_shader->set_uniform("use_vertexcolor", false);
				phong_shader->set_uniform("use_texture", false);
				phong_shader->set_uniform("front_color", Vec3f(1.0f, 0.0f, 0.0f));
				phong_shader->set_uniform("material", Vec4f(0.1, 1.0, 0.0, 0.0));

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selection_index_buffer_);
				glDrawElements(GL_POINTS, n_selected_, GL_UNSIGNED_INT, NULL);
			}


			// draw feature edges
			if (n_feature_)
			{
				phong_shader->set_uniform("use_lighting", false);
				phong_shader->set_uniform("use_vertexcolor", false);
				phong_shader->set_uniform("use_texture", false);
				phong_shader->set_uniform("front_color", Vec3f(0.961f, 0.624f, 0.0f));
				phong_shader->set_uniform("material", Vec4f(0.1, 1.0, 0.0, 0.0));

				glDepthRange(0.0, 1.0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, feature_index_buffer_);
				glDrawElements(GL_LINES, n_feature_, GL_UNSIGNED_INT, NULL);
			}


			// reset stuff
			glBindVertexArray(0);
			glPointSize(1.0);
			glDepthRange(0.0, 1.0);
			phong_shader->disable();
		}


		//-----------------------------------------------------------------------------


		geometry::Bounding_box
			Surface_mesh_node::
			bbox() const
		{
			return bbox_;
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			update_mesh(bool has_features)
		{
			utility::Stop_watch timer; timer.start();


			// generate buffers
			if (!vertex_array_object_)
			{
				initialize_buffers();
			}


			// activate VAO
			glBindVertexArray(vertex_array_object_);

			points.reserve(mesh_.n_faces() * 3);
			normals.reserve(mesh_.n_faces() * 3);

			auto vpoints = mesh_.vertex_property<Point>("v:point");
			auto vertex_indices = mesh_.vertex_property<size_t>("v:index");
			size_t i(0);
			for (auto f : mesh_.faces())
			{
				Surface_mesh::Vertex_around_face_circulator  fvit, fvend;
				Surface_mesh::Vertex v0, v1, v2;

				fvit = fvend = mesh_.vertices(f);
				v0 = *fvit; ++fvit;
				v2 = *fvit; ++fvit;
				do
				{
					v1 = v2;
					v2 = *fvit;

					points.push_back(vpoints[v0]);
					points.push_back(vpoints[v1]);
					points.push_back(vpoints[v2]);

					vertex_indices[v0] = i++;
					vertex_indices[v1] = i++;
					vertex_indices[v2] = i++; 
				} while (++fvit != fvend);
			}



			// vertices
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
			glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(float), points.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(gl::attrib_locations::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(gl::attrib_locations::VERTEX);
			n_vertices_ = points.size();
			
			

			// normals
			crease_normals(normals);

			glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(float), normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(gl::attrib_locations::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(gl::attrib_locations::NORMAL);

		

			// edge indices
			std::vector<unsigned int> edges;
			edges.reserve(mesh_.n_edges());
			for (auto e : mesh_.edges())
			{
				edges.push_back(vertex_indices[mesh_.vertex(e, 0)]);
				edges.push_back(vertex_indices[mesh_.vertex(e, 1)]);
			}
			
			if (!edges.empty())
			{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edge_index_buffer_);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(unsigned int), &edges[0], GL_STATIC_DRAW);
			n_edges_ = edges.size();
            }
			
			// upload either per-vertex or per-face colors
			update_colors();
			 
			// update texture coordinates
			update_texcoords();

				// collect feature edges for rendering (careful: disables VAO)
				update_features();

			//collect segmentation patch for rendering (careful: disables VAO)
			update_segmentation();

			// collect selected vertices (careful: disables VAO)
			update_selection();

			// recompute bounding box
			update_bbox();

			glBindVertexArray(0);

			timer.stop();
			LOG(Log_debug) << "Update mesh took " << timer << std::endl;
		}

		//-----------------------------------------------------------------------------
		void
			Surface_mesh_node::
			update_colors()
		{
			if (!has_colors())
				return;

			std::vector<Color> colors;
			colors.reserve(mesh_.n_faces() * 3);

			auto vcolors = mesh_.get_vertex_property<Color>("v:color");
			auto fcolors = mesh_.get_face_property<Color>("f:color");

			for (auto f : mesh_.faces())
			{
				Surface_mesh::Vertex_around_face_circulator fvit, fvend;
				Surface_mesh::Vertex v0, v1, v2;

				fvit = fvend = mesh_.vertices(f);
				v0 = *fvit; ++fvit;
				v2 = *fvit; ++fvit;
				do
				{
					v1 = v2;
					v2 = *fvit;

					if (vcolors)
					{
						colors.push_back(vcolors[v0]);
						colors.push_back(vcolors[v1]);
						colors.push_back(vcolors[v2]);
					}

					if (fcolors)
					{
						colors.push_back(fcolors[f]);
						colors.push_back(fcolors[f]);
						colors.push_back(fcolors[f]);
					}
				} while (++fvit != fvend);
			}

			glBindVertexArray(vertex_array_object_);
			glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
			glBufferData(GL_ARRAY_BUFFER, colors.size() * 3 * sizeof(float), colors.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(gl::attrib_locations::COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(gl::attrib_locations::COLOR);
			glBindVertexArray(0);
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			update_texcoords()
		{
			auto vtexcoords = mesh_.get_vertex_property<float>("v:tex_1D");

			if (!vtexcoords)
				return;

			std::vector<float> texcoords;
			texcoords.reserve(mesh_.n_faces() * 3);

			for (auto f : mesh_.faces())
			{
				Surface_mesh::Vertex_around_face_circulator fvit, fvend;
				Surface_mesh::Vertex v0, v1, v2;

				fvit = fvend = mesh_.vertices(f);
				v0 = *fvit; ++fvit;
				v2 = *fvit; ++fvit;
				do
				{
					v1 = v2;
					v2 = *fvit;

					if (vtexcoords)
					{
						texcoords.push_back(vtexcoords[v0]);
						texcoords.push_back(vtexcoords[v1]);
						texcoords.push_back(vtexcoords[v2]);
					}
				} while (++fvit != fvend);
			}

			glBindVertexArray(vertex_array_object_);
			glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer_);
			glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(gl::attrib_locations::TEXCOORDS, 1, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(gl::attrib_locations::TEXCOORDS);
			glBindVertexArray(0);
		}



		//-----------------------------------------------------------------------------
		void
			Surface_mesh_node::
			update_segmentation()
		{
			if (!mesh_.vsa_info.empty())
				for (auto f : mesh_.faces())
				{
					auto vsa_seg = mesh_.face_property<int>("f:segmentation");
					auto vpoints = mesh_.vertex_property<Point>("v:point");
					Surface_mesh::Vertex_around_face_circulator fvit = mesh_.vertices(f), fend = fvit;
					Surface_mesh::FaceInfo& tPatch = mesh_.vsa_info[vsa_seg[f]];
					Point p = vpoints[*fvit];
					tPatch.idx.push_back(p[0]);
					tPatch.idx.push_back(p[1]);
					tPatch.idx.push_back(p[2]);
					while (++fvit != fend)
					{
						p = vpoints[*fvit];
						tPatch.idx.push_back(p[0]);
						tPatch.idx.push_back(p[1]);
						tPatch.idx.push_back(p[2]);
					}
				}
		}
		//-----------------------------------------------------------------------------

		void
			Surface_mesh_node::
			update_features()
		{
				std::vector<unsigned int> features;
				features.reserve(2 * mesh_.n_edges());

				auto vertex_indices = mesh_.vertex_property<size_t>("v:index");
				auto efeature = mesh_.get_edge_property<bool>("e:feature");
				
				if (efeature)
				{
					for (auto e : mesh_.edges())
					{
						if (efeature[e])
						{
							features.push_back(vertex_indices[mesh_.vertex(e, 0)]);
							features.push_back(vertex_indices[mesh_.vertex(e, 1)]);
						}
					}
				}

				if (!features.empty())
				{
					glBindVertexArray(vertex_array_object_);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, feature_index_buffer_);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, features.size() * sizeof(unsigned int), &features[0], GL_STATIC_DRAW);
					glBindVertexArray(0);
				}
				n_feature_ = features.size();


			//ridge ravine
				
					auto fpoints = mesh_.get_feature_v_property<Point>("f:point");
					auto visual = mesh_.get_line_property<bool>("l:visual");
					Point v_pos;
					mesh_.vertset.clear();
					mesh_.ridges.clear();
					mesh_.ravines.clear();
					n_lines_ = 0;
					n_rav_lines_ = 0;
					for (auto v : mesh_.fvertices())
					{
						v_pos = fpoints[v];
						mesh_.vertset.push_back(v_pos);
					}
                if (!is_visual)
				  {
					for (auto l : mesh_.lines())
					{
						if (!mesh_.is_deleted(l))
						{
							Surface_mesh::FeatureHalfedge hhead;
							Surface_mesh::FeatureVertex vtail;
							hhead = mesh_.halfedge(l);
							vtail = mesh_.tail(l);
							if (mesh_.is_ridge(l))
							{
								while (1)
								{
									mesh_.ridges.push_back(mesh_.from_vertex(hhead).idx());
									mesh_.ridges.push_back(mesh_.to_vertex(hhead).idx());
									if (mesh_.to_vertex(hhead) == vtail) break;
									hhead = mesh_.next_halfedge(hhead);
								}
							}
							else
							{
								while (1)
								{
									mesh_.ravines.push_back(mesh_.from_vertex(hhead).idx());
									mesh_.ravines.push_back(mesh_.to_vertex(hhead).idx());
									if (mesh_.to_vertex(hhead) == vtail) break;
									hhead = mesh_.next_halfedge(hhead);
								}
							}
						}
					}

					if (!mesh_.ridges.empty())
					{
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_index_buffer_);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_.ridges.size() * sizeof(unsigned int), &mesh_.ridges[0], GL_STATIC_DRAW);
						n_lines_ = mesh_.ridges.size();
					}

					if (!mesh_.ravines.empty())
					{
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ravidx_buffer_);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_.ravines.size() * sizeof(unsigned int), &mesh_.ravines[0], GL_STATIC_DRAW);
						n_rav_lines_ = mesh_.ravines.size();
					}
				}
				else
				{
					for (auto l : mesh_.lines())
					{
						if (!mesh_.is_deleted(l) && mesh_.is_visual(l))
						{
							Surface_mesh::FeatureHalfedge hhead;
							Surface_mesh::FeatureVertex vtail;
							hhead = mesh_.halfedge(l);
							vtail = mesh_.tail(l);
							if (mesh_.is_ridge(l))
							{
								while (1)
								{
									mesh_.ridges.push_back(mesh_.from_vertex(hhead).idx());
									mesh_.ridges.push_back(mesh_.to_vertex(hhead).idx());
									if (mesh_.to_vertex(hhead) == vtail) break;
									hhead = mesh_.next_halfedge(hhead);
								}
							}
							else
							{
								while (1)
								{
									mesh_.ravines.push_back(mesh_.from_vertex(hhead).idx());
									mesh_.ravines.push_back(mesh_.to_vertex(hhead).idx());
									if (mesh_.to_vertex(hhead) == vtail) break;
									hhead = mesh_.next_halfedge(hhead);
								}
							}
						}
					}

					if (!mesh_.ridges.empty())
					{
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_index_buffer_);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_.ridges.size() * sizeof(unsigned int), &mesh_.ridges[0], GL_STATIC_DRAW);
						n_lines_ = mesh_.ridges.size();
					}

					if (!mesh_.ravines.empty())
					{
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_ravidx_buffer_);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_.ravines.size() * sizeof(unsigned int), &mesh_.ravines[0], GL_STATIC_DRAW);
						n_rav_lines_ = mesh_.ravines.size();
					}
				}
		}

		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			update_selection()
		{
			std::vector<unsigned int> selection;

			auto vertex_indices = mesh_.vertex_property<size_t>("v:index");
			auto vselected = mesh_.get_vertex_property<bool>("v:selected");

			if (vselected)
			{
				for (auto v : mesh_.vertices())
				{
					if (vselected[v])
					{
						selection.push_back(vertex_indices[v]);
					}
				}
			}
			if (!selection.empty())
			{

				glBindVertexArray(vertex_array_object_);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selection_index_buffer_);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, selection.size() * sizeof(unsigned int), &selection[0], GL_STATIC_DRAW);
				glBindVertexArray(0);
			}
			n_selected_ = selection.size();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			update_bbox()
		{
			auto points = mesh_.vertex_property<Point>("v:point");

			bbox_ = Bounding_box();
			for (auto v : mesh_.vertices())
			{
				bbox_ += points[v];
			}
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			select_point(Point p)
		{
			Surface_mesh::Vertex v_min;
			Scalar  d, d_min(FLT_MAX);

			auto points = mesh_.vertex_property<Point>("v:point");
			auto selected = mesh_.vertex_property<bool>("v:selected");

			for (auto v : mesh_.vertices())
			{
				d = distance(points[v], (Point)p);
				if (d < d_min)
				{
					v_min = v;
					d_min = d;
				}
			}
			selected[v_min] = true;
			update_selection();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			clear_selections()
		{
			auto selected = mesh_.get_vertex_property<bool>("v:selected");

			if (selected)
			{
				for (auto v : mesh_.vertices())
				{
					selected[v] = false;
				}
				update_selection();
			}
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			invert_selections()
		{
			auto selected = mesh_.get_vertex_property<bool>("v:selected");

			if (selected)
			{
				for (auto v : mesh_.vertices())
				{
					selected[v] = !selected[v];
				}
				update_selection();
			}
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			select_all()
		{
			auto selected = mesh_.vertex_property<bool>("v:selected");

			for (auto v : mesh_.vertices())
			{
				selected[v] = true;
			}
			update_selection();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			select_isolated()
		{
			for (auto v : mesh_.vertices())
			{
				if (mesh_.is_isolated(v))
				{
					auto selected = mesh_.vertex_property<bool>("v:selected");
					selected[v] = true;
				}
			}
			if (mesh_.get_vertex_property<bool>("v:selected"))
				update_selection();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			set_selection(const std::vector<size_t>& indices)
		{
			auto selected = mesh_.vertex_property<bool>("v:selected");

			for (size_t i(0); i < indices.size(); i++)
			{
				selected[Surface_mesh::Vertex(indices[i])] = true;
			}
			update_selection();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			delete_selected()
		{
			auto selected = mesh_.get_vertex_property<bool>("v:selected");

			if (selected)
			{
				for (auto v : mesh_.vertices())
				{
					if (selected[v])
					{
						selected[v] = false;
						mesh_.delete_vertex(v);
					}
				}
				mesh_.garbage_collection();
				update_mesh();
			}
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			get_selection(std::vector<size_t>& indices)
		{
			indices.clear();
			auto selected = mesh_.get_vertex_property<bool>("v:selected");

			if (selected)
			{
				for (auto v : mesh_.vertices())
				{
					if (selected[v])
					{
						indices.push_back(v.idx());
					}
				}
			}
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			clear_selection(const std::vector<size_t>& indices)
		{
			auto selected = mesh_.get_vertex_property<bool>("v:selected");
			if (selected)
			{
				for (auto i : indices)
				{
					selected[Surface_mesh::Vertex(i)] = false;
				}
			}
			update_selection();
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			grow_selection()
		{
			std::vector<Surface_mesh::Vertex> vertices;
			auto selected = mesh_.get_vertex_property<bool>("v:selected");

			if (selected)
			{
				for (auto v : mesh_.vertices())
				{
					if (selected[v])
					{
						for (auto vv : mesh_.vertices(v))
						{
							vertices.push_back(vv);
						}
					}
				}

				for (auto v : vertices)
					selected[v] = true;

				update_selection();
			}
		}


		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			initialize_buffers()
		{
			if (vertex_array_object_ == 0)
			{
				glGenVertexArrays(1, &vertex_array_object_);
				glBindVertexArray(vertex_array_object_);

				glGenBuffers(1, &vertex_buffer_);
				glGenBuffers(1, &normal_buffer_);
				glGenBuffers(1, &color_buffer_);
				glGenBuffers(1, &tex_coord_buffer_);
				glGenBuffers(1, &edge_index_buffer_);
				glGenBuffers(1, &feature_index_buffer_);
				glGenBuffers(1, &selection_index_buffer_);


				glGenBuffers(1, &line_index_buffer_);
				glGenBuffers(1, &lineVert_buffer_);
				glGenBuffers(1, &line_ravidx_buffer_);
				glGenBuffers(1, &vsa_index_buffer_);

				glBindVertexArray(0);
			}
			
		}

		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			delete_buffers()
		{
			if (vertex_buffer_)          glDeleteBuffers(1, &vertex_buffer_);
			if (normal_buffer_)          glDeleteBuffers(1, &normal_buffer_);
			if (color_buffer_)           glDeleteBuffers(1, &color_buffer_);
			if (tex_coord_buffer_)       glDeleteBuffers(1, &tex_coord_buffer_);

			if (edge_index_buffer_)      glDeleteBuffers(1, &edge_index_buffer_);
			if (feature_index_buffer_)   glDeleteBuffers(1, &feature_index_buffer_);
			if (selection_index_buffer_) glDeleteBuffers(1, &selection_index_buffer_);
            
			if (line_index_buffer_)      glDeleteBuffers(1, &line_index_buffer_);
			if (lineVert_buffer_)        glDeleteBuffers(1, &lineVert_buffer_);

			if (line_ravidx_buffer_)     glDeleteBuffers(1, &line_ravidx_buffer_);
			if (vsa_index_buffer_)       glDeleteBuffers(1, &vsa_index_buffer_);
			if (vertex_array_object_)    glDeleteVertexArrays(1, &vertex_array_object_);
		}



		//-----------------------------------------------------------------------------


		void
			Surface_mesh_node::
			crease_normals(std::vector<Normal>& vertex_normals)
		{
			bool is_triangle = mesh_.is_triangle_mesh();
			auto points = mesh_.vertex_property<Point>("v:point");

			if (crease_angle_ != 0 && is_triangle)
			{
				const Scalar crease = cos(crease_angle_ / 180.0 * M_PI);
				Normal n, ni, nni;
				Surface_mesh::Vertex v1, v2;
				Point e1, e2;
				Scalar w;

				mesh_.update_face_normals();
				auto fnormals = mesh_.face_property<Normal>("f:normal");

				for (auto f : mesh_.faces())
				{
					ni = fnormals[f];

					for (auto v : mesh_.vertices(f))
					{
						n = 0.0;

						for (auto h : mesh_.halfedges(v))
						{
							if (!mesh_.is_boundary(h))
							{
								nni = fnormals[mesh_.face(h)];

								if (dot(ni, nni) > crease)
								{
									v1 = mesh_.to_vertex(h);
									v2 = mesh_.to_vertex(mesh_.prev_halfedge(mesh_.prev_halfedge(h)));
									e1 = (points[v1] - points[v]).normalize();
									e2 = (points[v2] - points[v]).normalize();
									w = acos(std::max(-1.0f, std::min(1.0f, dot(e1, e2))));
									n += w * nni;
								}
							}
						}

						vertex_normals.push_back(n.normalize());
					}
				}
			}
			else
			{
				if (is_triangle)
					mesh_.update_vertex_normals();
				else
					mesh_.update_face_normals();

				auto vnormals = mesh_.get_vertex_property<Normal>("v:normal");
				auto fnormals = mesh_.face_property<Normal>("f:normal");

				for (auto f : mesh_.faces())
				{
					Surface_mesh::Vertex_around_face_circulator fvit, fvend;
					Surface_mesh::Vertex v0, v1, v2;

					fvit = fvend = mesh_.vertices(f);
					v0 = *fvit; ++fvit;
					v2 = *fvit; ++fvit;
					do
					{
						v1 = v2;
						v2 = *fvit;

						if (is_triangle)
						{
							vertex_normals.push_back(vnormals[v0]);
							vertex_normals.push_back(vnormals[v1]);
							vertex_normals.push_back(vnormals[v2]);
						}
						else
						{
							vertex_normals.push_back(fnormals[f]);
							vertex_normals.push_back(fnormals[f]);
							vertex_normals.push_back(fnormals[f]);
						}
					} while (++fvit != fvend);
				}
			}
		}

		

		//=============================================================================
	} // namespace scene_graph
} // namespace graphene
  //=============================================================================
