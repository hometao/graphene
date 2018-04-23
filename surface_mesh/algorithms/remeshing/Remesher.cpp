//== INCLUDES =================================================================

#include "Remesher.h"
#include <graphene/surface_mesh/algorithms/surface_mesh_tools/Curvature.h>
#include <graphene/geometry/bary_coord.h>
#include <graphene/geometry/distance_point_triangle.h>
#include <graphene/utility/Stop_watch.h>
#include <graphene/macros.h>

#include <float.h>
#include <cmath>

#define NORMAL_BASED  0


//== NAMESPACES ===============================================================

using graphene::utility::Stop_watch;

namespace graphene {
	namespace surface_mesh {


		//== IMPLEMENTATION ==========================================================


		void uniform_remeshing(Surface_mesh& mesh,
			Scalar edge_length,
			unsigned int iterations,
			bool use_projection)
		{
			Remesher(mesh).remesh(edge_length, iterations, use_projection);
		}


		//-----------------------------------------------------------------------------


		void adaptive_remeshing(Surface_mesh& mesh,
			Scalar min_edge_length,
			Scalar max_edge_length,
			Scalar approx_error,
			unsigned int iterations,
			bool use_projection)
		{
			Remesher(mesh).remesh(min_edge_length,
				max_edge_length,
				approx_error,
				iterations,
				use_projection);
		}


		//-----------------------------------------------------------------------------


		Remesher::
			Remesher(Surface_mesh& mesh)
			: mesh_(mesh), refmesh_(NULL), kD_tree_(NULL)
		{
			points_ = mesh_.vertex_property<Point>("v:point");
			vnormal_ = mesh_.vertex_property<Point>("v:normal");
		}


		//-----------------------------------------------------------------------------


