//== INCLUDES =================================================================

#include <graphene/surface_mesh/algorithms/subdivision/line_dilate.h>

//== NAMESPACE ================================================================

namespace graphene {
	namespace surface_mesh {


		//== IMPLEMENTATION ==========================================================
		const int MAX = 10000;

		void line_dilate(Surface_mesh& mesh)
		{

			typedef std::pair<int, int> Connect;
			typedef std::vector<Connect>  line_connect;
			line_connect connect_cache;
			connect_cache.clear();
			auto face_line = mesh.face_property<int>("line_id", -1);
			auto epdeleted = mesh.get_endpoint_property<bool>("ep:deleted");
			auto fpoints = mesh.feature_v_property<Point>("f:point");
			auto vpoints = mesh.vertex_property<Point>("v:point");
			/*Surface_mesh::Vertex_around_face_circulator  fvit, fvend;
			Surface_mesh::Vertex v;
			Surface_mesh::Face_around_vertex_circulator  fit, fend;*/

			/*************************The whole line dilate*****************************/
			//for (auto l:mesh.lines())                    //set line_id in face
			//{
			//	Surface_mesh::Halfedge_around_line_circulator  hit;
			//    Surface_mesh::FeatureVertex vend;
			//    Surface_mesh::Face f;
			//    Surface_mesh::FeatureLine line;
			//	hit = mesh.halfedges(l);
			//	vend = mesh.tail(l);
			//	while (mesh.to_vertex(*hit) != vend)
			//	{
			//		f = mesh.face(*hit);
			//		if (f.is_valid())
			//		{
			//			face_line[f] = mesh.line(*hit).idx();
			//		}
			//		++hit;
			//	}
			//	f = mesh.face(*hit);
			//	if (f.is_valid())
			//	{
			//		face_line[f] = mesh.line(*hit).idx();
			//	}
			//}
			////dilate 
			//for (auto f : mesh.faces())
			//{
			//	if (face_line[f] != -1)
			//	{
			//	Surface_mesh::Vertex_around_face_circulator  fvit, fvend;
			//	Surface_mesh::Face_around_vertex_circulator  fit, fend;
			//	Surface_mesh::Vertex v;
			//	Surface_mesh::Face face;
			//	fvit = fvend = mesh.vertices(f);
			//	do
			//	{
			//		v = *fvit;
			//	    fit = fend = mesh.faces(v);
			//		do 
			//		{
			//			if (f != *fit)
			//			{
			//				if (face_line[*fit] != -1 && face_line[*fit] != face_line[f])
			//				{
			//					line_connect::const_iterator ncIt(connect_cache.begin()), ncEnd(connect_cache.end());
			//					bool flag = false;
			//					for (; ncIt != ncEnd; ++ncIt)
			//					{
			//						if ((face_line[f] == ncIt->first && face_line[*fit] == ncIt->second) || (face_line[*fit] == ncIt->first && face_line[f] == ncIt->second))
			//						{
			//							flag = true;
			//						}
			//					}
			//					if (!flag)
			//					{
			//					if (face_line[*fit]>face_line[f])
			//						connect_cache.push_back(Connect(face_line[f], face_line[*fit]));
			//					else
			//						connect_cache.push_back(Connect(face_line[*fit], face_line[f]));
			//				    }
			//				}
			//				face_line[*fit] = face_line[f];
			//			}
			//		} while (++fit != fend);
			//		
			//	} while (++fvit != fvend);
			//	}
			//}

			/************************It only dilate the end point********************************/
			for (auto l : mesh.lines())
			{
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
						face_line[f] = l.idx();
					}
					++hit;
				}
				vit = mesh.to_vertex(*hit);
				f = mesh.face(*hit);
				if (f.is_valid())
				{
					face_line[f] = mesh.line(*hit).idx();
				}
			}

