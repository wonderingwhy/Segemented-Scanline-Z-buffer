#include "common.h"

REAL max_x, min_x, max_y, min_y, max_z, min_z;
vector<vector<int>> faces;
vector<vec3f> original_points;
void read() {
	max_x = -INF, min_x = INF;
	max_y = -INF, min_y = INF;
	max_z = -INF, min_z = INF;
	freopen(file_name.c_str(), "r", stdin);
	char str[STR_MAX_LENGTH];
	faces.clear();
	original_points.clear();
	while (gets(str)) {
		char type[STR_MAX_LENGTH];
		sscanf(str, "%s", type);
		if (strlen(type) == 1) {
			if (type[0] == 'v') {
				REAL x, y, z;
#ifdef DOUBLE
				sscanf(str + 1, "%lf%lf%lf", &x, &y, &z);
#else
				sscanf(str + 1, "%f%f%f", &x, &y, &z);
#endif
				original_points.push_back(vec3f(x, y, z));
				max_x = max(x, max_x);
				min_x = min(x, min_x);
				max_y = max(y, max_y);
				min_y = min(y, min_y);
				max_z = max(z, max_z);
				min_z = min(z, min_z);
			}
			else if (type[0] == 'f') {
				vector<int> temp_points;
				char temp[STR_MAX_LENGTH];
				for (int offset = 2; offset < strlen(str) && ~sscanf(str + offset, "%s", temp); offset += strlen(temp) + 1) {
					int index;
					sscanf(temp, "%d", &index);
					temp_points.push_back(index - 1);
				}
				if (temp_points.size() >= 3) {
					faces.push_back(temp_points);
				}
			}
		}
	}
	printf("Num of Vertexs = %d, Num of Faces = %d\n\n", original_points.size(), faces.size());
}
void data_process() {
	//计算中心和尺度，使图像显示在窗口中央
	vec3f center((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + min_z) / 2);
	REAL scale = WINDOW_SIZE / (max(max_x - min_x, max_y - min_y) * scale_level);
	for (int i = 0; i < original_points.size(); ++i) {
		vec3f point = (original_points[i] - center) * scale + BASE_TRANS;
		total_points.push_back(point);
		assert(total_points[i].x > 0 && total_points[i].x < WINDOW_SIZE);
		assert(total_points[i].y > 0 && total_points[i].y < WINDOW_SIZE);
	}
	for (int i = 0; i < faces.size(); ++i) {
		vector<int> temp_points;
		for (int j = 0; j < faces[i].size(); ++j) {
			temp_points.push_back(faces[i][j]);
		}
		struct_polygon polygon(temp_points, total_polygons.size());
		total_polygons.push_back(polygon);
	}
}