		Remesher::
			~Remesher()
		{
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			remesh(Scalar edge_length,
				unsigned int iterations,
				bool use_projection)
		{
			uniform_ = true;
			use_projection_ = use_projection;
			target_edge_length_ = edge_length;

			double t_pre(0);
			double t_split(0);
			double t_collapse(0);
			double t_flip(0);
			double t_smooth(0);
			double t_caps(0);
			Stop_watch timer;

			timer.start();
			preprocessing();
			timer.stop();
			t_pre = timer.elapsed();


			for (unsigned int i = 0; i<iterations; ++i)
			{
				timer.start();
				split_long_edges();
				timer.stop();
				t_split += timer.elapsed();

				mesh_.update_vertex_normals();

				timer.start();
				collapse_short_edges();
				timer.stop();
				t_collapse += timer.elapsed();

				timer.start();
				flip_edges();
				timer.stop();
				t_flip += timer.elapsed();

				timer.start();
				tangential_smoothing(5);
				timer.stop();
				t_smooth += timer.elapsed();
			}

			timer.start();
			remove_caps();
			timer.stop();
			t_caps += timer.elapsed();

			postprocessing();


			LOG(Log_info) << "Remeshing times:\n";
			LOG(Log_info) << "preproc:  " << t_pre << "ms\n";
			LOG(Log_info) << "sp lit:    " << t_split << "ms\n";
			LOG(Log_info) << "collapse: " << t_collapse << "ms\n";
			LOG(Log_info) << "flip:     " << t_flip << "ms\n";
			LOG(Log_info) << "smooth:   " << t_smooth << "ms\n";
			LOG(Log_info) << "caps:     " << t_caps << "ms\n";
			LOG(Log_info) << std::endl;
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			remesh(Scalar min_edge_length,
				Scalar max_edge_length,
				Scalar approx_error,
				unsigned int iterations,
				bool use_projection)
		{
			uniform_ = false;
			min_edge_length_ = min_edge_length;
			max_edge_length_ = max_edge_length;
			approx_error_ = approx_error;
			use_projection_ = use_projection;

			double t_pre(0);
			double t_split(0);
			double t_collapse(0);
			double t_flip(0);
			double t_smooth(0);
			double t_caps(0);
			Stop_watch timer;


			timer.start();
			preprocessing();
			timer.stop();
			t_pre = timer.elapsed();


			for (unsigned int i = 0; i<iterations; ++i)
			{
				timer.start();
				split_long_edges();
				timer.stop();
				t_split += timer.elapsed();

				mesh_.update_vertex_normals();

				timer.start();
				collapse_short_edges();
				timer.stop();
				t_collapse += timer.elapsed();

				timer.start();
				flip_edges();
				timer.stop();
				t_flip += timer.elapsed();

				timer.start();
				tangential_smoothing(5);
				timer.stop();
				t_smooth += timer.elapsed();
			}

			timer.start();
			remove_caps();
			timer.stop();
			t_caps += timer.elapsed();


			postprocessing();


			LOG(Log_info) << "Remeshing times:\n";
			LOG(Log_info) << "preproc:  " << t_pre << "ms\n";
			LOG(Log_info) << "split:    " << t_split << "ms\n";
			LOG(Log_info) << "collapse: " << t_collapse << "ms\n";
			LOG(Log_info) << "flip:     " << t_flip << "ms\n";
			LOG(Log_info) << "smooth:   " << t_smooth << "ms\n";
			LOG(Log_info) << "caps:     " << t_caps << "ms\n";
			LOG(Log_info) << std::endl;
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			preprocessing()
		{
			// properties
			vfeature_ = mesh_.vertex_property<bool>("v:feature", false);
			efeature_ = mesh_.edge_property<bool>("e:feature", false);
			vlocked_ = mesh_.add_vertex_property<bool>("v:locked", false);
			elocked_ = mesh_.add_edge_property<bool>("e:locked", false);
			vsizing_ = mesh_.add_vertex_property<Scalar>("v:sizing");



			// lock unselected vertices if some vertices are selected
			Surface_mesh::Vertex_property<bool> vselected = mesh_.get_vertex_property<bool>("v:selected");
			if (vselected)
			{
				bool has_selection = false;
				for (Surface_mesh::Vertex_iterator v_it = mesh_.vertices_begin(); v_it != mesh_.vertices_end(); ++v_it)
				{
					if (vselected[*v_it])
					{
						has_selection = true;
						break;
					}
				}

				if (has_selection)
				{
					for (Surface_mesh::Vertex_iterator v_it = mesh_.vertices_begin(); v_it != mesh_.vertices_end(); ++v_it)
					{
						vlocked_[*v_it] = !vselected[*v_it];
					}

					// lock an edge if one of its vertices is locked
					for (Surface_mesh::Edge_iterator e_it = mesh_.edges_begin(); e_it != mesh_.edges_end(); ++e_it)
					{
						elocked_[*e_it] = (vlocked_[mesh_.vertex(*e_it, 0)] || vlocked_[mesh_.vertex(*e_it, 1)]);
					}
				}
			}


			// set feature edges
			Surface_mesh::Edge_iterator eit, eend;
			Surface_mesh::Face          f0, f1;
			Normal n0, n1;
			int count = 0;
			for (eit = mesh_.edges_begin(), eend = mesh_.edges_end(); eit != eend; ++eit)
			{
				if (mesh_.is_boundary(*eit))
				{
					efeature_[*eit] = true;
				}
				else
				{
					f0 = mesh_.face(mesh_.halfedge(*eit, 0));
					f1 = mesh_.face(mesh_.halfedge(*eit, 1));
					n0 = mesh_.compute_face_normal(f0);
					n1 = mesh_.compute_face_normal(f1);
					if (angle(n0, n1) >= 30 / 180.0*M_PI)
					{
						efeature_[*eit] = true;
						/*vfeature_[mesh_.vertex(*eit,0)] = true;
						vfeature_[mesh_.vertex(*eit,1)] = true;*/
						count++;
					}
				}
			}

			Surface_mesh::Halfedge_around_vertex_circulator vhit, vhend;
			int n;
			for (Surface_mesh::Vertex_iterator vit = mesh_.vertices_begin(); vit != mesh_.vertices_end(); ++vit)
			{
				n = 0;
				vhit = vhend = mesh_.halfedges(*vit);
				do
				{
					if (efeature_[mesh_.edge(*vhit)])
						++n;
				} while (++vhit != vhend);

				if (n >= 2) vfeature_[*vit] = true;

			}

			LOG(Log_info) << count << " feature edges1" << std::endl;
			count = 0;



			// lock feature corners
			Surface_mesh::Halfedge_around_vertex_circulator vh_it, vh_end;
			int c;
			for (Surface_mesh::Vertex_iterator v_it = mesh_.vertices_begin(); v_it != mesh_.vertices_end(); ++v_it)
			{
				if (vfeature_[*v_it])  //vfeature
				{
					c = 0;
					vh_it = vh_end = mesh_.halfedges(*v_it);
					do
					{
						if (efeature_[mesh_.edge(*vh_it)])
							++c;
					} while (++vh_it != vh_end);

					if (c > 2)
					{
						vlocked_[*v_it] = true;                                   //feature顶点周围有>2条feature边则锁定
						count++;
					}
				}
			}
			LOG(Log_info) << count << " locked vertices1" << std::endl;



			// compute sizing field
			if (uniform_)
			{
				for (Surface_mesh::Vertex_iterator vit = mesh_.vertices_begin(); vit != mesh_.vertices_end(); ++vit)
				{
					vsizing_[*vit] = target_edge_length_;
				}
			}
			else
			{
				// compute curvature for all mesh vertices, using cotan or Cohen-Steiner
				// do 2 post-smoothing steps to get a smoother sizing field
				Curvature_analyzer curv(mesh_);
				//curv.analyze(1);
				curv.analyze_tensor(1, true);


				for (Surface_mesh::Vertex_iterator vit = mesh_.vertices_begin(); vit != mesh_.vertices_end(); ++vit)
				{
					// maximum absolute curvature
					Scalar c = curv.max_abs_curvature(*vit);


					// curvature of feature vertices: average of non-feature neighbors
					if (vfeature_[*vit])
					{
						Surface_mesh::Halfedge_around_vertex_circulator vhit, vhend;
						Surface_mesh::Vertex v;
						Scalar w, ww = 0.0;
						c = 0.0;

						vhit = vhend = mesh_.halfedges(*vit);
						if (vhit) do
						{
							v = mesh_.to_vertex(*vhit);
							if (!vfeature_[v])
							{
								w = std::max(0.0, cotan_weight(mesh_, mesh_.edge(*vhit)));
								ww += w;
								c += w * curv.max_abs_curvature(v);
							}
						} while (++vhit != vhend);

						c /= ww;
					}


					// get edge length from curvature
					const Scalar r = 1.0 / c;
					const Scalar e = approx_error_;
					Scalar h;
					if (e < r)
					{
						// see mathworld: "circle segment" and "equilateral triangle"
						//h = sqrt(2.0*r*e-e*e) * 3.0 / sqrt(3.0);
						h = sqrt(6.0*e*r - 3.0*e*e); // simplified...
					}
					else
					{
						// this does not really make sense
						h = e * 3.0 / sqrt(3.0);
					}


					// clamp to min. and max. edge length
					if (h < min_edge_length_) h = min_edge_length_;
					else if (h > max_edge_length_) h = max_edge_length_;


					// store target edge length
					vsizing_[*vit] = h;
				}
			}


			if (use_projection_)
			{
				// build reference mesh
				refmesh_ = new Surface_mesh();
				refmesh_->assign(mesh_);
				refmesh_->update_vertex_normals();
				refpoints_ = refmesh_->vertex_property<Point>("v:point");
				refnormals_ = refmesh_->vertex_property<Point>("v:normal");


				// copy sizing field from mesh_
				refsizing_ = refmesh_->add_vertex_property<Scalar>("v:sizing");
				for (Surface_mesh::Vertex_iterator vit = refmesh_->vertices_begin(); vit != refmesh_->vertices_end(); ++vit)
				{
					refsizing_[*vit] = vsizing_[*vit];
				}


				// build BSP
				kD_tree_ = new Triangle_kD_tree(*refmesh_, 0);
			}
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			postprocessing()
		{
			// delete BSP and reference mesh
			if (use_projection_)
			{
				delete kD_tree_;
				delete refmesh_;
			}

			// remove properties
			mesh_.remove_vertex_property(vlocked_);
			mesh_.remove_edge_property(elocked_);
			mesh_.remove_vertex_property(vsizing_);
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			project_to_reference(Surface_mesh::Vertex v)
		{
			if (!use_projection_)
			{
				return;
			}


			// find closest triangle of reference mesh
			Triangle_kD_tree::Nearest_neighbor nn = kD_tree_->nearest(points_[v]);
			const Point p = nn.nearest;
			const Surface_mesh::Face  f = nn.face;


			// get face data
			Surface_mesh::Vertex_around_face_circulator fv_it = refmesh_->vertices(f);
			const Point  p0 = refpoints_[*fv_it];
			const Point  n0 = refnormals_[*fv_it];
			const Scalar s0 = refsizing_[*fv_it];
			++fv_it;
			const Point  p1 = refpoints_[*fv_it];
			const Point  n1 = refnormals_[*fv_it];
			const Scalar s1 = refsizing_[*fv_it];
			++fv_it;
			const Point  p2 = refpoints_[*fv_it];
			const Point  n2 = refnormals_[*fv_it];
			const Scalar s2 = refsizing_[*fv_it];

			// get barycentric coordinates
			Point b = geometry::barycentric_coordinates(p, p0, p1, p2);

			// interpolate normal
			Point n;
			n = (n0 * b[0]);
			n += (n1 * b[1]);
			n += (n2 * b[2]);
			n.normalize();
#ifdef _WIN32
			assert(_finite(n[0]));
#else
			assert(!std::isnan(n[0]));
#endif

			// interpolate sizing field
			Scalar s;
			s = (s0 * b[0]);
			s += (s1 * b[1]);
			s += (s2 * b[2]);

			// set result
			points_[v] = p;
			vnormal_[v] = n;
			vsizing_[v] = s;
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			split_long_edges()
		{
			Surface_mesh::Vertex_iterator  v_it, v_end;
			Surface_mesh::Edge_iterator    e_it, e_end;
			Surface_mesh::Vertex           v, v0, v1;
			Surface_mesh::Edge             e, e0, e1;
			Surface_mesh::Face             f0, f1, f2, f3;
			bool                           ok, is_feature, is_boundary;
			int                            i;



			for (ok = false, i = 0; !ok && i<10; ++i)
			{
				ok = true;

				for (e_it = mesh_.edges_begin(), e_end = mesh_.edges_end(); e_it != e_end; ++e_it)
				{
					v0 = mesh_.vertex(*e_it, 0);
					v1 = mesh_.vertex(*e_it, 1);

					if (!elocked_[*e_it] && is_too_long(v0, v1))
					{
						const Point& p0 = points_[v0];
						const Point& p1 = points_[v1];

						is_feature = efeature_[*e_it];
						is_boundary = mesh_.is_boundary(*e_it);

						v = mesh_.add_vertex((p0 + p1)*0.5);
						mesh_.split(*e_it, v);

						// need normal or sizing for adaptive refinement
						//vnormal_[v] = normalize(vnormal_[v0] + vnormal_[v1]);
						vnormal_[v] = mesh_.compute_vertex_normal(v);
						vsizing_[v] = 0.5f * (vsizing_[v0] + vsizing_[v1]);

						if (is_feature)
						{
							e = is_boundary ? Surface_mesh::Edge(mesh_.n_edges() - 2) : Surface_mesh::Edge(mesh_.n_edges() - 3);
							efeature_[e] = true;
							vfeature_[v] = true;
						}
						else
						{
							project_to_reference(v);
						}

						ok = false;
					}
				}
			}
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			collapse_short_edges()
		{
			Surface_mesh::Edge_iterator     e_it, e_end;
			Surface_mesh::Vertex_around_vertex_circulator vv_it, vv_end;
			Surface_mesh::Vertex    v0, v1;
			Surface_mesh::Halfedge  h0, h1, h01, h10;
			bool  ok, skip, b0, b1, l0, l1, f0, f1;
			int   i;
			bool  hcol01, hcol10;



			for (ok = false, i = 0; !ok && i<10; ++i)
			{
				ok = true;

				for (e_it = mesh_.edges_begin(), e_end = mesh_.edges_end(); e_it != e_end; ++e_it)
				{
					if (!mesh_.is_deleted(*e_it) && !elocked_[*e_it])
					{
						h10 = mesh_.halfedge(*e_it, 0);
						h01 = mesh_.halfedge(*e_it, 1);
						v0 = mesh_.to_vertex(h10);
						v1 = mesh_.to_vertex(h01);

						if (is_too_short(v0, v1))
						{
							// get status
							b0 = mesh_.is_boundary(v0);
							b1 = mesh_.is_boundary(v1);
							l0 = vlocked_[v0];
							l1 = vlocked_[v1];
							f0 = vfeature_[v0];
							f1 = vfeature_[v1];
							hcol01 = hcol10 = true;

							// boundary rules
							if (b0 && b1) { if (!mesh_.is_boundary(*e_it)) continue; }
							else if (b0) hcol01 = false;
							else if (b1) hcol10 = false;

							// locked rules
							if (l0 && l1) continue;
							else if (l0) hcol01 = false;
							else if (l1) hcol10 = false;

							// feature rules
							if (f0 && f1)
							{
								// edge must be feature
								if (!efeature_[*e_it]) continue;

								// the other two edges removed by collapse must not be features
								h0 = mesh_.prev_halfedge(h01);
								h1 = mesh_.next_halfedge(h10);
								if (efeature_[mesh_.edge(h0)] || efeature_[mesh_.edge(h1)])
									hcol01 = false;
								h0 = mesh_.prev_halfedge(h10);
								h1 = mesh_.next_halfedge(h01);
								if (efeature_[mesh_.edge(h0)] || efeature_[mesh_.edge(h1)])
									hcol10 = false;
							}
							else if (f0) hcol01 = false;
							else if (f1) hcol10 = false;

							// topological rules
							bool collapse_ok = mesh_.is_collapse_ok(h01);
							if (hcol01)  hcol01 = collapse_ok;
							if (hcol10)  hcol10 = collapse_ok;

							// both collapses possible: collapse into vertex w/ higher valence
							if (hcol01 && hcol10)
							{
								if (mesh_.valence(v0) < mesh_.valence(v1))
									hcol10 = false;
								else
									hcol01 = false;
							}

							// try v1 -> v0
							if (hcol10)
							{
								// don't create too long edges
								skip = false;
								vv_it = vv_end = mesh_.vertices(v1);
								do
								{
									if (is_too_long(v0, *vv_it))
									{
										hcol10 = false;
										break;
									}
								} while (++vv_it != vv_end);

								if (hcol10)
								{
									mesh_.collapse(h10);
									ok = false;
								}
							}

							// try v0 -> v1
							else if (hcol01)
							{
								// don't create too long edges
								skip = false;
								vv_it = vv_end = mesh_.vertices(v0);
								do
								{
									if (is_too_long(v1, *vv_it))
									{
										hcol01 = false;
										break;
									}
								} while (++vv_it != vv_end);

								if (hcol01)
								{
									mesh_.collapse(h01);
									ok = false;
								}
							}
						}
					}
				}
			}

			mesh_.garbage_collection();
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			flip_edges()
		{
			Surface_mesh::Edge_iterator     e_it, e_end;
			Surface_mesh::Vertex_iterator   v_it, v_end;
			Surface_mesh::Vertex            v0, v1, v2, v3;
			Surface_mesh::Halfedge          h;
			int                          val0, val1, val2, val3;
			int                          val_opt0, val_opt1, val_opt2, val_opt3;
			int                          ve0, ve1, ve2, ve3, ve_before, ve_after;
			bool                         ok;
			int                          i;


			// precompute valences
			Surface_mesh::Vertex_property<int> valence = mesh_.add_vertex_property<int>("valence");
			Surface_mesh::Vertex_iterator  vit, vend = mesh_.vertices_end();
			for (vit = mesh_.vertices_begin(); vit != vend; ++vit)
			{
				valence[*vit] = mesh_.valence(*vit);
			}


			for (ok = false, i = 0; !ok && i<10; ++i)
			{
				ok = true;

				for (e_it = mesh_.edges_begin(), e_end = mesh_.edges_end(); e_it != e_end; ++e_it)
				{
					if (!elocked_[*e_it] && !efeature_[*e_it])
					{
						h = mesh_.halfedge(*e_it, 0);
						v0 = mesh_.to_vertex(h);
						v2 = mesh_.to_vertex(mesh_.next_halfedge(h));
						h = mesh_.halfedge(*e_it, 1);
						v1 = mesh_.to_vertex(h);
						v3 = mesh_.to_vertex(mesh_.next_halfedge(h));

						if (!vlocked_[v0] && !vlocked_[v1] && !vlocked_[v2] && !vlocked_[v3])
						{
							val0 = valence[v0];
							val1 = valence[v1];
							val2 = valence[v2];
							val3 = valence[v3];

							val_opt0 = (mesh_.is_boundary(v0) ? 4 : 6);
							val_opt1 = (mesh_.is_boundary(v1) ? 4 : 6);
							val_opt2 = (mesh_.is_boundary(v2) ? 4 : 6);
							val_opt3 = (mesh_.is_boundary(v3) ? 4 : 6);

							ve0 = (val0 - val_opt0);
							ve1 = (val1 - val_opt1);
							ve2 = (val2 - val_opt2);
							ve3 = (val3 - val_opt3);

							ve0 *= ve0;
							ve1 *= ve1;
							ve2 *= ve2;
							ve3 *= ve3;

							ve_before = ve0 + ve1 + ve2 + ve3;

							--val0;  --val1;
							++val2;  ++val3;

							ve0 = (val0 - val_opt0);
							ve1 = (val1 - val_opt1);
							ve2 = (val2 - val_opt2);
							ve3 = (val3 - val_opt3);

							ve0 *= ve0;
							ve1 *= ve1;
							ve2 *= ve2;
							ve3 *= ve3;

							ve_after = ve0 + ve1 + ve2 + ve3;

							if (ve_before > ve_after && mesh_.is_flip_ok(*e_it))
							{
								mesh_.flip(*e_it);
								--valence[v0];
								--valence[v1];
								++valence[v2];
								++valence[v3];
								ok = false;
							}
						}
					}
				}
			}

			mesh_.remove_vertex_property(valence);
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			tangential_smoothing(unsigned int iterations)
		{
			Surface_mesh::Vertex v1, v2, v3, vv;
			Surface_mesh::Edge  e;
			Scalar  w, ww, area;
			Point   u, n, t, b;

			Surface_mesh::Vertex_iterator vit, vend(mesh_.vertices_end());
			Surface_mesh::Halfedge_around_vertex_circulator hit, hend;


			// add property
			Surface_mesh::Vertex_property<Point>  update = mesh_.add_vertex_property<Point>("v:update");


			// project at the beginning to get valid sizing values and normal vectors
			// for vertices introduced by splitting
			if (use_projection_)
			{
				for (vit = mesh_.vertices_begin(); vit != vend; ++vit)
				{
					Surface_mesh::Vertex v(*vit);

					if (!mesh_.is_boundary(v) && !vlocked_[v])
					{
						project_to_reference(v);
					}
				}
			}


			for (unsigned int iters = 0; iters<iterations; ++iters)
			{
				for (vit = mesh_.vertices_begin(); vit != vend; ++vit)
				{
					Surface_mesh::Vertex v(*vit);

					if (!mesh_.is_boundary(v) && !vlocked_[v])
					{
						if (vfeature_[v])
						{
							u = 0.0;
							t = 0.0;
							ww = 0;
							int c = 0;

							hit = hend = mesh_.halfedges(v);
							do
							{
								Surface_mesh::Halfedge h = *hit;

								if (efeature_[mesh_.edge(h)])
								{
									vv = mesh_.to_vertex(h);

									b = points_[v];
									b += points_[vv];
									b *= 0.5;

									w = distance(points_[v], points_[vv]) / (0.5*(vsizing_[v] + vsizing_[vv]));
									ww += w;
									u += w * b;

									if (c == 0)
									{
										t += normalize(points_[vv] - points_[v]);
										++c;
									}
									else
									{
										++c;
										t -= normalize(points_[vv] - points_[v]);
									}
								}
							} while (++hit != hend);

							assert(c == 2);

							u *= (1.0 / ww);
							u -= points_[v];
							t = normalize(t);
							u = t * dot(u, t);
						}
						else
						{
							u = 0.0;
							t = 0.0;
							ww = 0;

							hit = hend = mesh_.halfedges(v);
							do
							{
								Surface_mesh::Halfedge h = *hit;

								v1 = v;
								v2 = mesh_.to_vertex(h);
								v3 = mesh_.to_vertex(mesh_.next_halfedge(h));

								b = points_[v1];
								b += points_[v2];
								b += points_[v3];
								b *= (1.0 / 3.0);

								area = norm(cross(points_[v2] - points_[v1], points_[v3] - points_[v1]));
								w = area / pow((vsizing_[v1] + vsizing_[v2] + vsizing_[v3]) / 3.0, 2.0);

								u += w * b;
								ww += w;
							} while (++hit != hend);

							u /= ww;
							u -= points_[v];
							n = vnormal_[v];
							u -= n*dot(u, n);

							update[v] = u;
						}
					}
				}


				// update vertex positions
				for (vit = mesh_.vertices_begin(); vit != vend; ++vit)
				{
					Surface_mesh::Vertex v(*vit);
					if (!mesh_.is_boundary(v) && !vlocked_[v])
					{
						points_[v] += update[v];
					}
				}


				// update normal vectors (if not done so through projection
				mesh_.update_vertex_normals();
			}


			// project at the end
			if (use_projection_)
			{
				for (vit = mesh_.vertices_begin(); vit != vend; ++vit)
				{
					Surface_mesh::Vertex v(*vit);
					if (!mesh_.is_boundary(v) && !vlocked_[v])
					{
						project_to_reference(v);
					}
				}
			}


			// remove property
			mesh_.remove_vertex_property(update);
		}


		//-----------------------------------------------------------------------------


		void
			Remesher::
			remove_caps()
		{
			Surface_mesh::Edge_iterator  e_it, e_end(mesh_.edges_end());
			Surface_mesh::Halfedge       h;
			Surface_mesh::Vertex         v, vb, vd;
			Surface_mesh::Face           fb, fd;
			Scalar                       a0, a1, amin, aa(::cos(170.0 * M_PI / 180.0));
			Point                        a, b, c, d;


			for (e_it = mesh_.edges_begin(); e_it != e_end; ++e_it)
			{
				if (!elocked_[*e_it] && mesh_.is_flip_ok(*e_it))
				{
					h = mesh_.halfedge(*e_it, 0);
					a = points_[mesh_.to_vertex(h)];

					h = mesh_.next_halfedge(h);
					b = points_[vb = mesh_.to_vertex(h)];

					h = mesh_.halfedge(*e_it, 1);
					c = points_[mesh_.to_vertex(h)];

					h = mesh_.next_halfedge(h);
					d = points_[vd = mesh_.to_vertex(h)];

					a0 = dot((a - b).normalize(), (c - b).normalize());
					a1 = dot((a - d).normalize(), (c - d).normalize());

					if (a0 < a1) { amin = a0; v = vb; }
					else { amin = a1; v = vd; }

					// is it a cap?
					if (amin < aa)
					{
						// feature edge and feature vertex -> seems to be intended
						if (efeature_[*e_it] && vfeature_[v])
							continue;

						// project v onto feature edge
						if (efeature_[*e_it])
							points_[v] = (a + c)*0.5;

						// flip
						mesh_.flip(*e_it);
					}
				}
			}
		}


		//=============================================================================
	} // namespace surface_mesh
} // namespace graphene
  //=============================================================================
