#include "common.h"
vector<struct_edge> total_edges;
vector<vec3f> total_points;
vector<struct_polygon> total_polygons;
vector<vector<int>> classified_edges;
vector<struct_line> lines;
REAL time_start, time_end;
void struct_polygon::get_edges() {
	edges.clear();
	for (int i = 0; i < points.size(); ++i) {
		vec3f p0 = total_points[points[i]], p1 = total_points[points[(i + 1) % int(points.size())]];
		p0.y = int(p0.y + 0.5);
		p1.y = int(p1.y + 0.5);
		struct_edge edge;
		if (p0.y > p1.y) {
			edge.y_max = p0.y;
			edge.dy = p0.y - p1.y;
			edge.x = p0.x;
		}
		else {
			edge.y_max = p1.y;
			edge.dy = p1.y - p0.y;
			edge.x = p1.x;
		}
		edge.polygon_id = polygon_id;
		if (edge.dy != 0) {
			edge.dx = (p0.x - p1.x) / (p1.y - p0.y);
			total_edges.push_back(edge);
			edges.push_back(total_edges.size() - 1);
		}
	}
}
void struct_polygon::calc_normal() {
	assert(points.size() >= 3);
	vec3f temp0 = total_points[points[1]] - total_points[points[0]];
	vec3f temp1 = total_points[points[0]] - total_points[points[2]];
	normal = (temp0 ^ temp1).normalize();
	d = (-normal.x * total_points[points[0]].x -
		normal.y * total_points[points[0]].y - normal.z * total_points[points[0]].z);
}
vec3f vec3f::rotate() {
	REAL cosine = cos(2 * PI / 360), sine = sin(2 * PI / 360);
	REAL matrix[3][3] = {
		(1 - cosine) + cosine, (1 - cosine) - sine, (1 - cosine) + sine,
		(1 - cosine) + sine, (1 - cosine) + cosine, (1 - cosine) - sine,
		(1 - cosine) - sine, (1 - cosine) + sine, (1 - cosine) + cosine};
	vec3f ret = *this - BASE_TRANS;
	return vec3f(
		ret.x * matrix[0][0] + ret.y * matrix[1][0] + ret.z * matrix[2][0],
		ret.x * matrix[0][1] + ret.y * matrix[1][1] + ret.z * matrix[2][1],
		ret.x * matrix[0][2] + ret.y * matrix[1][2] + ret.z * matrix[2][2]) + BASE_TRANS;
}
void run() {
	total_edges.clear();
	total_polygons.clear();
	classified_edges.clear();
	total_points.clear();
	lines.clear();
	 
	if (data_loaded == false) {
		printf("\n***   model \"%s\" loaded   ***\n\n", file_name.c_str());
		time_start = omp_get_wtime();
		read();
		time_end = omp_get_wtime();
		printf("Loading Time: %3.1f ms\n\n", (time_end - time_start) * 1000);
		data_loaded = true;
	}
	data_process();

	time_start = omp_get_wtime();
	init(WINDOW_SIZE, WINDOW_SIZE);
	set_color();
	scan(0, WINDOW_SIZE);
	time_end = omp_get_wtime();
	printf("Scanning Time: %3.1f ms\n", (time_end - time_start) * 1000);
	if (is_first_time) {
		is_first_time = false;
		render();
	}
	else {
		display();
	}
}
vector<string> get_files(string path, string suffix) {
	_finddata_t file;
	intptr_t handle;

	string file_type;
	if (path.substr(0, 1) == "." || path.substr(0, 2) == "..") {
		file_type = path + "/" + suffix;
	}
	else {
		file_type = path + "\\" + suffix;
	}
	const char* cfile_type = file_type.c_str();

	//遍历当前文件夹中的文件
	int k = handle = _findfirst(cfile_type, &file);
	int i = 0;
	if (k == -1) {
		cout << "not exist" + suffix << endl;
		system("pause");
		exit(-1);
	}
	vector<string> filenames;
	while (k != -1) {
		filenames.push_back(string(file.name));
		k = _findnext(handle, &file);
	}
	_findclose(handle);
	return filenames;
}