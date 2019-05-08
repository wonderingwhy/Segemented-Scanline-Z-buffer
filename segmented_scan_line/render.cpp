#include "common.h"
vector<string> file_names;
void set_color() {
	vec3f rand_color(rand() % 100 / 100.0, rand() % 100 / 100.0, rand() % 100 / 100.0);

	//根据光源和平面法向量设置面片颜色
	vec3f light_color(0.3, 0.3, 0.3);
	vec3f ambient_color(0.3, 0.3, 0.3);
	vec3f light_position(0.0, 0.0, -1000.0);
	for (int i = 0; i < total_polygons.size(); ++i) {
		total_polygons[i].calc_normal();
	}
	//虚拟背景不用设置颜色，默认为黑色
	for (int i = 0; i < total_polygons.size() - 1; ++i) {
		vec3f color = ambient_color;
		for (int j = 0; j < total_polygons[i].points.size(); ++j) {
			vec3f vertex(total_points[total_polygons[i].points[j]].x, 
				total_points[total_polygons[i].points[j]].y, 
				total_points[total_polygons[i].points[j]].z);
			vec3f ray_direction = light_position - vertex;
			vec3f normal(total_polygons[i].normal.x, total_polygons[i].normal.y, total_polygons[i].normal.z);
			float cosine = ray_direction.normalize() * normal;
			if (cosine > 0.0) {
				color = color + light_color * KD * cosine;
			}
		}
		int lens = total_polygons[i].points.size();
		total_polygons[i].color = vec3f(color.x / lens, color.y / lens, color.z / lens);
		//total_polygons[i].color = rand_color;
	}
}
bool mouse_left = false;
void mouse_event(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse_left = true;
			printf("\n---   Rotation Begin   ---\n");
			glutIdleFunc(replay);
		}
		else if (state == GLUT_UP) {
			printf("---   Rotation End   ---\n\n");
			glutIdleFunc(nullptr);
		}
	}
	else if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {
		if (scale_level >= 16) {
			printf("Out of Scale Minimum = 1/%.1f\n", scale_level);
		}
		else {
			printf("Down Scale = 1/%.1f\n", scale_level);
			scale_level += 0.2;
			run();
		}
	}
	else if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
		if (scale_level <= 1.6) {
			printf("Out of Scale Maximum = 1/%.1f\n", scale_level);
		}
		else {
			printf("Up Scale = 1/%.1f\n", scale_level);
			scale_level -= 0.2;
			run();
		}
	}
}
void replay() {
	//各点旋转后重新显示
	time_start = omp_get_wtime();
	int points_lens = total_points.size() - 4;
	int polygons_lens = total_polygons.size() - 1;
	for (int i = 0; i < points_lens; ++i) {
		total_points[i] = total_points[i].rotate();
	}
	//去除虚拟背景多边形和点
	total_points.resize(points_lens);
	total_polygons.resize(polygons_lens);
	total_edges.clear();
	classified_edges.clear();
	lines.clear();
	init(WINDOW_SIZE, WINDOW_SIZE);
	set_color();
	scan(0, WINDOW_SIZE);
	time_end = omp_get_wtime();
	printf("Scanning Time: %3.1f ms\n", (time_end - time_start) * 1000);
	glutPostRedisplay();
}
void display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < lines.size(); ++i) {
		glBegin(GL_LINES);
		glColor3f(total_polygons[lines[i].polygon_id].color.x, total_polygons[lines[i].polygon_id].color.y, total_polygons[lines[i].polygon_id].color.z);
		glVertex2f((lines[i].x0 / WINDOW_SIZE - 0.5) * 2, (lines[i].y0 / WINDOW_SIZE - 0.5) * 2);
		glVertex2f((lines[i].x1 / WINDOW_SIZE - 0.5) * 2, (lines[i].y1 / WINDOW_SIZE - 0.5) * 2);
		glEnd();
	}
#ifdef DOUBLE_BUFFER
	glutSwapBuffers();
#else
	glFlush();
#endif
}
void screen_menu(int value) {
	file_name = FILE_PATH + "/" + file_names[value];
	data_loaded = false;
	run();
}
void reshape(int width, int height) {
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
}
void render() {
#ifdef DOUBLE_BUFFER
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
#else
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutCreateWindow("OpenGL");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//添加鼠标事件（左键旋转，滚轮缩放）
	glutMouseFunc(mouse_event);
	file_names = get_files(FILE_PATH, "*.obj");
	//添加右键菜单
	glutCreateMenu(screen_menu);
	for (int i = 0; i < file_names.size(); ++i) {
		glutAddMenuEntry(file_names[i].c_str(), i);
	}
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}