#include "common.h"

void init(int x_max, int y_max) {

	total_points.push_back(vec3f(0, 0, -INF / 2));
	total_points.push_back(vec3f(0, y_max, -INF / 2));
	total_points.push_back(vec3f(x_max, y_max, -INF / 2));
	total_points.push_back(vec3f(x_max, 0, -INF / 2));
	//添加黑色虚拟背景面
	vector<int> points_inf;
	points_inf.push_back(total_points.size() - 4);
	points_inf.push_back(total_points.size() - 3);
	points_inf.push_back(total_points.size() - 2);
	points_inf.push_back(total_points.size() - 1);
	struct_polygon polygon_inf(points_inf, total_polygons.size());
	total_polygons.push_back(polygon_inf);

	//构造分类边表
	for (int y = 0; y <= y_max; ++y) {
		vector<int> temp;
		classified_edges.push_back(temp);
	}
	for (int i = 0; i < total_polygons.size(); ++i) {
		total_polygons[i].get_edges();
		for (int j = 0; j < total_polygons[i].edges.size(); ++j) {
			int y = total_edges[total_polygons[i].edges[j]].y_max;
			classified_edges[y].push_back(total_polygons[i].edges[j]);
		}
	}
}
bool cmp(int index0, int index1) {
	return total_edges[index1].x - total_edges[index0].x > EPS ;
}
/*
void update_active_edges(REAL y, vector<int>& ret_edges) {
	for (auto it = ret_edges.begin(); it != ret_edges.end();) {
		if (--(total_edges[*it].dy) <= 0) {
			ret_edges.erase(it);
		}
		else {
			total_edges[*it].x += total_edges[*it].dx;
			++it;
		}
	}

	ret_edges.insert(ret_edges.end(), classified_edges[y].begin(), classified_edges[y].end());
	//将活化边表按x值大小排序
	sort(ret_edges.begin(), ret_edges.end(), cmp);
}
*/
vector<int> update_active_edges(REAL y, vector<int> edges) {
	vector<int> ret_edges;
	
	for (auto it = edges.begin(); it != edges.end(); ++it) {
		if (--(total_edges[*it].dy) > 0) {
			total_edges[*it].x += total_edges[*it].dx;
			ret_edges.push_back(*it);
		}
	}

	ret_edges.insert(ret_edges.end(), classified_edges[y].begin(), classified_edges[y].end());
	//将活化边表按x值大小排序
	sort(ret_edges.begin(), ret_edges.end(), cmp);
	return ret_edges;
}

//取得最大z值的面片（性能瓶颈）
int get_closest_polygon_id(REAL x, REAL y, vector<int> polygons) {
	REAL max_z = -INF;
	int id = -1;
	assert(polygons.size() > 0);
	for (int i = 0; i < polygons.size(); ++i) {
		struct_polygon polygon = total_polygons[polygons[i]];
		REAL z = (-polygon.d - polygon.normal.x * x - polygon.normal.y * y) / polygon.normal.z;
		if (z > max_z) {
			max_z = z;
			id = polygon.polygon_id;
		}
	}
	assert(id >= 0);
	return id;
}
//删除in->out的面片（性能瓶颈）
void delete_polygon(vector<int> &polygons, int id) {
	bool flag = false;
	for (int i = 0; i < polygons.size(); ++i) {
		if (total_polygons[polygons[i]].polygon_id == id) {
			polygons.erase(polygons.begin() + i);
			flag = true;
			break;
		}
	}
	assert(flag);
}
void scan(int y_min, int y_max) {
	vector<int> active_edges;
	for (int y = y_max; y > y_min; --y) {
#ifdef DEBUG
		printf("scan line = %d\n", y);
#endif
		vector<int> active_in_polygons;
		//更新活化边表并递增排序
		active_edges = update_active_edges(y, active_edges);
		if (active_edges.size() > 0) {
			auto it0 = active_edges.begin();
			auto it1 = active_edges.begin();
			while (++it1 != active_edges.end()) {
				if (total_polygons[total_edges[*it0].polygon_id].is_in == false) {
					//多边形被活化（out->in）：加入活化多边形表
					total_polygons[total_edges[*it0].polygon_id].is_in = true;
					active_in_polygons.push_back(total_edges[*it0].polygon_id);
				}
				else {
					//（in->out）：从活化多边形表中删除
					total_polygons[total_edges[*it0].polygon_id].is_in = false;
					delete_polygon(active_in_polygons, total_edges[*it0].polygon_id);
				}
				int id = get_closest_polygon_id((total_edges[*it0].x + total_edges[*it1].x) / 2, y, active_in_polygons);
				//若线段两端点重合则不用画
				if (total_edges[*it0].x < total_edges[*it1].x) {
#ifdef DEBUG
					printf("(%lf->%lf), polygon_id = %d\n", total_edges[*it0].x, total_edges[*it1].x, id);
#endif
					lines.push_back(struct_line(total_edges[*it0].x, y, total_edges[*it1].x, y, id));
				}
				++it0;
			}
			//最后一个polygon单独处理
			total_polygons[total_edges[*it0].polygon_id].is_in = !total_polygons[total_edges[*it0].polygon_id].is_in;
		}
	}
}