			auto vertex_line = mesh.add_vertex_property<int>("v:the line id of vertex", -1);
			for (auto f : mesh.faces())
			{
				if (face_line[f] != -1)
				{
					Surface_mesh::Vertex_around_face_circulator fvit, fvend;
					fvit = fvend = mesh.vertices(f);
					do
					{
						vertex_line[*fvit] = face_line[f];
						++fvit;
					} while (fvit != fvend);
				}
			}

			for (auto l : mesh.lines())
			{
				if (mesh.head(l) == mesh.tail(l)) continue;             //环
				Surface_mesh::Halfedge_around_line_circulator  hit,hits;
				Surface_mesh::Vertex_around_face_circulator  fvit, fvend;
				Surface_mesh::Face_around_vertex_circulator  fit, fend;
				Surface_mesh::FeatureVertex vhead, vtail;
				Surface_mesh::Vertex v;
				Surface_mesh::Face f;
				Surface_mesh::FeatureLine line;
				Surface_mesh::EndPoint ep;
				hit = mesh.halfedges(l);
				vhead = mesh.head(l);
				vtail = mesh.tail(l);
				//仅仅在一个面上的线段不考虑
				if (mesh.to_vertex(*hit) == vtail) continue;
				if (mesh.to_vertex(mesh.next_halfedge(*hit)) == vtail)  continue;

				do
				{
					/*Surface_mesh::FeatureVertex v1, v2;
					v1 = mesh.from_vertex(*hit);
					v2 = mesh.to_vertex(*hit);*/
					if (mesh.from_vertex(*hit) == vhead || mesh.to_vertex(*hit) == vtail)
					{

						f = mesh.face(*hit);
						if (f.is_valid())
						{
							/*if (mesh.from_vertex(*hit) == vhead)
								ep = mesh.add_end_point(l, *hit, vhead, f, true);
							else
								ep = mesh.add_end_point(l, *hit, vtail, f, false);*/

							fvit = fvend = mesh.vertices(f);
							do
							{
								v = *fvit;
								fit = fend = mesh.faces(v);
								//bool stop = false;
								do
								{

									if (f != *fit)
									{
										Surface_mesh::Vertex_around_face_circulator  fvit1, fvend1;
										fvit1 = fvend1 = mesh.vertices(*fit);
										do
										{
											
											//int fit_id = face_line[*fit] % MAX, f_id = face_line[f] % MAX;
											int fit_id = vertex_line[*fvit1], f_id = l.idx();
											if (fit_id == f_id) continue;
											if (fit_id != -1 && fit_id != f_id)
											{
												Surface_mesh::FeatureLine ls = Surface_mesh::FeatureLine(fit_id);
												//int m = mesh.get_line_num(ls);
												//int n = mesh.get_line_num(ls);
												if (mesh.get_line_num(ls) == 1 ) continue;
												line_connect::const_iterator ncIt(connect_cache.begin()), ncEnd(connect_cache.end());
												bool flag = false;
												for (; ncIt != ncEnd; ++ncIt)
												{
													if ((f_id == ncIt->first && fit_id == ncIt->second) || (fit_id == ncIt->first && f_id == ncIt->second))
														flag = true;
												}
												if (!flag)
												{
												//	mesh.join(Surface_mesh::FeatureLine(fit_id), l);
												//	if (mesh.from_vertex(*hit) == vhead)
												//	{
												//		if (mesh.get_exten(l) == 3||mesh.get_exten(l) == 1)
												//		{
												//			Surface_mesh::FeatureVertex new_v1;
												//			mesh.insert_head_vertex(l, mesh.add_feature_vertex(vpoints[*fvit1]), -1);
												//			mesh.update_featureLine(l, *fvit1, true);
												//			new_v1 = mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(fit_id), vpoints[*fvit1]);
												//			if (new_v1.is_valid())
												//			{
												//				mesh.insert_head_vertex(l, new_v1, -1);
												//				if (mesh.get_exten(l) == 3)
												//					mesh.set_exten(l, 2);
												//				else
												//					mesh.set_exten(l, 0);
												//			}
												//		}
												//	}
												//	else
												//	{
												//		if (mesh.get_exten(l) == 3 || mesh.get_exten(l) == 2)
												//		{
												//			Surface_mesh::FeatureVertex new_v2;
												//			//new_v2 = mesh.add_feature_vertex(fpoints[mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(fit_id), vpoints[*fvit1])]);
												//			mesh.insert_tail_vertex(l, mesh.add_feature_vertex(vpoints[*fvit1]), -1);
												//			mesh.update_featureLine(l, *fvit1, false);
												//			new_v2 = mesh.find_shortest_distance_point(Surface_mesh::FeatureLine(fit_id), vpoints[*fvit1]);
												//			if (new_v2.is_valid())
												//			{
												//				mesh.insert_tail_vertex(l, new_v2, -1);
												//				if (mesh.get_exten(l) == 3)
												//					mesh.set_exten(l, 1);
												//				else
												//					mesh.set_exten(l, 0);
												//			}
												//		}
												//	}
												//	connect_cache.push_back(Connect(fit_id, f_id));
												//}
												//break;
											}
											/*else
												face_line[*fit] = face_line[f]+MAX;*/
										} while (++fvit1 != fvend1);
									}
								} while (++fit != fend);
								//if (stop)  break;
							} while (++fvit != fvend);
						}
					}
					if (mesh.to_vertex(*hit) == vtail) break;
					++hit;
				} while (1);
			}

			FILE  *out = fopen("connect.txt", "w");
			line_connect::const_iterator ncIt(connect_cache.begin()), ncEnd(connect_cache.end());
			for (; ncIt != ncEnd; ++ncIt)
			{
				fprintf(out, "%d %d\n", ncIt->first, ncIt->second);
			}
			//mesh.remove_face_property(face_line);
			for (auto l : mesh.lines())
			{
				//mesh.find_parent(l);
				fprintf(out, "%d %d\n", mesh.prefeatureline(l).idx(), l.idx());
			}
			fclose(out);

			auto  ldeleted = mesh.get_line_property<bool>("l:deleted");
			auto  save_fre = mesh.line_property<int>("l:save this line  frequency", 0);

			FILE *clr = NULL;
			FILE *clr1 = NULL;
			clr = fopen("input_high.clr", "r");
			clr1 = fopen("input_low.clr", "r");
			int num_high(0), num_low(0), vertex_n, patch_n, patchID, seed, patch_fN, faceID;
			double R, G, B;
			fscanf(clr, "%d", &num_high);
			fscanf(clr1, "%d", &num_low);
			
			
			for (int k = 0; k < num_high; ++k)
			{
				auto VSA_seg = mesh.add_face_property<int>("f:high density segment", -1);  //read VSA segmentation data
				auto VSA_seg1 = mesh.add_face_property<int>("f:low density segment", -1);  //read VSA segmentation data
				auto vsa_visited = mesh.add_vertex_property<bool>("v:vsa visited", false);
				auto  save_line = mesh.line_property<bool>("l:save featureline", false);
				auto  line_patch = mesh.line_property<int>("l:patch_id", -1);
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
						VSA_seg[Surface_mesh::Face(faceID)] = patchID;
					}
					fscanf(clr, "%lf %lf %lf", &R, &G, &B);
				}
				fscanf(clr1, "%d", &vertex_n);
				if (vertex_n != mesh.n_vertices())
				{
					std::cerr << "This *.clr file is not mathing mesh!" << std::endl;
					break;
				}
				fscanf(clr1, "%d", &patch_n);
				for (i = 0; i < patch_n; ++i)
				{
					fscanf(clr1, "%d %d %d", &patchID, &seed, &patch_fN);
					for (j = 0; j < patch_fN; ++j)
					{
						fscanf(clr1, "%d", &faceID);
						VSA_seg1[Surface_mesh::Face(faceID)] = patchID;
					}
					fscanf(clr1, "%lf %lf %lf", &R, &G, &B);
				}
				for (auto l : mesh.lines())
				{
					Surface_mesh::FeatureHalfedge hhead;
					Surface_mesh::FeatureVertex vtail;
					Surface_mesh::FeatureLine par_l;
					hhead = mesh.halfedge(l);
					vtail = mesh.tail(l);
					if (mesh.to_vertex(hhead) == vtail)
					{
						if (mesh.face(hhead).is_valid())
							continue;
					}
					if (mesh.average < mesh.get_length(l))
					{
						Surface_mesh::Face f;
						int patch_num(0);
						f = mesh.face(hhead);
						if (f.is_valid())
							patch_num = VSA_seg[f];
						while (1)
						{
							hhead = mesh.next_halfedge(hhead);
							f = mesh.face(hhead);
							if (f.is_valid())
							{
								int tmep = VSA_seg[f];
								if (patch_num != VSA_seg[f])
								{
									save_line[l] = true;
									break;
								}
							}
							if (mesh.to_vertex(hhead) == vtail) break;
						}
						line_patch[l] = patch_num;
					}
					else
					{
						Surface_mesh::Face f;
						int patch_num;
						f = mesh.face(hhead);
						if (f.is_valid())
							patch_num = VSA_seg1[f];
						while (1)
						{
							hhead = mesh.next_halfedge(hhead);
							f = mesh.face(hhead);
							if (f.is_valid())
							{
								int tmep = VSA_seg1[f];
								if (patch_num != VSA_seg1[f])
								{
									save_line[l] = true;
									break;
								}
							}
							if (mesh.to_vertex(hhead) == vtail) break;
						}
						line_patch[l] = patch_num;
					}
					par_l = mesh.get_parent(l);
					if (par_l != l)
					{
						if (save_line[par_l] || line_patch[l] != line_patch[par_l])
						{
							save_line[l] = true;
							if (!save_line[par_l])
							{
								save_line[par_l] = true;
							}
						
						for (auto ls : mesh.lines())
						{
							if(ls.idx() == l.idx()) break;
							if (mesh.get_parent(ls) == par_l)
							{
								save_line[ls] = true;
							}
						}
						}
					}
				}
				for (auto l : mesh.lines())
				{
					if (save_line[l])
						save_fre[l] = save_fre[l] + 1;
				}
				
