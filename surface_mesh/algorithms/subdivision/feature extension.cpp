//== INCLUDES =================================================================

#include <graphene/surface_mesh/algorithms/subdivision/feature extension.h>
#include <graphene/surface_mesh/algorithms/surface_mesh_tools/diffgeo.h>
#include <cmath>
#include <graphene/macros.h>

//== NAMESPACE ================================================================

namespace graphene {
	namespace surface_mesh {

		//== IMPLEMENTATION ==========================================================

		const Scalar PI = 3.1415926;
		void feature_extension(Surface_mesh& mesh)
		{
			auto  vsa_pro = mesh.vertex_property<Scalar>("v:VSA segmentation propablitiy", 0);
			auto  k_max = mesh.vertex_property<Scalar>("v:max curvature");
			auto  k_min = mesh.vertex_property<Scalar>("v:min curvature");
			auto  dir_min = mesh.vertex_property<Direction>("v:min direction");
			auto  dir_max = mesh.vertex_property<Direction>("v:max direction");
			auto  ldeleted = mesh.get_line_property<bool>("l:deleted");

			FILE *clr = NULL;
			clr = fopen("input_high.clr", "r");
			
			int num_high(0),num_low(0), vertex_n, patch_n, patchID, seed, patch_fN, faceID;
			double R, G, B;
			fscanf(clr, "%d", &num_high);

			for (int k = 0; k < num_high; ++k)
			{
				auto VSA_seg2 = mesh.add_face_property<int>("f:vsa segment", -1);  //read VSA segmentation data
				auto vsa_visited = mesh.add_vertex_property<bool>("v:vsa visited", false);

				fscanf(clr, "%d", &vertex_n);

				if (vertex_n != mesh.n_vertices())
				{
					std::cerr << "This *.clr file is not mathing mesh!" << std::endl;
					break;
				}
				fscanf(clr, "%d", &patch_n);
				int i, j;
				for (i = 0; i < patch_n; ++i)
				{
					fscanf(clr, "%d %d %d", &patchID, &seed, &patch_fN);
					for (j = 0; j < patch_fN; ++j)
					{
						fscanf(clr, "%d", &faceID);
						VSA_seg2[Surface_mesh::Face(faceID)] = patchID;
					}
					fscanf(clr, "%lf %lf %lf", &R, &G, &B);
				}

				Surface_mesh::Edge_iterator eit, eend;
				Surface_mesh::Face          f0, f1;
				Normal n0, n1;
				for (eit = mesh.edges_begin(), eend = mesh.edges_end(); eit != eend; ++eit)
				{
					if (!mesh.is_boundary(*eit))
					{
						f0 = mesh.face(mesh.halfedge(*eit, 0));
						f1 = mesh.face(mesh.halfedge(*eit, 1));

						if (VSA_seg2[f0] != VSA_seg2[f1])
						{
							if (!vsa_visited[mesh.to_vertex(mesh.halfedge(*eit, 0))])
							{
								vsa_pro[mesh.to_vertex(mesh.halfedge(*eit, 0))] += 1;
								vsa_visited[mesh.to_vertex(mesh.halfedge(*eit, 0))] = true;
							}
							if (!vsa_visited[mesh.from_vertex(mesh.halfedge(*eit, 0))])
							{
								vsa_pro[mesh.from_vertex(mesh.halfedge(*eit, 0))] += 1;
								vsa_visited[mesh.from_vertex(mesh.halfedge(*eit, 0))] = true;
							}
						}
					}
				}
				mesh.remove_face_property(VSA_seg2);
				mesh.remove_vertex_property(vsa_visited);

			}

			FILE *clr1 = NULL;
			clr1 = fopen("input_low.clr", "r");
			fscanf(clr1, "%d", &num_low);
			for (int k = 0; k < num_low; ++k)
			{
				auto VSA_seg2 = mesh.add_face_property<int>("f:vsa segment", -1);  //read VSA segmentation data
				auto vsa_visited = mesh.add_vertex_property<bool>("v:vsa visited", false);

				fscanf(clr1, "%d", &vertex_n);

				if (vertex_n != mesh.n_vertices())
				{
					std::cerr << "This *.clr file is not mathing mesh!" << std::endl;
					break;
				}
				fscanf(clr1, "%d", &patch_n);
				int i, j;
				for (i = 0; i < patch_n; ++i)
				{
					fscanf(clr1, "%d %d %d", &patchID, &seed, &patch_fN);
					for (j = 0; j < patch_fN; ++j)
					{
						fscanf(clr1, "%d", &faceID);
						VSA_seg2[Surface_mesh::Face(faceID)] = patchID;
					}
					fscanf(clr1, "%lf %lf %lf", &R, &G, &B);
				}

				Surface_mesh::Edge_iterator eit, eend;
				Surface_mesh::Face          f0, f1;
				Normal n0, n1;
				for (eit = mesh.edges_begin(), eend = mesh.edges_end(); eit != eend; ++eit)
				{
					if (!mesh.is_boundary(*eit))
					{
						f0 = mesh.face(mesh.halfedge(*eit, 0));
						f1 = mesh.face(mesh.halfedge(*eit, 1));

						if (VSA_seg2[f0] != VSA_seg2[f1])
						{
							if (!vsa_visited[mesh.to_vertex(mesh.halfedge(*eit, 0))])
							{
								vsa_pro[mesh.to_vertex(mesh.halfedge(*eit, 0))] += 1;
								vsa_visited[mesh.to_vertex(mesh.halfedge(*eit, 0))] = true;
							}
							if (!vsa_visited[mesh.from_vertex(mesh.halfedge(*eit, 0))])
							{
								vsa_pro[mesh.from_vertex(mesh.halfedge(*eit, 0))] += 1;
								vsa_visited[mesh.from_vertex(mesh.halfedge(*eit, 0))] = true;
							}
						}
					}
				}
				mesh.remove_face_property(VSA_seg2);
				mesh.remove_vertex_property(vsa_visited);

			}

			for (int i = 0; i < mesh.n_vertices(); i++)
			{
				vsa_pro[Surface_mesh::Vertex(i)] =vsa_pro[Surface_mesh::Vertex(i)] / (num_high+num_low);
			}

			auto lineface = mesh.add_face_property<int>("f:line face", -1);
			auto visted = mesh.add_vertex_property<int>("v:used", -1);    //标记延伸边界
			for (auto l : mesh.lines())
			{
				if (ldeleted[l]) continue;
				Surface_mesh::Halfedge_around_line_circulator  hit;
				Surface_mesh::FeatureVertex vit, vend;
				Surface_mesh::Face f;
				//Surface_mesh::FeatureLine line;
				int k(0);
				hit = mesh.halfedges(l);
				vend = mesh.tail(l);

				while (mesh.to_vertex(*hit) != vend)
				{
					vit = mesh.to_vertex(*hit);
					f = mesh.face(*hit);
					if (f.is_valid())
					{
						k++;
						//face_line[f] = mesh.line(*hit).idx();
						lineface[f] = l.idx();
					}
					++hit;
				}
				vit = mesh.to_vertex(*hit);
				f = mesh.face(*hit);
				if (f.is_valid())
				{
					lineface[f] = mesh.line(*hit).idx();
				}
			}

			for (auto f : mesh.faces())
			{
				if (lineface[f] != -1)
				{
					Surface_mesh::Vertex_around_face_circulator fvit, fvend;
					fvit = fvend = mesh.vertices(f);
					do
					{
						visted[*fvit] = lineface[f];
						++fvit;
					} while (fvit != fvend);
				}
			}

			auto is_visit = mesh.vertex_property<bool>("v:is_visit", false);
			auto vpoints = mesh.vertex_property<Point>("v:point");
			auto fpoints = mesh.feature_v_property<Point>("f:point");
			auto epdeleted = mesh.endpoint_property<bool>("ep:deleted");
			
			//start 
			//Optimization of the endpoint and Handle the endpoint
			std::vector<Endpoint>  group;
			std::vector<Endpoint>  goals;
			for (auto l : mesh.lines())
			{
				if (ldeleted[l] || mesh.head(l) == mesh.tail(l) || mesh.to_vertex(mesh.halfedge(l)) == mesh.tail(l)) continue;

				Surface_mesh::Halfedge_around_line_circulator  hit;
				Surface_mesh::FeatureVertex vit, vend;
				Surface_mesh::Face f;
				Surface_mesh::FeatureLine line;
				int k(0);
				hit = mesh.halfedges(l);
				vend = mesh.tail(l);

				while (1/*mesh.to_vertex(*hit) != vend*/)
				{
					//vit = mesh.to_vertex(*hit);
					f = mesh.face(*hit);
					if (f.is_valid())
					{
						k++;
						lineface[f] = l.idx();
					}
					if (mesh.to_vertex(*hit) == vend) break;
					++hit;
				}
				/*vit = mesh.to_vertex(*hit);
				f = mesh.face(*hit);
				if (f.is_valid())
				{
					lineface[f] = mesh.line(*hit).idx();
				
				}*/

				
				if (mesh.get_exten(l) != 2 && mesh.get_exten(l) != 0)          //处理头端点   
				{
					Surface_mesh::FeatureHalfedge hhead,next_hhead;
					Surface_mesh::FeatureVertex vhead;
					Surface_mesh::Halfedge_around_face_circulator hit, hend;
					Surface_mesh::Vertex_around_face_circulator vit, vend;
					Surface_mesh::Halfedge h;
					Scalar cosine, denom;
					Point  vec1, vec2, vec3;
					hhead = mesh.halfedge(l);
					f = mesh.face(hhead);
					vit = vend = mesh.vertices(f);
					do
					{
						group.push_back({ *vit,0,0,0 });
						//goals.push_back({ *vit,0,0,0 });
						++vit;
					} while (vit != vend);
					hit = hend = mesh.halfedges(f);
					do 
					{
						h = mesh.opposite_halfedge(*hit);
						f = mesh.face(h);
						vit = vend = mesh.vertices(f);
						do 
						{
							bool flag = true;
							for (auto iter = group.begin();iter != group.end();iter++)
							{
								Surface_mesh::Vertex v1, v2;
								v1 = iter->v;
								v2 = *vit;
								if (*vit == iter->v)
								{
									flag = false;
									break;
								}
							}
							if (flag)
								goals.push_back({ *vit,0,0,0 });
							++vit;
						} while (vit != vend);
						++hit;
					} while (hit != hend);
					Surface_mesh::Vertex aimVertex,aimVertex1;
					Scalar min(1000);
					Surface_mesh::FeatureVertex new_v;

					//检测末端有没有大的转折处
					Scalar maxcos(-1),mindis(10);
					vhead = mesh.from_vertex(hhead);
					vec1 = fpoints[mesh.from_vertex(hhead)];
					vec1 -= fpoints[mesh.to_vertex(hhead)];
					next_hhead = mesh.next_halfedge(hhead);
					vec3 = fpoints[mesh.from_vertex(next_hhead)];
					vec3 -= fpoints[mesh.to_vertex(next_hhead)];
					denom = sqrt(dot(vec1, vec1)*dot(vec3, vec3));
					cosine = dot(vec1, vec3) / denom;
					bool flag = false;
					if (cosine > 0.80)
					{
						for (auto iter = goals.begin(); iter != goals.end(); iter++)
						{
							vec2 = vpoints[iter->v];
							vec2 -= fpoints[mesh.from_vertex(hhead)];
							denom = sqrt(dot(vec1, vec1)*dot(vec2, vec2));
							cosine = dot(vec1, vec2) / denom;
							/*iter->cosine = cosine;
							iter->pro = vsa_pro[iter->v];
							iter->Fn = dist(fpoints[mesh.from_vertex(hhead)],vpoints[iter->v]);*/
							if (cosine > 0.83)
							{
								aimVertex = iter->v;
								new_v = mesh.add_feature_vertex(vpoints[aimVertex]);
								mesh.insert_head_vertex(l, new_v,-1);
								mesh.update_featureLine(l, aimVertex, true);
								flag = true;
								break;
							}
							/*else if (iter->Fn<min)
							{
							aimVertex = iter->v;
							min = iter->Fn;
							}*/
						}
					}
				 if (!flag)
					{
					 for (auto iter = group.begin(); iter != group.end(); iter++)
					 {
						 if (visted[iter->v])
						 {
							 vec2 = vpoints[iter->v];
							 vec2 -= fpoints[mesh.from_vertex(hhead)];
							 denom = sqrt(dot(vec3, vec3)*dot(vec2, vec2));
							 cosine = dot(vec3, vec2) / denom;
							 Scalar dis = dist(fpoints[mesh.from_vertex(hhead)], vpoints[iter->v]);
							 if (cosine > maxcos)
							 {
								 maxcos = cosine;
								 aimVertex = iter->v;
							 }
							 if (dis < mindis)
							 {
								 mindis = dis;
								 aimVertex1 = iter->v;
							 }
						 }
						 if (maxcos > 0.5)
						 {
							 mesh.exchange_feature_vertex(vhead, vpoints[aimVertex]);
							 mesh.update_featureLine(l, aimVertex, true);
						 }
						 else
						 {
							 mesh.exchange_feature_vertex(vhead, vpoints[aimVertex1]);
							 mesh.update_featureLine(l, aimVertex1, true);
						 }
					 }
						
					}
					
					/*int k = visted[aimVertex];
					if (visted[aimVertex] != -1 && visted[aimVertex] != l.idx())
					{
						if (mesh.get_exten(l) == 3)
							mesh.set_exten(l, 2);
						else
							mesh.set_exten(l, 0);
						if (!ldeleted[Surface_mesh::FeatureLine(visted[aimVertex])])
						{
							new_v = mesh.add_feature_vertex(fpoints[find_shortest_distance_point(mesh, Surface_mesh::FeatureLine(visted[aimVertex]), vpoints[aimVertex])]);
							mesh.insert_head_vertex(l, new_v);
							mesh.update_featureLine(l, aimVertex, true);
							
						}
					}*/
					
					group.clear();
					goals.clear();
				}
				if (mesh.get_exten(l) != 1 && mesh.get_exten(l) != 0)         //处理尾端点
				{
					Surface_mesh::FeatureHalfedge htail, pre_htail;
					Surface_mesh::FeatureVertex vtail;
					Surface_mesh::Halfedge_around_face_circulator hit, hend;
					Surface_mesh::Vertex_around_face_circulator vit, vend;
					Surface_mesh::Halfedge h;
					Scalar cosine, denom;
					Point  vec1, vec2, vec3;
					htail = mesh.tail_halfedge(l);
					
					f = mesh.face(htail);
					vit = vend = mesh.vertices(f);
					do
					{
						group.push_back({ *vit,0,0,0 });
						++vit;
					} while (vit != vend);
					hit = hend = mesh.halfedges(f);
					do
					{
						h = mesh.opposite_halfedge(*hit);
						f = mesh.face(h);
						vit = vend = mesh.vertices(f);
						do
						{
							bool flag = true;
							for (auto iter = group.begin(); iter != group.end(); iter++)
							{
								Surface_mesh::Vertex v1, v2;
								v1 = iter->v;
								v2 = *vit;
								if (*vit == iter->v)
								{
									flag = false;
									break;
								}
							}
							if (flag)
								goals.push_back({ *vit,0,0,0 });
							++vit;
						} while (vit != vend);
						++hit;
					} while (hit != hend);
					Surface_mesh::Vertex aimVertex,aimVertex1;
					Scalar maxcos(-1),mindis(10);
					Surface_mesh::FeatureVertex new_v1;

					vtail = mesh.to_vertex(htail);
					vec1 = fpoints[mesh.to_vertex(htail)];
					vec1 -= fpoints[mesh.from_vertex(htail)];
					pre_htail = mesh.prev_halfedge(htail);
					vec3 = fpoints[mesh.to_vertex(pre_htail)]; 
					vec3 -= fpoints[mesh.from_vertex(pre_htail)];
					denom = sqrt(dot(vec1, vec1)*dot(vec3, vec3));
					cosine = dot(vec1, vec3) / denom;
					bool flag = false;
					if (cosine < 0.80)
					{
						for (auto iter = goals.begin(); iter != goals.end(); iter++)
						{
							vec2 = vpoints[iter->v];
							vec2 -= fpoints[mesh.to_vertex(htail)];
							denom = sqrt(dot(vec1, vec1)*dot(vec2, vec2));
							cosine = dot(vec1, vec2) / denom;
							/*iter->cosine = cosine;
							iter->pro = vsa_pro[iter->v];
							iter->fn = dist(fpoints[mesh.to_vertex(htail)], vpoints[iter->v]);*/
							if (cosine > 0.83)
							{
								aimVertex = iter->v;
								new_v1 = mesh.add_feature_vertex(vpoints[aimVertex]);
								mesh.insert_tail_vertex(l, new_v1,-1);
								mesh.update_featureLine(l, aimVertex, false);
								flag = true;
								break;
							}
							/*if (iter->fn < min)
							{
								aimvertex = iter->v;
								min = iter->fn;
							}*/
						}
						
					}
				 if (!flag)
					{
						for (auto iter = group.begin(); iter != group.end(); iter++)
						{
							if (visted[iter->v])
							{
								vec2 = vpoints[iter->v];
								vec2 -= fpoints[mesh.from_vertex(htail)];
								denom = sqrt(dot(vec3, vec3)*dot(vec2, vec2));
								cosine = dot(vec3, vec2) / denom;
								Scalar dis = dist(fpoints[mesh.to_vertex(htail)], vpoints[iter->v]);
								if (cosine > maxcos)
								{
									maxcos = cosine;
									aimVertex = iter->v;
								}
								if (dis < mindis)
								{
									mindis = dis;
									aimVertex1 = iter->v;
								}
							}
						}
						if (maxcos > 0.5)
						{
							mesh.exchange_feature_vertex(vtail, vpoints[aimVertex]);
							mesh.update_featureLine(l, aimVertex, false);
						}
						else
						{
							mesh.exchange_feature_vertex(vtail, vpoints[aimVertex1]);
							mesh.update_featureLine(l, aimVertex1, false);
						}
					}

					/*int k = visted[aimVertex];
					if (visted[aimVertex] != -1)
					{
						int k = visted[aimVertex];
						mesh.set_exten(l, 0);
						Surface_mesh::FeatureLine l;
						l = Surface_mesh::FeatureLine(visted[aimVertex]);
						if (!ldeleted[Surface_mesh::FeatureLine(visted[aimVertex])])
						{
							new_v1 = mesh.add_feature_vertex(fpoints[find_shortest_distance_point(mesh, Surface_mesh::FeatureLine(visted[aimVertex]), vpoints[aimVertex])]);
							mesh.insert_tail_vertex(l, new_v1);
							mesh.update_featureLine(l, aimVertex, false);
							
						}
					}*/
					group.clear();
					goals.clear();
				}
			}

		
			
			std::vector<Candidate>  candidate;
			std::vector<Candidate>  add_point;
			bool stop = false;
			while (!stop)
			{
				for (auto l : mesh.lines())
				{
					if (mesh.is_deleted(l) || mesh.head(l) == mesh.tail(l) || mesh.to_vertex(mesh.halfedge(l)) == mesh.tail(l)) continue;
					Surface_mesh::Vertex_around_vertex_circulator vvit, vvend;
					int i;
					int k = mesh.get_exten(l);
					if (mesh.get_exten(l) != 2 && mesh.get_exten(l) != 0)
					{
						Surface_mesh::Vertex seed, vhead;
						Surface_mesh::FeatureHalfedge hhead;
						Candidate c;
						Scalar denom, cosine;
						Point vec1, vec2;
						hhead = mesh.halfedge(l);
						vhead = mesh.mesh_head(l);
						seed = vhead;
						int step(0);
						while (step < 2)
						{

							vvit = vvend = mesh.vertices(seed);
							candidate.clear();
							add_point.clear();

							do
							{
								c = { *vvit, 0,k_max[*vvit],dir_min[*vvit],0 };        //Initialize the struct
								candidate.push_back(c);
							} while (++vvit != vvend);
							Candidate addpoint(candidate[0]);
							int k = 0;
							for (i = 0; i < candidate.size(); ++i)
							{
								if (visted[candidate[i].v] != visted[vhead])
								{
									addpoint = candidate[i];
									k++;
								}
							}
							if (k == 1)
							{
								vec2 = vpoints[addpoint.v];
								vec2 -= fpoints[mesh.from_vertex(hhead)];
								denom = sqrt(dot(vec1, vec1)*dot(vec2, vec2));
								cosine = dot(vec1, vec2) / denom;
								if (cosine > 0.5)
								{
									Surface_mesh::FeatureVertex new_v2 = mesh.add_feature_vertex(vpoints[addpoint.v]);
									mesh.insert_head_vertex(l, new_v2, -1);

									step++;
									seed = addpoint.v;
									if (visted[seed] != -1 /*&& mesh.is_ridge(Surface_mesh::FeatureLine(visted[seed])) == mesh.is_ridge(l)*/)
									{
										if (mesh.get_exten(l) == 3)
											mesh.set_exten(l, 2);
										else
											mesh.set_exten(l, 0);
										new_v2 = mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(visted[seed]), fpoints[new_v2]);
										if (new_v2 != mesh.head(l))
											mesh.insert_head_vertex(l, new_v2, -1);
										break;
									}
									visted[seed] = l.idx();
									continue;
								}
							}
							vec1 = fpoints[mesh.from_vertex(hhead)];
							vec1 -= fpoints[mesh.to_vertex(hhead)];
							for (i = 0; i < candidate.size(); ++i)
							{
								Scalar s1 = vsa_pro[candidate[i].v];

								vec2 = vpoints[candidate[i].v];
								vec2 -= fpoints[mesh.from_vertex(hhead)];
								denom = sqrt(dot(vec1, vec1)*dot(vec2, vec2));
								cosine = dot(vec1, vec2) / denom;
								candidate[i].cosine = cosine;
								candidate[i].Fn += vsa_pro[candidate[i].v];
							}

							for (i = 0; i < candidate.size(); ++i)
							{
								if (candidate[i].cosine > 0)
									if (candidate[i].Fn > addpoint.Fn)
										addpoint = candidate[i];
							}
							for (i = 0; i < candidate.size(); ++i)
							{
								if (candidate[i].cosine > 0.3)
								{
									if (candidate[i].Fn == addpoint.Fn)
									{
										add_point.push_back(candidate[i]);
									}
								}
							}
							for (i = 0; i < add_point.size(); ++i)
							{
								if (visted[add_point[i].v] != -1 || visted[add_point[i].v] != l.idx())
								{
									addpoint = add_point[i];
								}
								/*if (add_point[i].cosine > addpoint.cosine)
									addpoint = add_point[i];*/
							}
							Surface_mesh::FeatureVertex new_v2 = mesh.add_feature_vertex(vpoints[addpoint.v]);
							mesh.insert_head_vertex(l, new_v2, -1);
							step++;
							seed = addpoint.v;
							if (visted[seed] != -1 /*&& mesh.is_ridge(Surface_mesh::FeatureLine(visted[seed])) == mesh.is_ridge(l)*/)
							{
								if (mesh.get_exten(l) == 3)
									mesh.set_exten(l, 2);
								else
									mesh.set_exten(l, 0);
								new_v2 = mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(visted[seed]), fpoints[new_v2]);
								if (new_v2 != mesh.head(l))
									mesh.insert_head_vertex(l, new_v2, -1);
								break;
							}
							visted[seed] = l.idx();
							mesh.update_lineporp(l,addpoint.v,true);
						}
					
					}
					if (mesh.get_exten(l) != 1 && mesh.get_exten(l) != 0)
					{
						Surface_mesh::Vertex seed, vtail;
						Surface_mesh::FeatureHalfedge htail;
						Candidate c;
						Point vec1, vec2;
						Scalar denom, cosine;
						htail = mesh.tail_halfedge(l);
						vtail = mesh.mesh_tail(l);
						vec1 = fpoints[mesh.to_vertex(htail)];
						vec1 -= fpoints[mesh.from_vertex(htail)];
						seed = vtail;
						int step = 0;
						while (step < 2)
						{

							vvit = vvend = mesh.vertices(seed);
							candidate.clear();
							add_point.clear();
							do
							{
								c = { *vvit, 0,k_max[*vvit],dir_min[*vvit] };        //Initialize the struct
								candidate.push_back(c);
							} while (++vvit != vvend);
							Candidate addpoint(candidate[0]);
							int k = 0;
							for (i = 0; i < candidate.size(); ++i)
							{
								if (visted[candidate[i].v] != visted[vtail])
								{
									addpoint = candidate[i];
									k++;
								}
							}
							if (k == 1)
							{
								step++;
								Surface_mesh::FeatureVertex new_v3 = mesh.add_feature_vertex(vpoints[addpoint.v]);
								mesh.insert_tail_vertex(l, new_v3, -1);
								seed = addpoint.v;
								if (visted[seed] != -1 /*&& mesh.is_ridge(Surface_mesh::FeatureLine(visted[seed])) == mesh.is_ridge(l)*/)
								{
									if (mesh.get_exten(l) == 3)
										mesh.set_exten(l, 1);
									else
										mesh.set_exten(l, 0);
									new_v3 = mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(visted[seed]), fpoints[new_v3]);
									if (new_v3 != mesh.tail(l))
									    mesh.insert_tail_vertex(l, new_v3, -1);
									break;
								}
								visted[seed] = l.idx();
								continue;
							}

							for (i = 0; i < candidate.size(); ++i)
							{
								Scalar s1 = vsa_pro[candidate[i].v];
								vec2 = vpoints[candidate[i].v];
								vec2 -= fpoints[mesh.to_vertex(htail)];
								denom = sqrt(dot(vec1, vec1)*dot(vec2, vec2));
								cosine = dot(vec1, vec2) / denom;
								candidate[i].cosine = cosine;
								candidate[i].Fn += vsa_pro[candidate[i].v];

							}

							for (i = 0; i < candidate.size(); ++i)
							{
								if (candidate[i].cosine > 0)
									if (candidate[i].Fn > addpoint.Fn)
										addpoint = candidate[i];
							}
							for (i = 0; i < candidate.size(); ++i)
							{
								if (candidate[i].cosine > 0.3)
								{
									if (candidate[i].Fn == addpoint.Fn)
									{
										add_point.push_back(candidate[i]);
									}
								}
							}
							for (i = 0; i < add_point.size(); ++i)
							{
								if (add_point[i].cosine > addpoint.cosine)
									addpoint = add_point[i];
							}
							Surface_mesh::FeatureVertex new_v3 = mesh.add_feature_vertex(vpoints[addpoint.v]);
							mesh.insert_tail_vertex(l, new_v3, -1);
							step++;
							seed = addpoint.v;
							if (visted[seed] != -1 /*&& mesh.is_ridge(Surface_mesh::FeatureLine(visted[seed])) == mesh.is_ridge(l)*/)
							{
								if (mesh.get_exten(l) == 3)
									mesh.set_exten(l, 1);
								else
									mesh.set_exten(l, 0);
								new_v3 = mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(visted[seed]), fpoints[new_v3]);
								if (new_v3 != mesh.tail(l))
								    mesh.insert_tail_vertex(l, new_v3, -1);
								break;
							}
							mesh.update_lineporp(l, addpoint.v, false);
							visted[seed] = l.idx();
						}
						
					}
				}

				bool flag = true;
				for (auto l : mesh.lines())
				{
					if (ldeleted[l] || mesh.head(l) == mesh.tail(l) || mesh.to_vertex(mesh.halfedge(l)) == mesh.tail(l)) continue;
					if (mesh.get_exten(l) != 0)
					{
						flag = false;
						break;
					}
				}
				if (flag)
				{
					stop = true;
				}
			}

			//-----------------输出特征线信息------------//
			FILE *out = fopen("Crest.txt", "w");
			fprintf(out, "%d\n", mesh.n_fvertices());
			for (Surface_mesh::FeatureVertex_iterator vit = mesh.fvertices_begin(); vit != mesh.fvertices_end(); ++vit)
			{
				const Point& p = fpoints[*vit];
				fprintf(out, "%.10f %.10f %.10f\n", p[0], p[1], p[2]);
			}

			/*int ridge_num(0), ravine_num(0);

			for (auto l : mesh.lines())
			{
				if (!ldeleted[l] && mesh.is_ridge(l))
				{
					ridge_num++;
				}else if (!ldeleted[l] && !mesh.is_ridge(l))
				{
					ravine_num++;
				}
			}*/

			bool ridge(false), ravine(false);
			int num;
			for (auto l : mesh.lines())
			{
				if (ldeleted[l]) continue;
				Surface_mesh::Halfedge_around_line_circulator  hit;
				Surface_mesh::FeatureVertex vit, vend;
				hit = mesh.halfedges(l);
				vend = mesh.tail(l);
				
				if (mesh.is_ridge(l) && !ridge)
				{
					fprintf(out, "ridge\n");
					num = 0;
					ridge = true;
				}
				else if (!mesh.is_ridge(l) && !ravine)
				{
					fprintf(out, "ravine\n");
					num = 0;
					ravine = true;
				}
				fprintf(out, "%d %d\n", num, mesh.get_line_num(l));      
				while (mesh.to_vertex(*hit) != vend)
				{
					fprintf(out, "%d %d\n", mesh.from_vertex(*hit).idx(), mesh.to_vertex(*hit).idx());
					++hit;
				}
				fprintf(out, "%d %d\n", mesh.from_vertex(*hit).idx(), mesh.to_vertex(*hit).idx());
				num++;
			}
			fclose(out);
		 //   //extension feature line  The depth of the 5
			//std::vector<Candidate>  candidate;
			//std::vector<Candidate>  add_point;
			//Scalar  max_k, min_k;
			//for (auto l : mesh.lines())
			//{
			//	if (mesh.is_deleted(l) || mesh.head(l) == mesh.tail(l)) continue;

			//	Surface_mesh::FeatureVertex head, tail;
			//	Surface_mesh::Vertex seed, vhead, vtail;
			//	Surface_mesh::FeatureHalfedge hhead;
			//	Candidate c;
			//	vhead = mesh.mesh_head(l);
			//	seed = vhead;
			//	add_point.clear();
			//	/*c = { seed,0,k_max[seed],dir_min[seed] };
			//	candidate.push_back(c);*/
			//	hhead = mesh.halfedge(l);
			//	Direction start;
			//	start = fpoints[mesh.to_vertex(hhead)] - fpoints[mesh.from_vertex(hhead)];
			//	start = start + (fpoints[mesh.to_vertex(mesh.next_halfedge(hhead))] - fpoints[mesh.from_vertex(mesh.next_halfedge(hhead))]);
			//	for (int step = 0; step < 5 ; ++step)
			//	{
			//		
			//		Surface_mesh::Vertex_around_vertex_circulator vvit, vvend;
			//		vvit = vvend = mesh.vertices(seed);
			//		candidate.clear();
			//		do
			//		{
			//			c = { *vvit, 0,k_max[*vvit],dir_min[*vvit] };        //Initialize the struct
			//			candidate.push_back(c);
			//		} while (++vvit != vvend);
			//		max_k = max_maximum_curvatures(candidate);
			//		min_k = min_maximum_curvatures(candidate);
			//		for (int i = 0; i < candidate.size(); ++i)             //the fist item
			//			candidate[i].Fn = std::abs((min_k - candidate[i].k_max) / (max_k - min_k));
			//		Direction tavg ,vmj(0, 0, 0), vm_;
			//		tavg = average_dir(start,add_point);
			//		for (int i = 0; i < candidate.size(); ++i)             //the second item  and last item
			//		{
			//			candidate[i].dir;
			//			Scalar s0 = angle(candidate[i].dir, tavg);
			//			candidate[i].Fn += angle(candidate[i].dir, tavg) / PI;
			//		}
 		//			for (int i = 0; i < candidate.size(); ++i)
			//		{
			//			Scalar s1 = vsa_pro[candidate[i].v];
			//			candidate[i].Fn += vsa_pro[candidate[i].v];
			//		}
			//		Scalar  cavg(0);
			//		if(!add_point.empty())
			//		{ 
			//		for (int i = 0; i < add_point.size(); ++i)
			//		{
			//			cavg += add_point[i].k_max;
			//			cavg /= add_point.size();
			//		}
			//		for (int i = 0; i < candidate.size(); ++i)
			//		{
			//			Scalar s2 = std::abs((cavg - candidate[i].k_max) / cavg);
			//			candidate[i].Fn += std::abs((cavg - candidate[i].k_max) / cavg);
			//		}
			//		}
			//		//find a low cost funtion
			//		Candidate addpoint(candidate[0]);
			//		for (int i = 0; i < candidate.size(); ++i)  
			//		{
			//			if (candidate[i].Fn < addpoint.Fn)
			//				addpoint = candidate[i];
			//		}
			//		addpoint = candidate[1];
 		//			add_point.push_back(addpoint);
			//		Surface_mesh::FeatureVertex new_v = mesh.add_feature_vertex(vpoints[addpoint.v]);
			//		mesh.insert_head_vertex(l, new_v);
			//		seed = addpoint.v;
			//		if (visted[seed]) break;
			//		visted[seed] = true;
			//		
			//	}
			//	vtail = mesh.mesh_head(l);
			//}
		}

		

		Scalar max_maximum_curvatures(const std::vector<Candidate>& candidate_cache)
		{
			Scalar max = -1000;
			for (int i = 0;i<candidate_cache.size();i++)
			{
				if (max < candidate_cache[i].k_max)
					max = candidate_cache[i].k_max;
			}
			return max;
		}

		Scalar min_maximum_curvatures(const std::vector<Candidate>& candidate_cache)
		{
			Scalar min = 10000;
			for (int i = 0; i < candidate_cache.size(); i++)
			{
				if (min > candidate_cache[i].k_max)
					min = candidate_cache[i].k_max;
			}
			return min;
		}

		Surface_mesh::FeatureVertex  find_shortest_distance_point(Surface_mesh& mesh, Surface_mesh::FeatureLine l, Point v)
		{
			auto fpoints = mesh.feature_v_property<Point>("f:point");
			Point p;
			Scalar dis, min(1000);
			Surface_mesh::Halfedge_around_line_circulator  hit;
			Surface_mesh::FeatureVertex vit, vhead, vend, temp;
			
			hit = mesh.halfedges(l);
			vhead = mesh.head(l);
			vend = mesh.tail(l);
			dis = dist(fpoints[vhead], v);
			if (dis < min)
			{
				min = dis;
				temp = vhead;
			}
			while (mesh.to_vertex(*hit) != vend)
			{
				vit = mesh.to_vertex(*hit);
				dis = dist(fpoints[vit], v);
				if (dis < min)
				{
					min = dis;
					temp = vit;
				}
				++hit;
			}
			if (temp == vhead)
			{
				if (mesh.get_exten(l) == 3)
					mesh.set_exten(l, 2);
				else mesh.set_exten(l, 0);
			}
			else if (temp == vend)
			{
				if (mesh.get_exten(l) == 3)
					mesh.set_exten(l, 1);
				else mesh.set_exten(l, 0);
			}

			return temp;
		}

		//=============================================================================
	} // namespace surface_mesh
} // namespace graphene
  //=============================================================================