				mesh.remove_vertex_property(vsa_visited);
				mesh.remove_face_property(VSA_seg);
				mesh.remove_face_property(VSA_seg1);
				mesh.remove_line_property(save_line);
				mesh.remove_line_property(line_patch);
			}
			fclose(clr);
			fclose(clr1);
			
			auto line_patch = mesh.add_line_property<int>("l:line patch id",-1);
			auto len_deleted = mesh.add_line_property<bool>("length deleted", false);
			auto vsa_deleted = mesh.add_line_property<bool>("vsa deleted", false);

			for (auto l : mesh.lines())
			{
				if (save_fre[l] <= num_high - 1)
					vsa_deleted[l] = true;
			}


			
			
			//基于长度过滤+膨胀算法
			auto length_save = mesh.add_line_property<bool>("l:length save", false);
			for (auto l : mesh.lines())
			{
				length_save[l] = length_save[mesh.get_parent(l)] | length_save[l];
				if (mesh.get_length(l) > 2.2)
				{
					length_save[l] = true;
					length_save[mesh.get_parent(l)] = true;
				}
			}
			for (auto l : mesh.lines())
			{
				if (!length_save[l])
					len_deleted[l] = true;
			}

			//基于长度过滤
			/*for (auto l : mesh.lines())
			{
				if (mesh.get_length(l) < 2.2)
				{
					len_deleted[l] = true;
				}
			}*/

			//模式选择
			for (auto l : mesh.lines())
			{
				ldeleted[l] = len_deleted[l] /*&& len_deleted[l]*/ ;
			}
			//ldeleted[Surface_mesh::FeatureLine(69)] = true;


		}
			//=============================================================================
		} // namespace surface_mesh
	} // namespace graphene
	  //=============================================================================

